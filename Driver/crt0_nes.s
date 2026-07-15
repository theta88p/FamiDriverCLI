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
	lda #$1e
	sta $c000  ; CPU $C000-$DFFF = second-last PRG bank
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

.ifdef VRC7
	lda #$00
	sta $8000  ; CPU $8000-$9FFF = PRG bank 0 until the first song selects bank 2
	lda #$00
	sta $8010  ; CPU $A000-$BFFF = PRG bank 0 (DSP display code)
	lda #$1e
	sta $9000  ; CPU $C000-$DFFF = DPCM bank 30
	lda #$00
	sta $a000  ; CHR $0000-$03FF = bank 0
	lda #$01
	sta $a010
	lda #$02
	sta $b000
	lda #$03
	sta $b010
	lda #$04
	sta $c000
	lda #$05
	sta $c010
	lda #$06
	sta $d000
	lda #$07
	sta $d010
	lda #$00
	sta $e000  ; Vertical mirroring
	sta $f000  ; Disable IRQ
.endif

.ifdef MMC5
	lda #$03
	sta $5100  ; 8K PRG bank mode
	lda #%10000000
	sta $5114
	lda #%10000001
	sta $5115
	lda #$fe       ; Map the second-last 8K PRG bank at $C000-$DFFF
	sta $5116
	lda #$ff       ; Map the last 8K PRG bank at $E000-$FFFF
	sta $5117
.endif

.ifdef MMC3
	lda #$06
	sta $8000
	lda #$00
	sta $8001  ; CPU $8000-$9FFF = PRG bank 0
	lda #$07
	sta $8000
	lda #$01
	sta $8001  ; CPU $A000-$BFFF = PRG bank 1

	lda #$00
	sta $8000
	sta $8001  ; CHR $0000-$07FF = banks 0-1
	lda #$01
	sta $8000
	lda #$02
	sta $8001  ; CHR $0800-$0FFF = banks 2-3
	lda #$02
	sta $8000
	lda #$04
	sta $8001
	lda #$03
	sta $8000
	lda #$05
	sta $8001
	lda #$04
	sta $8000
	lda #$06
	sta $8001
	lda #$05
	sta $8000
	lda #$07
	sta $8001
	lda #$00
	sta $a000  ; Vertical mirroring
.endif

.ifdef SS5B
	ldx #$00
@ss5b_chr:
	stx $8000  ; Select CHR bank register 0-7
	stx $a000  ; Map the matching 1K CHR bank
	inx
	cpx #$08
	bne @ss5b_chr

	lda #$09
	sta $8000  ; CPU $8000-$9FFF = PRG bank 0
	lda #$00
	sta $a000
	lda #$0a
	sta $8000  ; CPU $A000-$BFFF = PRG bank 1
	lda #$01
	sta $a000
	lda #$0b
	sta $8000  ; Select the $C000-$DFFF PRG bank register
	lda #$fe    ; Map the second-last 8K PRG bank
	sta $a000
	lda #$0c
	sta $8000  ; Mirroring: vertical
	lda #$00
	sta $a000
.endif

.ifdef N163
	lda #$00
	sta $8000
	lda #$01
	sta $8800
	lda #$02
	sta $9000
	lda #$03
	sta $9800
	lda #$04
	sta $a000
	lda #$05
	sta $a800
	lda #$06
	sta $b000
	lda #$07
	sta $b800
	lda #$e0
	sta $c000
	sta $d000
	lda #$e1
	sta $c800
	sta $d800
	lda #$00
	sta $e000
	lda #$01
	sta $e800
	lda #$fe
	sta $f000
.endif

.ifdef FDS
lda #$22	;ミラーリング設定
sta $4025
.endif

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
	sta	$0200,x
	sta	$0300,x
	sta	$0400,x
	sta	$0500,x
	cpx	#$40
	bcs	:+
	sta	$0600,x
:

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
	lda	#%00101000		;V-Blank NMI: disabled until driver init completes
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
@loop:	sty	$2004
	sta	$2004
	sta	$2004
	sty	$2004
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
