cd "%~dp0"

set MSYS_HOME=c:\msys64
set CC65_HOME=c:\cc65
set PATH=%CC65_HOME%\bin;%MSYS_HOME%\usr\bin
set DBG_DIR=..\Compiler\bin\Debug\bin
set REL_DIR=..\Compiler\bin\Release\bin
set WS_DBG_DIR=..\..\FamiDriver\FamiDriver\bin\x64\Debug\net9.0-windows7.0\bin
set WS_REL_DIR=..\..\FamiDriver\FamiDriver\bin\x64\Release\net9.0-windows7.0\bin
set WS_PRJ_DIR=..\..\FamiDriver\bin

del buildlog.txt
del errlog.txt
del comlog.txt
make -k >buildlog.txt 2>&1
if %errorlevel% equ 0	goto end
start errlog.txt
:end
copy /y drv.bin %WS_PRJ_DIR%
copy /y drv_*.bin %WS_PRJ_DIR%
copy /y dsp_*.bin %WS_PRJ_DIR%
copy /y drv.bin %WS_DBG_DIR%
copy /y drv_*.bin %WS_DBG_DIR%
copy /y dsp_*.bin %WS_DBG_DIR%
copy /y drv.bin %WS_REL_DIR%
copy /y drv_*.bin %WS_REL_DIR%
copy /y dsp_*.bin %WS_REL_DIR%
copy /y drv.bin %REL_DIR%
copy /y drv_*.bin %REL_DIR%
copy /y dsp_*.bin %REL_DIR%
copy /y drv.bin %DBG_DIR%
copy /y drv_*.bin %DBG_DIR%
copy /y dsp_*.bin %DBG_DIR%
