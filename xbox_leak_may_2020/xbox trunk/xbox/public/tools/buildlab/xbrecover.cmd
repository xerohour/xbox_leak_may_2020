@echo on
::
:: update XSS to latest without needing recovery CD
:: machine must be capable of booting to the dash

pushd .
setlocal
if "%1"=="/?" goto usage
if "%1"=="" goto usage
if "%1"=="-e" (
    shift
    set rec_ext=1
)

rem rom types are: dvt2, dvt2_ext, dvt3, dvt3_ext, dvt4, dvt4_ext.

for /f %%a in (\\xbuilds\release\usa\latest.txt) do set BuildVer=%%a
echo BuildVer %BuildVer%

if "%romtype%"=="" set romtype=dvt4
if "%rec_ext%"=="1" set romtype=%romtype%_ext
set source=\\xbuilds\release\usa
set xboxname=%1
set _NT386TREE=\\xbuilds\release\usa\%buildver%\checked
set rec=recovery
if "%rec_ext%"=="1" set rec=rec_ext

xbcp -x %xboxname% -y r:\devkit\cydrive.exe xe:\dxt\cydrive.dxt
xbreboot -x %xboxname%

goto flash
REM ***************************
rem **copybins without pauses**
echo Ready to update your target Xbox's kernel and dashboard from %_NT386TREE%
echo.
xbcp -x %Xboxname% -f -q -y %_NT386TREE%\boot\xboxrom_%romtype%.bin xc:\xboxrom.bin
if not "0" == "%errorlevel%" goto errormode
xbcp -x %Xboxname% -f -q -y %_NT386TREE%\devkit\cydrive.exe xe:\dxt\cydrive.dxt
if not "0" == "%errorlevel%" goto errormode
xbcp -x %Xboxname% -f -q -y %_NT386TREE%\dump\xshell.xbe xy:\
if not "0" == "%errorlevel%" goto errormode
echo.
echo Ready to reboot your target Xbox
echo.
xbreboot -x %Xboxname% -c
if not "0" == "%errorlevel%" goto errormode
rem  **end copybins**

:flash
echo flashing %romtype%
xbflash -x %xboxname% -y %_NT386TREE%\boot\xboxrom_%romtype%.bin
if not "0" == "%errorlevel%" goto errormode
echo xbreboot -x %Xboxname% -cs
xbreboot -x %Xboxname% -cs
if not "0" == "%errorlevel%" goto errormode
echo formatting xbox drives
xbformat -x %Xboxname% 1
if not "0" == "%errorlevel%" goto errormode
xbformat -x %Xboxname% 2
if not "0" == "%errorlevel%" goto errormode
xbformat -x %Xboxname% 3
if not "0" == "%errorlevel%" goto errormode
xbformat -x %Xboxname% 4
if not "0" == "%errorlevel%" goto errormode
xbformat -x %Xboxname% 5
if not "0" == "%errorlevel%" goto errormode

xbcp -x %Xboxname% /r /y /d /t \\xbuilds\%rec%\XC\* xc:\
if not "0" == "%errorlevel%" goto errormode
xbcp -x %Xboxname% /r /y /d /t \\xbuilds\%rec%\XY\*  xy:\
if not "0" == "%errorlevel%" goto errormode

xbreboot -x %Xboxname% -c
if not "0" == "%errorlevel%" goto errormode

goto end

:errormode
echo Recovery Error
pause

:usage
echo Usage: xbrecover {xboxname}
echo.

:end
endlocal
popd
