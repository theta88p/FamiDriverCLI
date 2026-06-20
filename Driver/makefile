#DEBUG = 1
#DEBUG_VRC6 = 1
#DEBUG_MMC5 = 1
#DEBUG_SS5B = 1

.DEFAULT_GOAL := all

TOOL_PASS =	
INC_PASS = 
LIB_PASS =

########################################
#;	LOG files
########################################

LOG_FILE    =	comlog.txt
ERRLOG_FILE =	errlog.txt
BLDLOG_FILE =	buildlog.txt



########################################
#;	Define of build tools & option
########################################

#---------------
#;  MML Compiler (for Music & SE data)
MC      = $(TOOL_PASS)
MCFLAGS =

#---------------
#;  C Compiler
CC      = $(TOOL_PASS)cl65
CFLAGS  = -t none --listing $*.lst --list-bytes 255 -g -c -o

#---------------
#;  Assembler
AS      = $(TOOL_PASS)ca65
ASFLAGS = -t none --listing $*.lst --list-bytes 255 -g

#---------------
#;  Archiver
AR      = $(TOOL_PASS)ar65
ARFLAGS = a

#---------------
#;  Linker
LINKER  = $(TOOL_PASS)ld65
LD_NSF_FLAGS = -C $(CFG_NSF_FILE) -L$(LIB_PASS) -v -m $(MAP_NSF_FILE) -vm --dbgfile $(DBG_NSF_FILE) -o
LD_DSP_FLAGS = -C $(CFG_DSP_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE)
LD_DSP_VRC6_FLAGS = -C $(CFG_DSP_VRC6_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE)
LD_DSP_MMC5_FLAGS = -C $(CFG_DSP_MMC5_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE)
LD_DSP_SS5B_FLAGS = -C $(CFG_DSP_SS5B_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE)
LD_DSP_FDS_FLAGS = -C $(CFG_DSP_FDS_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE)

#---------------
#;  File delete
RM      = rm -f

#---------------
#;  File copy
CP      = copy



#===============================
#; 依存関係（ヘッダーファイル等を、依存関係に追加するために、定義）

%.o : %.s $(INC_FILES)			#;".inc"ファイルが更新された場合は、.sは、全部アセンブルする。
	@echo [Assemble] : $< >>$(ERRLOG_FILE)
	$(AS) $(ASFLAGS) $< 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D SS5B=1 -o drv_ss5b.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D FDS=1 -o drv_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D MMC5=1 -D VRC6=1 -D SS5B=1 -D FDS=1 -o drv_all.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

	@echo [Assemble] : crt0_nes.s >>$(ERRLOG_FILE)
	$(AS) crt0_nes.s $(ASFLAGS) -D VRC6=1 -o crt0_nes_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D VRC6=1 -o drv_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : dsp.s >>$(ERRLOG_FILE)
	$(AS) dsp.s $(ASFLAGS) -D VRC6=1 -o dsp_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D MMC5=1 -o drv_mmc5.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : dsp.s >>$(ERRLOG_FILE)
	$(AS) dsp.s $(ASFLAGS) -D MMC5=1 -o dsp_mmc5.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

	@echo [Assemble] : dsp.s >>$(ERRLOG_FILE)
	$(AS) dsp.s $(ASFLAGS) -D FDS=1 -o dsp_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : crt0_nes.s >>$(ERRLOG_FILE)
	$(AS) crt0_nes.s $(ASFLAGS) -D FDS=1 -o crt0_nes_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : main_nes.s >>$(ERRLOG_FILE)
	$(AS) main_nes.s $(ASFLAGS) -D FDS=1 -o main_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)


dsp_ss5b.o : dsp.s $(INC_FILES)
	@echo [Assemble] : dsp.s >>$(ERRLOG_FILE)
	$(AS) dsp.s $(ASFLAGS) -D SS5B=1 -o dsp_ss5b.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

crt0_nes_ss5b.o : crt0_nes.s $(INC_FILES)
	@echo [Assemble] : crt0_nes.s >>$(ERRLOG_FILE)
	$(AS) crt0_nes.s $(ASFLAGS) -D SS5B=1 -o crt0_nes_ss5b.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

crt0_nes_fds.o : crt0_nes.s $(INC_FILES)
	@echo [Assemble] : crt0_nes.s >>$(ERRLOG_FILE)
	$(AS) crt0_nes.s $(ASFLAGS) -D FDS=1 -o crt0_nes_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

main_fds.o : main_nes.s $(INC_FILES)
	@echo [Assemble] : main_nes.s >>$(ERRLOG_FILE)
	$(AS) main_nes.s $(ASFLAGS) -D FDS=1 -o main_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
%.s : %.mml $(DMC_FILES)		#;".dmc"ファイルが更新された場合は、.mmlは、全部コンパイルする。
	@echo ======================================================== >>$(LOG_FILE)
	@echo [Compile] : $< >>$(LOG_FILE)
	@echo ======================================================== >>$(LOG_FILE)
	@echo [Compile] : $< >>$(ERRLOG_FILE)
	$(MC) $(MCFLAGS) $< 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

#---------------------------------------
#;	Header files
#---------------------------------------

#---------------
#; アセンブリ言語用ヘッダー
INC_FILES =	drv.inc		\
			data.bin	\

#---------------
#; ⊿ＰＣＭファイル
DMC_FILES =	


#---------------------------------------
#;	Library files
#---------------------------------------

LIB_FILES =	



########################################
#;	Build process
########################################

#---------------
#; リンカーの設定
ifdef DEBUG

DSP_DATA_FILE =	data_nesd.o

CFG_NSF_FILE   =	nsfd.cfg

ifdef DEBUG_VRC6

CFG_NSF_FILE   =	nsfd.cfg
CFG_DSP_FILE   =	nes_vrc6_d.cfg

else ifdef DEBUG_MMC5

CFG_NSF_FILE   =	nsfd.cfg
CFG_DSP_FILE   =	nes_mmc5_d.cfg

else ifdef DEBUG_SS5B

CFG_NSF_FILE   =	nsfd.cfg
CFG_DSP_FILE   =	nes_ss5b_d.cfg

else

CFG_DSP_FILE   =	nesd.cfg

endif

else

DSP_DATA_FILE =	data_nes.o

CFG_NSF_FILE   =	nsf.cfg
CFG_DSP_FILE   =	nes.cfg
CFG_DSP_VRC6_FILE   =	nes_vrc6.cfg
CFG_DSP_MMC5_FILE   =	nes_mmc5.cfg
CFG_DSP_SS5B_FILE   =	nes_ss5b.cfg
CFG_DSP_FDS_FILE   =	fds.cfg

endif

MAP_NSF_FILE   =	nsf.map
DBG_NSF_FILE   =	nsf.dbg

MAP_DSP_FILE   =	dsp.map
DBG_DSP_FILE   =	dsp.dbg

#---------------
#; ソースファイル
ifdef DEBUG

MAIN_NSF_FILES =	main_nsf.o		\
				nsf.o			\
				crt0_nsf.o		\
				data_nsfd.o		\
				drv.o			\

else

MAIN_NSF_FILES =	main_nsf.o		\
				nsf.o			\
				crt0_nsf.o		\
				data_nsf.o		\

endif

MAIN_DSP_FILES =	ines.o			\
				crt0_nes.o		\
				nmi_main_nes.o	\
				irq_main.o		\
				main_nes.o		\
				drv.o			\
				$(DSP_DATA_FILE)	\
				dsp.o			\

MAIN_DSP_VRC6_FILES =	ines_vrc6.o	\
				crt0_nes_vrc6.o	\
				nmi_main_nes.o	\
				irq_main.o		\
				main_nes.o		\
				drv_vrc6.o		\
				$(DSP_DATA_FILE)	\
				dsp_vrc6.o		\

MAIN_DSP_MMC5_FILES =	ines_mmc5.o	\
				crt0_nes.o		\
				nmi_main_nes.o	\
				irq_main.o		\
				main_nes.o		\
				drv_mmc5.o		\
				$(DSP_DATA_FILE)	\
				dsp_mmc5.o		\

MAIN_DSP_SS5B_FILES =	ines_ss5b.o	\
				crt0_nes_ss5b.o	\
				nmi_main_nes.o	\
				irq_main.o		\
				main_nes.o		\
				drv_ss5b.o		\
				$(DSP_DATA_FILE)	\
				dsp_ss5b.o		\

MAIN_DSP_FDS_FILES = crt0_nes_fds.o	\
				nmi_main_nes.o	\
				irq_main.o		\
				main_fds.o		\
				drv_fds.o		\
				$(DSP_DATA_FILE)	\
				dsp_fds.o		\


.PHONY: all clean del


#---------------
#; build of main program

ifdef DEBUG

ifdef DEBUG_VRC6

all : nsf.nsf dsp.nes

nsf.nsf : $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp.nes : $(MAIN_DSP_VRC6_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) -o $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

else ifdef DEBUG_MMC5

all : nsf.nsf dsp.nes

nsf.nsf : $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp.nes : $(MAIN_DSP_MMC5_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) -o $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

else ifdef DEBUG_SS5B

all : nsf.nsf dsp.nes

nsf.nsf : $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp.nes : $(MAIN_DSP_SS5B_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) -o $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

else

all : nsf.nsf dsp.nes

nsf.nsf : $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp.nes : $(MAIN_DSP_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) -o $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

endif

else

all : drv.bin drv_mmc5.bin drv_vrc6.bin drv_ss5b.bin drv_fds.bin drv_all.bin dsp dsp_vrc6 dsp_mmc5 dsp_ss5b dsp_fds

drv.bin : drv.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

drv_mmc5.bin : drv_mmc5.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

drv_vrc6.bin : drv_vrc6.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

drv_ss5b.bin : drv_ss5b.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

drv_fds.bin : drv_fds.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

drv_all.bin : drv_all.o $(MAIN_NSF_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp : $(MAIN_DSP_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp_vrc6 : $(MAIN_DSP_VRC6_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_VRC6_FLAGS) $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp_mmc5 : $(MAIN_DSP_MMC5_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_MMC5_FLAGS) $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp_ss5b : $(MAIN_DSP_SS5B_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_SS5B_FLAGS) $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp_fds : $(MAIN_DSP_FDS_FILES) $(CHR_FILES)
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FDS_FLAGS) $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

endif


#---------------
#; Crean up
clean:
	@$(RM) *.nes		#NESファイルは、消さないでおく。
	@$(RM) *.nsf
	@$(RM) *.bak
	@$(RM) *.map
	@$(RM) *.lst
	@$(RM) *.o
	@$(RM) drv*.bin
	@$(RM) dsp*.bin

	@$(RM) $(LOG_FILE)
	@$(RM) $(ERRLOG_FILE)
	@$(RM) $(BLDLOG_FILE)
