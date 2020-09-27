@echo off

SET FILENAME=%1
if not exist TestParams\%FILENAME%.txt goto NOEXIST


REM Copies the specified file to t:\testini.ini to be used as the test settings.
REM ---------------------------------------------------------------------------
set APPID=A7049955
set FILENAME=%1
xbcp -y -f TestParams\%FILENAME%.txt xc:\tdata\%APPID%\testini.ini
goto END

:NOEXIST
echo ------------------------------------------
echo ERROR: TestParams\%FILENAME%.txt DOES NOT EXIST!!!
echo ------------------------------------------
goto END

:END
