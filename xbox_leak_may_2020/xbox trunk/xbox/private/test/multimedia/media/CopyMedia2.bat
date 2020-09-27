@echo off
REM Only copies the media to the XBox.
REM ----------------------------------

if not "%1" == "" call p %1


:EXIST
set APPID=a7049955
set DSPIMAGE=%_NT386TREE%\DSSTDFX.BIN


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
xbmkdir xc:\tdata\%APPID%\MEDIA\DSPCode\
xbcp -f -y -r -t -d %_NTDRIVE%%_NTROOT%\private\test\multimedia\media\media\*.* xc:\tdata\%APPID%\MEDIA


echo -----------------------------------
echo Copying default DSP image to XBox
echo -----------------------------------
if not exist %DSPIMAGE% goto DSPIMAGE_NOEXIST
xbdel -f xc:\tdata\%APPID%\MEDIA\DSPCode\DSSTDFX.BIN
xbcp %DSPIMAGE% xc:\tdata\%APPID%\MEDIA\DSPCode\
goto DSPIMAGE_EXIST
:DSPIMAGE_NOEXIST
echo !
echo !
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo ERROR!!! %DSPIMAGE% does not exist on your machine and cannot be copied!!
echo The test app won't run without this file.   You must build DirectSound to create it.
echo (type "directx dsound", then "bcz")
echo If you cannot create this file, its build destination may have moved.
echo Please talk to GeorgioC or DerekS to discover where this file is.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo !
echo !
:DSPIMAGE_EXIST

echo -----------------------------------
echo Done copying MEDIA files to XBox.
echo -----------------------------------
goto END


:END
