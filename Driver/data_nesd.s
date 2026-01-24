.export BGM0
.export DPCMinfo
.export palette

.segment "MUSDATA"

BGM0:
	;.byte	"SEQ"
	.incbin "test.bin"

.segment "PCMDATA"

DPCMinfo:
	;.byte	"DPCM"
	;.incbin "C:\Programs\mck\Mumml\prog\dmc\909kick3_v60.dmc"
	;.incbin	"C:\Programs\mck\Mumml\prog\dmc\tight909kick_v60.dmc"
	;.incbin "C:\Programs\mck\Mumml\prog\dmc\cowbell.dmc"

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
