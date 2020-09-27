set XBENAME=DMHost.xbe

@echo WIPING %XBENAME% from XBox.
xbdel xe:\%XBENAME%

REM Deleting any old .XBE so if the build fails it won't be copied.
del /q %_NT386TREE%\dump\%XBENAME%

REM Setting environment variables to build symbols.
call d:\util\batch\s.cmd

REM wipe out the .EXE so the .XBE gets rebuilt too!  Since we wiped out the XBE in retail\dump, this must get rebuilt.
del obj\i386\*.exe
build -DIP

REM Copy the executable.
xbcp -y %_NT386TREE%\dump\%XBENAME% xe:\
