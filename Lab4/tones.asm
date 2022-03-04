$NOLIST
$MODLP51
$LIST

CLK equ 22118400 ; Microcontroller system crystal frequency in Hz
TIMER0_RATE equ 4096
TIMER0_RATEA EQU 1580*2
TIMER0_RATEE EQU 2350*2
TIMER0_RATEFS EQU 2600*2
TIMER0_RATED EQU 2100*2
TIMER0_RATECS EQU 2000*2
TIMER0_RATEB EQU 1780*2

TIMER0_RELOAD1 equ ((65536-(CLK/1000)))
TIMER0_RELOAD2 equ ((65536-(CLK/2000)))
TIMER0_RELOADA EQU ((65536-(CLK/TIMER0_RATEA)))
TIMER0_RELOADE EQU ((65536-(CLK/TIMER0_RATEE)))
TIMER0_RELOADFS EQU ((65536-(CLK/TIMER0_RATEFS)))
TIMER0_RELOADD EQU ((65536-(CLK/TIMER0_RATED)))
TIMER0_RELOADCS EQU ((65536-(CLK/TIMER0_RATECS)))
TIMER0_RELOADB EQU ((65536-(CLK/TIMER0_RATEB)))

SOUND_OUT EQU P1.1

org 0000H
   ljmp Startup

; Timer/Counter 0 overflow interrupt vector
org 0x000B
	ljmp Timer0_ISR

;---------------------------------;
; Routine to initialize the ISR   ;
; for timer 0                     ;
;---------------------------------;
InitTimer0:
	mov a, TMOD
	anl a, #0xf0 ; 11110000 Clear the bits for timer 0
	orl a, #0x01 ; 00000001 Configure timer 0 as 16-timer
	mov TMOD, a
	mov TH0, #high(TIMER0_RELOAD1)
	mov TL0, #low(TIMER0_RELOAD1)
	; Set autoreload value
	mov RH0, #high(TIMER0_RELOAD1)
	mov RL0, #low(TIMER0_RELOAD1)
	; Enable the timer and interrupts
    setb ET0  ; Enable timer 0 interrupt
    setb TR0  ; Start timer 0
	ret

;---------------------------------;
; ISR for timer 0.  Set to execute;
; every 1/4096Hz to generate a    ;
; 2048 Hz square wave at pin P1.1 ;
;---------------------------------;
Timer0_ISR:
	cpl SOUND_OUT ; Connect speaker to P1.1!
	reti

; When using a 22.1184MHz crystal in fast mode
; one cycle takes 1.0/22.1184MHz = 45.21123 ns
WaitHalfSec:
    mov R2, #89
L3: mov R1, #250
L2: mov R0, #166
L1: djnz R0, L1 ; 3 cycles->3*45.21123ns*166=22.51519us
    djnz R1, L2 ; 22.51519us*250=5.629ms
    djnz R2, L3 ; 5.629ms*89=0.5s (approximately)
    ret

;---------------------------------;
; Hardware and variable           ;
; initialization                  ;
;---------------------------------;
Startup:
    ; Initialize the hardware:
    mov SP, #7FH
    lcall InitTimer0
    setb EA
    
;---------------------------------;
; Main program loop               ;
;---------------------------------;  
forever:
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADA)
	mov RL0, #low(TIMER0_RELOADA)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADA)
	mov RL0, #low(TIMER0_RELOADA)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADE)
	mov RL0, #low(TIMER0_RELOADE)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADE)
	mov RL0, #low(TIMER0_RELOADE)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADFS)
	mov RL0, #low(TIMER0_RELOADFS)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADFS)
	mov RL0, #low(TIMER0_RELOADFS)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADE)
	mov RL0, #low(TIMER0_RELOADE)
	setb TR0
	lcall WaitHalfSec
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADD)
	mov RL0, #low(TIMER0_RELOADD)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADD)
	mov RL0, #low(TIMER0_RELOADD)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADCS)
	mov RL0, #low(TIMER0_RELOADCS)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADCS)
	mov RL0, #low(TIMER0_RELOADCS)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADB)
	mov RL0, #low(TIMER0_RELOADB)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADB)
	mov RL0, #low(TIMER0_RELOADB)
	setb TR0
	
	lcall WaitHalfSec
	clr TR0
	
	lcall WaitHalfSec
	clr TR0
	mov RH0, #high(TIMER0_RELOADA)
	mov RL0, #low(TIMER0_RELOADA)
	setb TR0
	lcall WaitHalfSec
	
	lcall WaitHalfSec
	clr TR0
	
    ljmp forever ; Repeat! 

end