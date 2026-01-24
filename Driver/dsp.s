;FamiDriverCLI FCDSP v0.3.7

.importzp	Frags
.import		IsProc
.import		Device
.import		NoteN
.import		Volume
.import		Tone
.import		Freq_L
.import		Freq_H
.import		PrevFreq_L
.import		PrevFreq_H
.import		palette
.import		DrvFrags

.exportzp	CpuCtrL
.exportzp	CpuCtrH
.export 	dsp_init
.export 	dsp_main
.export 	dsp_write
.export		drop_inc
.export		__c
.export		__cc
.export		__s
.export		__ss
.export		__m
.export		__mm

.include 	"drv.inc"


.zeropage

DspWork:	.res	4
CpuCtrL:	.res	1
CpuCtrH:	.res	1

.bss

__c:		.res	1
__cc:		.res	1
__s:		.res	1
__ss:		.res	1
__m:		.res	1
__mm:		.res	1

POctHead:		.res	3
POctave:		.res	3
PNote:			.res	3
PSharp:			.res	3
PToneL:			.res	3
PToneR:			.res	3
PTone4:			.res	1
PVolume:		.res	2
ExpVolume:		.res	1

PANote:			.res	2
PAVolume:		.res	4

DropCtr:		.res	1
Drop1:			.res	1
Drop2:			.res	1
CpuL1:			.res	1
CpuL2:			.res	1
CpuH1:			.res	1
CpuH2:			.res	1
CpuBar:			.res	1

DMA = $0700

CH1 = 0
CH2 = CH1 + 20
CH3 = CH2 + 20
CH4 = CH3 + 4
CH5 = CH4 + 20
EXP1 = CH5 + 4
CPU = EXP1 + 28

CH1KEY		=	DMA + 0
CH1VOL1		=	DMA + 4
CH1VOL2		=	DMA + 8
CH1VOL3		=	DMA + 12
CH1VOL4		=	DMA + 16

CH2KEY		=	DMA + CH2 + 0
CH2VOL1		=	DMA + CH2 + 4
CH2VOL2		=	DMA + CH2 + 8
CH2VOL3		=	DMA + CH2 + 12
CH2VOL4		=	DMA + CH2 + 16

CH3KEY		=	DMA + CH3 + 0

CH4KEY		=	DMA + CH4 + 0
CH4VOL1		=	DMA + CH4 + 4
CH4VOL2		=	DMA + CH4 + 8
CH4VOL3		=	DMA + CH4 + 12
CH4VOL4		=	DMA + CH4 + 16

CH5KEY		=	DMA + CH5 + 0

EXP1KEY = DMA + EXP1 + 0
EXP2KEY = DMA + EXP1 + 4
EXP3KEY = DMA + EXP1 + 8
EXPVOL1 = DMA + EXP1 + 12
EXPVOL2 = DMA + EXP1 + 16
EXPVOL3 = DMA + EXP1 + 20
EXPVOL4 = DMA + EXP1 + 24

CPUREM = DMA + CPU

YPOS1 = $27
YPOS2 = $3f
YPOS3 = $57
YPOS4 = $87
YPOS5 = $9f
YPOS_EXP = $6f
YPOS_CPU = $bf

.code

.proc dsp_main
	start:
	;----------------ch1----------------
	ch01:
		lda #DEV_2A03_SQR1
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta CH1KEY + 0
		lda #$b8
		sta CH1VOL1 + 3
		lda #$c0
		sta CH1VOL2 + 3
		lda #$c8
		sta CH1VOL3 + 3
		lda #$d0
		sta CH1VOL4 + 3
		lda #YPOS1
		sta CH1VOL1 + 0
		sta CH1VOL2 + 0
		sta CH1VOL3 + 0
		sta CH1VOL4 + 0
		lda #$02
		sta POctHead + 0
		sta POctave + 0
		sta PNote + 0
		sta PSharp + 0
		jmp ch02
	;発音中の場合
	@keyon:
		lda #YPOS1 + 8			;ハイライト鍵盤を表示
		sta CH1KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta CH1KEY + 3			;ハイライト鍵盤の位置
	;ノートナンバー表示
	;@notenum:
		lda DspWork + 2
		clc
		adc #$30
		sta POctave + 0				;オクターブ番号
		lda charmap, y
		sta PNote + 0				;音名
		lda #$42
		sta POctHead + 0
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda #$02
		sta PSharp + 0				;シャープ記号
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta CH1KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta CH1KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$45
		sta PSharp + 0				;シャープ記号
		lda #$5e
		sta CH1KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta CH1KEY + 2			;パレット番号
	;音量バー
	@volume:
		lda Volume, x
		cmp PAVolume + 0
		beq @duty
		asl
		clc
		adc #$b8
		sta CH1VOL1 + 3		;音量バー隠しのスプライト位置1
		clc
		adc #8
		sta CH1VOL2 + 3		;音量バー隠しのスプライト位置2
		clc
		adc #8
		sta CH1VOL3 + 3		;音量バー隠しのスプライト位置3
		clc
		adc #8
		sta CH1VOL4 + 3		;音量バー隠しのスプライト位置4
	;Duty
	@duty:
		lda Tone, x
		and #$0f
		cmp #3
		beq @rev
		clc
		adc #$2a
		sta PToneL + 0
		lda #$2e
		sta PToneR + 0
		jmp ch02
	@rev:
		clc
		adc #$2a
		sta PToneR + 0
		lda #$2f
		sta PToneL + 0

	;----------------ch2----------------
	ch02:
		lda #DEV_2A03_SQR2
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta CH2KEY + 0
		lda #$b8
		sta CH2VOL1 + 3
		lda #$c0
		sta CH2VOL2 + 3
		lda #$c8
		sta CH2VOL3 + 3
		lda #$d0
		sta CH2VOL4 + 3
		lda #$02
		sta POctHead + 1
		sta POctave + 1
		sta PNote + 1
		sta PSharp + 1
		jmp ch03
	;発音中の場合
	@keyon:
		lda #YPOS2 + 8			;ハイライト鍵盤を表示
		sta CH2KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta CH2KEY + 3			;ハイライト鍵盤の位置
	;ノートナンバー表示
	;@notenum:
		lda DspWork + 2
		clc
		adc #$30
		sta POctave + 1				;オクターブ番号
		lda charmap, y
		sta PNote + 1				;音名
		lda #$42
		sta POctHead + 1
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda #$02
		sta PSharp + 1			;シャープ記号
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta CH2KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta CH2KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$45
		sta PSharp + 1				;シャープ記号
		lda #$5e
		sta CH2KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta CH2KEY + 2			;パレット番号
	;音量バー
	@volume:
		lda Volume, x
		cmp PAVolume + 1
		beq @duty
		asl
		clc
		adc #$b8
		sta CH2VOL1 + 3		;音量バー隠しのスプライト位置1
		clc
		adc #8
		sta CH2VOL2 + 3		;音量バー隠しのスプライト位置2
		clc
		adc #8
		sta CH2VOL3 + 3		;音量バー隠しのスプライト位置3
		clc
		adc #8
		sta CH2VOL4 + 3		;音量バー隠しのスプライト位置4
	;Duty
	@duty:
		lda Tone, x
		and #$0f
		cmp #3
		beq @rev
		clc
		adc #$2a
		sta PToneL + 1
		lda #$2e
		sta PToneR + 1
		jmp ch03
	@rev:
		clc
		adc #$2a
		sta PToneR + 1
		lda #$2f
		sta PToneL + 1

	;----------------ch3----------------
	ch03:
		lda #DEV_2A03_TRI
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta CH3KEY + 0
		lda PVolume + 0
		beq :+
		dec PVolume + 0
	:	lda #$02
		sta POctHead + 2
		sta POctave + 2
		sta PNote + 2
		sta PSharp + 2
		jmp ch04
	;発音中の場合
	@keyon:
		lda #YPOS3 + 8			;ハイライト鍵盤を表示
		sta CH3KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
		dey						;三角波は1オクターブ低く表示
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta CH3KEY + 3			;ハイライト鍵盤の位置
	;ノートナンバー表示
	;@notenum:
		lda DspWork + 2
		clc
		adc #$2f				;三角波は1オクターブ低く表示
		sta POctave + 2			;オクターブ番号
		lda charmap, y
		sta PNote + 2			;音名
		lda #$42
		sta POctHead + 2
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda #$02
		sta PSharp + 2			;シャープ記号
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta CH3KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta CH3KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$45
		sta PSharp + 2				;シャープ記号
		lda #$5e
		sta CH3KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta CH3KEY + 2			;パレット番号
	;音量バー
	@volume:
		lda #4
		sta PVolume + 0

	;----------------ch4----------------
	ch04:
		lda #DEV_2A03_NOISE
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta CH4KEY + 0
		lda #$b8
		sta CH4VOL1 + 3
		lda #$c0
		sta CH4VOL2 + 3
		lda #$c8
		sta CH4VOL3 + 3
		lda #$d0
		sta CH4VOL4 + 3
		jmp ch05
	;発音中の場合
	@keyon:
		lda #YPOS4 + 8
		sta CH4KEY + 0			;ハイライト鍵盤を表示
		lda PANote + 0
		cmp NoteN, x
		beq @volume
	;鍵盤の位置計算
	@pos:
		lda NoteN, x
		sta PANote + 0
		lda #$0f
		sec
		sbc NoteN, x
		sta DspWork
		lda #0
		ldy #8
	@L:
		clc
		adc DspWork
		dey
		bne @L
		adc #$58
		sta CH4KEY + 3			;ハイライト鍵盤の位置
	;音量バー
	@volume:
		lda Volume, x
		cmp PAVolume + 3
		beq @tone
		asl
		clc
		adc #$b8
		sta CH4VOL1 + 3		;音量バー隠しのスプライト位置1
		clc
		adc #8
		sta CH4VOL2 + 3		;音量バー隠しのスプライト位置2
		clc
		adc #8
		sta CH4VOL3 + 3		;音量バー隠しのスプライト位置3
		clc
		adc #8
		sta CH4VOL4 + 3		;音量バー隠しのスプライト位置4
	;音色
	@tone:
		lda Tone, x
		and #$0f
		bne @p
		lda #$41
		sta PTone4
		jmp ch05
	@p:
		lda #$43
		sta PTone4
	
	;----------------ch5----------------
	ch05:
		lda #DEV_2A03_DPCM
		sta DspWork
		jsr gettrack
		lda $4015
		and #%00010000					;DPCM再生bitを直接読む
		bne @keyon
		lda PVolume + 1
		beq :+
		dec PVolume + 1
	:	lda #$ff
		sta CH5KEY + 0
		jmp cpu
	;発音中の場合
	@keyon:
		lda #YPOS5 + 8
		sta CH5KEY + 0				;ハイライト鍵盤を表示
		lda PANote + 1
		cmp NoteN, x
		beq @volume
	;鍵盤の位置計算
	@pos:
		lda #$0f
		sec
		sbc NoteN, x
		sta DspWork
		lda #0
		ldy #8
	@L:
		clc
		adc DspWork
		dey
		bne @L
		adc #$58
		sta CH5KEY + 3				;ハイライト鍵盤の位置
	@volume:
		lda #4
		sta PVolume + 1

	cpu:
		;ドロップカウンタ表示
		lda DropCtr
		lsr
		lsr
		lsr
		lsr
		clc
		adc #$30
		sta Drop1
		lda #%00001111
		and DropCtr
		clc
		adc #$30
		sta Drop2
		;残りCPU時間
		;上位バイト
		lda CpuCtrH
		lsr
		lsr
		lsr
		lsr
		clc
		adc #$30
		sta CpuH2
		lda #%00001111
		and CpuCtrH
		clc
		adc #$30
		sta CpuH1
		;下位バイト
		lda CpuCtrL
		lsr
		lsr
		lsr
		lsr
		clc
		adc #$30
		sta CpuL2
		lda #%00001111
		and CpuCtrL
		clc
		adc #$30
		sta CpuL1
		;下位バイトバー位置
		lda CpuCtrH
		sta CpuBar	;ここで保存しないとズレる
		asl
		asl
		asl
		sta DspWork
		lda CpuCtrL
		lsr
		lsr
		lsr
		lsr
		lsr
		sta DspWork + 1
		lda #8
		sec
		sbc DspWork + 1
		clc
		adc DspWork
		clc
		adc #88
		sta CPUREM + 3
		lda #YPOS_CPU
		sta CPUREM + 0
		;カウンタ初期化
		lda #0
		sta CpuCtrL
		lda #0
		sta CpuCtrH

	exp:
.ifdef VRC6
	;---------------VRC6 ch1---------------
		lda #DEV_VRC6_SQR1
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta EXP1KEY + 0
		jmp vrc6_02
	;発音中の場合
	@keyon:
		lda #YPOS_EXP + 8		;ハイライト鍵盤を表示
		sta EXP1KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta EXP1KEY + 3			;ハイライト鍵盤の位置
	;半音計算
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta EXP1KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta EXP1KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$5e
		sta EXP1KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta EXP1KEY + 2			;パレット番号
	@volume:
		lda Volume, x
		sta ExpVolume
	;--------------VRC6 ch2--------------
	vrc6_02:
		lda #DEV_VRC6_SQR2
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta EXP2KEY + 0
		jmp vrc6_saw
	;発音中の場合
	@keyon:
		lda #YPOS_EXP + 8		;ハイライト鍵盤を表示
		sta EXP2KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta EXP2KEY + 3			;ハイライト鍵盤の位置
	;半音計算
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta EXP2KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta EXP2KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$5e
		sta EXP2KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta EXP2KEY + 2			;パレット番号
	@volume:
		lda Volume, x
		clc
		adc ExpVolume
		sta ExpVolume
		;--------------VRC6 saw--------------
	vrc6_saw:
		lda #DEV_VRC6_SAW
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta EXP3KEY + 0
		jmp @volume
	;発音中の場合
	@keyon:
		lda #YPOS_EXP + 8		;ハイライト鍵盤を表示
		sta EXP3KEY + 0
		jsr freq2note_saw
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta EXP3KEY + 3			;ハイライト鍵盤の位置
	;半音計算
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta EXP3KEY + 1			;ハイライト鍵盤の形
		lda #%00000001
		sta EXP3KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$5e
		sta EXP3KEY + 1			;ハイライト鍵盤の形
		lda #%00000011
		sta EXP3KEY + 2			;パレット番号
		;--------------VRC6 Volume--------------
	@volume:
		lda Volume, x
		lsr
		clc
		adc ExpVolume
		lsr
		clc
		adc #$b8
		sta EXPVOL1 + 3		;音量バー隠しのスプライト位置1
		clc
		adc #8
		sta EXPVOL2 + 3		;音量バー隠しのスプライト位置2
		clc
		adc #8
		sta EXPVOL3 + 3		;音量バー隠しのスプライト位置3
		clc
		adc #8
		sta EXPVOL4 + 3		;音量バー隠しのスプライト位置4
		lda #0
		sta ExpVolume
.endif

.ifdef MMC5
	;---------------MMC5 ch1---------------
		lda #DEV_MMC5_SQR1
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta EXP1KEY + 0
		jmp mmc5_02
	;発音中の場合
	@keyon:
		lda #YPOS_EXP + 8		;ハイライト鍵盤を表示
		sta EXP1KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta EXP1KEY + 3			;ハイライト鍵盤の位置
	;半音計算
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta EXP1KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta EXP1KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$5e
		sta EXP1KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta EXP1KEY + 2			;パレット番号
	@volume:
		lda Volume, x
		sta ExpVolume
	;--------------MMC5 ch2--------------
	mmc5_02:
		lda #DEV_MMC5_SQR2
		sta DspWork
		jsr gettrack
		cmp #1
		beq @keyon
	@keyoff:
		lda #$ff
		sta EXP2KEY + 0
		jmp @sum_volume
	;発音中の場合
	@keyon:
		lda #YPOS_EXP + 8		;ハイライト鍵盤を表示
		sta EXP2KEY + 0
		jsr freq2note
	;鍵盤の位置計算
	@pos:
		lda #0
		ldy DspWork + 2
	@L:
		clc
		adc #21
		dey
		bne @L
		adc #$30
		ldy DspWork + 3
		adc posmap, y
		sta EXP2KEY + 3			;ハイライト鍵盤の位置
	;半音計算
	@note:
		lda DspWork + 3
		tay
		lda halftone, y
		bne @half
		lda DspWork + 3
		cmp #4
		beq @sqr
		cmp #11
		beq @sqr
		lda #$5d
		jmp @write
	@sqr:
		lda #$5c
	@write:
		sta EXP2KEY + 1			;ハイライト鍵盤の形
		lda #%00000000
		sta EXP2KEY + 2			;パレット番号
		jmp @volume
	@half:
		lda #$5e
		sta EXP2KEY + 1			;ハイライト鍵盤の形
		lda #%00000010
		sta EXP2KEY + 2			;パレット番号
	@volume:
		lda Volume, x
		clc
		adc ExpVolume
		sta ExpVolume
		;--------------MMC5 Volume--------------
	@sum_volume:
		lda ExpVolume
		clc
		adc #$b8
		sta EXPVOL1 + 3		;音量バー隠しのスプライト位置1
		clc
		adc #8
		sta EXPVOL2 + 3		;音量バー隠しのスプライト位置2
		clc
		adc #8
		sta EXPVOL3 + 3		;音量バー隠しのスプライト位置3
		clc
		adc #8
		sta EXPVOL4 + 3		;音量バー隠しのスプライト位置4
		lda #0
		sta ExpVolume
.endif


	end:
		rts
.endproc

;DspWorkに音源番号を入れて実行すると
;アクティブな音源があるかどうかを調べて
;aに結果、xにトラック番号を入れて返す
.proc gettrack
		ldx #LAST_TRACK
		stx DspWork + 1
		lda #0
		sta DspWork + 2
	@loop:
		lda Device, x
		cmp DspWork
		beq @exec
		bcc end			;目的の番号より小さくなったら打ち切る
		dex
		bmi end
		stx DspWork + 1
		jmp @loop
	@exec:
		lda Frags, x
		and #FRAG_END
		bne @next
		lda Volume, x
		beq @next
		lda #1
		sta DspWork + 2
		stx DspWork + 1
	@next:
		dex
		bmi end
		jmp @loop
	end:
		ldx DspWork + 1
		lda DspWork + 2
		rts
.endproc


.proc dsp_write
		;時間表示
		lda #$20
		sta $2006
		lda #$73
		sta $2006
		lda __mm
		sta $2007
		lda __m
		sta $2007
		lda #$44
		sta $2007
		lda __ss
		sta $2007
		lda __s
		sta $2007
		lda #$44
		sta $2007
		lda __cc
		sta $2007
		lda __c
		sta $2007
		;----------------ch1----------------
		lda #$20
		sta $2006
		lda #$d1
		sta $2006
		lda POctHead + 0
		sta $2007
		lda POctave + 0
		sta $2007
		lda PNote + 0
		sta $2007
		lda PSharp + 0
		sta $2007
		lda #$20
		sta $2006
		lda #$ce
		sta $2006
		lda PToneL + 0
		sta $2007
		lda PToneR + 0
		sta $2007
		;----------------ch2----------------
		lda #$21
		sta $2006
		lda #$31
		sta $2006
		lda POctHead + 1
		sta $2007
		lda POctave + 1
		sta $2007
		lda PNote + 1
		sta $2007
		lda PSharp + 1
		sta $2007
		lda #$21
		sta $2006
		lda #$2e
		sta $2006
		lda PToneL + 1
		sta $2007
		lda PToneR + 1
		sta $2007
		;----------------ch3----------------
		lda #$21
		sta $2006
		lda #$91
		sta $2006
		lda POctHead + 2
		sta $2007
		lda POctave + 2
		sta $2007
		lda PNote + 2
		sta $2007
		lda PSharp + 2
		sta $2007
		lda #$21
		sta $2006
		lda #$97
		sta $2006
		lda #$02
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		lda #$21
		sta $2006
		lda #$97
		sta $2006
		lda #$29
		ldx PVolume + 0
		beq @ch04
	:	sta $2007
		dex
		bne :-
		;----------------ch4----------------
	@ch04:
		lda #$22
		sta $2006
		lda #$4b
		sta $2006
		lda PTone4
		sta $2007
		;----------------ch5----------------
		lda #$22
		sta $2006
		lda #$b7
		sta $2006
		lda #$02
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		lda #$22
		sta $2006
		lda #$b7
		sta $2006
		lda #$29
		ldx PVolume + 1
		beq cpu
	:	sta $2007
		dex
		bne :-
	cpu:
		;ドロップカウンタ表示
		lda #$23
		sta $2006
		lda #$07
		sta $2006
		lda Drop2
		sta $2007
		lda Drop1
		sta $2007
		;残りCPU時間
		lda #$23
		sta $2006
		lda #$26
		sta $2006
		lda CpuH2
		sta $2007
		lda CpuH1
		sta $2007
		lda CpuL2
		sta $2007
		lda CpuL1
		sta $2007
		;残りCPUバー
		lda #$23
		sta $2006
		lda #$2b
		sta $2006
		lda #$10
		sec
		sbc CpuBar
		sta DspWork
		ldx CpuBar
		inx
	:	lda #$29
		sta $2007
		dex
		bne :-
		ldx DspWork
	:	lda #$02
		sta $2007
		dex
		bne :-
	end:
		rts
.endproc


.proc dsp_init
		lda #$30
		sta __c
		sta __cc
		sta __s
		sta __ss
		sta __m
		sta __mm
		lda #$02
		ldx #2
	@L:
		sta POctave, x
		dex
		bpl @L
		;描画停止
		lda #$80
		sta $2000
		lda #$06
		sta $2001
		;BG描画
		lda #$20
		sta $2006
		lda #$00
		sta $2006
		ldx #$80
	black1:
		sta $2007
		dex
		bne black1
		lda #$0a
		ldx #$20
	border1:
		sta $2007
		dex
		bne border1
		lda #$02
		ldy #$16
	blue1:
		ldx #$20
	blue2:
		sta $2007
		dex
		bne blue2
		dey
		bne blue1
		lda #$03
		ldx #$20
	border2:
		sta $2007
		dex
		bne border2
		lda #$00
		ldx #$80
	black2:
		sta $2007
		dex
		bne black2
		;FCDSP
		lda #$20
		sta $2006
		lda #$66
		sta $2006
		ldx #$04
	fcdsp:
		stx $2007
		inx
		cpx #$0a
		bcc fcdsp
		;:
		lda #$20
		sta $2006
		lda #$73
		sta $2006
		lda #$30
		sta $2007
		sta $2007
		lda #$3a
		sta $2007
		lda #$30
		sta $2007
		sta $2007
		lda #$3a
		sta $2007
		lda #$30
		sta $2007
		sta $2007
		;ch01
		lda #$20
		sta $2006
		lda #$c6
		sta $2006
		ldx #$0b
	@L1:
		stx $2007
		inx
		cpx #$0e
		bcc @L1
		jsr pulse
		lda #$20
		sta $2006
		lda #$d6
		sta $2006
		lda #$28
		sta $2007
		lda #$20
		sta $2006
		lda #$e6
		sta $2006
		jsr key
		;ch02
		lda #$21
		sta $2006
		lda #$26
		sta $2006
		lda #$0b
		sta $2007
		lda #$0e
		sta $2007
		lda #$0f
		sta $2007
		jsr pulse
		lda #$21
		sta $2006
		lda #$36
		sta $2006
		lda #$28
		sta $2007
		lda #$21
		sta $2006
		lda #$46
		sta $2006
		jsr key
		;ch03
		lda #$21
		sta $2006
		lda #$86
		sta $2006
		lda #$0b
		sta $2007
		lda #$10
		sta $2007
		lda #$11
		sta $2007
		lda #$02
		sta $2007
		ldx #$1a
	@L2:
		stx $2007
		inx
		cpx #$20
		bcc @L2
		lda #$21
		sta $2006
		lda #$96
		sta $2006
		lda #$28
		sta $2007
		lda #$21
		sta $2006
		lda #$a6
		sta $2006
		jsr key
		;chEXP
		lda #$21
		sta $2006
		lda #$e6
		sta $2006
		lda #$0b
		sta $2007
		lda #$60
		sta $2007
		lda #$61
		sta $2007
		lda #$62
		sta $2007

.ifdef VRC6
		lda #$63
		sta $2007
		lda #$64
		sta $2007
		lda #$65
		sta $2007
		lda #$21
		sta $2006
		lda #$f6
		sta $2006
		lda #$28
		sta $2007
		lda #$29
		sta $2007
		sta $2007
		sta $2007
		sta $2007
.endif

.ifdef MMC5
		lda #$66
		sta $2007
		lda #$67
		sta $2007
		lda #$68
		sta $2007
		lda #$69
		sta $2007
		lda #$21
		sta $2006
		lda #$f6
		sta $2006
		lda #$28
		sta $2007
		lda #$29
		sta $2007
		sta $2007
		sta $2007
		sta $2007
.endif

		lda #$22
		sta $2006
		lda #$06
		sta $2006
		jsr key
		;ch04
		lda #$22
		sta $2006
		lda #$46
		sta $2006
		lda #$0b
		sta $2007
		lda #$12
		sta $2007
		lda #$13
		sta $2007
		lda #$22
		sta $2006
		lda #$56
		sta $2006
		lda #$28
		sta $2007
		lda #$29
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		lda #$22
		sta $2006
		lda #$66
		sta $2006
		ldx #$20
	@L3:
		stx $2007
		inx
		cpx #$24
		bcc @L3
		lda #$02
		sta $2007
		lda #$5b
		ldx #$10
	@L4:
		sta $2007
		dex
		bne @L4
		;ch05
		lda #$22
		sta $2006
		lda #$a6
		sta $2006
		lda #$0b
		sta $2007
		lda #$14
		sta $2007
		lda #$15
		sta $2007
		lda #$22
		sta $2006
		lda #$b6
		sta $2006
		lda #$28
		sta $2007
		lda #$22
		sta $2006
		lda #$c6
		sta $2006
		ldx #$24
	@L5:
		stx $2007
		inx
		cpx #$28
		bcc @L5
		lda #$02
		sta $2007
		lda #$5b
		ldx #$10
	@L6:
		sta $2007
		dex
		bne @L6
		;cpu
		lda #$23
		sta $2006
		lda #$06
		sta $2006
		lda #$3d
		sta $2007

		;パレット設定
		lda #$3f
		sta $2006
		lda #$00
		sta $2006
		ldx #$00
	pal:
		lda palette, x
		sta $2007
		inx
		cpx #$20
		bcc pal
		;属性設定
		lda #$23
		sta $2006
		lda #$c0
		sta $2006
		lda #$00
		ldx #8
	attr:
		sta $2007
		dex
		bne attr
		lda #$55
		ldx #$18
	@L1:
		sta $2007
		dex
		bne @L1
		lda #$55
		sta $2007
		sta $2007
		lda #$95
		sta $2007
		lda #$a5
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		lda #$55
		sta $2007
		sta $2007
		sta $2007
		lda #$99
		sta $2007
		lda #$aa
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		lda #$55
		sta $2007
		
		lda #$aa
		ldx #$08
	@L3:
		sta $2007
		dex
		bne @L3
		;スプライト
	sprite:
		;非表示にするもの
		lda #$ff
		sta CH1KEY + 0
		sta CH2KEY + 0
		sta CH3KEY + 0
		sta CH4KEY + 0
		sta CH5KEY + 0

		;ch1
		lda #YPOS1
		sta CH1VOL1 + 0
		sta CH1VOL2 + 0
		sta CH1VOL3 + 0
		sta CH1VOL4 + 0
		
		lda #$02
		sta CH1VOL1 + 1
		sta CH1VOL2 + 1
		sta CH1VOL3 + 1
		sta CH1VOL4 + 1
		
		lda #$b8
		sta CH1VOL1 + 3
		lda #$c0
		sta CH1VOL2 + 3
		lda #$c8
		sta CH1VOL3 + 3
		lda #$d0
		sta CH1VOL4 + 3
		
		;ch2
		lda #YPOS2
		sta CH2VOL1 + 0
		sta CH2VOL2 + 0
		sta CH2VOL3 + 0
		sta CH2VOL4 + 0
		
		lda #$02
		sta CH2VOL1 + 1
		sta CH2VOL2 + 1
		sta CH2VOL3 + 1
		sta CH2VOL4 + 1
		
		lda #$b8
		sta CH2VOL1 + 3
		lda #$c0
		sta CH2VOL2 + 3
		lda #$c8
		sta CH2VOL3 + 3
		lda #$d0
		sta CH2VOL4 + 3
		
		;ch3
		;ch4
		lda #YPOS4
		sta CH4VOL1 + 0
		sta CH4VOL2 + 0
		sta CH4VOL3 + 0
		sta CH4VOL4 + 0
		
		lda #$5f
		sta CH4KEY + 1
		lda #$02
		sta CH4VOL1 + 1
		sta CH4VOL2 + 1
		sta CH4VOL3 + 1
		sta CH4VOL4 + 1
		
		lda #$b8
		sta CH4VOL1 + 3
		lda #$c0
		sta CH4VOL2 + 3
		lda #$c8
		sta CH4VOL3 + 3
		lda #$d0
		sta CH4VOL4 + 3
		
		;ch5
		lda #$5f
		sta CH5KEY + 1
		
		;cpu
		lda #$02
		sta CPUREM + 1
		lda #$00
		sta CPUREM + 2

.ifdef VRC6
		lda #$ff
		sta EXP1KEY + 0
		sta EXP2KEY + 0
		sta EXP3KEY + 0
		lda #YPOS_EXP
		sta EXPVOL1 + 0
		sta EXPVOL2 + 0
		sta EXPVOL3 + 0
		sta EXPVOL4 + 0
		lda #$02
		sta EXPVOL1 + 1
		sta EXPVOL2 + 1
		sta EXPVOL3 + 1
		sta EXPVOL4 + 1
		lda #$b8
		sta EXPVOL1 + 3
		lda #$c0
		sta EXPVOL2 + 3
		lda #$c8
		sta EXPVOL3 + 3
		lda #$d0
		sta EXPVOL4 + 3
		lda #%00000001
		sta EXP3KEY + 2
.endif

.ifdef MMC5
		lda #$ff
		sta EXP1KEY + 0
		sta EXP2KEY + 0
		lda #YPOS_EXP
		sta EXPVOL1 + 0
		sta EXPVOL2 + 0
		sta EXPVOL3 + 0
		sta EXPVOL4 + 0
		lda #$02
		sta EXPVOL1 + 1
		sta EXPVOL2 + 1
		sta EXPVOL3 + 1
		sta EXPVOL4 + 1
		lda #$b8
		sta EXPVOL1 + 3
		lda #$c0
		sta EXPVOL2 + 3
		lda #$c8
		sta EXPVOL3 + 3
		lda #$d0
		sta EXPVOL4 + 3
.endif
		
		;BG書き換えの変数初期化
		lda #00
		sta PVolume + 0
		sta PVolume + 1
		
		lda #$02
		sta POctHead + 0
		sta POctHead + 1
		sta POctHead + 2
		
		sta POctave + 0
		sta POctave + 1
		sta POctave + 2

		
		sta PToneL + 0
		sta PToneL + 1
		sta PToneR + 0
		sta PToneR + 1
		sta PTone4
		
		sta PNote + 0
		sta PNote + 1
		sta PNote + 2
		
		sta PSharp + 0
		sta PSharp + 1
		sta PSharp + 2
		
		lda #$00
		sta DropCtr
		
		;スプライト転送
		lda #$07
		sta $4014
		;スクロール値の設定
		lda #$00
		sta $2005
		lda #$00
		sta $2005
		;描画開始
		lda #$80
		sta $2000
		lda #$1e
		sta $2001
	rts
.endproc


.proc drop_inc
		;ドロップカウンタ10進計算
		inc DropCtr
		lda DropCtr
		and #%00001111
		cmp #$0a
		bcc :+
		lda DropCtr
		and #%11110000
		clc
		adc #$10
		sta DropCtr
	:	rts
.endproc

.proc pulse
		lda #$02
		sta $2007
		ldx #$16
	@L:
		stx $2007
		inx
		cpx #$1a
		bcc @L
		ldx #$08
		lda #$02
	@L2:
		sta $2007
		dex
		bne @L2
		lda #$28
		sta $2007
		lda #$29
		sta $2007
		sta $2007
		sta $2007
		sta $2007
		rts
.endproc

.proc key
		ldx #$46
	@L:
		stx $2007
		inx
		cpx #$5b
		bcc @L
		rts
.endproc

;a % DspWork
.proc rem
		ldx #0
	@L:
		cmp DspWork
		bcc end
		sec
		sbc DspWork
		inx
		jmp @L
	end:
		rts
.endproc


.proc freq2note
		lda Freq_L, x
		bne start
		lda Freq_H, x
		bne start
		rts
	start:
		ldy #0
		lda #0
		lda Freq_L, x
		sta DspWork
		lda Freq_H, x
		sta DspWork + 1
		bne @L
		lda DspWork
		cmp #$0b					;オクターブ9以上はo8b固定
		bcc @o9
		cmp #$1b					;オクターブ8以上は別処理
		bcc @o8
		jmp @L
	@o9:
		lda #8
		sta DspWork + 2
		lda #$0d
		sta DspWork + 3
		rts
	@o8:
		lda #8
		sta DspWork + 2
		lda DspWork
		sec
		sbc #$0d
		tay
		lda Freq_Note_H, y
		sta DspWork + 3
		rts
	@L:
		lda DspWork + 1
		bne @E
		lda DspWork
		cmp #$36				;レジスタ値が$34(52)+2より小さくなるまでオクターブを上げる
		bcc next
	@E:
		lsr DspWork + 1
		ror DspWork
		iny
		jmp @L
	next:
		sty DspWork + 2			;7-上げた数がオクターブ
		lda #7
		sec
		sbc DspWork + 2
		sta DspWork + 2
		lda DspWork
		sec
		sbc #$1b				;レジスタ値1b(27)がo7bなのでそれを0とする
		tay
		lda Freq_Note, y
		sta DspWork + 3
		rts
.endproc

.ifdef VRC6
.proc freq2note_saw
		lda Freq_L, x
		bne start
		lda Freq_H, x
		bne start
		rts
	start:
		ldy #0
		lda #0
		lda Freq_L, x
		sta DspWork
		lda Freq_H, x
		sta DspWork + 1
		bne @L
		lda DspWork
		cmp #$10					;オクターブ9以上はo8b固定
		bcc @o9
		cmp #$1b					;オクターブ8以上は別処理
		bcc @o8
		jmp @L
	@o9:
		lda #8
		sta DspWork + 2
		lda #$0d
		sta DspWork + 3
		rts
	@o8:
		lda #8
		sta DspWork + 2
		lda DspWork
		sec
		sbc #$10				;レジスタ値$10がo8bなのでそれを0とする
		tay
		lda Freq_Note_Saw_H, y
		sta DspWork + 3
		rts
	@L:
		lda DspWork + 1
		bne @E
		lda DspWork
		cmp #$40				;レジスタ値が$40+2より小さくなるまでオクターブを上げる
		bcc next
	@E:
		lsr DspWork + 1
		ror DspWork
		iny
		jmp @L
	next:
		sty DspWork + 2			;7-上げた数がオクターブ
		lda #7
		sec
		sbc DspWork + 2
		sta DspWork + 2
		lda DspWork
		sec
		sbc #$1f				;レジスタ値$1fがo7bなのでそれを0とする
		tay
		lda Freq_Note_Saw, y
		sta DspWork + 3
		rts
.endproc
.endif


;ノートナンバーを位置に変換するテーブル
posmap:
	.byte	$00, $01, $03, $04, $06, $09, $0A, $0C, $0D, $0F, $10, $12

;ノートナンバーを文字に変換するテーブル
charmap:
	.byte	$3c, $3c, $3d, $3d, $3e, $3f, $3f, $40, $40, $3a, $3a, $3b

;半音判別用のテーブル
halftone:
	.byte	0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0

;逆引きテーブル
;o7b~o7cまでのレジスタ値から音階を引ける
Freq_Note:
	.byte	$0b, $0a, $0a, $09, $09, $08, $08, $07, $07, $06, $06, $05, $05, $05, $04, $04
	.byte	$03, $03, $03, $02, $02, $01, $01, $01, $00, $00, $00

Freq_Note_H:
	.byte	$0b, $0a, $09, $08, $07, $06, $05, $04
	.byte	$04, $03, $02, $02, $01, $00, $00, $00

.ifdef VRC6
Freq_Note_Saw:
	.byte	$0b, $0b, $0a, $0a, $09, $09, $08, $08, $07, $07, $06, $06, $06, $05, $05, $04
	.byte	$04, $04, $03, $03, $03, $02, $02, $02, $01, $01, $01, $00, $00, $00, $00, $00

Freq_Note_Saw_H:
	.byte	$0b, $0a, $09, $08, $07, $06, $05, $05
	.byte	$04, $03, $03, $02, $01, $01, $00, $00
.endif