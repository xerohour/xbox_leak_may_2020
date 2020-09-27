@echo off
setlocal ENABLEEXTENSIONS
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM ---------------------------------------------------------------------------
REM    NTREBASE.CMD  -  rebase NT binaries
REM
REM ---------------------------------------------------------------------------

set _REBASE_FLAGS=%_NTREBASE_FLAGS%
if not "%_REBASE_FLAGS%" == "" echo Using flags from _NTREBASE_FLAGS == "%_NTREBASE_FLAGS%"
if "%_REBASE_FLAGS%" == "" set _REBASE_FLAGS=-v
if not "%1" == "" set _REBASE_FLAGS=%1 %2 %3 %4 %5 %6 %7 %8 %9

if not "%QFE_BUILD%"=="1" goto CHK1
if not "%REBASELANG%"=="" goto CHK1
echo REBASELANG not defined.
goto EXIT

:CHK1
if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
echo PROCESSOR_ARCHITECTURE not defined.
goto EXIT


:X86
if not "%IA64%"==""   goto IA64
set _TREE=%_NT386TREE%
set _BADDR=%_ntbindir%\public\sdk\lib\i386\%REBASELANG%\baseaddr.txt
set _SESSION_REBASE_ADDRESS=0xA07F0000
goto OK

:IA64
set _TREE=%_NTIA64TREE%
set _BADDR=%_ntbindir%\public\sdk\lib\ia64\%REBASELANG%\baseaddr.txt
set _SESSION_REBASE_ADDRESS=0xA07F0000
goto OK

:ALPHA
if not "%AXP64%"==""   goto ALPHA64
set _TREE=%_NTALPHATREE%
set _BADDR=%_ntbindir%\public\sdk\lib\alpha\%REBASELANG%\baseaddr.txt
set _SESSION_REBASE_ADDRESS=0xDE7F0000
goto OK

:ALPHA64
set _TREE=%_NTAXP64TREE%
set _BADDR=%_ntbindir%\public\sdk\lib\axp64\%REBASELANG%\baseaddr.txt
set _SESSION_REBASE_ADDRESS=0x0DE7F0000
goto OK


:OK
set RESTRICTED_WORKSET=
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET%
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\smss.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\csrss.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\lsass.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\services.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\winlogon.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\spoolss.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\rpcss.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\explorer.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\ntvdm.exe
set RESTRICTED_WORKSET=%RESTRICTED_WORKSET% %_TREE%\mstask.exe

imagecfg /q /r %RESTRICTED_WORKSET%

set LARGE_ADDRESS=
set LARGE_ADDRESS=%LARGE_ADDRESS% %_TREE%\lsass.exe
imagecfg /q -l %LARGE_ADDRESS%

for /f %%i in ('dir /s /b %_TREE%\winnt32\*.dll %_TREE%\winnt32\*.exe') do imagecfg /q -x -y %%i
for /f %%i in ('dir /s /b %_TREE%\winnt32\winntupg\*.dll %_TREE%\winnt32\winntupg\*.exe') do imagecfg /q -x -y %%i
if not "%QFE_BUILD%"=="1" goto OK1
if exist %_BADDR% goto OK1
echo Could Not Find %_BADDR%
goto EXIT

:OK1

REM ------------------------------------------------
REM Set Terminal Server compatible bit on all exes
REM ------------------------------------------------
set _TSCOMPAT_PATHS=
set _TSCOMPAT_SYMBOL_TREES=%_TREE%\Symbols\retail;%_TREE%\Symbols\system32;%_TREE%\Symbols\mstools;%_TREE%\Symbols\idw;%_TREE%\Symbols\winnt32
set _TSCOMPAT_PATHS=%_TSCOMPAT_PATHS% %_TREE%\*.exe %_TREE%\system32\*.exe %_TREE%\mstools\*.exe %_TREE%\idw\*.exe %_TREE%\winnt32\*.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 1 %_TSCOMPAT_PATHS%
REM  Do not set bit on the following exes
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\ntvdm.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\regini.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\idw\regini.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\winhlp32.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\winhstb.exe
imagecfg /q /S %_TSCOMPAT_SYMBOL_TREES% /h 0 %_TREE%\acregl.exe

echo Rebasing images in %_TREE%
set _REBASE_SYMS=
set _REBASE_LOG=%_TREE%\rebase.log

set _REBDIR=%_ntbindir%\public\tools

set _REBPATHS=
set _REBPATHS=%_REBPATHS% %_TREE%\*.acm %_TREE%\*.dll %_TREE%\*.cpl %_TREE%\*.drv
set _REBPATHS=%_REBPATHS% %_TREE%\*.ocx %_TREE%\*.ax %_TREE%\*.tsp
set _REBPATHS=%_REBPATHS% %_TREE%\winnt32\winntupg\netupgrd.dll
set _REBASE_SYMBOL_TREES=%_TREE%\Symbols\retail;%_TREE%\Symbols\winnt32

REM -------------------------------------------------------
REM There are duplications between binaries and the tools directories.
REM So, rebase the tools separately.
REM -------------------------------------------------------

set _REBPATHS_SYS32=%_TREE%\system32\*.dll
set _REBPATHS_MSTOOLS=%_TREE%\mstools\*.dll
set _REBPATHS_IDW=%_TREE%\idw\*.dll

set _REBASE_SYMS_SYS32=-u %_TREE%\symbols\system32
set _REBASE_SYMS_MSTOOLS=-u %_TREE%\symbols\mstools
set _REBASE_SYMS_IDW=-u %_TREE%\symbols\idw

REM set _REBPATHS=%_REBPATHS% %_TREE%\dump\*.dll
REM set _REBASE_SYMBOL_TREES=%_REBASE_SYMBOL_TREES%;%_TREE%\Symbols\dump

if NOT EXIST %_TREE%\Symbols\nul goto nodbgfiles
set _REBASE_SYMS=-u %_REBASE_SYMBOL_TREES%
set _REBASE_LOG=%_TREE%\Symbols\rebase.log
echo ... updating .DBG files in %_REBASE_SYMBOL_TREES%

:nodbgfiles

erase %_REBASE_LOG%

set _REBASE_FLAGS=%_REBASE_FLAGS% -l %_REBASE_LOG%

if "%QFE_BUILD%"=="1" goto QFE


REM ------------------------------------------------
REM  rebase the dlls.
REM ------------------------------------------------

REM   do not rebase the csr, wow, or mail dlls.
set _REBASE_EXCLUDES=
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\csrss.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\kbd.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\video.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\ntvdm.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\wx86.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\wx86thnk.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\never.reb
set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -N %_REBDIR%\termsrv.reb

set _REBASE_EXCLUDES=%_REBASE_EXCLUDES% -G %_REBDIR%\known.reb -G %_REBDIR%\net.reb -G %_REBDIR%\printer.reb
rebase %_REBASE_SYMS% %_REBASE_FLAGS% -d -b 0x78000000 -R %_TREE% %_REBASE_EXCLUDES% %_REBASE_EXCLUDES2% %_REBPATHS%

rebase %_REBASE_SYMS_SYS32%   %_REBASE_FLAGS% -d -b 0x50000000 -R %_TREE% %_REBPATHS_SYS32%
rebase %_REBASE_SYMS_MSTOOLS% %_REBASE_FLAGS% -d -b 0x4c000000 -R %_TREE% %_REBPATHS_MSTOOLS%
rebase %_REBASE_SYMS_IDW%     %_REBASE_FLAGS% -d -b 0x48000000 -R %_TREE% %_REBPATHS_IDW%

REM
REM rebase csrss as a group
REM

rebase %_REBASE_SYMS% %_REBASE_FLAGS% -d -b 0x60000000 -R %_TREE% -G %_REBDIR%\csrss.reb -O %_REBDIR%\kbd.reb -O %_REBDIR%\video.reb


REM
REM rebase wow as a group
REM

rebase %_REBASE_SYMS% %_REBASE_FLAGS% -d -b 0x10000000 -R %_TREE% -G %_REBDIR%\ntvdm.reb


REM
REM rebase wx86 as two groups, one starting at 0x60000000 and going up (cpu),
REM and one ending at 0x60000000 and going down (thunks).
REM
rebase %_REBASE_SYMS% %_REBASE_FLAGS%    -b 0x60000000 -R %_TREE% -G %_REBDIR%\wx86.reb
rebase %_REBASE_SYMS% %_REBASE_FLAGS% -d -b 0x60000000 -R %_TREE% -G %_REBDIR%\wx86thnk.reb

REM
REM --------------------------------------------------------------------
REM Rebase kernel mode drivers loaded in session space (Terminal Server)
REM --------------------------------------------------------------------
rebase %_REBASE_SYMS% %_REBASE_FLAGS% -z -d -b %_SESSION_REBASE_ADDRESS% -R %_TREE% -G %_REBDIR%\termsrv.reb



goto EXIT


REM ------------------------------------------------
REM  Rebase for QFE build:
REM ------------------------------------------------
:QFE

set REBFLAGS=
set REBFLAGS=%REBFLAGS% -N %_REBDIR%\norebase.reb
set REBFLAGS=%REBFLAGS% -G %_REBDIR%\known.reb
set REBFLAGS=%REBFLAGS% -O %_REBDIR%\kbd.reb
set REBFLAGS=%REBFLAGS% -O %_REBDIR%\video.reb
set REBFLAGS=%REBFLAGS% -G %_REBDIR%\printer.reb %_REBPATHS%

rebase %_REBASE_SYMS% %_REBASE_FLAGS% -i %_BADDR% -R %_TREE% %REBFLAGS%
rebase %_REBASE_SYMS% %_REBASE_FLAGS% -i %_BADDR% %_TREE%\*.exe %_TREE%\mstools\*.exe %_TREE%\idw\*.exe %_TREE%\dump\*.exe

:EXIT
endlocal
