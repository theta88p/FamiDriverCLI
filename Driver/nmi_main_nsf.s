
	.export		NMI_main
	.import		drv_main
	
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

;	jsr	drv_main

;---------------------------------------
;register pop
;---------------------------------------
exit:
	pla
	tax
	pla
	tay
	pla

	rti
.endproc
