/**************
* Display time with 0.1 sec accuracy
* 10 XII 2014
* Authors: Damian Lesniak, Igor Sieradzki, Patrycja Cielecka
*/


#include <avr/io.h>
#include "task_manager.h"


#define DISPLAY_STATE PORTA
#define DISPLAY_STATE_MODE DDRA				// 0 - input, 1 - output
#define DISPLAY_POWER_CONTROL PORTB			// pin state: 0 - on, 1 - off
#define DISPLAY_POWER_CONTROL_MODE DDRB
#define DISPLAY_OFF 0b11111111
#define TIME_SHOW_LEADING_ZEROS 0			// 1 - show leading zeros (e.g. 002.1, 043.7), 0 - the opposite (e.g. 2.1, 43.7)
#define BLANK_DIGIT 10

#define KEYBOARD_PORT PORTC
#define KEYBOARD_MODE DDRC						// 0 - input, 1 - output
#define KEYBOARD_PIN PINC


void check_keyboard();	// settles which key was pressed
void draw_digit();		// sets displayed number to the number of key pressed (0...16)
void multiplexer();		// rotates between displays and sends digits to them
void setup_timer();		// timer setup
void time_inc();			// counts from 0.0 to 9999


uint8_t DIGITS[] = {0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001, 0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00011001, DISPLAY_OFF};

volatile int time_counter = 0;			// time in sec/10
volatile int display_digit[4] = {BLANK_DIGIT, BLANK_DIGIT, 0, 0}; // displayed digits (calculated using time_counter every time it changes)
volatile int dot = 0;						// display dot - true or false
volatile int multiplexer_memory = 0;	// number of active display (0..3)
volatile int key_number = 0;				// 0..16, 0==nothing
volatile int key_ready = 1;				// t/f



ISR(TIMER0_COMP_vect){
	// every millisecond do:
	schedule();		// schedule new tasks
	multiplexer(); // change active display
}


int main(void)
{
	DISPLAY_POWER_CONTROL_MODE = 0b11111111;
	DISPLAY_STATE_MODE = 0b11111111;
	DISPLAY_POWER_CONTROL = 0b11111111;
	DISPLAY_STATE = DISPLAY_OFF;
	setupTimer();
//	addTask(0, 100, time_inc, (void*)0); // working timer from 0.0 to 9999

//	addTask(0, 5, time_inc, (void*)0); // please ignore: for testing purposes (on simulator)
//	addTask(1, 1, multiplexer, (void*)0); // please ignore: bad idea

	addTask(0, 50, check_keyboard, (void*)0); // settle which key was pressed 
	addTask(1, 50, draw_digit, (void*)0);		// set display_digit[4] array content to the number of key pressed
	execute();
}


void check_keyboard() {
	int column = 0;
	int row = 0;
	
	KEYBOARD_PORT = 0x00; // disable pull-ups
	KEYBOARD_MODE = 0x0F; // set row pins to input
	KEYBOARD_PORT = 0xF0; // enable pull-ups for rows (higher order bits)
	for(int i = 0; i < 20; i++); // delay
	row = KEYBOARD_PIN / 16;
	
	KEYBOARD_PORT = 0x00; // disable pull-ups
	KEYBOARD_MODE = 0xF0; // set column pins to input 
	KEYBOARD_PORT = 0x0F; // enable pull-ups for columns (lower order bits)
	for(int i = 0; i < 20; i++); // delay
	column = KEYBOARD_PIN;
	
	// evaluate decimal value of row and column
	int row_counter = 0;
	while((row % 2) == 1 && row_counter<4) {
		row = row/2;
		row_counter++;
	}

	int column_counter = 0;
	while((column % 2) == 1 && column_counter<4) {
		column = column/2;
		column_counter++;
	}
	
	int temp_key_number;
	
	// evaluate number of key pressed
	if(row_counter == 4 || column_counter == 4) temp_key_number = 0;
	else temp_key_number = 4*row_counter + column_counter + 1;
	
	if(key_number != temp_key_number) {
		key_number = temp_key_number;
		key_ready = 1;
	}
}


void draw_digit() {
	// handle key change - do nothing if user is holding a key (key is not ready)
	if(key_ready == 1) {
		key_ready = 0;
		int current_key = key_number;
		display_digit[3] = current_key % 10;
		current_key = current_key / 10;
		display_digit[2] = current_key;		
	}
}

void multiplexer() {
	uint8_t temp_display; // this will be displayed
	temp_display = DIGITS[display_digit[multiplexer_memory]]; // from DIGITS learn how to show digit assigned to the current display
	if(dot == 1 && multiplexer_memory == 2) {
		// add dot on third display if needed
		temp_display &= ~(1<<0);
	}
	DISPLAY_STATE = DISPLAY_OFF;
	DISPLAY_POWER_CONTROL = ~(1<<multiplexer_memory);	// power-on display number multiplexer_memory
	DISPLAY_STATE = temp_display;
	multiplexer_memory++;
	multiplexer_memory = (multiplexer_memory) % 4;		// increment display number (0..3)
}

void setup_timer(){
	TCCR0 |= (1<<WGM01) | (0<<WGM00);	// set clock type as CTC
	OCR0 = 250;									// set Output Compare Register - together with prescaler this will give us
	// interrupt every 1ms
	sei();										// turn interrupts on
	TIMSK |= (1<<OCIE0);						// set interrupts co compare
	TCCR0 |= (1<<CS00) | (1<<CS01 );		// set clock prescaler at 64*250 = 16,000; 16MHz * 16,000 = 1KHZ;
}

void time_inc()
{
	// increment time_counter and update display_digits
	int temp_time_counter = time_counter;
	if(temp_time_counter >= 10000) {
		// display time in seconds without dot
		dot = 0;
		temp_time_counter /= 10;
	}
	else{
		// display time in seconds/10 with dot
		dot = 1;	
	}
	// populate display_digit array with digits of the current time_counter
	for(int i = 3; i >= 0; i--){
		display_digit[i] = temp_time_counter % 10;
		temp_time_counter /= 10;
	}	
	
	if(!TIME_SHOW_LEADING_ZEROS && display_digit[0]==0) {
		// turn off segments displaying leading zeros
		display_digit[0] = BLANK_DIGIT;
		if(display_digit[1]==0) display_digit[1] = BLANK_DIGIT;
	}
	
	time_counter++;
}