@echo off
SET __TITLEID=0000000D
SET __PROGNAME=GRate3T

if "%1"=="" goto usage

xbmkdir -x %1 xs:\%__TITLEID%
xbcp /r /y /d /t /f %_NTBINDIR%\private\test\ui\GameRatings\%__PROGNAME%\tdata\*.* xs:\%__TITLEID%
xbattrib /r /s /h -ro -hid xs:\%__TITLEID%\*.*
xbcp /y /f %_NT386TREE%\dump\%__PROGNAME%.xbe xe:\
goto end

:usage
Echo Copies the XBox Test Game Ratings program to the specified XBox.
Echo.
Echo COPYTOXB xboxname
Echo.
Echo xboxname     The name of the XBOX to where you wish to copy the program.
goto end

:end
SET __TITLEID=
SET __PROGNAME=