REM wipe out the .EXE so the .XBE gets rebuilt too!  Since we wiped out the XBE in retail\dump, this must get rebuilt.
del exe\obj\i386\*.exe
build -DIP
