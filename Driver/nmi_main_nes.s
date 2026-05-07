
.export		NMI_main

.importzp	MainExecFrag
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
	
	jsr dsp_write
	;スクロール位置
	lda	#0
	sta	$2005
	lda	#8
	sta	$2005
	;スプライト転送
	lda #$07
	sta $4014
	
	lda #1
	sta MainExecFrag

;---------------------------------------
; Count-up
;---------------------------------------
Count:
@cc:
	inc	__c
	ldx	__c
	cpx	#$3a
	bne	exit
	lda	#$30
	sta	__c
	inc __cc
	ldx __cc
	cpx #$36
	bne exit
	sta __cc
	inc __s
@ss:
	ldx	__s
	cpx	#$3a
	bne	exit
	lda	#$30
	sta	__s
	inc __ss
	ldx __ss
	cpx #$36
	bne exit
	sta __ss
	inc __m
@mm:
	ldx	__m
	cpx	#$3a
	bne	exit
	lda #$30
	sta __m
	inc __mm
exit:

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
