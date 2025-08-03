
.export		NMI_main

.importzp	sync
.import		DrvFrags
.import		drv_main
.import		dsp_main
.import		dsp_write
.import		__c
.import		__cc
.import		__s
.import		__ss
.import		__m
.import		__mm

.include	"drv.inc"

.segment	"LOWCODE"

.proc	NMI_main

;---------------------------------------
;register push
;---------------------------------------
	pha
	tya
	pha
	txa
	pha

;---------------------------------------
; Call sound driver main routine
;---------------------------------------

	lda DrvFrags
	and #DRV_INIT | DRV_IS_FREE
	beq Count

	lda #0
	sta sync
	
	jsr dsp_write
	;スクロール位置
	lda	#0
	sta	$2005
	sta	$2005
	;スプライト転送
	lda #$07
	sta $4014
	
	jsr dsp_main
	jsr drv_main
	
	lda #1
	sta sync
	
;---------------------------------------
; Count-up
;---------------------------------------
Count:
@cc:
	inc	__c
	ldx	__c
	cpx	#$3a
	bne	@exit
	lda	#$30
	sta	__c
	inc __cc
	ldx __cc
	cpx #$36
	bne @exit
	sta __cc
	inc __s
@ss:
	ldx	__s
	cpx	#$3a
	bne	@exit
	lda	#$30
	sta	__s
	inc __ss
	ldx __ss
	cpx #$36
	bne @exit
	sta __ss
	inc __m
@mm:
	ldx	__m
	cpx	#$3a
	bne	@exit
	lda #$30
	sta __m
	inc __mm
@exit:
end:

;---------------------------------------
;register pop
;---------------------------------------
	pla
	tax
	pla
	tay
	pla

	rti
.endproc
