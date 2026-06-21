	.export		exit

	.import		_main
	.import		_init
	.import		_play

	; Linker generated symbols
	.import		__STACK_START__,	__STACK_SIZE__

	.include	"drv.inc"


; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment	"STARTUP"
.byte		"DRFMNSF"
.addr		_init, _play

start:

	sei
	cld

	DISP_OFF


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
;	メインルーチン呼出し
;===============================
; Push arguments and call main()

	lda	#%00101000		;V-Blank NMI: disabled until driver init completes
	sta	$2000

	jsr	_main

; Call module destructors. This is also the _exit entry.

exit:

; Reset the NES

	jmp	start
