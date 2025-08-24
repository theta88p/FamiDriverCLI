.export		_main
.export		_init
.export		_play

.importzp	sync
.import		drv_init
.import		drv_sndreq
.import		drv_main
.import		DPCMinfo
.import		BGM0
.import		dsp_init

.include	"drv.inc"


; ------------------------------------------------------------------------
; play
; ------------------------------------------------------------------------

.rodata

;Address of BGM Sequence
bgm_00:		.addr	BGM0

; ------------------------------------------------------------------------
; main
; ------------------------------------------------------------------------
.code

.byte	"DRFM  "

.proc	_main
		jsr _init

	Loop:
		jmp	Loop
.endproc

.proc _init
	pha
	jsr dsp_init
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
.endproc