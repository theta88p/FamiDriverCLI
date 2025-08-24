	.export		exit

	.import		_main

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

	jsr	_main

; Call module destructors. This is also the _exit entry.

exit:

; Reset the NES

	jmp	start
