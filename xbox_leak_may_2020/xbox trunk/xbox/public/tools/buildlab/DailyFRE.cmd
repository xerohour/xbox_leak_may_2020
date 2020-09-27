@echo off
ECHO DailyFRE

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
if not "%1"=="" goto usage

rem If called from DailyCHK, _BUILDVER will be inherited

net stop cisvc
rem stop search service so file system runs faster

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
cd /d %_NTROOT%\private
call xbuild
call cpbuild
if "%label%"=="quit" goto end

REM :buildvc
REM rem REQUIRES LocStudio, check http://toolzone/locstudio
REM echo :buildvc %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
REM cd /d %_NTROOT%\Private\VC6AddOn\Loc
REM rem LocStudio needs the edb file to be writeable, just to open it
REM attrib -R XboxDLL_A.Edb
REM Build -cz
REM attrib +R XboxDLL_A.Edb
REM rem cpbuild copies the build to XBLD_PVT, defined in the environment
REM call cpbuild
REM if "%label%"=="quit" goto end

:burn
rem CHK has to be built first
rem the external recovery cd uses a combination of chk and fre
rem Retail, located at XBLD_RET, must be built to be included
rem on the recovery cd.  Can be skipped by setting noret=1
echo :burn %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
if "%noret%"=="1" (
    call updrec -e %XBLD_FRE%bins %XBLD_FRE%bins %XBLD_CHK%bins %XBLD_CHK%
) else (
    call updrec -e %XBLD_RET%bins %XBLD_FRE%bins %XBLD_CHK%bins %XBLD_CHK%
)
copy %XBLD_CHK%bins\rec_ext.iso %XBLD_PVT%\%_BUILDVER%\checked\.
if "%label%"=="quit" goto end

:muconfig
cd /d %_NTROOT%\private\test\ui\MUConfig
call createiso.cmd
copy %_NTROOT%\private\test\ui\MUConfig\MUConfig.iso %XBLD_PVT%\%_BUILDVER%\checked\.
if "%label%"=="quit" goto end

:builddocs
echo :builddocs %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
xcopy /i /s /y "%PATH_SDDOCROOT%\build\docbuild" "%PATH_DOCBUILD%"

attrib -r "%PATH_DOCBUILD%\*.*"
attrib -r "%PATH_DOCBUILD%\bin\*.*" /s

cd /d %PATH_DOCBUILD%
rem see joehall for docbuild issues
call docbuild.bat -realbuild -nosync -buildtype online
xcopy /i /y %PATH_DOCBUILD%\xboxsdk.*     %XBLD_PVT%\%_BUILDVER%\Doc
xcopy /i /y %PATH_DOCBUILD%\samples\*.doc %XBLD_PVT%\%_BUILDVER%\Doc\Samples
if "%label%"=="quit" goto end

:buildXBSE
rem Build Setup for Windows Explorer Shell Extension for Xbox
rem see mitchd for shell extension issues
cd /d %XBLD_FRE%\private\setup\xbse
call xbsebuild.bat %1
if "%label%"=="quit" goto end

:buildSDK
rem see jcahill for SDK issues
rem chk and fre must be in XBLD_PVT\_BUILDVER
rem xpacker (called by xdkbuild) places setup in XBLD_PVT\_BUILDVER
echo :buildsdk %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
cd /d %XBLD_FRE%\private\setup\xdk
call xdkbuild.bat %1
if "%label%"=="quit" goto end

:makecd
rem make XboxSDK CD and Samples CD
REM call makecd
call makesamples
if "%label%"=="quit" goto end

echo fine %TIME%  >> %XBLD_LOG%
:notify
rem use sendfile from //toolbox to email from command line
rem sendfile only works with Outlook 2K SP1, XP will block it
cd /d %XBLD_FRE%\private
if exist %XBLD_CHK%\private\build.err (
    echo [CHK err] >> build_err_mail.txt
    echo.          >> build_err_mail.txt
    cat %XBLD_CHK%\private\build.err >> build_err_mail.txt
    echo.          >> build_err_mail.txt
    echo.          >> build_err_mail.txt
    echo.          >> build_err_mail.txt
    )
if exist build.err (
    echo [FRE err] >> build_err_mail.txt
    echo.          >> build_err_mail.txt
    cat build.err  >> build_err_mail.txt
    )
if exist build_err_mail.txt (
    call sendfile xboxos -f "MS Exchange Settings" -s "Build ERRORS (%_BUILDVER%)"  -t build_err_mail.txt -a %XBLD_LOG%
    call sendfile 1149257@skytel.com -f "MS Exchange Settings" -s "Build ERRORS (%_BUILDVER%)"
    ) else (
    call sendfile xboxos -f "MS Exchange Settings" -s "No Build Errors (%_BUILDVER%)" -a %XBLD_LOG%
    call sendfile 1149257@skytel.com -f "MS Exchange Settings" -s "No Build Errors (%_BUILDVER%)" -a %XBLD_LOG%
)


goto end
:noras
@echo dailybuild must be run from a Razzle window

:usage
echo %0 [-now OR -nosync OR -lc label OR -lq label] [-noret]
echo   -now        sync current time
echo   -nosync     do not sync or setversion
echo   -lc label   start at :label and continue script
echo   -lq label   only execute lable block
echo   -noret      do not include retail into rec_ext.iso
echo.
echo   Default syncs to 4am this morning
echo   Labled blocks are: sync, setversion, build, buildvc, burn, builddocs, 
echo   buildSDK, buildXBSE, makecd, and notify.
echo.
echo   Progress is logged to XBLD_LOG, defined in environment.

:end
rem net start cisvc
echo %date%    %time%
