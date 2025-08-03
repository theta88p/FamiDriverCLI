.exportzp	Frags
.export		DrvFrags
.export		Device
.export		NoteN
.export		Volume
.export		Tone
.export		Freq_L
.export		Freq_H
.export		drv_main
.export		drv_init
.export		drv_sndreq

.include	"drv.inc"

;-----------------------------------------------------------------------
; Zeropage works
;-----------------------------------------------------------------------
.zeropage

Frags:			.res	MAX_TRACK	;通常のフラグ
EnvFrags:		.res	MAX_TRACK	;エンベロープのフラグ
Work:			.res	6

;-----------------------------------------------------------------------
; Non Zeropage works
;-----------------------------------------------------------------------
.bss

Device:			.res	MAX_TRACK	;トラックで使用している音源
Ptr_L:			.res	MAX_TRACK	;再生箇所のアドレスL
Ptr_H:			.res	MAX_TRACK	;再生箇所のアドレスH
LenCtr:			.res	MAX_TRACK	;音長カウンタ
GateCtr:		.res	MAX_TRACK	;ゲートカウンター
NoteN:			.res	MAX_TRACK	;ノートナンバー
DefLen:			.res	MAX_TRACK	;デフォルト音長
Length:			.res	MAX_TRACK	;音長
Gate:			.res	MAX_TRACK	;上位2bit 使用中のゲートコマンド 下位5bit ゲートコマンドの値
TrVolume:		.res	MAX_TRACK	;トラック音量
Volume:			.res	MAX_TRACK	;音量
Tone:			.res	MAX_TRACK	;音色
Freq_L:			.res	MAX_TRACK	;周波数L
Freq_H:			.res	MAX_TRACK	;周波数H
RefFreq_L:		.res	MAX_TRACK	;音程エンベロープ値を加算する前の周波数L
RefFreq_H:		.res	MAX_TRACK	;音程エンベロープ値を加算する前の周波数H
RefNoteN:		.res	MAX_TRACK	;ノートエンベロープ値を加算する前のノートナンバー
RefTone:		.res	MAX_TRACK	;音色エンベロープ値を加算する前の音色
PrevFreq_L:		.res	MAX_TRACK	;前回レジスタに書き込んだ周波数L（音源ごとに保存）
PrevFreq_H:		.res	MAX_TRACK	;前回レジスタに書き込んだ周波数H（音源ごとに保存）
KeyShift:		.res	MAX_TRACK	;キーシフト値
Detune:			.res	MAX_TRACK	;デチューン値
InfLoopAddr_L:	.res	MAX_TRACK	;無限ループの戻り先L
InfLoopAddr_H:	.res	MAX_TRACK	;無限ループの戻り先H
SSwpEnd:		.res	MAX_TRACK	;ソフトウェアスイープの終了音程（+-半音単位）
SSwpDelay:		.res	MAX_TRACK	;ソフトウェアスイープのディレイ
SSwpDepth:		.res	MAX_TRACK	;ソフトウェアスイープの一回に加算する値
SSwpRate:		.res	MAX_TRACK	;ソフトウェアスイープで何フレームおきに加算するか
SSwpEndFreq_L:	.res	MAX_TRACK	;ソフトウェアスイープの終了周波数L
SSwpEndFreq_H:	.res	MAX_TRACK	;ソフトウェアスイープの終了周波数H
SSwpCtr:		.res	MAX_TRACK	;ソフトウェアスイープのカウンタ
VEnvAddr_L:		.res	MAX_TRACK	;音量エンベロープのアドレスL
VEnvAddr_H:		.res	MAX_TRACK	;音量エンベロープのアドレスH
VEnvPos:		.res	MAX_TRACK	;音量エンベロープの現在位置
VEnvCtr:		.res	MAX_TRACK	;音量エンベロープのカウンタ
VEnvDelay:		.res	MAX_TRACK	;音量エンベロープのディレイ
FEnvAddr_L:		.res	MAX_TRACK	;音程エンベロープ以下省略
FEnvAddr_H:		.res	MAX_TRACK
FEnvPos:		.res	MAX_TRACK
FEnvCtr:		.res	MAX_TRACK
FEnvDelay:		.res	MAX_TRACK
NEnvAddr_L:		.res	MAX_TRACK	;ノートエンベロープ
NEnvAddr_H:		.res	MAX_TRACK
NEnvPos:		.res	MAX_TRACK
NEnvCtr:		.res	MAX_TRACK
NEnvDelay:		.res	MAX_TRACK
TEnvAddr_L:		.res	MAX_TRACK	;音色エンベロープ
TEnvAddr_H:		.res	MAX_TRACK
TEnvPos:		.res	MAX_TRACK
TEnvCtr:		.res	MAX_TRACK
TEnvDelay:		.res	MAX_TRACK
HSwpReg:		.res	MAX_TRACK	;ハードウェアスイープレジスタに書き込む値
HEnvReg:		.res	MAX_TRACK	;ハードウェアエンベロープレジスタに書き込む値
LoopDepth:		.res	MAX_TRACK	;ループ深度

LoopN:		.res	MAX_TRACK * MAX_LOOP	;残りループ回数
LoopAddr_L:	.res	MAX_TRACK * MAX_LOOP	;ループの戻り先L
LoopAddr_H:	.res	MAX_TRACK * MAX_LOOP	;ループの戻り先H

DrvFrags:		.res	1	;ドライバ全体のフラグ
SpdCtr:			.res	1	;速度カウンタ
SpdFreq:		.res	1	;速度カウンタに加算する値
ProcTr:			.res	1	;処理中のトラック
SeqAddr_L:		.res	1	;シーケンス情報のアドレスL
SeqAddr_H:		.res	1	;シーケンス情報のアドレスH
PrevDev:		.res	1	;前回の音源（レジスタ書き込み用）
LastTrack:		.res	1	;使用する最大トラック数 - 1

.ifdef SS5B
SS5BTone:		.res	3
SS5BHWEnv:		.res	3	;ハードウェアエンベロープが有効なら1無効なら0
.endif

.ifdef FDS
FdsWavAddr_L:	.res	1
FdsWavAddr_H:	.res	1
FdsPrevWav:		.res	1	;前回書き込んだ音色番号
FdsModAddr_L:	.res	1
FdsModAddr_H:	.res	1
FdsPrevMod:		.res	1
FdsModTone:		.res	1	;モジュレータの音色番号
FdsModFreq_L:	.res	1	;モジュレータの周波数L
FdsModFreq_H:	.res	1	;モジュレータの周波数H＋上位1bitに同期フラグ
.endif

;00～6b	:o0c～o8b	音長デフォ
;6c	:r		休符（音長デフォ）
;6d	:[x		ループ開始
;6e	:]		ループ終了
;6f	::		ループ途中終了
;70	:qx		ゲートタイム（音長-nの方式。他と排他）
;70	:ux		ゲートタイム（音長nの方式。他と排他）
;70	:Qx		ゲートタイム（音長n/8の方式。他と排他）
;71	:kx		キーシフト相対指定
;72	:Kx		キーシフト絶対指定
;73	:&		次の音がタイ・スラーになる
;74	:@x		音色指定
;75	:tx		フレームスキップ値。コンパイラでテンポから計算
;76 :@p		指定した曲番号のデータを再生
;77	:@vx	音量エンベロープ指定（外部定義）
;78 :@v*	音量エンベロープ停止
;79	:@fx	音程エンベロープ指定（外部定義）
;7a :@f*	音程エンベロープ停止
;7b	:@nx	ノートエンベロープ指定（外部定義）
;7c :@n*	ノートエンベロープ停止
;7d	:@tx	音色エンベロープ指定（外部定義）
;7e	:@t*	音色エンベロープ停止
;7f	:		トラック終了
;80～eb	:o0c～o8b	音長指定
;ec	:r		休符（音長指定）
;ed :L		無限ループ
;ee	:lx		デフォ音長
;ef	:v+-x	ボリューム相対指定（-15～15）
;f0	:vx		ボリューム絶対指定（0～15）
;f1	:@dx	デチューン
;f2	:hsx	ハードウェアスイープ
;f3	:hex	ハードウェアエンベロープ
;f4	:sx		ソフトウェアスイープ
;f5	:s*		ソフトウェアスイープ無効
;f6	:r-		エンベロープ無効
;f7	:w		メモリ書き込み
;f8	:\x		サブルーチン

;f9 :@fdsf	FDSモジュレーション周波数
;fa	:@fdsm	FDSモジュレータ番号
;fb	:@fdse	FDSモジュレーションエンベロープ

; ------------------------------------------------------------------------
; main
; ------------------------------------------------------------------------
.code

.proc drv_main
		lda DrvFrags
		and #DRV_IS_PROC
		bne exit
		lda DrvFrags
		ora #DRV_IS_PROC
		sta DrvFrags
		ldx LastTrack
		jsr pretrack	;トラック処理の前に毎フレームやる処理をここでやる
		lda DrvFrags
		and #DRV_SKIP_DIR
		bne acc
		lda SpdCtr		;減速の場合
		clc
		adc SpdFreq
		sta SpdCtr
		bcs env			;SpdFreqを足していって桁上がりしたらスキップ
		ldx LastTrack
		jsr track		;トラック処理
		jmp env
	acc:				;加速の場合
		lda SpdCtr
		clc
		adc SpdFreq
		sta SpdCtr
		bcc single		;桁上がりしたら二重処理
		ldx LastTrack
		jsr track		;トラック処理
		ldx LastTrack
		jsr envelope
		ldx LastTrack
		jsr pretrack
		lda DrvFrags
		ora #DRV_DOUBLE
		sta DrvFrags
	single:
		ldx LastTrack
		jsr track		;トラック処理
	env:				;エンベロープと書き込み処理は毎フレームやる
		ldx LastTrack
		jsr envelope
		lda DrvFrags
		and #DRV_DOUBLE_CLR & DRV_IS_PROC_CLR
		sta DrvFrags
	exit:
		rts
.endproc

;ドライバ初期化
.proc drv_init
		;変数初期化
		lda #%00111111
		sta $4000
		sta $4004
		lda #%00001000
		sta $4001
		sta $4005
		lda #%11111111
		sta $4008
		lda #%00001111
		sta $4015
.ifdef MMC5
		lda #%00111111
		sta $5000
		sta $5004
		lda #%00000011
		sta $5015
.endif
.ifdef SS5B
		lda #0
		sta SS5BTone + 0
		sta SS5BTone + 1
		sta SS5BTone + 2
		sta SS5BHWEnv + 0
		sta SS5BHWEnv + 1
		sta SS5BHWEnv + 2
		lda #$7
		sta $c000
		lda #%00111000
		sta $e000
.endif
.ifdef FDS
		lda #%00000010
		sta $4023
.endif
		lda #0
		sta SpdCtr
		sta ProcTr
		sta SpdFreq
		sta SpdCtr
		lda #$ff
		sta PrevDev
		lda #DRV_INIT
		sta DrvFrags
		rts
.endproc

;トラック初期化
.proc track_init
		lda #0
		sta Volume, x
		sta InfLoopAddr_L, x
		sta InfLoopAddr_H, x
		sta Gate, x
		sta KeyShift, x
		sta Detune, x
		sta VEnvCtr, x
		sta VEnvPos, x
		sta EnvFrags, x
		sta Tone, x
		sta RefTone, x
		sta Frags, x
		lda #1
		sta LenCtr, x
		sta GateCtr, x
		lda #FRAG_ENV_DIS
		sta EnvFrags, x
		lda Device, x
		cmp #4
		bne @N
		lda #0
		jmp @E
	@N:
		lda #15
	@E:
		sta TrVolume, x
		lda #%00001000
		sta HSwpReg, x
		lda #%00110000
		sta HEnvReg, x
		rts
.endproc

;サウンド要求
.proc drv_sndreq
		sta Work
		sta SeqAddr_L
		stx Work + 1
		stx SeqAddr_H
		tya
		asl
	load:
		tay
		lda (Work), y
		clc
		adc Work
		sta Work + 2
		iny
		lda (Work), y
		adc Work + 1
		sta Work + 1
		lda Work + 2
		sta Work
		ldx #0			;ここからポインタ初期化
		ldy #0
	loop:
		lda #$ff
		cmp (Work), y	;トラック番号を比較
		beq nouse		;$ffならこれ以降は未使用
		iny
		lda (Work), y
		sta Device, x	;音源番号を取得して保存
		iny
		lda (Work), y
		clc
		adc SeqAddr_L
		sta Ptr_L, x	;トラックの開始アドレスを保存
		iny
		lda (Work), y
		adc SeqAddr_H
		sta Ptr_H, x
		jsr track_init	;トラック初期化
		inx
		iny
		cpx #MAX_TRACK
		bcc loop
		jmp def
	nouse:
		dex
		stx LastTrack
		inx
	@L:
		lda #FRAG_END
		sta Frags, x
		lda #$ff
		sta Device, x	;$ffを入れて処理しないようにしておく
		inx
		cpx #MAX_TRACK
		bcc @L
	def:
		iny				;トラック終端を飛ばす
		ldx LastTrack
	@L:
		lda (Work), y
		sta DefLen, x	;デフォルト音長を保存
		dex
		bpl @L
.ifdef FDS
		iny
		lda (Work), y
		clc
		adc SeqAddr_L	;相対アドレスを絶対アドレスに直す
		sta FdsWavAddr_L
		iny
		lda (Work), y
		adc SeqAddr_H
		sta FdsWavAddr_H
		lda #$ff
		sta FdsPrevWav	;0初期化すると音色番号0が読み込まれないため
						;以下モジュレータも同様に処理
		iny
		lda (Work), y
		clc
		adc SeqAddr_L
		sta FdsModAddr_L
		iny
		lda (Work), y
		adc SeqAddr_H
		sta FdsModAddr_H
		lda #$ff
		sta FdsModTone	;モジュレータ波形を指定しない場合無効
		sta FdsPrevMod
.endif
		rts
.endproc


.proc pretrack
	start:
		lda Frags, x
		and #FRAG_END
		beq frag
		dex
		bpl start
		rts
	frag:
		lda Frags, x
		;キーオン・キーオフ・キーオン無効フラグを降ろす
		and #FRAG_KEYON_CLR & FRAG_KEYON_DIS_CLR & FRAG_KEYOFF_CLR
		;ロードフラグを立てる
		ora #FRAG_LOAD
		sta Frags, x
		dex
		bpl start
		rts
.endproc
; ------------------------------------------------------------------------
; トラック処理
; ------------------------------------------------------------------------
.proc track
	start:
		lda Frags, x
		and #FRAG_END
		bne next		;終了フラグが立っていなければ処理へ
		stx ProcTr
		lda LenCtr, x
		cmp #1
		beq seq				;音長カウンタが1になったらシーケンスのロード
	gate:					;音長カウンタが1でなければゲート処理へ
		lda GateCtr, x
		cmp #1
		bne cnt				;ゲートカウンタが1でなければカウント処理へ
		lda Frags, x		;ゲートカウンタが1になったらキーオフ
		and #FRAG_KEYON_CLR	& FRAG_KEYON_DIS_CLR & FRAG_IS_KEYON_CLR
		ora #FRAG_KEYOFF
		sta Frags, x
		jmp cnt				;キーオフしたら終了
	seq:
		jsr loadseq
		lda Frags, x
		and #FRAG_LOAD		;ロードフラグが立っていれば続けてロード
		bne seq
		lda Frags, x
		and #FRAG_END
		bne next			;終了フラグが立っていなければ処理へ
		jsr procnote
		jmp next
	cnt:
		lda LenCtr, x
		beq next
		dec LenCtr, x
		lda GateCtr, x
		beq next
		dec GateCtr, x
	next:
		dex
		bpl start
		rts
.endproc


; ------------------------------------------------------------------------
; シーケンスデータのロード
; ------------------------------------------------------------------------
.proc loadseq
		lda Ptr_L, x
		sta Work
		lda Ptr_H, x
		sta Work + 1

		ldy #0
		lda (Work), y

		cmp #$6c
		bcc def_note
		cmp #$80
		bcc lower_cmd
		cmp #$ec
		bcc len_note
		cmp #$fc
		bcc upper_cmd
		
	unknown_cmd:
		; 未知のコマンドは無視
		lda #1
		jsr addptr
		rts
		
	lower_cmd:
		sec
		sbc #$6c
		cmp #upper_table - lower_table
		bcs unknown_cmd
		asl						; *2 for word table
		tay
		lda lower_table + 1, y
		pha
		lda lower_table, y
		pha
		rts						; ジャンプ実行
		
	upper_cmd:
		sec
		sbc #$ec
		cmp #upper_table_end - upper_table
		bcs unknown_cmd
		asl						; *2 for word table
		tay
		lda upper_table + 1, y
		pha
		lda upper_table, y
		pha
		rts						; ジャンプ実行
		
	def_note:
		sta NoteN, x
		lda Frags, x
		and #FRAG_KEYON_DIS
		bne @N
		lda Frags, x
		ora #FRAG_KEYON | FRAG_IS_KEYON	;キーオンフラグとキーオン中判定フラグを立てる
		sta Frags, x
	@N:
		lda Frags, x
		and #FRAG_LOAD_CLR		;ロードフラグを降ろす
		sta Frags, x
		lda EnvFrags, x
		and #FRAG_ENV_DIS_CLR	;エンベロープ無効フラグを降ろす
		sta EnvFrags, x
		lda DefLen, x
		sta Length, x
		lda #1
		jsr addptr
		rts
		
	len_note:
		sec
		sbc #$80
		sta NoteN, x
		lda Frags, x
		and #FRAG_KEYON_DIS
		bne @N
		lda Frags, x
		ora #FRAG_KEYON | FRAG_IS_KEYON	;キーオンフラグとキーオン中判定フラグを立てる
		sta Frags, x
	@N:
		lda Frags, x
		and #FRAG_LOAD_CLR		;ロードフラグを降ろす
		sta Frags, x
		lda EnvFrags, x
		and #FRAG_ENV_DIS_CLR	;エンベロープ無効フラグを降ろす
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta Length, x
		lda #2
		jsr addptr
		rts

	def_rest:			;音長なし休符
		lda Frags, x
		and #FRAG_IS_KEYON		;キーオン中でなければキーオフはしない
		beq	@N
		lda Frags, x
		ora #FRAG_KEYOFF		;キーオフフラグを立てる
		sta Frags, x
	@N:
		lda Frags, x
		and #FRAG_IS_KEYON_CLR & FRAG_LOAD_CLR	;キーオン中フラグとロードフラグを降ろす
		sta Frags, x
		lda DefLen, x
		sta Length, x
		lda #1
		jsr addptr
		rts
		
	loop_start:			;ループ開始
		inc LoopDepth, x
		jsr loopoffset
		sty Work + 2
		ldy #1
		lda (Work), y
		ldy Work + 2
		sta LoopN, y
		lda #2
		jsr addptr
		lda Ptr_L, x
		sta LoopAddr_L, y
		lda Ptr_H, x
		sta LoopAddr_H, y
		rts
		
	loop_end:			;ループ終了
		jsr loopoffset
		lda LoopN, y		;yだと直接decできない
		sec
		sbc #1
		sta LoopN, y
		beq @E2
		cmp #1				;ループ回数が1になったらループ終了＋１を保存しておく
		beq @E3
		lda LoopAddr_L, y
		sta Ptr_L, x
		lda LoopAddr_H, y
		sta Ptr_H, x
		rts
	@E2:
		dec LoopDepth, x	;ループを抜けたら深度減算
		lda #1
		jsr addptr
		rts
	@E3:
		lda #1
		jsr addptr
		lda Ptr_L, x
		pha
		lda LoopAddr_L, y
		sta Ptr_L, x
		pla
		sta LoopAddr_L, y
		lda Ptr_H, x
		pha
		lda LoopAddr_H, y
		sta Ptr_H, x
		pla
		sta LoopAddr_H, y
		rts
		
	loop_mid_end:			;ループ途中終了
		jsr loopoffset
		lda LoopN, y
		cmp #2
		bcs @E
		lda #0
		sta LoopN, y
		lda LoopAddr_L, y
		sta Ptr_L, x
		lda LoopAddr_H, y
		sta Ptr_H, x
		dec LoopDepth, x	;ループを抜けたら深度減算
		rts
	@E:
		lda #1
		jsr addptr
		rts
		
	gate:					;ゲート
		ldy #1
		lda (Work), y
		sta Gate, x
		lda #2
		jsr addptr
		rts
		
	rel_shift:				;相対キーシフト(k)
		ldy #1
		lda (Work), y
		clc
		adc KeyShift, x
		sta KeyShift, x
		lda #2
		jsr addptr
		rts
		
	abs_shift:				;絶対キーシフト(K)
		ldy #1
		lda (Work), y
		sta KeyShift, x
		lda #2
		jsr addptr
		rts
		
	tai_slur:				;タイ・スラー
		lda Frags, x
		ora #FRAG_KEYON_DIS				;キーオン無効フラグを立てる
		sta Frags, x
		lda #1
		jsr addptr
		rts
		
	tone:					;音色指定
		lda EnvFrags, x
		and #FRAG_TENV_CLR	;音色エンベロープを解除
		sta EnvFrags, x
		lda Device, x
		cmp #DEV_2A03_DPCM	;DPCMトラックなら
		beq @D
.ifdef SS5B
		cmp #DEV_SS5B_SQR3 + 1
		bcs @N
		cmp #DEV_SS5B_SQR1
		bcc @N
		lda #$7
		sta $c000
		ldy #1
		lda (Work), y
		sta $e000
		ldy #2
		lda (Work), y
		sta SS5BTone + 0
		ldy #3
		lda (Work), y
		sta SS5BTone + 1
		ldy #4
		lda (Work), y
		sta SS5BTone + 2
		lda #5
		jsr addptr
		rts
	@N:
.endif
		ldy #1
		lda (Work), y
		sta Tone, x
		sta RefTone, x		;音色番号を保存（音色エンベロープ解除用）
		lda #2
		jsr addptr
		rts
	@D:
		ldy #1
		lda (Work), y
		sta $4012
		ldy #2
		lda (Work), y
		sta $4013
		ldy #3
		lda (Work), y
		sta Volume, x
		lda #4
		jsr addptr
		rts
		
	tempo:					;フレームスキップ加算値（テンポ）
		ldy #1
		lda (Work), y
		beq @dec
		lda DrvFrags
		ora #DRV_SKIP_DIR
		jmp @next
	@dec:
		lda DrvFrags
		and #DRV_SKIP_DIR_CLR
	@next:
		sta DrvFrags
		ldy #2
		lda (Work), y
		sta SpdFreq
		lda #3
		jsr addptr
		rts
		
	play:					;指定した曲番号のデータを再生
		ldy #1
		lda (Work), y
		tay
		lda SeqAddr_L
		ldx SeqAddr_H
		jsr drv_sndreq
		ldx ProcTr
		lda #2
		jsr addptr
		rts
		
	volume_env:				;音量エンベロープ。引数はアドレスL、アドレスH、ディレイ
		lda EnvFrags, x
		ora #FRAG_VENV		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta VEnvAddr_L, x
		ldy #2
		lda (Work), y
		sta VEnvAddr_H, x
		ldy #3
		lda (Work), y
		sta VEnvDelay, x
		lda #1
		sta VEnvCtr, x
		lda #1
		sta VEnvPos, x
		clc
		lda VEnvAddr_L, x	;相対アドレスを絶対アドレスに直す
		adc SeqAddr_L
		sta VEnvAddr_L, x
		lda VEnvAddr_H, x
		adc SeqAddr_H
		sta VEnvAddr_H, x
		lda #4
		jsr addptr
		rts
		
	volume_env_clear:		;音量エンベロープのクリア
		lda EnvFrags, x
		and #FRAG_VENV_CLR	;フラグを降ろす
		sta EnvFrags, x
		lda TrVolume, x
		sta Volume, x		;音量を戻す
		lda #1
		jsr addptr
		rts
		
	freq_env:				;音程エンベロープ。引数はアドレスL、アドレスH、ディレイ
		lda EnvFrags, x
		ora #FRAG_FENV		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta FEnvAddr_L, x
		ldy #2
		lda (Work), y
		sta FEnvAddr_H, x
		ldy #3
		lda (Work), y
		sta FEnvDelay, x
		lda #1
		sta FEnvCtr, x
		lda #1
		sta FEnvPos, x
		clc
		lda FEnvAddr_L, x	;相対アドレスを絶対アドレスに直す
		adc SeqAddr_L
		sta FEnvAddr_L, x
		lda FEnvAddr_H, x
		adc SeqAddr_H
		sta FEnvAddr_H, x
		lda #4
		jsr addptr
		rts
		
	freq_env_clear:			;音程エンベロープのクリア
		lda EnvFrags, x
		and #FRAG_FENV_CLR	;フラグを降ろす
		sta EnvFrags, x
		lda #1
		jsr addptr
		rts
		
	note_env:				;ノートエンベロープ。引数はアドレスL、アドレスH、ディレイ
		lda EnvFrags, x
		ora #FRAG_NENV		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta NEnvAddr_L, x
		ldy #2
		lda (Work), y
		sta NEnvAddr_H, x
		ldy #3
		lda (Work), y
		sta NEnvDelay, x
		lda #1
		sta NEnvCtr, x
		lda #1
		sta NEnvPos, x
		clc
		lda NEnvAddr_L, x	;相対アドレスを絶対アドレスに直す
		adc SeqAddr_L
		sta NEnvAddr_L, x
		lda NEnvAddr_H, x
		adc SeqAddr_H
		sta NEnvAddr_H, x
		lda #4
		jsr addptr
		rts
		
	note_env_clear:			;ノートエンベロープのクリア
		lda EnvFrags, x
		and #FRAG_NENV_CLR	;フラグを降ろす
		sta EnvFrags, x
		lda #1
		jsr addptr
		rts
		
	tone_env:				;音色エンベロープ。引数はアドレスL、アドレスH、ディレイ
		lda EnvFrags, x
		ora #FRAG_TENV		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta TEnvAddr_L, x
		ldy #2
		lda (Work), y
		sta TEnvAddr_H, x
		ldy #3
		lda (Work), y
		sta TEnvDelay, x
		lda #1
		sta TEnvCtr, x
		lda #1
		sta TEnvPos, x
		clc
		lda TEnvAddr_L, x	;相対アドレスを絶対アドレスに直す
		adc SeqAddr_L
		sta TEnvAddr_L, x
		lda TEnvAddr_H, x
		adc SeqAddr_H
		sta TEnvAddr_H, x
		lda #4
		jsr addptr
		rts
		
	tone_env_clear:			;音色エンベロープのクリア
		lda EnvFrags, x
		and #FRAG_TENV_CLR	;フラグを降ろす
		sta EnvFrags, x
		lda RefTone, x
		sta Tone, x			;音色番号を戻す
		lda #1
		jsr addptr
		rts
		
	track_end:				;トラック終了
		clc
		lda InfLoopAddr_L, x			;無限ループアドレスが設定されていればジャンプ
		bne @N
		lda InfLoopAddr_H, x
		beq @E
	@N:
		lda InfLoopAddr_L, x
		sta Ptr_L, x
		lda InfLoopAddr_H, x
		sta Ptr_H, x
		rts
	@E:
		lda Frags, x
		and #FRAG_LOAD_CLR				;ロードフラグを降ろす
		ora #FRAG_END					;エンドフラグを立てる
		sta Frags, x
		rts
		
	len_rest:				;音長あり休符
		lda Frags, x
		and #FRAG_IS_KEYON		;キーオン中でなければキーオフはしない
		beq	@N
		lda Frags, x
		ora #FRAG_KEYOFF		;キーオフフラグを立てる
		sta Frags, x
	@N:
		lda Frags, x
		and #FRAG_IS_KEYON_CLR & FRAG_LOAD_CLR	;キーオン中フラグとロードフラグを降ろす
		sta Frags, x
		ldy #1
		lda (Work), y
		sta Length, x
		lda #2
		jsr addptr
		rts
		
	inf_loop_def:			;無限ループ設定
		lda #1
		jsr addptr
		lda Ptr_L, x
		sta InfLoopAddr_L, x
		lda Ptr_H, x
		sta InfLoopAddr_H, x
		rts
		
	def_len:				;デフォ音長
		ldy #1
		lda (Work), y
		sta DefLen, x
		lda #2
		jsr addptr
		rts
		
	rel_volume:				;ボリューム相対指定
		ldy #1
		lda (Work), y
		clc
		adc TrVolume, x
		bpl @P
		lda #0
	@P:
		sta TrVolume, x
		lda #2
		jsr addptr
		rts
		
	abs_volume:				;ボリューム絶対指定
		ldy #1
		lda (Work), y
		sta TrVolume, x
		lda #2
		jsr addptr
		rts
		
	detune:					;デチューン
		ldy #1
		lda (Work), y
		sta Detune, x
		lda #2
		jsr addptr
		rts
		
	hw_sweep:				;ハードウェアスイープ
		ldy #1
		lda (Work), y
		sta HSwpReg, x
		lda #2
		jsr addptr
		rts
		
	hw_env:					;ハードウェアエンベロープ
.ifdef SS5B
		lda Device, x
		cmp #DEV_SS5B_SQR3 + 1
		bcs @N
		cmp #DEV_SS5B_SQR1
		bcc @N
		sec
		sbc #DEV_SS5B_SQR1
		tax
		ldy #1
		lda (Work), y
		sta SS5BHWEnv, x
		bne @E
		ldx ProcTr
		rts
	@E:
		ldx ProcTr
		lda #$0b
		sta $c000
		ldy #2
		lda (Work), y
		sta $e000
		lda #$0c
		sta $c000
		ldy #3
		lda (Work), y
		sta $e000
		lda #$0d
		sta $c000
		ldy #4
		lda (Work), y
		sta $e000
		lda #5
		jsr addptr
	@N:
.endif
		ldy #1
		lda (Work), y
		sta HEnvReg, x
		lda #2
		jsr addptr
		rts
		
	sw_sweep:				;ソフトウェアスイープ
		lda EnvFrags, x		;引数は終了周波数（+-半音単位）、Delay、Speed。
							;開始周波数はノートの方を変更する。
		ora #FRAG_SSWP		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		sta SSwpEnd, x
		ldy #2
		lda (Work), y
		sta SSwpDelay, x
		ldy #3
		lda (Work), y
		bpl @N0				;speed値がマイナスだったら
		sta SSwpRate, x		;その値をRate、Depthを1とする
		lda #1
		sta SSwpDepth, x
		jmp @N1
	@N0:					;プラスだったら
		sta SSwpDepth, x	;その値をDepth、Rateを0とする
		lda #0
		sta SSwpRate, x
	@N1:
		lda #0
		sta SSwpCtr, x		;カウンタリセット
		lda SSwpEnd, x
		bmi @neg			;変化方向がプラスならDepthをマイナスにする。マイナスなら何もしない
		lda SSwpDepth, x
		eor #$ff
		clc
		adc #1
		sta SSwpDepth, x
	@neg:
		lda #4
		jsr addptr
		rts
		
	sw_sweep_clear:			;ソフトウェアスイープのクリア
		lda EnvFrags, x
		and #FRAG_SSWP_CLR	;フラグを降ろす
		sta EnvFrags, x
		lda #1
		jsr addptr
		rts
		
	disable_env:			;エンベロープ無効
		lda EnvFrags, x
		ora #FRAG_ENV_DIS	;エンベロープ無効フラグを立てる
		sta EnvFrags, x
		lda #1
		jsr addptr
		rts
		
	mem_write:				;メモリ書き込み
		ldy #1
		lda (Work), y
		sta Work + 2
		ldy #2
		lda (Work), y
		sta Work + 3
		ldy #3
		lda (Work), y
		ldy #0
		sta (Work + 2), y
		lda #4
		jsr addptr
		rts
		
	subroutine:				;サブルーチン
		lda #3
		jsr addptr
		ldy #1
		lda (Work), y
		sta Work + 2
		iny
		lda (Work), y
		sta Work + 3
		inc LoopDepth, x
		jsr loopoffset
		lda LoopN, y
		clc
		adc #1
		sta LoopN, y
		lda Ptr_L, x
		sta LoopAddr_L, y
		lda Ptr_H, x
		sta LoopAddr_H, y
		clc
		lda Work + 2
		adc SeqAddr_L
		sta Ptr_L, x
		lda Work + 3
		adc SeqAddr_H
		sta Ptr_H, x
		rts
		
.ifdef FDS
	fds_mod_freq:		;FDSモジュレータ周波数
		ldy #1
		lda (Work), y
		sta FdsModFreq_L
		ldy #2
		lda (Work), y
		sta FdsModFreq_H
		lda #3
		jsr addptr
		rts
		
	fds_mod_tone:		;FDSモジュレータ番号
		ldy #1
		lda (Work), y
		sta FdsModTone
		lda #2
		jsr addptr
		rts
		
	fds_mod_env:		;FDSモジュレータエンベロープ
		ldy #1
		lda (Work), y
		sta $4084
		lda #2
		jsr addptr
		rts
.endif
		
	lower_table:
		.word def_rest - 1
		.word loop_start - 1
		.word loop_end - 1
		.word loop_mid_end - 1
		.word gate - 1
		.word rel_shift - 1
		.word abs_shift - 1
		.word tai_slur - 1
		.word tone - 1
		.word tempo - 1
		.word play - 1
		.word volume_env - 1
		.word volume_env_clear - 1
		.word freq_env - 1
		.word freq_env_clear - 1
		.word note_env - 1
		.word note_env_clear - 1
		.word tone_env - 1
		.word tone_env_clear - 1
		.word track_end - 1
	upper_table:
		.word len_rest - 1
		.word inf_loop_def - 1
		.word def_len - 1
		.word rel_volume - 1
		.word abs_volume - 1
		.word detune - 1
		.word hw_sweep - 1
		.word hw_env - 1
		.word sw_sweep - 1
		.word sw_sweep_clear - 1
		.word disable_env - 1
		.word mem_write - 1
		.word subroutine - 1
.ifdef FDS
		.word fds_mod_freq - 1
		.word fds_mod_tone - 1
		.word fds_mod_env - 1
.endif
	upper_table_end:
	

.endproc


; ------------------------------------------------------------------------
; ノート関係の処理
; ------------------------------------------------------------------------
.proc procnote
		lda Length, x
		sta LenCtr, x
		lda Frags, x
		lda Gate, x
		and #%00111111
		sta Work
		lda Gate, x
		and #%11000000
		cmp #%01000000
		beq @G0				;上位2bitが01ならq
		cmp #%10000000
		beq @G1				;10ならu
		cmp #%11000000
		beq @G2				;11ならQ
		lda LenCtr, x
		sta GateCtr, x
		jmp next
	@G0:					;ゲートタイム設定
		lda LenCtr, x
		sec
		sbc Work
		sta GateCtr, x
		jmp next
	@G1:
		lda Work
		sta GateCtr, x
		jmp next
	@G2:
		ldy Work
		lda LenCtr, x
		jsr mult		;a * y / 8
		lsr Work + 3
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		lda Work + 2
		sta GateCtr, x
	next:
		lda Frags, x		;キーオフの場合これ以降は処理しない
		and #FRAG_IS_KEYON
		beq end1
		lda Frags, x
		and #FRAG_KEYON	;キーオンされていない
		bne @N
		lda EnvFrags, x
		and #FRAG_SSWP | FRAG_FENV	;かつスイープか音程エンベロープ有効
		beq @N
		lda RefNoteN, x				;かつノートが前回と同じならこれ以降は処理しない
		cmp NoteN, x				;（ノート分割した時処理が途中で途切れるため）
		beq end1
	@N:
		lda NoteN, x
		clc
		adc KeyShift, x		;キーシフト値を加算
		sta NoteN, x
		sta RefNoteN, x		;ノートナンバーを記憶
		lda Device, x
		cmp #DEV_2A03_NOISE	;ノイズとDPCM以外は周波数計算へ
		beq rem
		cmp #DEV_2A03_DPCM
		beq rem
.ifdef SS5B
		cmp #DEV_SS5B_SQR3 + 1
		bcs @N2
		cmp #DEV_SS5B_SQR1
		bcc @N2
		sec
		sbc #DEV_SS5B_SQR1
		tay
		lda SS5BTone, y
		beq @N2
		lda NoteN, x
		and #$1f
		sta Work
		lda #$1f
		sec
		sbc Work
		sta NoteN, x
		sta RefNoteN, x
		jmp end1
	@N2:
.endif
		jmp calcoct
	rem:
		lda NoteN, x
		and #$0f
		sta Work
		lda #$0f
		sec
		sbc Work
		sta NoteN, x
		sta RefNoteN, x
	end1:
		rts
	calcoct:
		lda NoteN, x		;周波数計算
		jsr calcfreq		;Work + 2とWork + 3に入って帰ってくる
	last:
		lda Detune, x		;0でなければデチューン値を加算
		beq end2
		bmi neg
		clc
		adc Work + 2
		sta Work + 2
		bcc end2
		inc Work + 3
		jmp end2
	neg:
		clc
		adc Work + 2
		sta Work + 2
		bcs end2
		dec Work + 3
	end2:
		lda Work + 2
		sta Freq_L, x
		sta RefFreq_L, x
		lda Work + 3
		sta Freq_H, x
		sta RefFreq_H, x
		rts
.endproc


;ノートナンバーから周波数を計算する
;a=ノートナンバー
.proc calcfreq
		ldy #0
	oct:
		cmp #12
		bcc load
		sec
		sbc #12
		iny
		jmp oct
	;@N:
		;dey				;ノートナンバー0は-1オクターブなので1オクターブ下げる
		;bpl load
		;ldy #0			;マイナスになったらゼロに
	load:;周波数テーブルから周波数を取得
		sty Work + 4
		asl
		tay
		lda Device, x
		cmp #DEV_VRC6_SAW
		beq saw
		cmp #DEV_SS5B_SQR1
		bcs ss5b
		cmp #DEV_FDS
		beq fds
		lda Freq_Tbl, y
		sta Work + 2
		lda Freq_Tbl + 1, y
		sta Work + 3
		jmp calc
	saw:
.ifdef	VRC6
		lda Freq_Saw, y
		sta Work + 2
		lda Freq_Saw + 1, y
		sta Work + 3
		jmp calc
.endif
	ss5b:
.ifdef SS5B
		inc Work + 4	;5Bは-1オクターブから
		lda Freq_5B, y
		sta Work + 2
		lda Freq_5B + 1, y
		sta Work + 3
		jmp calc
.endif
	fds:
.ifdef FDS
		lda Freq_FDS, y
		sta Work + 2
		lda Freq_FDS + 1, y
		sta Work + 3
		lda Work + 4	;オクターブから周波数を計算する(FDSは周波数と比例なので6オクターブから)
		cmp #6
		bcc @N
		lda #6
		sta Work + 4
		jmp end
	@N:
		lda #6
		sec
		sbc Work + 4
		tay
	@L:
		beq end
		lsr Work + 3
		ror Work + 2
		dey
		jmp @L
.endif
	calc:
		ldy Work + 4	;オクターブから周波数を計算する
	@L:
		beq end
		lsr Work + 3
		ror Work + 2
		dey
		jmp @L
	end:
		rts
.endproc


; ------------------------------------------------------------------------
; エンベロープ処理
; ------------------------------------------------------------------------
.proc envelope
	start:
		lda Device, x
		cmp #$ff
		beq next			;未使用トラックは処理しない
		stx ProcTr
		lda EnvFrags, x
		and #FRAG_ENV_DIS	;エンベロープ無効フラグが立っていたら音量処理へ
		bne vol
	@N0:
		lda EnvFrags, x
		and #FRAG_NENV
		beq @N1
		jsr noteenv
	@N1:
		lda EnvFrags, x
		and #FRAG_SSWP
		beq @N2
		jsr ssweep
	@N2:
		lda EnvFrags, x
		and #FRAG_FENV
		beq @N3
		jsr freqenv
	@N3:
		lda EnvFrags, x
		and #FRAG_TENV
		beq @N4
		jsr toneenv
	@N4:
		lda EnvFrags, x
		and #FRAG_VENV
		beq vol
		jsr volenv
		jsr calc_volume
		jmp next
	vol:
		lda Frags, x
		and #FRAG_IS_KEYON	;キーオフされていたら無音に
		beq ld0				;それ以外は最大値をロード
		lda Device, x
		cmp #DEV_VRC6_SAW
		beq ld63
		cmp #DEV_FDS
		beq ld63
		lda #15
		jmp calc
	ld0:
		lda #0
		jmp calc
	ld63:
		lda #63
	calc:
		sta Volume, x
		jsr calc_volume
	next:
		dex
		bpl start			;xがマイナスになったら全トラック終了
		ldx LastTrack
		jmp writereg		;割り込み処理に移行
.endproc


;ソフトウェアスイープ
.proc ssweep
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq start
		rts
	keyon:
		lda NoteN, x
		clc
		adc SSwpEnd, x			;スイープ終了周波数を計算
		jsr calcfreq
		lda Work + 2
		sta SSwpEndFreq_L, x
		lda Work + 3
		sta SSwpEndFreq_H, x
		lda SSwpDelay, x
		clc						;カウンタにディレイ値を加算
		adc SSwpCtr, x
		sta SSwpCtr, x
		rts
	start:
		lda SSwpCtr, x
		beq exec				;カウンタが0でなければデクリメントして抜ける
		dec SSwpCtr, x
		rts
	exec:
		lda SSwpDepth, x
		bmi @M
		lda RefFreq_L, x			;周波数に加算
		clc
		adc SSwpDepth, x
		sta RefFreq_L, x
		bcc detect
		inc RefFreq_H, x
		jmp detect
	@M:							;マイナス値の場合
		lda RefFreq_L, x			;周波数に加算
		clc
		adc SSwpDepth, x
		sta RefFreq_L, x
		bcs detect
		dec RefFreq_H, x
	detect:
		lda SSwpEnd, x			;終了判定
		bpl pos
		lda SSwpEndFreq_H, x	;変化がマイナス方向の場合
		cmp RefFreq_H, x
		bcc end					;終了値より大きくなったら終了
		bne next				;終了値より小さかったら次へ
		lda RefFreq_L, x
		cmp SSwpEndFreq_L, x			;下位バイトも比較
		bcc next
		jmp end
	pos:
		lda SSwpEndFreq_H, x			;変化がプラス方向の場合
		cmp RefFreq_H, x
		bcc next				;終了値より大きかったら次へ
		bne end					;終了値より小さくなったら終了（レジスタ値が小さい方が高いので）
		lda SSwpEndFreq_L, x
		cmp RefFreq_L, x
		bcc next
		jmp end
	next:
		lda RefFreq_L, x
		sta Freq_L, x
		lda RefFreq_H, x
		sta Freq_H, x
		lda SSwpRate, x			;Rateをカウンタに代入して抜ける
		sta SSwpCtr, x
		rts
	end:
		lda SSwpEndFreq_L, x	;終了値を代入
		sta RefFreq_L, x
		sta Freq_L, x
		lda SSwpEndFreq_H, x
		sta RefFreq_H, x
		sta Freq_H, x
		rts
.endproc


;音量エンベロープ
.proc volenv
		lda VEnvAddr_L, x
		sta Work
		lda VEnvAddr_H, x
		sta Work + 1
		lda Frags, x
		and #FRAG_KEYOFF
		bne keyoff
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq other
		rts
	keyon:
		lda #1
		sta VEnvPos, x		;キーオン位置に移動
		clc
		adc VEnvDelay, x	;ディレイを加算
		sta VEnvCtr, x
		jmp other
	keyoff:
		ldy #1
		lda (Work), y
		sta VEnvPos, x		;キーオフ位置に移動
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq get
	other:
		lda VEnvCtr, x
		cmp #2
		bcs end				;カウンタが2以上ならカウントして終了
		cmp #0				;カウンタが0になったらrts
		beq ret
		lda VEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta VEnvPos, x
	get:
		lda VEnvPos, x
		asl a
		tay
		lda (Work), y	;アドレスにあるデータを取得（音量）
		sta Volume, x		;いったん保存
		iny
		lda (Work), y	;アドレスにあるデータを取得（フレーム数）
		sta VEnvCtr, x		;カウンタに代入
		beq ret			;カウンタが0ならエンベロープ位置を移動しない
		inc VEnvPos, x		;エンベロープ位置移動
		rts
	end:
		dec VEnvCtr, x
	ret:
		rts
.endproc


;音量計算
.proc calc_volume
		lda TrVolume, x
		beq store		;トラックボリュームが0ならボリュームを0にして終了
		cmp #15			;15（最大）ならそのまま終了
		beq end
		sta Work + 2
		lda Volume, x
		beq end			;ボリュームが0ならそのまま終了
		cmp #16
		bcs mult		;16以上は乗算
						;15以下はテーブルから引く
		asl
		asl
		asl
		asl
		ora Work + 2
		lsr
		tay
		lda Work + 2
		and #1
		beq even
		lda Vol_Tbl, y
		and #$0f
		sta Volume, x
		rts
	even:
		lda Vol_Tbl, y
		lsr
		lsr
		lsr
		lsr
		sta Volume, x
		rts
	mult:
		ldy TrVolume, x
		iny					;16で割る都合上1を足す
		jsr mult			;a * y
		lsr Work + 3		;16で割る
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		lda Work + 2
		cmp #1				;3bit右シフトした時点で1の場合そのまま終了（四捨五入）
		beq store
		lsr Work + 3
		ror Work + 2
		lda Work + 2
	store:
		sta Volume, x
	end:
		rts
.endproc


;音程エンベロープ
.proc freqenv
		lda FEnvAddr_L, x
		sta Work
		lda FEnvAddr_H, x
		sta Work + 1
		lda Frags, x
		and #FRAG_KEYOFF
		bne keyoff
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq other
		rts
	keyon:
		lda #1
		sta FEnvPos, x		;キーオン位置に移動
		clc
		adc FEnvDelay, x	;ディレイを加算
		sta FEnvCtr, x
		jmp other
	keyoff:
		ldy #1
		lda (Work), y
		sta FEnvPos, x		;キーオフ位置に移動
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq get
	other:
		lda FEnvCtr, x
		cmp #2
		bcs end				;カウンタが2以上ならカウントして終了
		cmp #0				;カウンタが0になったらrts
		beq ret
		lda FEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta FEnvPos, x
	get:
		lda FEnvPos, x
		asl a
		tay
		lda (Work), y	;アドレスにあるデータを取得
		eor #$ff
		clc
		adc #1				;符号反転
		bmi neg				;負の値だったら
		clc
		adc RefFreq_L, x	;周波数に加算
		sta Freq_L, x
		lda RefFreq_H, x
		adc #0
		sta Freq_H, x
		jmp next
	neg:
		clc
		adc RefFreq_L, x	;周波数に加算
		sta Freq_L, x
		lda RefFreq_H, x
		sbc #0
		sta Freq_H, x
	next:
		iny
		lda (Work), y	;アドレスにあるデータを取得（フレーム数）
		sta FEnvCtr, x		;カウンタに代入
		beq @S				;カウンタが0ならエンベロープ位置を移動しない
		inc FEnvPos, x		;エンベロープ位置移動
	@S:
		rts
	end:
		dec FEnvCtr, x
	ret:
		rts
.endproc


;ノートエンベロープ
.proc noteenv
		lda NEnvAddr_L, x
		sta Work
		lda NEnvAddr_H, x
		sta Work + 1
		lda Frags, x
		and #FRAG_KEYOFF
		bne keyoff
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq other
		rts
	keyon:
		lda #1
		sta NEnvPos, x		;キーオン位置に移動
		clc
		adc NEnvDelay, x	;ディレイを加算
		sta NEnvCtr, x
		jmp other
	keyoff:
		ldy #1
		lda (Work), y
		sta NEnvPos, x		;キーオフ位置に移動
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq get
	other:
		lda NEnvCtr, x
		cmp #2
		bcs end				;カウンタが2以上ならカウントして終了
		cmp #0				;カウンタが0になったらrts
		beq ret
		lda NEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta NEnvPos, x
	get:
		lda NEnvPos, x
		asl a
		pha
		lda Device, x
		cmp #DEV_2A03_NOISE
		beq @N
		cmp #DEV_2A03_DPCM
		beq @N
.ifdef SS5B
		cmp #DEV_SS5B_SQR3 + 1
		bcs @N2
		cmp #DEV_SS5B_SQR1
		bcc @N2
		sec
		sbc #DEV_SS5B_SQR1
		tay
		lda SS5BTone, y
		beq @N2
		jmp @N
	@N2:
.endif
		pla
		tay
		lda (Work), y	;アドレスにあるデータを取得
		clc
		adc RefNoteN, x		;ノートナンバーに加算
		sta NoteN, x
		jsr calcfreq		;周波数計算
		lda Work + 2
		sta RefFreq_L, x
		sta Freq_L, x
		lda Work + 3
		sta RefFreq_H, x
		sta Freq_H, x
		lda NEnvPos, x
		asl a
		tay
		jmp last
	@N:
		pla
		tay
		lda (Work), y	;アドレスにあるデータを取得
		eor #$ff			;反転して加算
		clc
		adc #1
		clc
		adc RefNoteN, x
		sta NoteN, x
	last:
		iny
		lda (Work), y	;アドレスにあるデータを取得（フレーム数）
		sta NEnvCtr, x		;カウンタに代入
		beq @S				;カウンタが0ならエンベロープ位置を移動しない
		inc NEnvPos, x		;エンベロープ位置移動
	@S:
		rts
	end:
		dec NEnvCtr, x
	ret:
		rts
.endproc


;音色エンベロープ
.proc toneenv
		lda TEnvAddr_L, x
		sta Work
		lda TEnvAddr_H, x
		sta Work + 1
		lda Frags, x
		and #FRAG_KEYOFF
		bne keyoff
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq other
		rts
	keyon:
		lda #1
		sta TEnvPos, x		;キーオン位置に移動
		clc
		adc TEnvDelay, x	;ディレイを加算
		sta TEnvCtr, x
		jmp other
	keyoff:
		ldy #1
		lda (Work), y
		sta TEnvPos, x		;キーオフ位置に移動
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq get
	other:
		lda TEnvCtr, x
		cmp #2
		bcs end				;カウンタが2以上ならカウントして終了
		cmp #0				;カウンタが0になったらrts
		beq ret
		lda TEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta TEnvPos, x
	get:
		lda TEnvPos, x
		asl a
		tay
		lda (Work), y		;アドレスにあるデータを取得
		clc
		sta Tone, x			;代入
		iny
		lda (Work), y		;アドレスにあるデータを取得（フレーム数）
		sta TEnvCtr, x		;カウンタに代入
		beq @S				;カウンタが0ならエンベロープ位置を移動しない
		inc TEnvPos, x		;エンベロープ位置移動
	@S:
		rts
	end:
		dec TEnvCtr, x
	ret:
		rts
.endproc


; ------------------------------------------------------------------------
; レジスタ書き込み
; ------------------------------------------------------------------------
.proc writereg
		stx Work
	start:
		lda Device, x
		cmp #$ff
		beq next			;未使用トラックは処理しない
		cmp PrevDev			;前の音源と違う場合書き込み処理に移行
		bne exec
		lda Volume, x		;同じ場合、発音していれば書き込む
		beq next
		lda Frags, x
		and #FRAG_END
		bne next
		stx Work
	next:
		cpx #0
		beq exec
		lda Device, x
		sta PrevDev
		dex
		bpl start			;xがマイナスになったら全トラック終了
		rts
	exec:
		ldx Work
		stx ProcTr
		lda Device, x
		sta PrevDev

	int_sqr1:
		cmp #DEV_2A03_SQR1
		bne int_sqr2
		ldy #0
		jmp writesqr

	int_sqr2:
		cmp #DEV_2A03_SQR2
		bne int_tri
		ldy #4
		jmp writesqr

	int_tri:
		cmp #DEV_2A03_TRI
		bne int_noise
		lda Freq_L, x
		sta $400a
		lda Freq_H, x
		sta $400b
		lda Volume, x
		beq @stop
		lda #$ff
		jmp @write
	@stop:
		lda #$80
	@write:
		sta $4008
		jmp writereg_end

	int_noise:
		cmp #DEV_2A03_NOISE
		bne int_dpcm
		lda HEnvReg, x
		and #%00010000		;ハードウェアエンベロープが有効なら以下を実行
		bne @softenv
		lda Frags, x
		and #FRAG_KEYOFF
		bne @softenv
		lda Frags, x
		and #FRAG_KEYON
		beq @r400e
		lda #%00001000
		sta $400f
		lda HEnvReg, x
		jmp @r400c
	@softenv:
		lda #%00001000
		sta $400f
		lda #%00110000
		ora Volume, x
	@r400c:
		sta $400c
		lda Volume, x		;音量が0ならこれ以降は処理しない
		beq @end
	@r400e:
		lda Tone, x
		clc
		ror a
		ror a
		ora NoteN, x
		sta $400e
	@end:
		jmp writereg_end

	int_dpcm:
		cmp #DEV_2A03_DPCM
		bne ext
		lda Frags, x
		and #FRAG_KEYON | FRAG_KEYOFF	;キーオンもキーオフもたっていなければ終了
		beq @end
		lda Frags, x
		and #FRAG_KEYOFF	;キーオフが立っていたら再生終了
		beq @play
		lda #%00001111
		sta $4015
		jmp @end
	@play:
		lda NoteN, x
		sta $4010
		lda Volume, x
		bmi @N
		sta $4011
	@N:
		lda #%00001111
		sta $4015
		lda #%00011111
		sta $4015
	@end:
		jmp writereg_end

	ext:
.ifdef VRC6
	vrc6_sqr1:
		cmp #DEV_VRC6_SQR1
		bne vrc6_sqr2
		ldy #$90
		jmp write_vrc6

	vrc6_sqr2:
		cmp #DEV_VRC6_SQR2
		bne vrc6_saw
		ldy #$a0
		jmp write_vrc6

	vrc6_saw:
		cmp #DEV_VRC6_SAW
		bne vrc6_end
		ldy #$b0
		jmp write_vrc6
	vrc6_end:
.endif

.ifdef MMC5
	mmc5_sqr1:
		cmp #DEV_MMC5_SQR1
		bne mmc5_sqr2
		ldy #0
		jmp write_mmc5

	mmc5_sqr2:
		cmp #DEV_MMC5_SQR2
		bne mmc5_end
		ldy #4
		jmp write_mmc5
	mmc5_end:
.endif

.ifdef SS5B
	ss5b_sqr1:
		cmp #DEV_SS5B_SQR1
		bne ss5b_sqr2
		ldy #0
		jmp write_ss5b

	ss5b_sqr2:
		cmp #DEV_SS5B_SQR2
		bne ss5b_sqr3
		ldy #1
		jmp write_ss5b

	ss5b_sqr3:
		cmp #DEV_SS5B_SQR3
		bne ss5b_end
		ldy #2
		jmp write_ss5b
	ss5b_end:
.endif

.ifdef FDS
	fds:
		cmp #DEV_FDS
		bne fds_end
		jmp write_fds
	fds_end:
.endif
		jmp writereg_end
.endproc


;1トラック書き込み終了
.proc writereg_end
		lda Volume, x		;書き込んだ周波数の保存
		beq @N
		ldy Device, x
		lda Freq_L, x
		sta PrevFreq_L, y
		lda Freq_H, x
		sta PrevFreq_H, y
	@N:
		dex
		bmi end
		lda Device, x
		cmp PrevDev
		beq @N
		sta PrevDev
		jmp writereg
	end:
		rts
.endproc


.proc writesqr
		sty Work + 1		;一旦yを保存
		lda Tone, x
		clc
		ror
		ror
		ror
		sta Work
		lda HEnvReg, x
		and #%00010000		;ハードウェアエンベロープが有効かどうか
		bne softenv
		;ハードウェアエンベロープ処理
		lda Frags, x
		and #FRAG_KEYOFF
		bne hweoff
		lda Frags, x
		and #FRAG_KEYON
		beq hws
		lda Work
		ora HEnvReg, x
		sta $4000, y
		jmp hws
	hweoff:
		lda Work
		ora #%00110000
		ora Volume, x
		sta $4000, y
		jmp r4003
	softenv:
		lda Work
		ora #%00110000
		ora Volume, x
	r4000:
		sta $4000, y
		lda Volume, x		;音量が0ならこれ以降は処理しない
		beq end
	hws:
		ldy Work + 1
		lda HSwpReg, x
		and #%10000000		;ハードウェアスイープ有効なら以下を実行
		beq r4002
		lda Frags, x
		and #FRAG_KEYON
		beq end
	r4002:
		lda HSwpReg, x
		sta $4001, y
		lda Freq_L, x
		ldy Work + 1
		sta $4002, y
		lda Frags, x
		and #FRAG_KEYON		;キーオンなら
		bne r4003
		lda Freq_H, x
		ldy Device, x
		cmp PrevFreq_H, y
		beq end
	r4003:
		ldy Work + 1
		lda #%00001000
		ora Freq_H, x
		sta $4003, y		;ここに書き込むと波形がリセットされるので注意
	end:
		jmp writereg_end
.endproc


;-----------------------------------------------------------------------
; 拡張音源
;-----------------------------------------------------------------------
;VRC6
.ifdef VRC6
.proc write_vrc6
		sty Work + 1		;yにレジスタの上位アドレスが入ってくるので保存
		lda #0
		sta Work
		cpy #$b0			;sawトラックは別処理
		beq saw
	r9000:
		ldy #0
		lda Tone, x
		asl
		asl
		asl
		asl
		ora Volume, x
		sta (Work), y
		jmp next
	saw:
		ldy #0
		lda Volume, x
		sta (Work), y
	next:
		lda Volume, x		;音量が0ならこれ以降は処理しない
		beq end
	r9001:
		lda #1
		sta Work
		lda Freq_L, x
		sta (Work), y
	r9002:
		lda #2
		sta Work
		lda Frags, x
		and #FRAG_KEYON		;キーオンなら
		beq @N
		lda #0				;いったん0書き込み
		sta (Work), y
	@N:
		lda #%10000000
		ora Freq_H, x
		sta (Work), y
	end:
		jmp writereg_end
.endproc
.endif

;MMC5
.ifdef MMC5
.proc write_mmc5
		sty Work		;一旦yを保存
		lda Tone, x
		clc
		ror
		ror
		ror
		ora #%00110000
		ora Volume, x
	r5000:
		sta $5000, y
		lda Volume, x		;音量が0ならこれ以降は処理しない
		bne next
		jmp end
	next:
		lda Frags, x
		and #FRAG_KEYON		;キーオンなら
		bne r5003
	r5002:
		ldy Device, x
		lda Freq_L, x
		cmp PrevFreq_L, y
		beq end
		ldy Work
		sta $5002, y
		lda Freq_H, x
		ldy Device, x
		cmp PrevFreq_H, y
		bne r5003
		jmp end
	r5003:
		lda Freq_L, x
		ldy Work
		sta $5002, y
		lda #%00001000
		ora Freq_H, x
		sta $5003, y		;ここに書き込むと波形がリセットされるので注意
	end:
		jmp writereg_end
.endproc
.endif

;SS5B
.ifdef SS5B
.proc write_ss5b
		tya
		clc
		adc #$08
		sta $c000
		lda SS5BHWEnv, y
		beq @N
		lda #%00010000
	@N:
		ora Volume, x
		sta $e000
		tya
		asl a
		tay
		sty $c000
		lda Freq_L, x
		sta $e000
		iny
		sty $c000
		lda Freq_H, x
		sta $e000
		lda SS5BTone, y
		beq end
		lda #$06
		sta $c000
		lda NoteN, x
		sta $e000
	end:
		jmp writereg_end
.endproc
.endif

;FDS
.ifdef FDS
.proc write_fds
		lda FdsPrevWav
		cmp Tone, x
		beq mod				;前回書き込んだ音色と同じならスキップ
		lda #%10000000
		sta $4089			;Wavetable書き込み許可
		lda #64
		sta Work + 2
		ldy Tone, x
		lda FdsWavAddr_H	;波形アドレス計算
		sta Work + 1
		lda FdsWavAddr_L
		sta Work
		jsr fds_addr
		ldy #63				;波形書き込み
	@W:
		lda (Work), y
		sta $4040, y
		dey
		bpl @W
		lda #0
		sta $4089			;Wavetable書き込み禁止
	mod:
		lda FdsPrevMod
		cmp FdsModTone
		beq vol				;前回書き込んだ音色と同じならスキップ
		lda #%10000000
		sta $4087			;Mod停止
		lda #16
		sta Work + 2
		ldy FdsModTone
		lda FdsModAddr_H	;波形アドレス計算
		sta Work + 1
		lda FdsModAddr_L
		sta Work
		jsr fds_addr
		ldy #0				;波形書き込み
	@W:
		lda (Work), y		;モジュレーション波形は3bitデータが
		and #%00001111		;下位4bit→上位4bitの順で格納されている
		sta $4088
		lda (Work), y
		lsr
		lsr
		lsr
		lsr
		sta $4088
		iny
		cpy #16
		bcc @W
		lda #0
		sta $4085			;カウンタリセット
		sta $4087			;Mod有効
	vol:
		lda Tone, x			;音色番号を保存
		sta FdsPrevWav
		lda FdsModTone
		sta FdsPrevMod
		lda HEnvReg, x
		and #%10000000		;ハードウェアエンベロープが有効なら以下を実行
		bne softenv
		lda Frags, x
		and #FRAG_KEYOFF
		bne hweoff
		lda Frags, x
		and #FRAG_KEYON
		beq freq
		lda Volume, x
		ora #%10000000
		sta $4080
		lda HEnvReg, x
		sta $4080
		jmp freq
	hweoff:
		lda #%10000000
		ora Volume, x
		sta $4080
		jmp r4083
	softenv:
		lda #%10000000
		ora Volume, x
		sta $4080
		lda Volume, x		;音量が0ならこれ以降は処理しない
		beq end
	freq:
		lda FdsModFreq_H
		and #%10000000		;最上位bitにフラグが立っていたら同期
		bne fsync
		lda FdsModFreq_L	;周波数書き込み
		sta $4086
		lda FdsModFreq_H
		sta $4087
		lda Freq_L, x
		sta $4082
		lda Frags, x
		and #FRAG_KEYON		;キーオンなら
		bne r4083
		lda Freq_H, x
		ldy Device, x
		cmp PrevFreq_H, y
		beq end
	r4083:
		lda Freq_H, x
		sta $4083			;ここに書き込むと波形がリセットされるので注意
		jmp end
	fsync:
		lda Freq_L, x		;周波数書き込み
		sta $4082
		sta $4086
		lda Frags, x
		and #FRAG_KEYON		;キーオンなら
		bne @N
		lda Freq_H, x
		ldy Device, x
		cmp PrevFreq_H, y
		beq end
	@N:
		lda Freq_H, x
		sta $4083			;ここに書き込むと波形がリセットされるので注意
		sta $4087
	end:
		jmp writereg_end
.endproc


;FDSの波形アドレス計算
.proc fds_addr
	@L:
		dey
		bmi @E
		clc
		adc Work + 2
		bcc @L
		inc Work + 1
		jmp @L
	@E:
		sta Work
		rts
.endproc
.endif


;ポインタをa個進める
.proc addptr
		clc
		adc Ptr_L, x
		sta Ptr_L, x
		bcc @E
		inc Ptr_H, x
	@E:
		rts
.endproc


;ループ値のあるアドレスのオフセットを計算してyにセット
.proc loopoffset
		lda LoopDepth, x
		sec
		sbc #1				;深度1がメモリ0の位置なので1引く
		ldx ProcTr			;トラック0なら終了
		beq @E
	@L:
		clc
		adc #MAX_LOOP
		dex
		bne @L
	@E:
		ldx ProcTr
		tay
		rts
.endproc


;乗算（a * y）
.proc mult
		sty Work + 4
		sta Work + 5
		lda #0
		sta Work + 6
		sta Work + 2
		sta Work + 3
		ldy #8
	loop:
		lsr Work + 4
		bcc next
		lda Work + 2
		clc
		adc Work + 5
		sta Work + 2
		lda Work + 3
		adc Work + 6
		sta Work + 3
	next:
		asl Work + 5
		rol Work + 6
		dey
		bne loop
	end:
		rts
.endproc


.rodata

Vol_Tbl:
	;.byte $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0
	;.byte $0, $0, $0, $0, $0, $0, $0, $1, $1, $1, $1, $1, $1, $1, $1, $1
	;.byte $0, $0, $0, $0, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $2
 	;.byte $0, $0, $0, $1, $1, $1, $1, $1, $1, $1, $2, $2, $2, $2, $2, $3
 	;.byte $0, $0, $1, $1, $1, $1, $1, $1, $2, $2, $2, $2, $3, $3, $3, $4
 	;.byte $0, $0, $1, $1, $1, $1, $2, $2, $2, $3, $3, $3, $4, $4, $4, $5
 	;.byte $0, $0, $1, $1, $1, $2, $2, $2, $3, $3, $4, $4, $4, $5, $5, $6
 	;.byte $0, $1, $1, $1, $1, $2, $2, $3, $3, $4, $4, $5, $5, $6, $6, $7
 	;.byte $0, $1, $1, $1, $2, $2, $3, $3, $4, $4, $5, $5, $6, $6, $7, $8
 	;.byte $0, $1, $1, $1, $2, $3, $3, $4, $4, $5, $6, $6, $7, $7, $8, $9
 	;.byte $0, $1, $1, $2, $2, $3, $4, $4, $5, $6, $6, $7, $8, $8, $9, $a
 	;.byte $0, $1, $1, $2, $2, $3, $4, $5, $5, $6, $7, $8, $8, $9, $a, $b
 	;.byte $0, $1, $1, $2, $3, $4, $4, $5, $6, $7, $8, $8, $9, $a, $b, $c
 	;.byte $0, $1, $1, $2, $3, $4, $5, $6, $6, $7, $8, $9, $a, $b, $c, $d
 	;.byte $0, $1, $1, $2, $3, $4, $5, $6, $7, $8, $9, $a, $b, $c, $d, $e
 	;.byte $0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $a, $b, $c, $d, $e, $f

	.byte	$00, $00, $00, $00, $00, $00, $00, $00
	.byte	$00, $00, $00, $01, $11, $11, $11, $11
	.byte	$00, $00, $11, $11, $11, $11, $11, $12
 	.byte	$00, $01, $11, $11, $11, $22, $22, $23
 	.byte	$00, $11, $11, $11, $22, $22, $33, $34
 	.byte	$00, $11, $11, $22, $23, $33, $44, $45
 	.byte	$00, $11, $12, $22, $33, $44, $45, $56
 	.byte	$01, $11, $12, $23, $34, $45, $56, $67
 	.byte	$01, $11, $22, $33, $44, $55, $66, $78
 	.byte	$01, $11, $23, $34, $45, $66, $77, $89
 	.byte	$01, $12, $23, $44, $56, $67, $88, $9a
 	.byte	$01, $12, $23, $45, $56, $78, $89, $ab
 	.byte	$01, $12, $34, $45, $67, $88, $9a, $bc
 	.byte	$01, $12, $34, $56, $67, $89, $ab, $cd
 	.byte	$01, $12, $34, $56, $78, $9a, $bc, $de
 	;.byte	$01, $23, $45, $67, $89, $ab, $cd, $ef


Freq_Tbl:
	.word	$1a7f
	.word	$18ff
	.word	$177f
	.word	$163d
	.word	$14f9
	.word	$13de
	.word	$12bd
	.word	$11bf
	.word	$10be
	.word	$0fbb
	.word	$0ed7
	.word	$0df6

.ifdef VRC6
Freq_Saw:
	.word	$1e6c
	.word	$1ca2
	.word	$1b18
	.word	$1991
	.word	$1815
	.word	$16ba
	.word	$1584
	.word	$144f
	.word	$1315
	.word	$1214
	.word	$1110
	.word	$1010
.endif

.ifdef SS5B
Freq_5B:
	.word	$1ab9
	.word	$1935
	.word	$17ce
	.word	$1675
	.word	$1531
	.word	$1402
	.word	$12e1
	.word	$11d4
	.word	$10d3
	.word	$0fdf
	.word	$0efc
	.word	$0e24
.endif

.ifdef FDS
Freq_FDS:
	.word	$09a4
	.word	$0a36
	.word	$0ad1
	.word	$0b74
	.word	$0c22
	.word	$0cda
	.word	$0d9c
	.word	$0e6b
	.word	$0f45
	.word	$102d
	.word	$1122
	.word	$1226
.endif