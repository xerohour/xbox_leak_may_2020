@echo off
if "%1"=="" goto usage

REM xbmkdir -x %1 xc:\tdata\FFEE0000
xbmkdir -x %1 xs:\FFEE0000
xbcp /r /q /y /f %_NTBINDIR%\private\test\ui\dukeinfo\tdata\*.* xs:\FFEE0000 >nul
xbcp /q /y /f %_NT386TREE%\dump\dukeinfo.xbe xe:\ >nul
goto end

:usage
Echo Copies the Duke Information (DukeInfo) program to the specified XBox.
Echo.
Echo COPYDI xboxname
Echo.
Echo xboxname     The name of the XBOX to where you wish to copy the DukeInfo program.
goto end

:end