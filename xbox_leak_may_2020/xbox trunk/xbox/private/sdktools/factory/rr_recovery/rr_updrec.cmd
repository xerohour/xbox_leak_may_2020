@echo on
REM setlocal

if "" == "%_NT386TREE%" goto usage
if "-?" == "%1" goto usage
if "/?" == "%1" goto usage

set __RETAILTREE=%1
set __DEST=%2
if "" == "%1" set __RETAILTREE=
if "" == "%2" set __DEST=%_NTBINDIR%\private\sdktools\factory\rr_recovery\cdfiles
set __XBETREE=%_NT386TREE%
set __X386TREE=%_NT386TREE%
set __XBINDIR=%_NTBINDIR%
set __IMAGE=rr_recovery.iso

@echo Cleaning up %__DEST%...
@echo.

mkdir %__DEST% 2>nul
delnode /q %__DEST%\*.*

@echo Copying files to %__DEST%...
@echo.

rd /s /q %__DEST%\RETAIL
if not "%__RETAILTREE%" == "" mkdir %__DEST%\RETAIL 2>nul

if "%__RETAILTREE%" == "" copy /y %__XBETREE%\dump\rr_recovery.xbe %__DEST%\default.xbe
if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\dump\rr_recovery.xbe %__DEST%\default.xbe
copy /y %__X386TREE%\devkit\xbdm.dll %__DEST%\xbdm.dll

copy /y %__X386TREE%\boot\XBOXROM_DVT4.BIN %__DEST%\XBOXROM.BIN


if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\boot\XBOXROM_DVT4.BIN %__DEST%\RETAIL\XBOXROM.BIN

mkdir %__DEST%\RECMEDIA 2>nul
xcopy /y /e %__XBINDIR%\private\sdktools\factory\rr_recovery\recmedia %__DEST%\RECMEDIA

rd /s /q %__DEST%\XDASH
mkdir %__DEST%\XDASH 2>nul


mkdir %__DEST%\TDATA 2>nul
mkdir %__DEST%\TDATA\fffe0000 2>nul
copy /y %__XBETREE%\dump\xboxdash.xbe %__DEST%\XDASH
if not "%__RETAILTREE%" == "" copy /y %__RETAILTREE%\dump\xboxdash.xbe %__DEST%\RETAIL
xcopy /y %__XBINDIR%\private\ui\dash\*.xip %__DEST%\XDASH
xcopy /y %__XBINDIR%\private\ui\dash\*.xtf %__DEST%\XDASH
xcopy /y %__XBINDIR%\private\ui\dash\SoundFXADPCM %__DEST%\XDASH
xcopy /y %__XBINDIR%\private\ui\dash\Audio\AmbientAudioADPCM %__DEST%\XDASH\Audio\AmbientAudio\
xcopy /y %__XBINDIR%\private\ui\dash\Audio\TransitionAudioADPCM %__DEST%\XDASH\Audio\TransitionAudio\
xcopy /y %__XBINDIR%\private\ui\dash\Audio\MusicAudioADPCM %__DEST%\XDASH\Audio\MusicAudio\
xcopy /y %__XBINDIR%\private\ui\dash\Audio\MainAudioADPCM %__DEST%\XDASH\Audio\MainAudio\
xcopy /y %__XBINDIR%\private\ui\dash\Audio\MemoryAudioADPCM %__DEST%\XDASH\Audio\MemoryAudio\

:image
@echo off

gdfimage %__DEST% %__X386TREE%\%__IMAGE%
goto end

:usage
echo "usage: updrec [-e [XBETREE]] destpath (_NT386TREE must be defined; -e (external version))"

:end
endlocal
