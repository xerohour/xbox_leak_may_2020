@echo off
ECHO DailyRET

if "%1"=="/?" goto usage
if "%1"=="-?" goto usage
if "%_BUILDVER%" == "" goto noraz

if "%1"=="-now" (
    set syncnow=1
    shift
)
if "%1"=="-nosync" (
    set nosync=1
    shift
)
if "%1"=="-lc" (
    rem jump to label and continue
    set label=continue
    shift
    shift
    if not "%2"=="" goto %2
)
if "%1"=="-lq" (
    rem jump to label then quit
    set label=quit
    shift
    shift
    if not "%2" == "" goto %2
)
if not "%1" == "" goto usage

net stop cisvc
rem stop search service so file system runs faster

:sync
%_NTDRIVE%
cd %_NTROOT%
if "%nosync%"=="1" goto build
echo :sync %XBLABTYPE% start %DATE% %TIME%  > %XBLD_LOG%
if "%syncnow%"=="1" (
    call sd sync >> %XBLD_LOG%
    ) else (
    call sd sync @%date:~-4%/%date:~4,2%/%date:~7,2%:04:00:00 >> %XBLD_LOG%
    rem sync 4am this morning
)
if "%label%"=="quit" goto end

:setversion
echo :setversion %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
rem find verion number in private\inc\xboxverp.h and increment by one
for /f "tokens=5" %%s in ('grep "#define VER_PRODUCTBUILD " %_NTROOT%\private\inc\xboxverp.h') do set oldver=%%s
set /a newver=%oldver%+1

rem set new version number into current environment
set _BUILDVER=%newver%
set SDFORMEDITOR=sdforms.exe -c "VER_PRODUCTBUILD <- %newver%"

cd %_NTROOT%\private\inc
call sd edit xboxverp.h
type xboxverp.h | perl -p -e "s/VER_PRODUCTBUILD\s+%oldver%/VER_PRODUCTBUILD            %newver%/;s/_XTL_VER\s+%oldver%/_XTL_VER                   %newver%/" > tmpf
move /y tmpf xboxverp.h
call sd submit xboxverp.h

rem set version number into setenv
cd %_NTROOT%\private\developr\%_NTUSER%
call sd edit setenv.cmd
type setenv.cmd | perl -p -e "s/set _BUILDVER=.*/set _BUILDVER=%newver%/" > tmpf
move /y tmpf setenv.cmd
call sd submit setenv.cmd
echo %_BUILDVER% > %XBLD_PVT%\latest.txt
if "%label%"=="quit" goto end

:build
echo :build %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
cd %_NTROOT%\private
call xbuild
rem cpbuild copies the build to XBLD_PVT, defined in the environment
call cpbuild -q
if "%label%"=="quit" goto end

:burn
echo :burn %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
REM call updrec
REM copy %_NT386TREE%\rec_retail.iso %XBLD_PVT%\%_BUILDVER%\checked\.
if "%label%"=="quit" goto end

rem start chk build, options are saved in the environment, and do not need to be passed
if "%_ALLOFIT%" == "1" if "%fBuildBranch%" == "0" %XBLD_CHK%\private\developr\xblds\myrazzle.cmd D:\scripts\DailyCHK.cmd
if "%_ALLOFIT%" == "1" if "%fBuildBranch%" == "1" %_NTDRIVE%\xbox-%CurrentBranch%\private\developr\xblds\%CurrentBranch%razzle.cmd D:\scripts\DailyCHK.cmd

goto end
:noraz
@echo dailybuild must be run from a Razzle window

:usage
echo %0 [-now OR -nosync OR -lc label OR -lq label]
echo   -now        sync current time, and build
echo   -nosync     do not sync or setversion, and build
echo   -lc label   start at :label and continue script
echo   -lq label   only execute lable block
echo.
echo   Default syncs to 4am this morning
echo   Labled blocks are: sync, setversion, build, and burn.
echo.
echo   Progress is logged to XBLD_LOG, defined in environment.

:end
rem net start cisvc
echo %date%    %time%

