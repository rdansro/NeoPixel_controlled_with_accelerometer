.nolist
.include "../m1284def.inc"
.list

.section .data
	.org 0x00
result: .byte
.section .text
.global write_rgb
	.org 0x00
	rjmp write_rgb


write_rgb:
	LDI r30, 0b00000001
    OUT DDRB, r30      ; Set port as output
	LDI R18, 0b10000000
	MOV R22, R22 ;moves Green value to correct register
	MOV R21, R24 ;moves Red value to correct register
	MOV R23, R20 ;moves Blue value to correct register

	;goes through each register bit by bit
	;calls either zero or one depending on the bit
wg_start:
	MOV R24, R22
	AND R24, R18
	BRNE wg_one
	CALL zero
	RJMP wg_loop
wg_one:
	CALL one
	RJMP wg_loop
wg_loop:
	LSR R18
	BRNE wg_start

	LDI R18, 0b10000000
wr_start:
	MOV R24, R21
	AND R24, R18
	BRNE wr_one
	CALL zero
	RJMP wr_loop
wr_one:
	CALL one
	RJMP wr_loop
wr_loop:
	LSR R18
	BRNE wr_start

	LDI R18, 0b10000000
wb_start:
	MOV R24, R23
	AND R24, R18
	BRNE wb_one
	CALL zero
	RJMP wb_loop
wb_one:
	CALL one
	RJMP wb_loop
wb_loop:
	LSR R18
	BRNE wb_start
	ret

	;writes a zero to the NeoPixel
zero:
	ldi r20, 0b00000001
	out PORTB, r20
	nop
	nop
	nop
	nop
	ldi r20, 0b00000000
	out PORTB, r20
	nop
	nop
	nop
	nop
	nop
	nop
	ret

	;writes a one to the NeoPixel
one:
	ldi r20, 0b00000001
	out PORTB, r20
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	ldi r20, 0b00000000
	out PORTB, r20
	ret

	;various delays
delay_more:
	ldi r26, 15
delay_m:
	call delay_small
	dec r26
	brne delay_m
	ret

delay_small:
	ldi r27, 255
delay:
	nop
	nop
	nop
	nop
	dec r27
	brne delay
    ret

rgb:
	LDI R21, 0xFF ;Red
	LDI R22, 0x1  ;Green
	LDI R23, 1    ;Blue
	CALL write_rgb

here: jmp here
