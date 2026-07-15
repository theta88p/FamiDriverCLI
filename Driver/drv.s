;FamiDriverCLI NES Sound Driver v0.4.0

.exportzp	Frags
.export		DrvFrags
.export		Device
.export		NoteN
.export		Volume
.export		Tone
.export		Freq_L
.export		Freq_H
.export		Freq_X
.export		ActTbl
.export		drv_main
.export		drv_init
.export		drv_sndreq
.export		DrvBankedMode
.ifdef N163
.export		N163ChCount
.export		N163FreqShift
.endif

.include	"drv.inc"

;-----------------------------------------------------------------------
; Zeropage works
;-----------------------------------------------------------------------
.zeropage

Frags:			.res	MAX_TRACK	;通常のフラグ
EnvFrags:		.res	MAX_TRACK	;エンベロープのフラグ
Work:			.res	10

;-----------------------------------------------------------------------
; Non Zeropage works
;-----------------------------------------------------------------------
.bss

Device:			.res	MAX_TRACK	;トラックで使用している音源
Ptr_L:			.res	MAX_TRACK	;再生箇所のアドレスL
Ptr_H:			.res	MAX_TRACK	;再生箇所のアドレスH
TrackBank:		.res	MAX_TRACK	;NSFバンク切り替え用のトラックバンク
LenCtr:			.res	MAX_TRACK	;音長カウンタ
GateCtr:		.res	MAX_TRACK	;ゲートカウンター
NoteN:			.res	MAX_TRACK	;ノートナンバー
DefLen:			.res	MAX_TRACK	;デフォルト音長
Gate:			.res	MAX_TRACK	;上位2bit:使用中のゲートコマンド 下位6bit:ゲートコマンドの値
TrVolume:		.res	MAX_TRACK	;トラック音量
Volume:			.res	MAX_TRACK	;音量
Tone:			.res	MAX_TRACK	;上位4bit:元の音色 下位4bit:現在の音色
Freq_L:			.res	MAX_TRACK	;周波数L
Freq_H:			.res	MAX_TRACK	;周波数H
Freq_X:			.res	MAX_TRACK	;周波数上位
RefFreq_L:		.res	MAX_TRACK	;音程エンベロープ値を加算する前の周波数L
RefFreq_H:		.res	MAX_TRACK	;音程エンベロープ値を加算する前の周波数H
RefFreq_X:		.res	MAX_TRACK
RefNoteN:		.res	MAX_TRACK	;ノートエンベロープ値を加算する前のノートナンバー
PrevFreq_L:		.res	MAX_DEVICE	;前回レジスタに書き込んだ周波数L（音源ごとに保存）
PrevFreq_H:		.res	MAX_DEVICE	;前回レジスタに書き込んだ周波数H（音源ごとに保存）
KeyShift:		.res	MAX_TRACK	;キーシフト値
Detune:			.res	MAX_TRACK	;デチューン値
InfLoopAddr_L:	.res	MAX_TRACK	;無限ループの戻り先L
InfLoopAddr_H:	.res	MAX_TRACK	;無限ループの戻り先H
SSwpEndHT:		.res	MAX_TRACK	;ソフトウェアスイープの終了音程（+-半音単位）
SSwpDelay:		.res	MAX_TRACK	;ソフトウェアスイープのディレイ
SSwpDepth:		.res	MAX_TRACK	;ソフトウェアスイープの一回に加算する値
SSwpRate:		.res	MAX_TRACK	;ソフトウェアスイープで何フレームおきに加算するか
SSwpCur_L:		.res	MAX_TRACK	;ソフトウェアスイープの現在値（相対値）
SSwpCur_H:		.res	MAX_TRACK	;ソフトウェアスイープの現在値（相対値）
SSwpCur_X:		.res	MAX_TRACK
SSwpEnd_L:		.res	MAX_TRACK	;ソフトウェアスイープの終了値（相対値）
SSwpEnd_H:		.res	MAX_TRACK	;ソフトウェアスイープの終了値（相対値）
SSwpEnd_X:		.res	MAX_TRACK
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
FEnvShift:		.res	MAX_TRACK
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

ActTbl:			.res	MAX_DEVICE	;デバイス番号から発音中トラックを引くテーブル
DrvFrags:		.res	1	;ドライバ全体のフラグ
SpdCtr:			.res	1	;速度カウンタ
SpdFreq:		.res	1	;速度カウンタに加算する値
ProcTr:			.res	1	;処理中のトラック
SeqAddr_L:		.res	1	;シーケンス情報のアドレスL
SeqAddr_H:		.res	1	;シーケンス情報のアドレスH
PrevDev:		.res	1	;前回の音源（レジスタ書き込み用）
LastTrack:		.res	1	;使用する最大トラック数 - 1
DrvBankedMode:	.res	1	;0:従来形式 1:NSFトラックバンク形式

.ifdef SS5B
SS5BTone:		.res	3
SS5BHWEnv:		.res	3	;ハードウェアエンベロープが有効なら1無効なら0
.endif

.ifdef N163
N163WavAddr_L:	.res	1
N163WavAddr_H:	.res	1
N163ChCount:	.res	1
N163ChOffset:	.res	1
N163ChReg:		.res	1
N163WaveOffset:	.res	MAX_TRACK
N163WaveLenReg:	.res	MAX_TRACK
N163FreqShift:	.res	MAX_TRACK
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
FdsModEnv:		.res	1	;モジュレータエンベロープの値
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
;ef	:vx/v+-x	ボリューム指定（絶対0～15、相対-15～15）
;f0	:@fsx	音程エンベロープシフト
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
;fc	:@n163c	N163発音数
;fd	:N163波形設定
;fe	:VRC7ユーザー音色設定

; ------------------------------------------------------------------------
; main
; ------------------------------------------------------------------------
.code

.proc drv_main
		lda DrvFrags
		and #DRV_IS_FREE_CLR
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
		bcc single
	acc:
		lda SpdCtr		;加速の場合
		clc
		adc SpdFreq
		sta SpdCtr
		bcc single		;桁上がりしたら二重処理
		ldx LastTrack
		jsr track		;トラック処理
		ldx LastTrack
		jsr envelope
		ldx LastTrack
		jsr writereg	;書き込みも2回しないとDPCMが発音しないタイミングがある
		ldx LastTrack
		jsr pretrack
		lda DrvFrags
		ora #DRV_DOUBLE
		sta DrvFrags
	single:
		ldx LastTrack
		jsr track		;トラック処理
	env:
		ldx LastTrack	;エンベロープと書き込み処理は毎フレームやる
		jsr envelope
		ldx LastTrack
		jsr writereg
		lda DrvFrags
		and #DRV_DOUBLE_CLR
		ora #DRV_IS_FREE
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
.ifdef VRC7
		ldy #$30
		lda #$0f
	@mute_vrc7:
		jsr vrc7_write
		iny
		cpy #$36
		bcc @mute_vrc7
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
.ifdef N163
		lda #0
		sta $e000
		sta N163ChOffset
		lda #8
		sta N163ChCount
		lda #$70
		sta N163ChReg
.endif
		lda #0
		sta ProcTr
		sta SpdFreq
		sta SpdCtr
		.if .defined(MMC3) .or .defined(VRC6) .or .defined(VRC7) .or .defined(MMC5) .or .defined(SS5B) .or .defined(N163)
		lda #2
		.else
		lda #0
		.endif
		sta DrvBankedMode
		lda #$ff				;↓初回必ず実行したいので$ffを書き込んでおく
		sta PrevDev
		sta PrevFreq_L
		sta PrevFreq_H
		lda DrvFrags
		ora #DRV_IS_FREE
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
		sta Frags, x
.ifdef VRC7
		lda Device, x
		cmp #DEV_VRC7_CH1
		bcc :+
		cmp #DEV_VRC7_CH6 + 1
		bcs :+
		lda #1
		sta Tone, x
	:
.endif
		lda #0
		sta FEnvShift, x
.ifdef N163
		sta N163FreqShift, x
		lda #$80
		sta N163WaveOffset, x
		lda #$e0
		sta N163WaveLenReg, x
.endif
		lda #1
		sta LenCtr, x
		sta GateCtr, x
		lda #FRAG_ENV_DIS
		sta EnvFrags, x
		lda #15
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
		lda DrvBankedMode
		cmp #2
		bne @address_ready
		lda #0
		sta Work
		sta SeqAddr_L
		.if .defined(VRC6) .or .defined(VRC7)
		lda #$80
		.elseif .defined(MMC3) .or .defined(MMC5) .or .defined(SS5B) .or .defined(N163)
		lda #$a0
		.else
		lda #$c0
		.endif
		sta Work + 1
		sta SeqAddr_H
		tya
		clc
		adc #2
		jsr select_dsp_bank
	@address_ready:
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
		lda DrvBankedMode
		beq @bank_done
		iny
		lda (Work), y
		sta TrackBank, x
	@bank_done:
		jsr track_init	;トラック初期化
		inx
		iny
		cpx #MAX_TRACK
		bcc loop
		dex
		stx LastTrack
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
.ifdef N163
		iny
		lda (Work), y
		clc
		adc SeqAddr_L
		sta N163WavAddr_L
		iny
		lda (Work), y
		adc SeqAddr_H
		sta N163WavAddr_H
		jsr n163_load_wave
.endif
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
		lda #$ff
		ldx #MAX_DEVICE - 1	;テーブル初期化
	:	sta ActTbl, x
		dex
		bpl :-
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
		jsr select_track_bank
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
		sta Frags, x		;キーオフしたらカウントして終了
	cnt:
		lda LenCtr, x
		beq next
		dec LenCtr, x
		lda GateCtr, x
		beq next
		dec GateCtr, x
		jmp next
	seq:
		jsr loadseq
		lda Frags, x
		and #FRAG_LOAD		;ロードフラグが立っていれば続けてロード
		bne seq
		lda Frags, x
		and #FRAG_END
		bne next			;終了フラグが立っていなければ処理へ
		jsr procnote
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
		cmp #$ff
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
		sta LenCtr, x
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
		sta LenCtr, x
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
		sta LenCtr, x
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
		sta Work + 6
		asl
		asl
		asl
		asl
		ora Work + 6
		sta Tone, x
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
		lda Tone, x			;上位4bitにある元の音色をロード
		and #$f0
		sta Work + 6
		lsr
		lsr
		lsr
		lsr
		ora Work + 6
		sta Tone, x
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
		sta LenCtr, x
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
		
	volume:					;ボリューム指定
		ldy #1
		lda (Work), y
		cmp #$10
		bcc @abs
		cmp #$20
		bcs @rel
		and #$0f
	@rel:
		clc
		adc TrVolume, x
		bpl @P
		lda #0
	@P:
		sta TrVolume, x
		lda #2
		jsr addptr
		rts
	@abs:
		sta TrVolume, x
		lda #2
		jsr addptr
		rts

	freq_env_shift:			;音程エンベロープシフト
		ldy #1
		lda (Work), y
		sta FEnvShift, x
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
		lda EnvFrags, x		;引数1はSpeedの符号と終了周波数（+-半音単位）。
							;引数2はDelayテーブル番号とSpeed値。
							;開始周波数はノートの方を変更する。
		ora #FRAG_SSWP		;フラグを立てる
		sta EnvFrags, x
		ldy #1
		lda (Work), y
		pha
		and #$7f
		cmp #$40
		bcc @pitch_positive
		ora #$80
	@pitch_positive:
		sta SSwpEndHT, x
		ldy #2
		lda (Work), y
		pha
		lsr a
		lsr a
		lsr a
		tay
		lda sw_sweep_delay_table, y
		sta SSwpDelay, x
		pla
		and #$07
		sta SSwpDepth, x
		pla
		bpl @shift_mode		;speed値がプラスならビットシフト量とする
		lda SSwpDepth, x
		sta SSwpRate, x		;マイナスなら値をRate、Depthを1とする
		lda #1
		sta SSwpDepth, x
		jmp @speed_ready
	@shift_mode:
		lda #$81			;上位ビットはシフトモード、下位はRate=1
		sta SSwpRate, x
	@speed_ready:
		lda #1
		sta SSwpCtr, x		;カウンタリセット
		lda Device, x
		cmp #DEV_FDS
		beq @fds
.ifdef VRC7
		cmp #DEV_VRC7_CH1
		bcc @not_vrc7
		cmp #DEV_VRC7_CH6 + 1
		bcc @fds			;VRC7も周波数値と音程の増減方向が同じ
	@not_vrc7:
.endif
		lda SSwpEndHT, x
		bmi @neg			;変化方向がプラスならDepthをマイナスにする。マイナスなら何もしない
		jmp @invert
	@fds:
		lda SSwpEndHT, x	;FDSは周波数値と音程の増減方向が同じ
		bpl @neg
	@invert:
		lda SSwpRate, x
		bmi @invert_shift
		lda SSwpDepth, x
		eor #$ff
		clc
		adc #1
		sta SSwpDepth, x
		jmp @neg
	@invert_shift:
		lda SSwpDepth, x
		ora #$80
		sta SSwpDepth, x
	@neg:
		lda #3
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
		
	fds_mod_freq:		;FDSモジュレータ周波数
.ifdef FDS
		ldy #1
		lda (Work), y
		sta FdsModFreq_L
		ldy #2
		lda (Work), y
		sta FdsModFreq_H
.endif
		lda #3
		jsr addptr
		rts
		
	fds_mod_tone:		;FDSモジュレータ番号
.ifdef FDS
		ldy #1
		lda (Work), y
		sta FdsModTone
.endif
		lda #2
		jsr addptr
		rts
		
	fds_mod_env:		;FDSモジュレータエンベロープ
.ifdef FDS
		ldy #1
		lda (Work), y
		sta FdsModEnv
		sta $4084
.endif
		lda #2
		jsr addptr
		rts

	n163_ch_count:		;N163発音数
.ifdef N163
		ldy #1
		lda (Work), y
		sta N163ChCount
		lda #8
		sec
		sbc N163ChCount
		sta N163ChOffset
		lda N163ChCount
		sec
		sbc #1
		asl
		asl
		asl
		asl
		sta N163ChReg
		jsr n163_mute_channels
		jsr n163_update_freq
.endif
		lda #2
		jsr addptr
		rts

	n163_wave_setup:	;N163波形設定
.ifdef N163
		ldy #1
		lda (Work), y
		sta N163WaveOffset, x
		ldy #2
		lda (Work), y
		sta N163WaveLenReg, x
		ldy #3
		lda (Work), y
		sta N163FreqShift, x
.endif
		lda #4
		jsr addptr
		rts

	vrc7_patch:		;VRC7ユーザー音色設定
.ifdef VRC7
		ldy #1
	@L:
		sty Work + 6
		lda (Work), y
		pha
		tya
		sec
		sbc #1
		tay
		pla
		jsr vrc7_write
		ldy Work + 6
		iny
		cpy #9
		bcc @L
.endif
		lda #9
		jsr addptr
		rts
		
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
		.word volume - 1
		.word freq_env_shift - 1
		.word detune - 1
		.word hw_sweep - 1
		.word hw_env - 1
		.word sw_sweep - 1
		.word sw_sweep_clear - 1
		.word disable_env - 1
		.word mem_write - 1
		.word subroutine - 1
		.word fds_mod_freq - 1
		.word fds_mod_tone - 1
		.word fds_mod_env - 1
		.word n163_ch_count - 1
		.word n163_wave_setup - 1
		.word vrc7_patch - 1
	upper_table_end:
	

.endproc

sw_sweep_delay_table:
	.byte 0, 1, 2, 3, 4, 5, 6, 7
	.byte 8, 9, 10, 11, 12, 13, 14, 15
	.byte 16, 18, 20, 22, 24, 28, 32, 36
	.byte 40, 48, 56, 64, 80, 96, 128, 255


; ------------------------------------------------------------------------
; ノート関係の処理
; ------------------------------------------------------------------------
.proc procnote
		lda Frags, x
		and #FRAG_IS_KEYON
		beq @S				;キーオフしていたらスキップ
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
	@S:
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
		jsr multiply		;a * y / 8
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
		bne @N
		rts
	@N:
		lda Frags, x
		and #FRAG_KEYON	;キーオンされていない
		bne note
		lda EnvFrags, x
		and #FRAG_SSWP | FRAG_FENV	;かつスイープか音程エンベロープ有効
		beq note
		lda RefNoteN, x				;かつノートが前回と同じならこれ以降は処理しない
		cmp NoteN, x				;（ノート分割した時処理が途中で途切れるため）
		bne note
		rts
	note:
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
		rts
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
		rts
	calcoct:
		lda NoteN, x		;周波数計算
		jsr calcfreq		;Work + 2とWork + 3 (N163はWork + 4も)に入って帰ってくる
		lda Detune, x		;0でなければデチューン値を加算
		beq end
		bmi neg
		clc
		adc Work + 2
		sta Work + 2
		bcc end
		inc Work + 3
		jmp end
	neg:
		clc
		adc Work + 2
		sta Work + 2
		bcs end
		dec Work + 3
	end:
		lda Work + 2
		sta Freq_L, x
		sta RefFreq_L, x
		lda Work + 3
		sta Freq_H, x
		sta RefFreq_H, x
		lda Work + 4
		sta Freq_X, x
		sta RefFreq_X, x
		rts
.endproc


;ノートナンバーから周波数を計算する
;a=ノートナンバー
.proc calcfreq
		ldy #0
		cmp #12
		bcc load
		cmp #60
		bcc oct
		ldy #5
		sec
		sbc #60
		jmp comp
	oct:
		sec
		sbc #12
		iny
	comp:
		cmp #12
		bcs oct
	load:
		sty Work + 8	;周波数テーブルから周波数を取得
		sta Work + 9
		lda Device, x
.ifdef VRC6
		cmp #DEV_VRC6_SAW
		beq saw
.endif
.ifdef VRC7
		cmp #DEV_VRC7_CH1
		bcs vrc7
.endif
.ifdef FDS
		cmp #DEV_FDS
		beq fds
.endif
.ifdef N163
		cmp #DEV_N163_CH1
		bcc :+
		jmp n163
	:
.endif
.ifdef SS5B
		cmp #DEV_SS5B_SQR1
		bcs ss5b
.endif
		lda Work + 9
		asl
		tay
		lda Freq_Tbl, y
		sta Work + 2
		lda Freq_Tbl + 1, y
		sta Work + 3
		jmp calc
	saw:
.ifdef	VRC6
		lda Work + 9
		asl
		tay
		lda Freq_Saw, y
		sta Work + 2
		lda Freq_Saw + 1, y
		sta Work + 3
		jmp calc
.endif
	ss5b:
.ifdef SS5B
		inc Work + 8	;5Bは-1オクターブから
		lda Work + 9
		asl
		tay
		lda Freq_5B, y
		sta Work + 2
		lda Freq_5B + 1, y
		sta Work + 3
		jmp calc
.endif
	vrc7:
.ifdef VRC7
		lda Work + 9
		asl
		tay
		lda Freq_VRC7, y
		sta Work + 2
		lda Freq_VRC7 + 1, y
		and #$01
		sta Work + 3
		lda Work + 8
		cmp #8
		bcc :+
		lda #7
	:	asl
		ora Work + 3
		sta Work + 3
		lda #0
		sta Work + 4		;VRC7は16bit値。スイープ用の上位バイトを必ずクリア
		rts
.endif
	fds:
.ifdef FDS
		lda Work + 9
		asl
		tay
		lda Freq_FDS, y
		sta Work + 2
		lda Freq_FDS + 1, y
		sta Work + 3
		lda Work + 8	;オクターブから周波数を計算する(FDSは周波数と比例なので6オクターブから)
		cmp #6
		bcc @N
		lda #6
		sta Work + 8
		rts
	@N:
		lda #6
		sec
		sbc Work + 8
		tay
		beq :+
		jmp calc_loop
	:	rts
.endif
	n163:
.ifdef N163
		lda Work + 9
		asl
		clc
		adc Work + 9
		tay
		lda Freq_N163, y
		sta Work + 2
		lda Freq_N163 + 1, y
		sta Work + 3
		lda Freq_N163 + 2, y
		sta Work + 4
		lda Work + 8	;N163は周波数と比例なので、発音数と波形長をシフト量に畳み込む
		cmp #8
		bcc @oct_ok
		lda #8
		sta Work + 8
	@oct_ok:
		lda N163ChCount
		cmp #1
		bne :+
		lda #8
		jmp @shift_sub
	:	cmp #2
		bne :+
		lda #7
		jmp @shift_sub
	:	cmp #4
		bne :+
		lda #6
		jmp @shift_sub
	:	cmp #8
		bne :+
		lda #5
		jmp @shift_sub
	:	lda #8
		jsr @shift_sub
		lda Work + 2
		sta Work + 5
		lda Work + 3
		sta Work + 6
		lda Work + 4
		sta Work + 7
		lda Work + 9
		asl
		clc
		adc Work + 9
		tay
		lda Freq_N163_6, y
		sta Work + 2
		lda Freq_N163_6 + 1, y
		sta Work + 3
		lda Freq_N163_6 + 2, y
		sta Work + 4
		lda #8
		jsr @shift_sub
		lda N163ChCount
		cmp #3
		beq @C3
		cmp #5
		beq @C5
		cmp #7
		beq @C7
		rts
	@C3:
		lsr Work + 4
		ror Work + 3
		ror Work + 2
		rts
	@C5:
		sec
		lda Work + 2
		sbc Work + 5
		sta Work + 2
		lda Work + 3
		sbc Work + 6
		sta Work + 3
		lda Work + 4
		sbc Work + 7
		sta Work + 4
		rts
	@C7:
		clc
		lda Work + 2
		adc Work + 5
		sta Work + 2
		lda Work + 3
		adc Work + 6
		sta Work + 3
		lda Work + 4
		adc Work + 7
		sta Work + 4
		rts
	@shift_sub:
		sec
		sbc Work + 8
		sec
		sbc N163FreqShift, x
		beq @SR
		bmi @SL
		tay
	@RR:
		lsr Work + 4
		ror Work + 3
		ror Work + 2
		dey
		bne @RR
	@SR:
		rts
	@SL:
		eor #$ff
		clc
		adc #1
		tay
	@LL:
		asl Work + 2
		rol Work + 3
		rol Work + 4
		dey
		bne @LL
		rts
.endif
	calc:
		ldy Work + 8	;オクターブから周波数を計算する
		bne calc_loop
		rts
	calc_loop:
		lsr Work + 3
		ror Work + 2
		dey
		bne calc_loop
		rts
.endproc


; ------------------------------------------------------------------------
; エンベロープ処理
; ------------------------------------------------------------------------
.proc envelope
	start:
		jsr select_track_bank
		lda Device, x
		cmp #$ff
		beq next			;未使用トラックは処理しない
		lda Frags, x
		and #FRAG_END
		bne next		;終了フラグが立っていなければ処理へ
		stx ProcTr
		lda EnvFrags, x
		and #FRAG_ENV_DIS
		beq load				;エンベロープ無効フラグが立っていたら音量処理へ
	vol:
		lda Device, x
		cmp #DEV_2A03_DPCM	;DPCMは音量計算しない
		beq next
.ifdef VRC7
		cmp #DEV_VRC7_CH1
		bcc :+
		cmp #DEV_VRC7_CH6 + 1
		bcs :+
		lda #15			;VRC7のキーオフはハードウェアのリリースに任せる
		jmp store			;明示的なトラックボリュームはcalc_volumeで反映
	:
.endif
		lda Frags, x
		and #FRAG_IS_KEYON	;キーオフされていたら無音に
		beq ld0				;それ以外は最大値をロード
		lda Device, x
		cmp #DEV_VRC6_SAW
		beq ld63
		cmp #DEV_FDS
		beq ld63
		lda #15
		jmp store
	ld0:
		lda #0
		jmp store
	ld63:
		lda #63
	store:
		sta Volume, x
	calc:
		jsr calc_volume
	next:
		dex
		bpl start			;xがマイナスになったら全トラック終了
		rts
	load:
		lda RefFreq_L, x
		sta Freq_L, x
		lda RefFreq_H, x
		sta Freq_H, x
		lda RefFreq_X, x
		sta Freq_X, x
	@N0:
		lda EnvFrags, x
		and #FRAG_NENV
		beq @N1
		jsr noteenv
		lda NoteN, x
		jsr calcfreq
		lda Work + 2
		sta Freq_L, x
		sta RefFreq_L, x
		lda Work + 3
		sta Freq_H, x
		sta RefFreq_H, x
		lda Work + 4
		sta Freq_X, x
		sta RefFreq_X, x
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
		beq tovol
		jsr volenv
		jmp calc
	tovol:
		jmp vol
.endproc

;NSFのトラック別シーケンスバンクを選択する
.proc select_track_bank
		lda DrvBankedMode
		beq @E
		cmp #2
		beq @dsp
		lda TrackBank, x
		sta $5ffa
		clc
		adc #1
		sta $5ffb
		jmp @E
	@dsp:
		lda TrackBank, x
		jsr select_dsp_bank
	@E:
		rts
.endproc

;拡張音源DSPの$C000-$DFFFへ8KB PRGバンクを選択する
.proc select_dsp_bank
.ifdef MMC3
		pha
		lda #$07
		sta $8000
		pla
		sta $8001
.elseif .defined(VRC6)
		sta $8000
.elseif .defined(VRC7)
		sta $8000
.elseif .defined(MMC5)
		ora #$80
		sta $5115
.elseif .defined(SS5B)
		pha
		lda #$0a
		sta $8000
		pla
		sta $a000
.elseif .defined(N163)
		sta $e800
.endif
		rts
.endproc


;ソフトウェアスイープ
.proc ssweep
.ifdef VRC7
		lda Device, x
		cmp #DEV_VRC7_CH1
		bcc :+
		cmp #DEV_VRC7_CH6 + 1
		bcs :+
		jmp vrc7_ssweep
	:
.endif
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq count
		jmp get
	keyon:
		lda #0
		sta SSwpCur_L, x
		sta SSwpCur_H, x
		sta SSwpCur_X, x
		lda RefNoteN, x			;いったん素の周波数を保存しておく
		jsr calcfreq
		lda Work + 2
		sta Work + 5
		lda Work + 3
		sta Work + 6
		lda Work + 4
		sta Work + 7
		lda RefNoteN, x
		clc
		adc SSwpEndHT, x		;スイープ終了周波数を計算
		jsr calcfreq
		lda Work + 2
		sec
		sbc Work + 5
		sta SSwpEnd_L, x
		lda Work + 3
		sbc Work + 6
		sta SSwpEnd_H, x
		lda Work + 4
		sbc Work + 7
		sta SSwpEnd_X, x
		lda SSwpDelay, x
		clc						;カウンタにディレイ値を加算
		adc SSwpCtr, x
		sta SSwpCtr, x
		rts
	count:
		lda SSwpCtr, x
		cmp #1
		beq plus				;カウンタが1になったら実行
		dec SSwpCtr, x
		jmp get
	plus:
		lda SSwpRate, x
		bpl @unit_mode
		jmp shift_plus
	@unit_mode:
		clc
		lda SSwpDepth, x
		bpl @plus_depth
		jmp minus
	@plus_depth:
		adc SSwpCur_L, x
		sta SSwpCur_L, x
		bcc @N
		inc SSwpCur_H, x
		bne @N
		inc SSwpCur_X, x
	@N:
	compare_plus:
.ifdef VRC7
		lda Device, x
		cmp #DEV_VRC7_CH1
		bcc :+
		cmp #DEV_VRC7_CH6 + 1
		bcs :+
		lda #0
		sta SSwpCur_X, x
		sta SSwpEnd_X, x	;VRC7スイープは16bit値として終点を比較
	:
.endif
		lda SSwpEnd_X, x
		cmp SSwpCur_X, x
		bcs @compare_plus_h
		jmp end
	@compare_plus_h:
		beq @compare_plus_h_eq
		jmp exec
	@compare_plus_h_eq:
		lda SSwpEnd_H, x		;Depthがプラスの（音が下がる）場合
		cmp SSwpCur_H, x
		bcs @compare_or_exec
		jmp end					;終了値より大きくなったら終了
	@compare_or_exec:
		beq @compare_low
		jmp exec				;終了値より小さかったら次へ
	@compare_low:
		lda SSwpEnd_L, x
		cmp SSwpCur_L, x		;下位バイトも比較
		bcc @compare_plus_end
		jmp exec
	@compare_plus_end:
		jmp end
	shift_plus:
		clc
		lda Freq_L, x
		adc SSwpCur_L, x
		sta Work
		lda Freq_H, x
		adc SSwpCur_H, x
		sta Work + 1
		lda Freq_X, x
		adc SSwpCur_X, x
		sta Work + 2
		lda SSwpDepth, x
		and #$07
		clc
		adc #2				;Speed 1～7をシフト回数3～9に変換
		tay
	@shift_loop:
		lsr Work + 2
		lsr Work + 1
		ror Work
		dey
		bne @shift_loop
		lda Work			;シフト結果が0でも終点まで進める
		ora Work + 1
		ora Work + 2
		bne @depth_ready
		inc Work
	@depth_ready:
		lda SSwpDepth, x
		bmi shift_minus
		clc
		lda SSwpCur_L, x
		adc Work
		sta SSwpCur_L, x
		lda SSwpCur_H, x
		adc Work + 1
		sta SSwpCur_H, x
		lda SSwpCur_X, x
		adc Work + 2
		sta SSwpCur_X, x
		jmp compare_plus
	shift_minus:
		sec
		lda SSwpCur_L, x
		sbc Work
		sta SSwpCur_L, x
		lda SSwpCur_H, x
		sbc Work + 1
		sta SSwpCur_H, x
		lda SSwpCur_X, x
		sbc Work + 2
		sta SSwpCur_X, x
		jmp compare_minus
	minus:
		adc SSwpCur_L, x
		sta SSwpCur_L, x
		bcs @N
		dec SSwpCur_H, x
		lda SSwpCur_H, x
		cmp #$ff
		bne @N
		dec SSwpCur_X, x
	@N:
	compare_minus:
.ifdef VRC7
		lda Device, x
		cmp #DEV_VRC7_CH1
		bcc :+
		cmp #DEV_VRC7_CH6 + 1
		bcs :+
		lda #0
		sta SSwpCur_X, x
		sta SSwpEnd_X, x	;VRC7スイープは16bit値として終点を比較
	:
.endif
		lda SSwpEnd_X, x
		cmp SSwpCur_X, x
		bcc exec
		bne end
		lda SSwpEnd_H, x		;Depthがマイナスの（音が上がる）場合
		cmp SSwpCur_H, x
		bcc exec				;終了値より大きかったら次へ
		bne end					;終了値より小さくなったら終了（レジスタ値が小さい方が高いので）
		lda SSwpEnd_L, x
		cmp SSwpCur_L, x
		bcc exec
	end:
		lda SSwpEnd_L, x
		sta SSwpCur_L, x
		lda SSwpEnd_H, x
		sta SSwpCur_H, x
		lda SSwpEnd_X, x
		sta SSwpCur_X, x
		lda #1
		sta SSwpCtr, x
		jmp get
	exec:
		lda SSwpRate, x			;Rateをカウンタに代入
		and #$7f
		sta SSwpCtr, x
	get:
		clc
		lda SSwpCur_L, x
		adc Freq_L, x
		sta Freq_L, x
		lda SSwpCur_H, x
		adc Freq_H, x
		sta Freq_H, x
		lda SSwpCur_X, x
		adc Freq_X, x
		sta Freq_X, x
		rts
.endproc

.ifdef VRC7
;VRC7はF-NumberとBlockの境界でレジスタ値が不連続になるため、
;スイープ中は実周波数に比例する F-Number << Block の24bit値を使用する。
.proc vrc7_ssweep
		lda Frags, x
		and #FRAG_KEYON
		bne keyon
		lda DrvFrags
		and #DRV_DOUBLE
		beq :+
		jmp output
	:
		lda SSwpCtr, x
		cmp #1
		beq update
		dec SSwpCtr, x
		jmp output
	keyon:
		lda RefNoteN, x
		jsr calcfreq
		jsr normalize_freq
		lda Work + 5
		sta SSwpCur_L, x
		lda Work + 6
		sta SSwpCur_H, x
		lda RefNoteN, x
		clc
		adc SSwpEndHT, x
		jsr calcfreq
		jsr normalize_freq
		lda Work + 5
		sta SSwpEnd_L, x
		lda Work + 6
		sta SSwpEnd_H, x
		lda #0
		sta SSwpCur_X, x
		sta SSwpEnd_X, x
		lda SSwpDelay, x
		clc
		adc SSwpCtr, x
		sta SSwpCtr, x
		jmp output
	update:
		lda SSwpRate, x
		bpl unit_step
		lda #1
		sta Work + 5
		lda #7
		sec
		sbc SSwpDepth, x
		and #$07
		tay
		beq @step_ready
	@step_shift:
		asl Work + 5
		dey
		bne @step_shift
	@step_ready:
		lda #1
		sta SSwpCtr, x
		jmp step_ready
	unit_step:
		lda #1
		sta Work + 5
		lda SSwpRate, x
		sta SSwpCtr, x
	step_ready:
		lda SSwpEndHT, x
		bmi subtract
		jsr add_fnum
		jsr clamp_plus
		jmp output
	subtract:
		jsr sub_fnum
		jsr clamp_minus
	output:
		lda SSwpCur_L, x
		sta Freq_L, x
		lda SSwpCur_H, x
		sta Freq_H, x
		lda #0
		sta Freq_X, x
		rts

	;Work+2/3のVRC7値をF-Number 256～511へ寄せ、Block/F-Number比較を一意にする。
	normalize_freq:
		lda Work + 2
		sta Work + 5
		lda Work + 3
		and #$01
		sta Work + 7		;F-Number上位bit
		lda Work + 3
		lsr
		and #$07
		sta Work + 8		;Block
	@normalize:
		lda Work + 7
		bne @encode
		lda Work + 8
		beq @encode
		asl Work + 5
		rol Work + 7
		dec Work + 8
		jmp @normalize
	@encode:
		lda Work + 8
		asl
		ora Work + 7
		sta Work + 6
		rts

	add_fnum:
		lda SSwpCur_H, x
		lsr
		and #$07
		sta Work + 8		;Block
		lda SSwpCur_H, x
		and #$01
		sta Work + 7		;F-Number上位bit
		clc
		lda SSwpCur_L, x
		adc Work + 5
		sta Work + 6
		lda Work + 7
		adc #0
		sta Work + 7
		cmp #2
		bcc @encode
		lda Work + 8
		cmp #7
		bcs @encode
		lsr Work + 7
		ror Work + 6
		inc Work + 8
	@encode:
		lda Work + 6
		sta SSwpCur_L, x
		lda Work + 8
		asl
		sta Work + 6
		lda Work + 7
		and #$01
		ora Work + 6
		sta SSwpCur_H, x
		rts

	sub_fnum:
		lda SSwpCur_H, x
		lsr
		and #$07
		sta Work + 8		;Block
		lda SSwpCur_H, x
		and #$01
		sta Work + 7		;F-Number上位bit
		sec
		lda SSwpCur_L, x
		sbc Work + 5
		sta Work + 6
		lda Work + 7
		sbc #0
		sta Work + 7
		bne @encode
		lda Work + 8
		beq @encode
		asl Work + 6
		rol Work + 7
		dec Work + 8
	@encode:
		lda Work + 6
		sta SSwpCur_L, x
		lda Work + 8
		asl
		sta Work + 6
		lda Work + 7
		and #$01
		ora Work + 6
		sta SSwpCur_H, x
		rts

	clamp_plus:
		lda SSwpEnd_H, x
		lsr
		sta Work + 8
		lda SSwpCur_H, x
		lsr
		cmp Work + 8
		bcc @done
		bne @clamp
		jsr compare_fnum
		bcc @done
	@clamp:
		jsr clamp
	@done:
		rts

	clamp_minus:
		lda SSwpEnd_H, x
		lsr
		sta Work + 8
		lda SSwpCur_H, x
		lsr
		cmp Work + 8
		bcc @clamp
		bne @done
		jsr compare_fnum
		bcc @clamp
		beq @clamp
		jmp @done
	@clamp:
		jsr clamp
	@done:
		rts

	;C=1: current >= end、Z=1: current == end
	compare_fnum:
		lda SSwpCur_H, x
		and #$01
		sta Work + 8
		lda SSwpEnd_H, x
		and #$01
		cmp Work + 8
		bcc @current_greater
		bne @current_less
		lda SSwpCur_L, x
		cmp SSwpEnd_L, x
		rts
	@current_greater:
		lda #1
		cmp #0
		rts
	@current_less:
		lda #0
		cmp #1
		rts

	clamp:
		lda SSwpEnd_L, x
		sta SSwpCur_L, x
		lda SSwpEnd_H, x
		sta SSwpCur_H, x
		lda #1
		sta SSwpCtr, x
		rts
.endproc
.endif


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
		bne get
		jmp count
	keyon:
		lda #1
		sta VEnvPos, x		;キーオン位置に移動
		clc
		adc VEnvDelay, x	;ディレイを加算
		sta VEnvCtr, x
		jmp get
	keyoff:
		;ボリュームエンベロープはキーオフ無効しない
		;ldy #0
		;lda (Work), y
		;and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		;bne get
		ldy #1
		lda (Work), y
		sta VEnvPos, x		;キーオフ位置に移動
		lda #1
		sta VEnvCtr, x
		jmp get
	count:
		lda VEnvCtr, x
		cmp #1
		bne get				;カウンタが1なら位置移動してから値取得
		inc VEnvPos, x		;エンベロープ位置移動
		lda Frags, x		;キーオン中なら以下を実行
		and #FRAG_IS_KEYON
		beq get
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
		asl
		tay
		lda (Work), y		;アドレスにあるデータを取得（音量）
		sta Volume, x
		lda VEnvCtr, x
		cmp #2
		bcc :+
		dec VEnvCtr, x		;カウンタが2以上ならカウントダウンして終了
		rts
	:	cmp #0
		beq end				;0ならそのまま終了
		iny					;それ以外（つまり1）ならフレーム数取得
		lda (Work), y
		sta VEnvCtr, x
	end:
		rts
.endproc


;音量計算
.proc calc_volume
		lda TrVolume, x
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
		tay
		lda Vol_Tbl, y
		sta Volume, x
		rts
	mult:
		ldy TrVolume, x
		iny					;16で割る都合上1を足す
		jsr multiply		;a * y
		lsr Work + 3		;16で割る
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		lsr Work + 3
		ror Work + 2
		;lda Work + 2
		;cmp #1				;3bit右シフトした時点で1の場合そのまま終了（四捨五入）
		;beq store
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
		bne get
		jmp count
	keyon:
		lda #1
		sta FEnvPos, x		;キーオン位置に移動
		clc
		adc FEnvDelay, x	;ディレイを加算
		sta FEnvCtr, x
		jmp get
	keyoff:
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		bne get
		ldy #1
		lda (Work), y
		sta FEnvPos, x		;キーオフ位置に移動
		jmp get
	count:
		lda FEnvCtr, x
		cmp #1
		bne get				;カウンタが1なら位置移動してから値取得
		inc FEnvPos, x		;エンベロープ位置移動
		ldy #0				;キーオフ無効ならループ処理する
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq :+
		lda Frags, x		;キーオフ無効でない場合、キーオン中のみループ処理する
		and #FRAG_IS_KEYON
		beq get
	:	lda FEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta FEnvPos, x
	get:
		lda FEnvPos, x
		asl
		tay
		lda (Work), y	;アドレスにあるデータを取得
		eor #$ff
		clc
		adc #1				;符号反転
		sta Work + 5
		lda #0
		sta Work + 6
		sta Work + 7
		lda Work + 5
		bpl :+
		dec Work + 6
		dec Work + 7
	:	lda FEnvShift, x
		sta Work + 8
		beq @add
	@shift_loop:
		asl Work + 5
		rol Work + 6
		rol Work + 7
		dec Work + 8
		bne @shift_loop
	@add:
		clc
		lda Freq_L, x
		adc Work + 5	;周波数に加算
		sta Freq_L, x
		lda Freq_H, x
		adc Work + 6
		sta Freq_H, x
		lda Freq_X, x
		adc Work + 7
		sta Freq_X, x
	frame:
		lda FEnvCtr, x
		cmp #2
		bcc :+
		dec FEnvCtr, x		;カウンタが2以上ならカウントダウンして終了
		rts
	:	cmp #0
		beq end				;0ならそのまま終了
		iny					;それ以外（つまり1）ならフレーム数取得
		lda (Work), y
		sta FEnvCtr, x
	end:
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
		beq count
		rts
	keyon:
		lda #1
		sta NEnvPos, x		;キーオン位置に移動
		clc
		adc NEnvDelay, x	;ディレイを加算
		sta NEnvCtr, x
		jmp get
	keyoff:
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		bne get
		ldy #1
		lda (Work), y
		sta NEnvPos, x		;キーオフ位置に移動
		jmp get
	count:
		lda NEnvCtr, x
		cmp #1
		bne get				;カウンタが1なら位置移動してから値取得
		inc NEnvPos, x		;エンベロープ位置移動
		ldy #0				;キーオフ無効ならループ処理する
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq :+
		lda Frags, x		;キーオフ無効でない場合、キーオン中のみループ処理する
		and #FRAG_IS_KEYON
		beq get
	:	lda NEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta NEnvPos, x
	get:
		lda NEnvPos, x
		asl
		sta Work + 6
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
		ldy Work + 6
		lda (Work), y	;アドレスにあるデータを取得
		clc
		adc RefNoteN, x		;ノートナンバーに加算
		sta NoteN, x
		ldy Work + 6
		jmp frame
	@N:
		ldy Work + 6
		lda (Work), y	;アドレスにあるデータを取得
		eor #$ff			;反転して加算
		clc
		adc #1
		clc
		adc RefNoteN, x
		sta NoteN, x
	frame:
		lda NEnvCtr, x
		cmp #2
		bcc :+
		dec NEnvCtr, x		;カウンタが2以上ならカウントダウンして終了
		rts
	:	cmp #0
		beq end				;0ならそのまま終了
		ldy Work + 6
		iny					;それ以外（つまり1）ならフレーム数取得
		lda (Work), y
		sta NEnvCtr, x
	end:
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
		beq count
		rts
	keyon:
		lda #1
		sta TEnvPos, x		;キーオン位置に移動
		clc
		adc TEnvDelay, x	;ディレイを加算
		sta TEnvCtr, x
		jmp get
	keyoff:
		ldy #0
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		bne get
		ldy #1
		lda (Work), y
		sta TEnvPos, x		;キーオフ位置に移動
		jmp get
	count:
		lda TEnvCtr, x
		cmp #1
		bne get				;カウンタが1なら位置移動してから値取得
		inc TEnvPos, x		;エンベロープ位置移動
		ldy #0				;キーオフ無効ならループ処理する
		lda (Work), y
		and #%10000000		;ヘッダ1個目に最上位ビットが立っていたらキーオフ無効
		beq :+
		lda Frags, x		;キーオフ無効でない場合、キーオン中のみループ処理する
		and #FRAG_IS_KEYON
		beq get
	:	lda TEnvPos, x
		ldy #1
		cmp (Work), y
		bne get				;ヘッダ2番目（キーオフ位置）に達したら
		ldy #0				;ヘッダ1番目（ループ位置）に戻る
		lda (Work), y
		and #%01111111		;最上位ビットを消す
		sta TEnvPos, x
	get:
		lda TEnvPos, x
		asl
		tay
		lda (Work), y		;アドレスにあるデータを取得
		eor Tone, x			;下位4bitに保存
		and #$0f
		eor Tone, x
		sta Tone, x			;代入
		lda TEnvCtr, x
		cmp #2
		bcc :+
		dec TEnvCtr, x		;カウンタが2以上ならカウントダウンして終了
		rts
	:	cmp #0
		beq end				;0ならそのまま終了
		iny					;それ以外（つまり1）ならフレーム数取得
		lda (Work), y
		sta TEnvCtr, x
	end:
		rts
.endproc


; ------------------------------------------------------------------------
; レジスタ書き込み
; ------------------------------------------------------------------------
.proc writereg
		lda #$ff
		sta PrevDev
	start:
		lda Device, x		;音源ごとにどのトラックが発音しているか調べる
		cmp #$ff
		beq next
		cmp PrevDev			;前の音源と違う場合テーブルに書き込む
		beq vol
	wtbl:
		lda Device, x
		sta PrevDev
		tay
		txa
		sta ActTbl, y
	next:
		dex
		bpl start
		jmp int_sqr1		;xがマイナスになったら全トラック終了
	vol:
		lda Volume, x		;音量が0なら書き込まない
		beq next
		jmp wtbl
		
	int_sqr1:
		ldx #DEV_2A03_SQR1
		lda ActTbl, x
		cmp #$ff
		beq int_sqr2
		tax
		ldy #0
		jsr writesqr
		jsr writereg_end

	int_sqr2:
		ldx #DEV_2A03_SQR2
		lda ActTbl, x
		cmp #$ff
		beq int_tri
		tax
		ldy #4
		jsr writesqr
		jsr writereg_end

	int_tri:
		ldx #DEV_2A03_TRI
		lda ActTbl, x
		cmp #$ff
		beq int_noise
		tax
		lda Freq_L, x
		sta $400a
		lda Freq_H, x
		sta $400b
		lda Frags, x
		and #FRAG_IS_KEYON
		beq @stop
		lda #$ff
		jmp @write
	@stop:
		lda #$80
	@write:
		sta $4008
		jsr writereg_end

	int_noise:
		ldx #DEV_2A03_NOISE
		lda ActTbl, x
		cmp #$ff
		beq int_dpcm
		tax
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
		and #$0f
		clc
		ror
		ror
		ora NoteN, x
		sta $400e
	@end:
		jsr writereg_end

	int_dpcm:
		ldx #DEV_2A03_DPCM
		lda ActTbl, x
		cmp #$ff
		beq ext
		tax
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
		jsr writereg_end

	ext:
.ifdef VRC6
	vrc6_sqr1:
		ldx #DEV_VRC6_SQR1
		lda ActTbl, x
		cmp #$ff
		beq vrc6_sqr2
		tax
		ldy #$90
		jsr write_vrc6
		jsr writereg_end

	vrc6_sqr2:
		ldx #DEV_VRC6_SQR2
		lda ActTbl, x
		cmp #$ff
		beq vrc6_saw
		tax
		ldy #$a0
		jsr write_vrc6
		jsr writereg_end

	vrc6_saw:
		ldx #DEV_VRC6_SAW
		lda ActTbl, x
		cmp #$ff
		beq vrc6_end
		tax
		ldy #$b0
		jsr write_vrc6
		jsr writereg_end
	vrc6_end:
.endif

.ifdef VRC7
	vrc7_ch:
		ldx #DEV_VRC7_CH1
	@L:
		lda ActTbl, x
		cmp #$ff
		beq @next
		stx Work + 6
		txa
		sec
		sbc #DEV_VRC7_CH1
		tay
		lda ActTbl, x
		tax
		jsr write_vrc7
		jsr writereg_end
		ldx Work + 6
	@next:
		inx
		cpx #DEV_VRC7_CH6 + 1
		bcc @L
.endif

.ifdef MMC5
	mmc5_sqr1:
		ldx #DEV_MMC5_SQR1
		lda ActTbl, x
		cmp #$ff
		beq mmc5_sqr2
		tax
		ldy #0
		jsr write_mmc5
		jsr writereg_end

	mmc5_sqr2:
		ldx #DEV_MMC5_SQR2
		lda ActTbl, x
		cmp #$ff
		beq mmc5_end
		tax
		ldy #4
		jsr write_mmc5
		jsr writereg_end
	mmc5_end:
.endif

.ifdef SS5B
	ss5b_sqr1:
		ldx #DEV_SS5B_SQR1
		lda ActTbl, x
		cmp #$ff
		beq ss5b_sqr2
		tax
		ldy #0
		jsr write_ss5b
		jsr writereg_end

	ss5b_sqr2:
		ldx #DEV_SS5B_SQR2
		lda ActTbl, x
		cmp #$ff
		beq ss5b_sqr3
		tax
		ldy #1
		jsr write_ss5b
		jsr writereg_end

	ss5b_sqr3:
		ldx #DEV_SS5B_SQR3
		lda ActTbl, x
		cmp #$ff
		beq ss5b_end
		tax
		ldy #2
		jsr write_ss5b
		jsr writereg_end
	ss5b_end:
.endif

.ifdef N163
	n163_ch:
		ldx #DEV_N163_CH1
		lda #DEV_N163_CH1
		clc
		adc N163ChCount
		sta Work + 7
	@L:
		stx Work + 6
		lda ActTbl, x
		cmp #$ff
		beq @next
		tax
		lda Work + 6
		sec
		sbc #DEV_N163_CH1
		clc
		adc N163ChOffset
		tay
		jsr write_n163
		jsr writereg_end
		ldx Work + 6
	@next:
		inx
		cpx Work + 7
		bcc @L
	n163_end:
.endif

.ifdef FDS
	fds:
		ldx #DEV_FDS
		lda ActTbl, x
		cmp #$ff
		beq fds_end
		tax
		jsr write_fds
		jsr writereg_end
	fds_end:
.endif
		rts
.endproc


;1トラック書き込み終了
.proc writereg_end
		ldy Device, x
		lda Freq_L, x
		sta PrevFreq_L, y
		lda Freq_H, x
		sta PrevFreq_H, y
		rts
.endproc


.proc writesqr
		sty Work + 1		;一旦yを保存
		lda Tone, x
		and #$0f
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
		rts
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
		and #$0f
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
		rts
.endproc
.endif

;VRC7
.ifdef VRC7
.proc vrc7_write
		sty $9010
		jsr wait_9010
		sta $9030
		jsr wait_9030
		rts

	wait_9030:
		stx Work + 5
		ldx #$08
	@wait_loop:
		dex
		bne @wait_loop
		ldx Work + 5
	wait_9010:
		rts
.endproc

.proc write_vrc7
		sty Work + 1
		tya
		clc
		adc #$30
		tay
		lda #$0f
		sec
		sbc Volume, x
		sta Work
		lda Tone, x
		and #$0f
		asl
		asl
		asl
		asl
		ora Work
		jsr vrc7_write
		lda Volume, x
		beq end
		lda Work + 1
		clc
		adc #$10
		tay
		lda Freq_L, x
		jsr vrc7_write
		lda Work + 1
		clc
		adc #$20
		tay
		lda Freq_H, x
		sta Work
		lda Frags, x
		and #FRAG_KEYON
		beq @trigger
		lda Work
		jsr vrc7_write		;トリガーを一度下げて再キーオン可能にする
	@trigger:
		lda Work
		pha
		lda Frags, x
		and #FRAG_IS_KEYON
		beq @write_trigger
		pla
		ora #$10
		pha
	@write_trigger:
		pla
		jsr vrc7_write
	end:
		rts
.endproc
.endif

;MMC5
.ifdef MMC5
.proc write_mmc5
		sty Work		;一旦yを保存
		lda Tone, x
		and #$0f
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
		rts
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
		rts
.endproc
.endif

;N163
.ifdef N163
.proc n163_mute_channels
		lda #$47
		sta Work
		ldy #7
		lda #0
	@L:
		lda Work
		ora #$80
		sta $f800
		lda #0
		sta $4800
		lda Work
		clc
		adc #8
		sta Work
		dey
		bne @L
		lda #$ff
		sta $f800
		lda N163ChReg
		sta $4800
		rts
.endproc

.proc n163_update_freq
		ldx #LAST_TRACK
	@L:
		lda Frags, x
		and #FRAG_END
		bne @N
		lda Device, x
		cmp #DEV_N163_CH1
		bcc @N
		cmp #DEV_N163_CH8 + 1
		bcs @N
		lda NoteN, x
		jsr calcfreq
		lda Work + 2
		sta Freq_L, x
		sta RefFreq_L, x
		lda Work + 3
		sta Freq_H, x
		sta RefFreq_H, x
		lda Work + 4
		sta Freq_X, x
		sta RefFreq_X, x
	@N:
		dex
		bpl @L
		ldx ProcTr
		rts
.endproc

.proc n163_load_wave
		lda #$80
		sta $f800
		lda N163WavAddr_L
		sta Work
		lda N163WavAddr_H
		sta Work + 1
		ldy #0
	@L:
		lda (Work), y
		sta $4800
		iny
		cpy #64
		bcc @L
		lda #$c0
		sta $f800
		lda #0
		ldy #64
	@C:
		sta $4800
		dey
		bne @C
		lda #$ff
		sta $f800
		lda N163ChReg
		sta $4800
		rts
.endproc

.proc write_n163
		tya
		asl
		asl
		asl
		clc
		adc #$40
		sta Work
		ora #$80
		sta $f800
		lda Freq_L, x
		sta $4800
		lda Work
		clc
		adc #2
		ora #$80
		sta $f800
		lda Freq_H, x
		sta $4800
		lda Work
		clc
		adc #4
		ora #$80
		sta $f800
		lda Freq_X, x
		and #$03
		ora N163WaveLenReg, x
		sta $4800
		lda Work
		clc
		adc #6
		ora #$80
		sta $f800
		lda N163WaveOffset, x
		bpl @addr
		lda Tone, x
		and #$0f
		asl
		asl
		asl
		asl
		asl
	@addr:
		sta $4800
		lda Volume, x
		cpy #7
		bne @vol
		ora N163ChReg
	@vol:
		sta $4800
		rts
.endproc
.endif

;FDS
.ifdef FDS
.proc write_fds
		lda Tone, x
		and #$0f
		cmp FdsPrevWav
		beq mod				;前回書き込んだ音色と同じならスキップ
		tay
		lda #%10000000
		sta $4089			;Wavetable書き込み許可
		lda #64
		sta Work + 2
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
		and #$0f
		sta FdsPrevWav
		lda FdsModTone
		sta FdsPrevMod
		lda FdsModEnv		;モジュレータエンベロープが有効なら以下を実行
		and #%10000000
		bne hwenv
		lda Frags, x
		and #FRAG_KEYON		;キーオンしていたらリセット
		beq hwenv
		lda FdsModEnv
		and #%01000000
		bne :+
		lda #%10111111		;減少の場合ゲインを63にリセット
		sta $4084
		jmp :++
	:	lda #%11000000		;増加の場合ゲインを0にリセット
		sta $4084
	:	lda FdsModEnv
		sta $4084
	hwenv:
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
		rts
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
.proc multiply
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




Vol_Tbl:
	.byte $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0
	.byte $0, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1
	.byte $0, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $1, $2
 	.byte $0, $1, $1, $1, $1, $1, $1, $1, $1, $1, $2, $2, $2, $2, $2, $3
 	.byte $0, $1, $1, $1, $1, $1, $1, $1, $2, $2, $2, $2, $3, $3, $3, $4
 	.byte $0, $1, $1, $1, $1, $1, $2, $2, $2, $3, $3, $3, $4, $4, $4, $5
 	.byte $0, $1, $1, $1, $1, $2, $2, $2, $3, $3, $4, $4, $4, $5, $5, $6
 	.byte $0, $1, $1, $1, $1, $2, $2, $3, $3, $4, $4, $5, $5, $6, $6, $7
 	.byte $0, $1, $1, $1, $2, $2, $3, $3, $4, $4, $5, $5, $6, $6, $7, $8
 	.byte $0, $1, $1, $1, $2, $3, $3, $4, $4, $5, $6, $6, $7, $7, $8, $9
 	.byte $0, $1, $1, $2, $2, $3, $4, $4, $5, $6, $6, $7, $8, $8, $9, $a
 	.byte $0, $1, $1, $2, $2, $3, $4, $5, $5, $6, $7, $8, $8, $9, $a, $b
 	.byte $0, $1, $1, $2, $3, $4, $4, $5, $6, $7, $8, $8, $9, $a, $b, $c
 	.byte $0, $1, $1, $2, $3, $4, $5, $6, $6, $7, $8, $9, $a, $b, $c, $d
 	.byte $0, $1, $1, $2, $3, $4, $5, $6, $7, $8, $9, $a, $b, $c, $d, $e
 	.byte $0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $a, $b, $c, $d, $e, $f

	;.byte	$00, $00, $00, $00, $00, $00, $00, $00
	;.byte	$01, $11, $11, $11, $11, $11, $11, $11
	;.byte	$01, $11, $11, $11, $11, $11, $11, $12
 	;.byte	$01, $11, $11, $11, $11, $22, $22, $23
 	;.byte	$01, $11, $11, $11, $22, $22, $33, $34
 	;.byte	$01, $11, $11, $22, $23, $33, $44, $45
 	;.byte	$01, $11, $12, $22, $33, $44, $45, $56
 	;.byte	$01, $11, $12, $23, $34, $45, $56, $67
 	;.byte	$01, $11, $22, $33, $44, $55, $66, $78
 	;.byte	$01, $11, $23, $34, $45, $66, $77, $89
 	;.byte	$01, $12, $23, $44, $56, $67, $88, $9a
 	;.byte	$01, $12, $23, $45, $56, $78, $89, $ab
 	;.byte	$01, $12, $34, $45, $67, $88, $9a, $bc
 	;.byte	$01, $12, $34, $56, $67, $89, $ab, $cd
 	;.byte	$01, $12, $34, $56, $78, $9a, $bc, $de
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

.ifdef VRC7
Freq_VRC7:
	.word	172, 182, 193, 205, 217, 230
	.word	244, 258, 274, 290, 307, 326
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

.ifdef N163
Freq_N163:
	.byte	$66, $1f, $01
	.byte	$7d, $30, $01
	.byte	$98, $42, $01
	.byte	$c7, $55, $01
	.byte	$19, $6a, $01
	.byte	$a1, $7f, $01
	.byte	$71, $96, $01
	.byte	$9c, $ae, $01
	.byte	$37, $c8, $01
	.byte	$5a, $e3, $01
	.byte	$16, $00, $02
	.byte	$89, $1e, $02
Freq_N163_6:
	.byte	$61, $bc, $06
	.byte	$ec, $22, $07
	.byte	$8e, $8f, $07
	.byte	$a7, $02, $08
	.byte	$97, $7c, $08
	.byte	$c7, $fd, $08
	.byte	$a6, $86, $09
	.byte	$a8, $17, $0a
	.byte	$4a, $b1, $0a
	.byte	$19, $54, $0b
	.byte	$81, $00, $0c
	.byte	$35, $b7, $0c
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
