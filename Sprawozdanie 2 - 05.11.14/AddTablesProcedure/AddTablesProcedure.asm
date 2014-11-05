/*
 * AddTablesProcedure.asm
 *
 *  Created: 2014-11-05 08:40:16
 *   Author: Patrycja, Igor i Micha³
 */ 


 ; dlugosc liczb w bajtach do dodania
.EQU LEN = 16

; rezerwujê pamiec dla tablic/liczb ktore bede dodawal
.DSEG
tab1: .BYTE LEN
tab2: .BYTE LEN

.CSEG
; ustawiamy miejsce w pamiêci na 0 gdzie bêdzie umieszczony kod poni¿ej
; ustawiamy wektor przerwañ
.ORG 0 
jmp START
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ
jmp NO_IRQ

START:
; inicjujemy stos
	ldi r16,high(RAMEND)
	out SPH,r16
	ldi r16,low(RAMEND)
	out SPL,r16
; w³¹czamy przerwania, choc nie wiemuy po co
	sei
; X - adres tablicy do ktorej kopiujemy
; Y - adres tablicy z ktorej kopiujemy
	LDI XL, low(tab1)
	LDI XH, high(tab1)
	LDI YL, low(tab2)
	LDI YH, high(tab2)

; Z - licznik ile pozosta³o komórek do dodania
	LDI ZL, low(LEN)
	LDI ZH, high(LEN)

; wywolujemy procedure DODAJ
	CALL DODAJ

; wirtualna procedura na brak obslugi przerwania
NO_IRQ:
	reti

DODAJ:
; void DODAJ (&L1, &L2, LEN)
; L1 = rejestr X, 
; L2 = rejestr Y, 
; LEN = rejestr Z

; odk³adamy na stos wszystkie póŸniej u¿ywane rejestry
	PUSH r16
	PUSH r17
	PUSH r18
	PUSH r0
	PUSH r1

; zerujê rejestr w któym zapamiêtam rejestr stanu po pierwszej operacji dodawania
	LDI r16, 0
; rejestry pomocnicze wykorzystywane do dodawania
	LDI r17, 1
	LDI r18, 0

loopBegin:
	CPI	ZL, 0 ; jeœli ZL!=0, to wykonaj pêtlê, jeœli nie to sprawdŸ ZH
	BRNE bodyOfLoop 
innerCheck:
	CPI ZH, 0 ; jeœli ZH!=0, to wykonaj pêtlê, jeœni nie to zakoñcz pêtlê
	BREQ endOfLoop
bodyOfLoop:
	
; g³ówna czêœæ dodawania, z uwzglêdnieniem przepe³nienia z poprzedniej operacji
	out SREG, r16 ; przywracam wartoœæ rejestru stanu z poprzedniej operacji
	LD r0, X
	LD r1, Y
	ADC r0, r1 ; dodajê to co pod adresem Y do tego co pod adresem X
	ST X, r0 ; zapisujê wynik do komórki pod adresem X
	in r16, SREG ; zapisujê rejestr stanu. Operacja ST nie modyfikuje go, wiêc mogê to zrobiæ teraz

; odejmujê 1 od pary XL, XH, od XL normalnie, a od XH tylko jeœli by³a ustawiona carry flag - wynik dodawania
	ADD XL, r17
	ADC XH, r18

; odejmujê 1 od pary YL, YH, j.w. - sk³adnik dodawania
	ADD YL, r17
	ADC YH, r18

; odejmujê 1 od pary ZL, XHm j.w. - iterator
	SUBI ZL, 1
	SBCI ZH, 0

; powrót na pocz¹tek pêtli
	RJMP loopBegin


; w tym momencie wynik powinien byæ w pamiêci o adresie X

endOfLoop:
; przywracamy wszystkie uzywane rejestry wczesniej zapisane na stos
	POP r1
	POP r0
	POP r18
	POP r17
	POP r16
; powrót z funkcji
	RET