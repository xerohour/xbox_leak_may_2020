@echo off
REM setlocal
echo Build Recovery Disk .iso for Hardware Test(s)

if "-?" == "%1" goto usage
if "/?" == "%1" goto usage

set __XBETREE=%_NT386TREE%
set __X386TREE=%_NT386TREE%
set __XBINDIR=%_NTBINDIR%
set __DEST=%_NTBINDIR%\private\ntos\recovery\cdfiles
goto cleanupfiles

:cleanupfiles
echo Cleaning up %__DEST%...
echo.

mkdir %__DEST% 2>nul
delnode /q %__DEST%\*.*

@echo Copying files to %__DEST%...
@echo.

if not "%__RETAILTREE%" == "" mkdir %__DEST%\RETAIL 2>nul

if "%__RETAILTREE%" == "" copy /y %__XBETREE%\dump\recovery.xbe %__DEST%\default.xbe
if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\dump\recovery.xbe %__DEST%\default.xbe
copy /y %__X386TREE%\devkit\xbdm.dll %__DEST%\xbdm.dll

copy /y %__X386TREE%\boot\XBOXROM_DVT6.BIN %__DEST%\XBOXROM.BIN
copy /y %__X386TREE%\boot\XBOXROM_DVT4.BIN %__DEST%\XBOXROM_DVT4.BIN


if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\boot\XBOXROM_DVT6.BIN %__DEST%\RETAIL\XBOXROM.BIN
if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\boot\XBOXROM_DVT4.BIN %__DEST%\RETAIL\XBOXROM_DVT4.BIN


mkdir %__DEST%\RECMEDIA 2>nul
xcopy /y /e %__XBINDIR%\private\ntos\recovery\recmedia %__DEST%\RECMEDIA

rd /s /q %__DEST%\XDASH
mkdir %__DEST%\XDASH 2>nul
REM Copy Common Media
mkdir %__DEST%\DEVKIT 2>nul
mkdir %__DEST%\DEVKIT\dxt 2>nul
mkdir %__DEST%\DEVKIT\TESTS 2>nul
mkdir %__DEST%\DEVKIT\TESTS\media 2>nul
mkdir %__DEST%\DEVKIT\TESTS\filler 2>nul
mkdir %__DEST%\tdata\00000000 2>nul
xcopy /y /e %_NTBINDIR%\private\test\usb\media\*.* %__DEST%\DEVKIT\TESTS\media
xcopy /y /e %_NTBINDIR%\private\test\usb\cpx\tdata\*.* %__DEST%\tdata\00000000
rem 20 megs of filler to amke sure the image is large enough
rem xcopy /y /e %_NTBINDIR%\private\test\multimedia\media\media\music\Scott\BBall\*.* %__DEST%\DEVKIT\TESTS\filler
copy /y %__X386TREE%\devkit\cydrive.exe %__DEST%\devkit\dxt\cydrive.dxt
if "-mu" == "%1" goto muimage
if "-gp" == "%1" goto gpimage
if "-dvd" == "%1" goto dvdimage
if "-duke" == "%1" goto dukeimage
if "-sl" == "%1" goto slimage



:allimage
set __IMAGE=rec_hwtest.iso

REM Copy Launcher Files as the default app

REM Copy Launcher
mkdir %__DEST%\XDASH 2>nul

copy /y %__XBETREE%\dump\xshell.xbe %__DEST%\XDASH
xcopy /y /e %__XBINDIR%\private\test\ui\xshell\tdata %__DEST%\XDASH

copy /y %__XBINDIR%\private\test\ui\xshell\dashboard.xbx %__DEST%\dashboard.xbx


mkdir %__DEST%\DEVKIT\SAMPLES 2>nul
mkdir %__DEST%\DEVKIT\SAMPLES\DolphinClassic 2>nul
mkdir %__DEST%\DEVKIT\SAMPLES\DolphinClassic\Media 2>nul
copy /y %__XBETREE%\dump\DolphinClassic.xbe %__DEST%\DEVKIT\SAMPLES\DolphinClassic
xcopy /y /e %__XBINDIR%\private\atg\samples\graphics\dolphinclassic\media %__DEST%\DEVKIT\SAMPLES\DolphinClassic\Media

REM Copy GamePad Files
mkdir %__DEST%\DEVKIT\TESTS\Gamepad 2>nul
mkdir %__DEST%\DEVKIT\TESTS\Gamepad\Media 2>nul
copy /y %__XBETREE%\dump\Gamepad.xbe %__DEST%\DEVKIT\TESTS\Gamepad
xcopy /y /e %__XBINDIR%\private\atg\samples\input\gamepad\media %__DEST%\DEVKIT\TESTS\Gamepad\Media

REM Copy Rumble Files
mkdir %__DEST%\DEVKIT\TESTS\rumble 2>nul
mkdir %__DEST%\DEVKIT\TESTS\rumble\Media 2>nul
copy /y %__XBETREE%\dump\rumble.xbe %__DEST%\DEVKIT\TESTS\rumble
xcopy /y /e %__XBINDIR%\private\atg\samples\input\rumble\media %__DEST%\DEVKIT\TESTS\rumble\Media

REM COPY SLIX Files
mkdir %__DEST%\DEVKIT\TESTS\SLIX 2>nul
copy /y %__XBETREE%\dump\slixbox.xbe %__DEST%\DEVKIT\TESTS\SLIX

REM Copy Duke Files
mkdir %__DEST%\DEVKIT\TESTS\Duke 2>nul
copy /y %__XBETREE%\dump\dukeprod.xbe %__DEST%\DEVKIT\TESTS\Duke

REM Copy MU Files
mkdir %__DEST%\DEVKIT\TESTS\MU 2>nul
copy /y %__XBETREE%\dump\muprod23.xbe %__DEST%\DEVKIT\TESTS\MU

REM Copy DVD Files
mkdir %__DEST%\DEVKIT\TESTS\DVD 2>nul
copy /y %__XBETREE%\dump\DVDProd.xbe %__DEST%\DEVKIT\TESTS\DVD

REM Copy MURW Files
mkdir %__DEST%\DEVKIT\TESTS\MURW 2>nul
copy /y %__XBETREE%\dump\MURW.xbe %__DEST%\DEVKIT\TESTS\MURW

REM Copy REENUM Files
mkdir %__DEST%\DEVKIT\TESTS\REENUM 2>nul
copy /y %__XBETREE%\dump\REENUM.xbe %__DEST%\DEVKIT\TESTS\REENUM

REM Copy THUMB Files
mkdir %__DEST%\DEVKIT\TESTS\THUMB 2>nul
copy /y %__XBETREE%\dump\THUMBPOSITION.xbe %__DEST%\DEVKIT\TESTS\THUMB

REM Copy HWHarness Files
mkdir %__DEST%\DEVKIT\TESTS\HWHARNESS 2>nul
copy /y %__XBETREE%\dump\HWHARNESS.xbe %__DEST%\DEVKIT\TESTS\HWHARNESS
copy /y %__XBETREE%\xboxtest\hwtestini\emc.ini %__DEST%\DEVKIT\TESTS\HWHARNESS\testini.ini

goto image


:dukeimage
REM Copy Duke Files

set __IMAGE=rec_duketest.iso
mkdir %__DEST%\XDASH 2>nul

REM Copy DukeProd.xbe to xboxdash.xbe
copy /y %__XBETREE%\dump\dukeprod.xbe %__DEST%\XDASH\xboxdash.xbe

goto image


:muimage
REM Copy MU Files

set __IMAGE=rec_mutest.iso
mkdir %__DEST%\XDASH 2>nul

REM Copy Mutest.xbe to Xboxdash.xbe
copy /y %__XBETREE%\dump\MUPROD23.xbe %__DEST%\XDASH\xboxdash.xbe

goto image


:dvdimage
set __IMAGE=rec_Dvdtest.iso

REM Copy Launcher
rd /s /q %__DEST%\XDASH
mkdir %__DEST%\XDASH 2>nul

copy /y %__XBETREE%\dump\xshell.xbe %__DEST%\XDASH
xcopy /y /e %__XBINDIR%\private\test\ui\xshell\tdata %__DEST%\XDASH
copy /y %__XBINDIR%\private\test\ui\xshell\dashboard.xbx %__DEST%\dashboard.xbx


REM Copy DVDProd Region1
mkdir %__DEST%\DEVKIT\TESTS\DVDR1 2>nul
REM Copy DVDProd.xbe
copy /y %__XBETREE%\dump\DVDprod.xbe %__DEST%\DEVKIT\TESTS\DVDR1\DVDR1.xbe
REM Copy DVD Library file
copy /y %__XBETREE%\dump\dvdkey1d.bin %__DEST%\DEVKIT\TESTS\DVDR1\dvdsource.lib


REM Copy DVDProd Region2
mkdir %__DEST%\DEVKIT\TESTS\DVDR2 2>nul
REM Copy DVDProd.xbe
copy /y %__XBETREE%\dump\DVDprod.xbe %__DEST%\DEVKIT\TESTS\DVDR2\DVDR2.xbe
REM Copy DVD Library file
copy /y %__XBETREE%\dump\dvdkey2d.bin %__DEST%\DEVKIT\TESTS\DVDR2\dvdsource.lib

REM Copy DVDProd Region3
mkdir %__DEST%\DEVKIT\TESTS\DVDR3 2>nul
REM Copy DVDProd.xbe
copy /y %__XBETREE%\dump\DVDprod.xbe %__DEST%\DEVKIT\TESTS\DVDR3\DVDR3.xbe
REM Copy DVD Library file
copy /y %__XBETREE%\dump\dvdkey3d.bin %__DEST%\DEVKIT\TESTS\DVDR3\dvdsource.lib

goto image


:slimage

set __IMAGE=rec_thdpty.iso

REM Copy Launcher Files as the default app

REM Copy Launcher
mkdir %__DEST%\XDASH 2>nul

copy /y %__XBETREE%\dump\xshell.xbe %__DEST%\XDASH
xcopy /y /e %__XBINDIR%\private\test\ui\xshell\tdata %__DEST%\XDASH

copy /y %__XBINDIR%\private\test\ui\xshell\dashboard.xbx %__DEST%\dashboard.xbx


mkdir %__DEST%\DEVKIT\SAMPLES 2>nul
mkdir %__DEST%\DEVKIT\SAMPLES\DolphinClassic 2>nul
mkdir %__DEST%\DEVKIT\SAMPLES\DolphinClassic\Media 2>nul
copy /y %__XBETREE%\dump\DolphinClassic.xbe %__DEST%\DEVKIT\SAMPLES\DolphinClassic
xcopy /y /e %__XBINDIR%\private\atg\samples\graphics\dolphinclassic\media %__DEST%\DEVKIT\SAMPLES\DolphinClassic\Media

REM Copy GamePad Files
mkdir %__DEST%\DEVKIT\TESTS\Gamepad 2>nul
mkdir %__DEST%\DEVKIT\TESTS\Gamepad\Media 2>nul
copy /y %__XBETREE%\dump\Gamepad.xbe %__DEST%\DEVKIT\TESTS\Gamepad
xcopy /y /e %__XBINDIR%\private\atg\samples\input\gamepad\media %__DEST%\DEVKIT\TESTS\Gamepad\Media

REM Copy MURW Files as HUBTEST
mkdir %__DEST%\DEVKIT\TESTS\HUBTest 2>nul
copy /y %__XBETREE%\dump\MURW.xbe %__DEST%\DEVKIT\TESTS\HUBTest\Hubtest.xbe

REM Copy MUTGen Files
mkdir %__DEST%\DEVKIT\TESTS\MU 2>nul
copy /y %__XBETREE%\dump\mutgen.xbe %__DEST%\DEVKIT\TESTS\MU\MUTgen.xbe

REM Copy MUProd Files
mkdir %__DEST%\DEVKIT\TESTS\MU 2>nul
copy /y %__XBETREE%\dump\muprod23.xbe %__DEST%\DEVKIT\TESTS\MU\MUProd.xbe

REM Copy REENUM Files
mkdir %__DEST%\DEVKIT\TESTS\REENUM 2>nul
copy /y %__XBETREE%\dump\REENUM.xbe %__DEST%\DEVKIT\TESTS\REENUM

goto image




:gpimage

set __IMAGE=rec_gptest.iso
REM Copy Launcher
rd /s /q %__DEST%\XDASH
mkdir %__DEST%\XDASH 2>nul

copy /y %__XBETREE%\dump\xshell.xbe %__DEST%\XDASH
xcopy /y /e %__XBINDIR%\private\test\ui\xshell\tdata %__DEST%\XDASH
copy /y %__XBINDIR%\private\test\ui\xshell\dashboard.xbx %__DEST%\dashboard.xbx

REM Copy GamePad Files
mkdir %__DEST%\DEVKIT\TESTS\Gamepad 2>nul
mkdir %__DEST%\DEVKIT\TESTS\Gamepad\Media 2>nul
copy /y %__XBETREE%\dump\Gamepad.xbe %__DEST%\DEVKIT\TESTS\Gamepad
xcopy /y /e %__XBINDIR%\private\atg\samples\misc\gamepad\media %__DEST%\DEVKIT\TESTS\Gamepad\Media

REM Copy Rumble Files
mkdir %__DEST%\DEVKIT\TESTS\rumble 2>nul
mkdir %__DEST%\DEVKIT\TESTS\rumble\Media 2>nul
copy /y %__XBETREE%\dump\rumble.xbe %__DEST%\DEVKIT\TESTS\rumble
xcopy /y /e %__XBINDIR%\private\atg\samples\misc\rumble\media %__DEST%\DEVKIT\TESTS\rumble\Media

goto image


:image
@echo off
gdfimage %__DEST% %__X386TREE%\%__IMAGE%
goto end

:usage
echo "usage: hwtrec [-all | -dvd | -gp | -mu]"
echo "  -mu  = Build Memory Unit Production Test Image"
echo "  -gp  = Build Gamepad Production Test Image"
echo "  -dvd = Build DVD Dongle Production Test Image"
echo "  -all = Build All Peripheral Test Image"
echo "  (_NT386TREE must be defined)"

:end
endlocal
