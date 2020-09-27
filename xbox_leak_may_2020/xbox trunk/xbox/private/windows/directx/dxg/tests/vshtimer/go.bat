@echo off

if not "%1" == "" set opt="%1"
if "%1" == "opt" set opt=

build -z

@echo ... copying files ...
xbcp -y %_NT386TREE%\dump\vshtimer.xbe xe:\vshtimer.xbe

@echo.

%_nt386tree%\idw\xsasm %opt% -nologo -l vstune.vsh
type vstune.lst
xbcp -f vstune.xvu xe:\vstune.xvu

@REM vstune loads the file based on date
@REM but touch fails on ro files and makes
@REM my editor reload the file as well.
@REM hence the mess below.

@echo.

copy vstune.ini ~vstune.~ni
touch ~vstune.~ni
xbcp -t -f -y ~vstune.~ni xe:\vstune.ini
del ~vstune.~ni
