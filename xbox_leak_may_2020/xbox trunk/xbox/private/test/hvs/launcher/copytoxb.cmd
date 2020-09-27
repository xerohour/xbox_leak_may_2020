@echo off

if "%1"=="" goto usage

xbmkdir -x %1 xe:\HVS
xbcp -x %1 /r /y /d /t /f %_NTBINDIR%\private\test\hvs\launcher\media\*.* xe:\HVS\media
xbattrib -x %1 /r /s /h -ro -hid xe:\HVS\*.*
xbcp -x %1 /y /f %_NT386TREE%\dump\hvslaunch.xbe xe:\HVS\default.xbe
xbcp -x %1 /y /f %_NTBINDIR%\private\test\hvs\launcher\default.cfg xe:\HVS\default.cfg
xbdel -x %1 /r /s xs:\48570000
xbattrib -x %1 /r /s /h -ro -hid xe:\hvs
goto end

:usage
Echo Copies the HVS Launcher program to the specified XBox.
Echo.
Echo COPYTOXB xboxname
Echo.
Echo xboxname     The name of the XBOX to where you wish to copy the program.
goto end

:end
