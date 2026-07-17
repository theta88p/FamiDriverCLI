cd "%~dp0"

set MSYS_HOME=c:\msys64
set CC65_HOME=c:\cc65
set "PATH=%CC65_HOME%\bin;%MSYS_HOME%\usr\bin;%PATH%"

make clean
