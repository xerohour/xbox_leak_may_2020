set XBENAME=AudConsole.xbe
set APPID=00000000

if not "%1" == "" call p %1

@echo WIPING %XBENAME% from XBox.
xbdel xe:\tools\audioconsole\%XBENAME%

REM Deleting any old .XBE so if the build fails it won't be copied.
del /q %_NT386TREE%\dump\%XBENAME%

REM wipe out the .EXE so the .XBE gets rebuilt too!  Since we wiped out the XBE in retail\dump, this must get rebuilt.
del exe\obj\i386\*.exe
build -cz

REM Copy the executable.
xbcp -y -f %_NT386TREE%\dump\%XBENAME% xe:\tools\audioconsole

REM Copy the media
xbmkdir xe:\tools\
xbmkdir xe:\tools\audioconsole\
xbmkdir xe:\tools\audioconsole\media
xbcp -f -r Media\*.* xe:\tools\audioconsole\media
