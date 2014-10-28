/*
 * AddTwo.asm
 *
 *  Created: 2014-10-26 02:17:09
 *   Author: Patrycja Cielecka
 */ 

.include "m32def.inc"

.EQU size = 24 ; Rozmiar tablicy

.DSEG
	tab1: .BYTE size ; Rezerwujemy size bajtów na tablicê, rozpoczynaj¹c od 0x0060
	tab2: .BYTE size
	
.CSEG
	LDI R16, size ; Rejestr u¿ywany do przemieszczania siê po tablicach

	; Zapisujemy w X i Y adresy pocz¹tku tablic (Jeœli size = 16, to mamy adresy 0x0060 i 0x0070)
	LDI XL,  low (tab1)
	LDI XH,  high(tab1)

	LDI YL,  low (tab2)
	LDI YH,  high(tab2)

	; Przesuwamy siê po obu tablicach zmniejszaj¹c R16 i zwiêkszaj¹c o 1 adresy w X i Y
	; Rejestry R17 i R18 to rejestry pomocnicze, zapisujemy w nich kolejne bajty i dodajemy je
	LD R17, X
	LD R18, Y+ 
	ADD R17, R18 ; U¿ywamy ADD, poniewa¿ dodajemy mniej znacz¹ce bajty
	ST X+, R17 ; Zapisujemy wynik dodawania z R17 w tab1
	DEC R16

	cycle:
		LD R17, X
		LD R18, Y+ 
		ADC R17, R18 ; Dla ka¿dych kolejnych bajtów u¿ywamy ADC, ¿eby nadmiar przenosiæ dalej
		ST X+, R17
		DEC R16
	BRNE cycle