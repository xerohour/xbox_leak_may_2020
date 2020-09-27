@echo off
build -z

@echo ... copying files ...
xbcp -y %_NT386TREE%\dump\fillrate.xbe xe:\fillrate.xbe

@REM fillrate loads the file based on date
@REM but touch fails on ro files and makes
@REM my editor reload the file as well.
@REM hence the mess below.

copy fillrate.ini ~fillrate.~ni
xbcp -t -f -y ~fillrate.~ni xe:\fillrate.ini
del ~fillrate.~ni
