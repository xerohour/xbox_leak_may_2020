@echo off
setlocal ENABLEEXTENSIONS
if DEFINED _echo    echo on
if DEFINED verbose  echo on
if DEFINED verbose2 echo on

REM ---------------------------------------------------------------------------
REM      NTSTAT.CMD  -  invoked by spltstat.cmd to do the dirty work of
REM                     getting status for the NT tree.
REM    Parameters:
REM                     all - get status for all projects in %NTPROJECTS%
REM 			      if all is set, we invoke ntstat.cmd with each
REM			      element in NTPROJECTS
REM                     [projects] - names of projects to get status on
REM
REM ---------------------------------------------------------------------------

for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

if /i "%_NTSTATLOG%"   == "" set _NTSTATLOG=%_ntroot%\ntstat.log
if /i "%_NTSLMBACKUP%" == "" goto nobackup
if /i "%_NTSTATCMD%"   == "" set _NTSTATCMD=%_ntroot%\ntstatsr.cmd
goto dostatus1

:nobackup
set _NTSTATCMD=nul
set _NTSTATCMD1=

:dostatus1
if /i "%1" == "all" goto statall
if NOT "%1" == "" goto doprojects
if "%NTPROJECTS%" == "" goto noprojs
if NOT "%_NTSTATCMD%" == "" erase %_NTSTATCMD% 2>nul
if NOT "%_NTSTATCMD1%" == "" erase %_NTSTATCMD1% 2>nul
erase %_NTSTATLOG% 2>nul
call %0 %NTPROJECTS%
goto done

:statall
if NOT "%_NTSTATCMD%" == "" erase %_NTSTATCMD% 2>nul
if NOT "%_NTSTATCMD1%" == "" erase %_NTSTATCMD1% 2>nul
erase %_NTSTATLOG% 2>nul
call %0 %NTPROJECTS% %NTPROJECTS_EXTRA%
goto done

:noprojs
echo Must define NTPROJECTS environment variable to use this command without arguments
goto done

:doprojects
set _ntgetargs_=

:loop
if "%1" == "" goto loopexit
set _ntgetargs_=%_ntgetargs_% %1
shift
goto loop

:loopexit
call ntslmop status %_ntgetargs_%
goto done

REM ------------------------------------------------
REM  Display Usage:
REM ------------------------------------------------
:Usage
echo.
echo   NTSTAT.CMD  -  invoked by spltstat.cmd to do the dirty work of
echo                  getting status for the NT tree.
echo   Parameters:
echo                    all - get status for all projects in %NTPROJECTS%
echo			      if all is set, we invoke ntstat.cmd with each
echo                          element in NTPROJECTS
echo             [projects] - names of projects to get status on
echo.

:done
endlocal