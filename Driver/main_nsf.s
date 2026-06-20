
	.export		_main
	.export		_init
	.export		_play
	
	.import		drv_init
	.import		drv_sndreq
	.import		drv_main
	.import		_DPCMinfo
	.import		_BGM0
	
	.include	"drv.inc"


; ------------------------------------------------------------------------
; play
; ------------------------------------------------------------------------

.rodata

;Address of BGM Sequence
bgm_00:		.addr	_BGM0

; ------------------------------------------------------------------------
; main
; ------------------------------------------------------------------------
.segment	"MAIN"

.proc	_main

	jsr _init
	
	lda	#%10101000
	sta	$2000

;Wait_Next_Flame:
;	jmp	Wait_Next_Flame
	rts

.endproc

.proc _init
	pha
	jsr drv_init
	
	pla
	tay
	lda	bgm_00
	ldx	bgm_00 + 1
	jsr	drv_sndreq
	rts
.endproc

.proc	_play
	jmp	drv_main
	rts
.endproc