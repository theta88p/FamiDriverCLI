
	.setcpu		"6502"

	.include	"nes.inc"
	.include	"macro.inc"


MAX_TRACK 	=	16
LAST_TRACK	=	MAX_TRACK - 1
MAX_LOOP	=	3

DRV_IS_PROC			=	%00000001	;処理中フラグ
DRV_IS_PROC_CLR		=	%11111110
DRV_DOUBLE			=	%00000010	;二重処理（加速）フラグ
DRV_DOUBLE_CLR		=	%11111101
DRV_SKIP_DIR		=	%00000100	;速度方向（0:減速 1:加速）
DRV_SKIP_DIR_CLR	=	%11111011
DRV_INIT			=	%00001000	;初期化済みフラグ
DRV_INIT_CLR		=	%11110111

FRAG_KEYON			=	%00000001
FRAG_KEYON_CLR		=	%11111110
FRAG_KEYOFF			=	%00000010
FRAG_KEYOFF_CLR		=	%11111101
FRAG_LOAD			=	%00000100
FRAG_LOAD_CLR		=	%11111011
FRAG_KEYON_DIS		=	%00001000
FRAG_KEYON_DIS_CLR	=	%11110111
FRAG_IS_KEYON		=	%01000000
FRAG_IS_KEYON_CLR	=	%10111111
FRAG_END			=	%10000000
FRAG_END_CLR		=	%01111111

FRAG_VENV			=	%00000001
FRAG_VENV_CLR		=	%11111110
FRAG_FENV			=	%00000010
FRAG_FENV_CLR		=	%11111101
FRAG_NENV			=	%00000100
FRAG_NENV_CLR		=	%11111011
FRAG_TENV			=	%00001000
FRAG_TENV_CLR		=	%11110111
FRAG_SSWP			=	%00010000
FRAG_SSWP_CLR		=	%11101111
FRAG_ENV_DIS		=	%10000000
FRAG_ENV_DIS_CLR	=	%01111111

DEV_2A03_SQR1		=	0
DEV_2A03_SQR2		=	1
DEV_2A03_TRI		=	2
DEV_2A03_NOISE		=	3
DEV_2A03_DPCM		=	4
DEV_VRC6_SQR1		=	5
DEV_VRC6_SQR2		=	6
DEV_VRC6_SAW		=	7
DEV_MMC5_SQR1		=	8
DEV_MMC5_SQR2		=	9
DEV_SS5B_SQR1		=	10
DEV_SS5B_SQR2		=	11
DEV_SS5B_SQR3		=	12
DEV_FDS				=	13
