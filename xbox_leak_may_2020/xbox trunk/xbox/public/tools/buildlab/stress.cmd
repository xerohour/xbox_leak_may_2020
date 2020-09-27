rem STRESS BATCH FILE
@echo off

if not "%6"=="" goto setenv

rem if environment has been set externally, jump over

if "%_xsconfig%"=="1" goto aftersetenv

:usage
    echo USAGE:   stress.cmd {XboxName} {ComPort} {RecoveryType} {Source} {BuildVer|latest} {Kernel_HarnessType}
    echo EXAMPLE: stress.cmd danrosexbox com1 internal latest checked checked
    goto end

:setenv
set XboxName=%1
set ComPort=%2
set recovery=%3
set Source=%4
set BuildVer=%5
set K_Htype=%6

rem copy media for stress?
set CopyMedia=1
rem pause before opening debugger?
set pause=0
rem memory in devkit?
set Testini=stress
set AllowSysCopy=0
:aftersetenv


set _NT_DEBUG_PORT=%ComPort%
set _NT_DEBUG_BAUD_RATE=115200

set NEXT=\\xstress\stress\next.txt

if /i "%BuildVer%"=="latest" for /f %%a in (%Source%\latest.txt) do set BuildVer=%%a

if "%BuildVer%"=="" (
    echo %Source% appears to be offline
    goto errormode
)

if /i "%K_Htype%"=="random" for /f %%n in (%NEXT%) do set K_Htype=%%n

if /i "%K_Htype%"=="chk_chk" (
    set KernelType=checked
    set HarnessType=checked
    echo chk_fre > %NEXT%
)
if /i "%K_Htype%"=="chk_fre" (
    set KernelType=checked
    set HarnessType=free
    echo fre_fre > %NEXT%
)
if /i "%K_Htype%"=="fre_fre" (
    set KernelType=free
    set HarnessType=free
    echo fre_chk > %NEXT%
)
if /i "%K_Htype%"=="fre_chk" (
    set KernelType=free
    set HarnessType=checked
    echo chk_chk > %NEXT%
)

set LocalTools=%TEMP%
set IDWTools=%Source%\%BuildVer%\%HarnessType%\public\idw
set MSTools=%Source%\%BuildVer%\%HarnessType%\public\mstools
set _NT_DEBUGGER_EXTENSION_PATH=%MSTools"
rem  copy tools

rem if not exist %LocalTools% mkdir %LocalTools%
cd /d %LocalTools%
copy /y \\xstress\stress\MSVCRTD.dll    %LocalTools%
copy /y %MSTools%\dbgeng.dll            %LocalTools%
copy /y %MSTools%\dbghelp.dll           %LocalTools%
copy /y %MSTools%\i386kd.exe            %LocalTools%
copy /y %MSTools%\kdextx86.dll          %LocalTools%
copy /y %MSTools%\symsrv.dll            %LocalTools%
copy /y %IDWTools%\remote.exe           %LocalTools%
copy /y %IDWTools%\xboxdbg.dll          %LocalTools%
copy /y %IDWTools%\xbcp.exe             %LocalTools%
copy /y %IDWTools%\xbmkdir.exe          %LocalTools%
copy /y %IDWTools%\xbdel.exe            %LocalTools%
copy /y %IDWTools%\xbreboot.exe         %LocalTools%

copy \\xstress\stress\randfile.exe .

rem  set paths

set REMOTE=start %LocalTools%\remote.exe /s
set KD=%LocalTools%\i386kd.exe
set XBCP=%LocalTools%\xbcp.exe /x %XBOXNAME% /y /f
set XBMKDIR=%LocalTools%\xbmkdir.exe /x %XBOXNAME%
set XBDEL=%LocalTools%\xbdel.exe /x %XBOXNAME% /f /q /r
set XBREBOOT=%LocalTools%\xbreboot.exe /x %XBOXNAME%
set RANDFILE=%LocalTools%\randfile.exe
set REGISTER=\\xstress\stress\registerBox.wsf

set USERS=\\xstress\stress\users.txt
set MEDIADIR=\\xdb01\stresscontent\xbox_media
set WMADIR=\\xdb01\stresscontent\wma
set MEDIAFILES=50
set TDATA=xs:\a7049955

if /i "%recovery%"=="internal" (
    set DUMPDIR=%Source%\%BuildVer%\checked\dump
)
if /i "%recovery%"=="external" (
    set DUMPDIR=%Source%\%BuildVer%\free\dump
)    

set X_TESTINI=%TDATA%\testini.ini
set X_HARNESS=xe:\stress.xbe
set X_YDRIVE=xe:\dxt\cydrive.dxt

set TDATA_MEDIA=%TDATA%\media
set TDATA_MEDIA_AUDIO=%TDATA_MEDIA%\audio
set TDATA_MEDIA_AUDIO_PCM=%TDATA_MEDIA_AUDIO%\pcm
set TDATA_MEDIA_AUDIO_WMA=%TDATA_MEDIA_AUDIO%\wma
set TDATA_MEDIA_AUDIO_PCM_FILES=%TDATA_MEDIA_AUDIO_PCM%\*.*
set TDATA_MEDIA_AUDIO_WMA_FILES=%TDATA_MEDIA_AUDIO_WMA%\*.*

set KTREE=%Source%\%BuildVer%\%KernelType%
set HTREE=%Source%\%BuildVer%\%HarnessType%
set HARNESS=%HTREE%\dump\harness.xbe
set CYDRIVE=%KTREE%\devkit\cydrive.exe

for /f %%n in ('\\xstress\stress\rand.exe 3') do set rand=%%n
rem   Go ahead and pick a random number, because the set 
rem   won't work (take effect) inside brackets.
if /i "%Testini%"=="random" (
    if "%rand%"=="2" set Testini=wfvo
    if "%rand%"=="1" set Testini=stress
    if "%rand%"=="0" set Testini=media
)
set TESTINI=%Source%\%BuildVer%\%HarnessType%\xboxtest\%Testini%.ini

rem  set the symbol path up
rem set _NT_SYMBOL_PATH=%HTREE%\xboxtest;%KTREE%;%KTREE%\devkit;%DUMPDIR%
rem use symbol server
set _NT_SYMBOL_PATH=symsrv*symsrv.dll*\\xbuilds\symbols

:retry

rem  Make the harness tree
    %XBMKDIR% %TDATA%
    %XBMKDIR% %TDATA_MEDIA%
    %XBMKDIR% %TDATA_MEDIA_AUDIO%
    %XBMKDIR% %TDATA_MEDIA_AUDIO_PCM%
    %XBMKDIR% %TDATA_MEDIA_AUDIO_WMA%

rem  remove the harness and the testini
    %XBDEL% %X_TESTINI%
    %XBDEL% %X_HARNESS%


rem  copy over media files
if /i "%CopyMedia%"=="0" goto copybins


    %XBDEL% %TDATA_MEDIA_AUDIO_PCM_FILES%
    echo Copying over media files, this may take up to 10 minutes

    %XBMKDIR% xc:\tdata\a7049955\MEDIA\DSPCode
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Perf8
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Perf8\AddNot
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Perf8\SetGlob
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Perf8\GetTime
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Segment8
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Segment8\Download
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Segment8\GetLength
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Segment8\SetRep
    %XBMKDIR% xc:\tdata\a7049955\MEDIA\Segment8\MIDIAndWave
    %XBMKDIR% xs:\a7049955\MEDIA
    %XBMKDIR% xs:\a7049955\MEDIA\DSPCode\

rem    The following lines were removed because DMusic tests now use medialoader
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Perf8\AddNot\*.*          xc:\tdata\a7049955\MEDIA\Perf8\AddNot\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Perf8\SetGlob\*.*         xc:\tdata\a7049955\MEDIA\Perf8\SetGlob\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Perf8\GetTime\*.*         xc:\tdata\a7049955\MEDIA\Perf8\GetTime\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\*.*                       xc:\tdata\a7049955\MEDIA\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\Download\*.*     xc:\tdata\a7049955\MEDIA\Segment8\Download\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\Download\*.*     xc:\tdata\a7049955\MEDIA\Segment8\Download\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\GetLength\*.*    xc:\tdata\a7049955\MEDIA\Segment8\GetLength\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\SetRep\*.*       xc:\tdata\a7049955\MEDIA\Segment8\SetRep\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\MIDIAndWave\*.*  xc:\tdata\a7049955\MEDIA\Segment8\MIDIAndWave\
rem    %XBCP% -t -d \\xdb01\stresscontent\dmusic\media\Segment8\GetLength\*.*    xc:\tdata\a7049955\MEDIA\Segment8\GetLength\

rem    The following line was removed because mediastress now uses medialoader
rem    %XBCP% -t -r \\xdb01\stresscontent\mediastress\*.*                        xs:\a7049955\MEDIA

rem    Its possible that the following line is no longer neccessary
    %XBCP% -t -r %KTREE%\DSSTDFX.BIN                                      xs:\a7049955\MEDIA\DSPCode\

rem    The following lines will not be neccessary when DSound tests are updated to use medialoader
    for /f %%f in ('%RANDFILE% %MEDIADIR% wav %MEDIAFILES%') do %XBCP% %%f %TDATA_MEDIA_AUDIO_PCM%
    for /f %%f in ('%RANDFILE% %WMADIR% wma %MEDIAFILES%') do %XBCP% %%f %TDATA_MEDIA_AUDIO_WMA%
    echo Done copying media files

rem update the kernel
:copybins
    if /i "%recovery%"=="external" goto AFTERSYSTEMCOPY
    if not "%AllowSysCopy%"=="1" goto AFTERSYSTEMCOPY

    %XBCP% %KTREE%\boot\xboxrom.bin xc:\
    if not "0" == "%errorlevel%" goto errormode
    %XBCP% %KTREE%\devkit\xbdm.dll xc:\
    if not "0" == "%errorlevel%" goto errormode
    %XBCP% %KTREE%\devkit\cydrive.exe xe:\dxt\cydrive.dxt
    if not "0" == "%errorlevel%" goto errormode

:AFTERSYSTEMCOPY

rem  copy over the new ini and new harness
    %XBCP% %TESTINI% %X_TESTINI%
    if not "0" == "%errorlevel%" goto errormode
    %XBCP% %HARNESS% %X_HARNESS%
    if not "0" == "%errorlevel%" goto errormode

rem pause before opening debugger?
if "%pause%"=="1" pause

rem  start the remote debugger
    %REMOTE% "%KD%" %XBOXNAME%

rem  reboot the xbox
    %XBREBOOT% xe:\stress.xbe /c
    
rem Register with lab dB
call cscript %REGISTER% /box=%XBOXNAME% /debugger=%COMPUTERNAME% /owner=%USERNAME% /romver=%BuildVer%.%KernelType% /swver=%BuildVer%.%HarnessType%

rem  add the user to the database
echo %USERNAME% %COMPUTERNAME% %XBOXNAME% %DATE% %TIME% %BuildVer% %KernelType% %HarnessType%>> %USERS%

    goto END

:ERRORMODE
    echo Stress is having trouble starting. Please try again.
    pause
    exit 1

:END
