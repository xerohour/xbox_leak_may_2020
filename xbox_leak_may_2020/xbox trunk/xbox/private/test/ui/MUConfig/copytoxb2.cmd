@echo off
SET __TITLEID=00001123

if "%1"=="" goto usage

xbmkdir -x %1 xc:\tdata\%__TITLEID%
rem xbcp /r /q /y /f %_NTBINDIR%\private\test\ui\muconfig\tdata\*.* xc:\tdata\%__TITLEID% >nul
xbcp /r /q /y /f tdata\*.* xc:\tdata\%__TITLEID% >nul
xbcp /q /y /f %_NT386TREE%\dump\muconfig.xbe xe:\ >nul
goto end

:usage
Echo Copies the MU Configuration (MUCONFIG) program to the specified XBox.
Echo.
Echo COPYTOXB xboxname
Echo.
Echo xboxname     The name of the XBOX to where you wish to copy the program.
goto end

:end
SET __TITLEID=
