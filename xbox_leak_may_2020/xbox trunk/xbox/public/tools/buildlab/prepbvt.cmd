@echo off
if "%1"=="/?" goto usage
if "%1"=="" goto usage

if %1==1 set source=%XBLD_PVT%\%_BUILDVER%\checked
if %1==2 set source=%XBLD_PVT%\%_BUILDVER%\checked
if %1==3 set source=%XBLD_PVT%\%_BUILDVER%\free
if %1==4 set source=%XBLD_PVT%\%_BUILDVER%\free

if %1==5 set source=%XBLD_PVT%\%_BUILDVER%\checked
if "%2"=="-x" (
    set target=%3
    shift /2
    shift /2
) else (
    set target=xbvt%1
)

echo SOURCE %source%
echo TARGET %target%
if "%2"=="-i" goto copyini

:CopySys
REM xbcp -y -f -x %target% "%source%\boot\xboxrom.bin"    xc:\
REM if not "0" == "%errorlevel%" goto end
REM xbcp -y -f -x %target% "%source%\devkit\xbdm.dll"      xc:\
REM if not "0" == "%errorlevel%" goto end

:DelLogs
xbdel -x %target% xt:\*.log

:putmedia
if not "%2"=="-q" call putmedia "%target%"

:CopyHarness
xbcp -y -f -x %target% "%source%\dump\harness.xbe"     xe:\harness.xbe
if not "0" == "%errorlevel%" goto end
:copyini
xbcp -y -f -x %target% "%source%\xboxtest\xonline.ini"  xe:\xonline.ini
if exist %source%\xboxtest\bvt%1.ini (
    xbcp -y -f -t -x %target% "%source%\xboxtest\bvt%1.ini"  xs:\a7049955\testini.ini
) else (
    xbcp -y -f -t -x %target% "%source%\xboxtest\bvt.ini"  xs:\a7049955\testini.ini
)
if not "0" == "%errorlevel%" goto end



set DSPIMAGE=%_NT386TREE%\DefaultScratchImg.bin
set APPID=A7049955
xbmkdir xs:\%APPID%\MEDIA\DSPCode
echo -----------------------------------
echo Copying default DSP image to XBox
echo -----------------------------------
if not exist %DSPIMAGE% goto DSPIMAGE_NOEXIST
xbdel -f xs:\%APPID%\MEDIA\DSPCode\DefaultScratchImg.bin
xbcp %DSPIMAGE% xs:\%APPID%\MEDIA\DSPCode\
goto DSPIMAGE_EXIST
:DSPIMAGE_NOEXIST
echo ERROR!!! %DSPIMAGE% does not exist on your machine and cannot be copied!!
echo The test app won't run without this file.   You must build DirectSound to create it.
echo (type "directx dsound", then "bcz")
echo If you cannot create this file, its build destination may have moved.
echo Please talk to GeorgioC or DerekS to discover where this file is.
:DSPIMAGE_EXIST


:reboot
xbreboot -x "%target%" xe:\harness.xbe -c

echo rebooting %target%
goto end

:usage
echo prepbvt xbvt# "[-x name | ip] [-q | -i]"
echo     -x if ip or name different from xbvt#
echo     -q Skip putmedia call
echo     -i Just copy testini.ini and reboot
:end
