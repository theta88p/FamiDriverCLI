.exportzp	MainExecFrag
.export		_main
.export		_init
.export		_play

.importzp	CpuCtrL
.importzp	CpuCtrH
.importzp	CpuFrameL
.importzp	CpuFrameH
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
MainExecFrame:	.res	1

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

		lda	#%10000000
		sta	$2000

		jmp @count
	@loop:
		tax
		sec
		sbc MainExecFrame
		stx MainExecFrame
		cmp #1
		beq @driver
		jsr drop_inc
		lda #0
		sta CpuCtrL
		sta CpuCtrH
		lda DrvFrags
		and #DRV_IS_FREE
		bne @run
		jmp @count
	@driver:
		lda DrvFrags
		and #DRV_IS_FREE
		bne @exec
		jsr drop_inc
		lda #0
		sta CpuCtrL
		sta CpuCtrH
		jmp @count
	@exec:
		lda CpuCtrL
		sta CpuFrameL
		lda CpuCtrH
		sta CpuFrameH
	@run:
		lda #0
		sta CpuCtrL
		sta CpuCtrH
		jsr drv_main
		jsr dsp_main
		lda MainExecFrag
		cmp MainExecFrame
		beq @count
		sta MainExecFrame
		jsr drop_inc
		lda #0
		sta CpuCtrL
		sta CpuCtrH
	@count:
		lda MainExecFrag
		cmp MainExecFrame
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
	sta MainExecFrame
	sta CpuCtrL
	sta CpuCtrH
	sta CpuFrameL
	sta CpuFrameH
	
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
