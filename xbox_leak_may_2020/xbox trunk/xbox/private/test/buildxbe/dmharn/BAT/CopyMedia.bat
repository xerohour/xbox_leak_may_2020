pushd .
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1


@echo off
REM Only copies the media to the XBox.
REM ----------------------------------


set FILENAME=%1
set APPID=A7049955

REM echo -----------------------------------
REM echo Wiping old MEDIA files from XBox.
REM echo -----------------------------------
REM call WipeMedia
echo -----------------------------------
echo Copying needed MEDIA files to XBox.
echo -----------------------------------
xbmkdir xc:\tdata
xbmkdir xc:\tdata\%APPID%\
xbmkdir xc:\tdata\%APPID%\MEDIA\
xbcp -d -y -r -f %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1\media\*.* xc:\tdata\%APPID%\MEDIA


REM Copy the test case, if specified.
REM ---------------------------------
if "%1"=="" goto SKIP_PARAM
if not exist TestParams\%FILENAME%.txt goto NOEXIST
call Param %filename%
:SKIP_PARAM


echo -----------------------------------
echo Done copying MEDIA files to XBox.
echo -----------------------------------
goto END

:NOEXIST
echo ------------------------------------------
echo ERROR: TestParams\%FILENAME%.txt DOES NOT EXIST!!!
echo ------------------------------------------
goto END

:END




popd .