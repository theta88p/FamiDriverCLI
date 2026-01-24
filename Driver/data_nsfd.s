.export _BGM0
.export _DPCMinfo

.segment "MUSDATA"

_BGM0:
	;.byte	"SEQ"
	.incbin "test.bin"

.segment "PCMDATA"

_DPCMinfo:
	.byte	"DPCM"
	;.incbin	"C:\Programs\mck\Mumml\prog\dmc\tight909kick_v60.dmc"
	;.incbin "C:\Programs\mck\Mumml\prog\dmc\cowbell.dmc"

.segment "CHARS"
