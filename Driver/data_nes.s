.export BGM0
.export DPCMinfo
.export palette

.segment "MUSDATA"

BGM0:

.segment "PCMDATA"

DPCMinfo:

; パレット
.segment "RODATA"
palette:
	.byte	$0f, $0f, $15, $30
	.byte	$0f, $0c, $1c, $30
	.byte	$0f, $0c, $05, $30
	.byte	$0f, $13, $36, $30

	.byte	$0f, $0c, $15, $25
	.byte	$0f, $0c, $12, $22
	.byte	$0f, $0c, $15, $15
	.byte	$0f, $0c, $12, $12

; パターンテーブル
.segment "CHARS"
	.incbin "bg.chr"
