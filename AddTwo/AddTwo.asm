/*
 * AddTwo.asm
 *
 *  Created: 2014-10-26 02:17:09
 *   Author: Patrycja Cielecka
 */ 

.include "m32def.inc"

.EQU size = 24 ; Rozmiar tablicy

.DSEG
	tab1: .BYTE size ; Rezerwujemy size bajt�w na tablic�, rozpoczynaj�c od 0x0060
	tab2: .BYTE size
	
.CSEG
	LDI R16, size ; Rejestr u�ywany do przemieszczania si� po tablicach

	; Zapisujemy w X i Y adresy pocz�tku tablic (Je�li size = 16, to mamy adresy 0x0060 i 0x0070)
	LDI XL,  low (tab1)
	LDI XH,  high(tab1)

	LDI YL,  low (tab2)
	LDI YH,  high(tab2)

	; Przesuwamy si� po obu tablicach zmniejszaj�c R16 i zwi�kszaj�c o 1 adresy w X i Y
	; Rejestry R17 i R18 to rejestry pomocnicze, zapisujemy w nich kolejne bajty i dodajemy je
	LD R17, X
	LD R18, Y+ 
	ADD R17, R18 ; U�ywamy ADD, poniewa� dodajemy mniej znacz�ce bajty
	ST X+, R17 ; Zapisujemy wynik dodawania z R17 w tab1
	DEC R16

	cycle:
		LD R17, X
		LD R18, Y+ 
		ADC R17, R18 ; Dla ka�dych kolejnych bajt�w u�ywamy ADC, �eby nadmiar przenosi� dalej
		ST X+, R17
		DEC R16
	BRNE cycle