@echo off

if "%1"=="" goto usage

REM Cleanup Previous Files
xbdel -x %1 /r /s /h /f xy:\3dinfo
xbdel -x %1 /r /s /h /f xy:\data
xbdel -x %1 /r /s /h /f xy:\images
xbdel -x %1 /r /s /h /f xy:\media
xbdel -x %1 /r /s /h /f xy:\menus
xbdel -x %1 /r /s /h /f xy:\sounds
xbdel -x %1 /r /s /h /f xy:\xshell.xbe

REM Copy the new files over
xbattrib -x %1 /r /s /h -ro -hid xy:\
xbcp -x %1 /r /y /d /t /f %_NTBINDIR%\private\test\ui\xshell\tdata\*.* xy:\
xbcp -x %1 /y /f %_NT386TREE%\dump\xshell.xbe xy:\xshell.xbe
xbattrib -x %1 /r /s /h -ro -hid xy:\
goto end

:usage
Echo Copies the XDK Launcher program to the specified XBox.
Echo.
Echo COPYTOXB xboxname
Echo.
Echo xboxname     The name of the XBOX to where you wish to copy the program.
goto end

:end
