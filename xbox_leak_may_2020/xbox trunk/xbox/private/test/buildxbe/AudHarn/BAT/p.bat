@echo off
REM Copies the specified file to t:\testini.ini to be used as the test settings.
REM ---------------------------------------------------------------------------
set APPID=A7049955
set FILENAME=%1

REM Copy the test case, if specified.
REM ---------------------------------
if "%1"=="" goto END
if not exist %_NTDRIVE%%_NTROOT%\private\test\multimedia\dsound\TestParams\%FILENAME%.txt goto NOEXIST
xbcp -y -f %_NTDRIVE%%_NTROOT%\private\test\multimedia\dsound\TestParams\%FILENAME%.txt xc:\tdata\%APPID%\testini.ini
goto END

:NOEXIST
echo ------------------------------------------
echo ERROR: TestParams\%FILENAME%.txt DOES NOT EXIST!!!
echo ------------------------------------------
goto END


:END
