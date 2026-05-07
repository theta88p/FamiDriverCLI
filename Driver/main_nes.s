.exportzp	MainExecFrag
.export		_main
.export		_init
.export		_play

.importzp	CpuCtrL
.importzp	CpuCtrH
.import		DrvFrags
.import		drv_init
.import		drv_sndreq
.import		drv_main
.import		dsp_main
.import		DPCMinfo
.import		BGM0
.import		dsp_init
.import		drop_inc

.include	"drv.inc"

.zeropage

MainExecFrag:	.res	1

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
		jmp @count
	@loop:
		lda DrvFrags
		and #DRV_IS_FREE
		bne @exec
		jsr drop_inc
		jmp @count
	@exec:
		jsr drv_main
		jsr dsp_main
		lda #0
		sta MainExecFrag
	@count:
		lda MainExecFrag
		bne @loop
		inc CpuCtrL
		bne @count
		inc CpuCtrH
		jmp	@count
.endproc

.proc _init
	pha
	jsr dsp_init
	jsr drv_init
	lda #0
	sta MainExecFrag
	
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