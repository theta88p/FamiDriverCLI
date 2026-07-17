cd "%~dp0"

set MSYS_HOME=c:\msys64
set CC65_HOME=c:\cc65
set "PATH=%CC65_HOME%\bin;%MSYS_HOME%\usr\bin;%PATH%"

del buildlog.txt
del errlog.txt
del comlog.txt
make -k >buildlog.txt 2>&1
set "BUILD_RESULT=%ERRORLEVEL%"
if not %BUILD_RESULT% equ 0 start "" errlog.txt
exit /b %BUILD_RESULT%
