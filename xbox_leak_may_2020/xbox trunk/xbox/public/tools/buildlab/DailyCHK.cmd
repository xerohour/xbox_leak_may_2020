@echo on
ECHO DailyCHK

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
if "%1"=="-noret" (
    rem no retail for recovery
    set noret=1
    shift
)
if not "%1" == "" goto usage

net stop cisvc
rem stop search service so file system runs faster
set >> %XBLD_LOG%

:sync
%_NTDRIVE%
cd %_NTROOT%
if "%nosync%"=="1" goto build
echo :sync %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
if "%syncnow%"=="1" (
    call sd sync >> %XBLD_LOG%
) else (
    call sd sync @%date:~-4%/%date:~4,2%/%date:~7,2%:04:00:00 >> %XBLD_LOG%
    rem sync 4am this morning
)

:setversion
echo :setversion %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
call sd sync private\inc\xboxverp.h
call sd sync private\developr\xblds\...
if "%label%"=="quit" goto end

:build
echo :build %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
cd %_NTROOT%\private
call xbuild
rem build hardware verification suite CD
call %_NTDRIVE%%_NTROOT%\private\test\hvs\makeDisk.cmd
rem cpbuild copies the build to XBLD_PVT, defined in the environment
call cpbuild
if "%label%"=="quit" goto end

:burn
echo :burn %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
rem Retail, located at XBLD_RET, must be built to be included
rem on the recovery cd.  Can be skipped by setting noret=1
if "%noret%"=="1" (
    call updrec
    call updrec -m
) else (
    call updrec %XBLD_RET%bins
    call updrec -m %XBLD_RET%bins
)
copy %XBLD_CHK%bins\recovery.iso %XBLD_PVT%\%_BUILDVER%\checked\.
copy %XBLD_CHK%bins\rec_manu.iso %XBLD_PVT%\%_BUILDVER%\checked\.
if "%label%"=="quit" goto end

goto next
net send bmckeex %_BUILDVER% BVTs R2R %DATE% %TIME%

:stresscd
setlocal
set STRESSCD=1
call updrec
copy %XBLD_CHK%bins\rec_stress.iso %XBLD_PVT%\%_BUILDVER%\checked\.
endlocal
if "%label%"=="quit" goto end

:hrdimg
setlocal
call hwtrec -duke   
call hwtrec -mu     
call hwtrec -sl
call hwtrec -dvd
endlocal
copy %XBLD_CHK%bins\rec_duketest.iso %XBLD_PVT%\%_BUILDVER%\checked\.
copy %XBLD_CHK%bins\rec_mutest.iso %XBLD_PVT%\%_BUILDVER%\checked\.
copy %XBLD_CHK%bins\rec_thdpty.iso %XBLD_PVT%\%_BUILDVER%\checked\.
copy %XBLD_CHK%bins\rec_dvdtest.iso %XBLD_PVT%\%_BUILDVER%\checked\.
if "%label%"=="quit" goto end

:next
rem start fre build, options are saved in the environment, and do not need to be passed
if "%_ALLOFIT%" == "1" if "%fBuildBranch%" == "0" %XBLD_FRE%\private\developr\xblds\FRErazzle.cmd D:\scripts\DailyFRE.cmd
if "%_ALLOFIT%" == "1" if "%fBuildBranch%" == "1" %_NTDRIVE%\xbox-%CurrentBranch%\private\developr\xblds\%CurrentBranch%FRErazzle.cmd D:\scripts\DailyFRE.cmd

goto end
:noras
@echo dailybuild must be run from a Razzle window

:usage
echo %0 [-now OR -nosync OR -lc label OR -lq label] [-noret]
echo   -now        sync current time, and build
echo   -nosync     do not sync or setversion, and build
echo   -lc label   start at :label and continue script
echo   -lq label   only execute lable block
echo   -noret      do not include retail into recovery.iso
echo.
echo   Default syncs to 4am this morning
echo   Labled blocks are: sync, setversion, build, burn, and hrdimg.
echo.
echo   Progress is logged to XBLD_LOG, defined in environment.

:end
rem net start cisvc
echo %date%    %time%

