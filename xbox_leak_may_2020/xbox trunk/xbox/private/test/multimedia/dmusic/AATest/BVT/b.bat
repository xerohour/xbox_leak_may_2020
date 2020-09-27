REm @echo off
set XBENAME=AATest_BVT.xbe
set APPID=A7049955

@echo WIPING %XBENAME% from XBox.
xbdel xe:\%XBENAME%

REM Deleting any old .XBE so if the build fails it won't be copied.
del /q %_NT386TREE%\dump\%XBENAME%

REM wipe out the .EXE so the .XBE gets rebuilt too!  Since we wiped out the XBE in retail\dump, this must get rebuilt.
del obj\i386\*.exe
build -zc

REM Copy the executable.
xbcp -y -f %_NT386TREE%\dump\%XBENAME% xe:\

