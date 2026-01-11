	.export		exit

	.import		_main
	.import		NMI_main
	.import		IRQ_main

	.import		ppudrv_init
	.import		Bank_Change_Prg
	
	; Linker generated symbols
	.import		__STACK_START__,	__STACK_SIZE__

	.include	"drv.inc"


; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment	"STARTUP"

start:

	sei
	cld

	DISP_OFF

.ifdef VRC6
	lda #$00
	sta $8000  ; CPU $8000-$BFFF = PRG banks 0 and 1
	sta $9002  ; Silence all channels
	sta $a002
	sta $b002
	lda #$20
	sta $b003  ; Mirroring: Vertical
	lda #$02
	sta $c000  ; CPU $C000-$DFFF = PRG bank 2
	lda #$00
	sta $d000  ; CHR $0000-$0FFF = identity
	lda #$01
	sta $d001
	lda #$02
	sta $d002
	lda #$03
	sta $d003
	lda #$00
	sta $f001  ; disable IRQ
.endif

	lda #%10000000
	sta $5114
	lda #%10000001
	sta $5115

;===============================
;	メモリ初期化
;===============================
Clear_Memory:

	lda	#0
	ldx	#<(__STACK_START__ + __STACK_SIZE__ - 1)
	txs
	tax
@L0:
	sta	$0000,x
;	sta	$0100,x
;	sta	$0200,x
;	sta	$0300,x
;	sta	$0400,x
;	sta	$0500,x
;	sta	$0600,x
;	sta	$0700,x

	inx
	bne	@L0

;===============================
;	サウンド初期化
;===============================
;Sound_Init:

	lda	#$40
	sta	$4017

;===============================
;	画面初期化
;===============================
;Disp_Init:

	; Call initialize PPU
	jsr	_ppu_init

;===============================
;	メインルーチン呼出し
;===============================
; Push arguments and call main()

	jsr	_main

; Call module destructors. This is also the _exit entry.

exit:

; Reset the NES

	jmp	start

; ------------------------------------------------------------------------
; Init PPU
; ------------------------------------------------------------------------
.proc	_ppu_init

	;---------------
	; PPU Control
	lda	#%10101000		;V-Blank NMI: enable
	sta	$2000

	lda	#%00011110
	sta	$2001

	;---------------
	; Wait for vblank
@wait:	lda	$2002
	bpl	@wait

	;---------------
	; reset scrolling
	lda	#0
	sta	$2005
	sta	$2005

	;---------------
	; Make all sprites invisible
	lda	#$00
	ldy	#$f0
	sta	$2003
	ldx	#$40
@loop:	sty	$2007
	sta	$2007
	sta	$2007
	sty	$2007
	dex
	bne	@loop

	rts

.endproc

; ------------------------------------------------------------------------
; hardware vectors
; ------------------------------------------------------------------------

.segment "VECTORS"

	.word	NMI_main	; $fffa vblank nmi
	.word	start		; $fffc reset
	.word	IRQ_main	; $fffe irq / brk
