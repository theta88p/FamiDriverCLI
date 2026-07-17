cd "%~dp0"

set MSYS_HOME=c:\msys64
set CC65_HOME=c:\cc65
set PATH=%CC65_HOME%\bin;%MSYS_HOME%\usr\bin;%PATH%

del buildlog.txt
del errlog.txt
del comlog.txt
make -k clean all >buildlog.txt 2>&1

::if %errorlevel% equ 0	goto end
::start errlog.txt
:::end
