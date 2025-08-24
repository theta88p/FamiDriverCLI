#DEBUG = 1

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
LD_DSP_FLAGS = -C $(CFG_DSP_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE) -o
LD_DSP_VRC6_FLAGS = -C $(CFG_DSP_VRC6_FILE) -L$(LIB_PASS) -v -m $(MAP_DSP_FILE) -vm --dbgfile $(DBG_DSP_FILE) -o

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
	$(AS) drv.s $(ASFLAGS) -D MMC5=1 -o drv_mmc5.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D SS5B=1 -o drv_ss5b.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D FDS=1 -o drv_fds.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : crt0_nes.s >>$(ERRLOG_FILE)
	$(AS) crt0_nes.s $(ASFLAGS) -D VRC6=1 -o crt0_nes_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : drv.s >>$(ERRLOG_FILE)
	$(AS) drv.s $(ASFLAGS) -D VRC6=1 -o drv_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)
	@echo [Assemble] : dsp.s >>$(ERRLOG_FILE)
	$(AS) dsp.s $(ASFLAGS) -D VRC6=1 -o dsp_vrc6.o 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)


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
CFG_NSF_FILE   =	nsfd.cfg
CFG_DSP_FILE   =	nesd.cfg

else

CFG_NSF_FILE   =	nsf.cfg
CFG_DSP_FILE   =	nes.cfg
CFG_DSP_VRC6_FILE   =	nes_vrc6.cfg
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
					data_nsf_dbg.o	\
					drv_fds.o		\

MAIN_DSP_FILES =	ines_vrc6.o		\
					crt0_nes_vrc6.o	\
					nmi_main_nes.o	\
					irq_main.o		\
					main_nes.o		\
					drv_vrc6.o		\
					data_dbg.o		\
					dsp_vrc6.o		\

else

MAIN_NSF_FILES =	main_nsf.o		\
					nsf.o			\
					crt0_nsf.o		\
					data_nsf.o		\

MAIN_DSP_FILES =	ines.o			\
					crt0_nes.o		\
					nmi_main_nes.o	\
					irq_main.o		\
					main_nes.o		\
					drv.o			\
					data_nes.o		\
					dsp.o			\

MAIN_DSP_VRC6_FILES =	ines_vrc6.o	\
					crt0_nes_vrc6.o	\
					nmi_main_nes.o	\
					irq_main.o		\
					main_nes.o		\
					drv_vrc6.o		\
					data_nes.o		\
					dsp_vrc6.o		\

endif

.PHONY: all clean del


#---------------
#; build of main program

ifdef DEBUG

all : dsp.nes

dsp.nes : $(MAIN_DSP_FILES) $(CHR_FILES) 
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

#dbg.nsf : $(MAIN_NSF_FILES)
#	@echo [Link] : $@ >>$(ERRLOG_FILE)
#	$(LINKER) $(LD_NSF_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

else

all : drv.bin drv_mmc5.bin drv_vrc6.bin drv_ss5b.bin drv_fds.bin dsp.bin dsp_vrc6.bin del

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

dsp.bin : $(MAIN_DSP_FILES) $(CHR_FILES) 
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

dsp_vrc6.bin : $(MAIN_DSP_VRC6_FILES) $(CHR_FILES) 
	@echo [Link] : $@ >>$(ERRLOG_FILE)
	$(LINKER) $(LD_DSP_VRC6_FLAGS) $@ $^ 1>>$(LOG_FILE) 2>>$(ERRLOG_FILE)

del: dsp.bin dsp_vrc6.bin
	$(RM) $^
	
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

	@$(RM) $(LOG_FILE)
	@$(RM) $(ERRLOG_FILE)
	@$(RM) $(BLDLOG_FILE)
