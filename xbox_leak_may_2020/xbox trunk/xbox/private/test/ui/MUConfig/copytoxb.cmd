@echo off
SET __TITLEID=00001123

if "%1"=="" goto usage

REM xbmkdir -x %1 xs:\%__TITLEID%
xbmkdir -x %1 xe:\MUConfig
xbcp /r /y /d /t /f %_NTBINDIR%\private\test\ui\muconfig\tdata\*.* xe:\MUConfig
xbattrib /r /s /h -ro -hid xe:\MUConfig\*.*
xbcp /y /f %_NT386TREE%\dump\muconfig.xbe xe:\MUConfig
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
