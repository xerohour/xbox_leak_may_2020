REM
REM Updates several enlistments by iterating over each,
REM setting up the environment, and calling syncbuild.cmd
REM to do the sync and build
REM Logs to c:\UpdateEnlistments.log
REM
REM I recommend setting this up as a Scheduled Task from 
REM the control panel.  If you do this, pick a non-even
REM number time a good bit after 4am so that the build
REM machine sync doesn't get slowed down and so that source 
REM depot doesn't get slammed by 10 sync's at exactly
REM 5am
REM

REM Set up to run
REM @echo off
echo "Beginning nightly update..." > c:\UpdateEnlistments.log
DATE /T >> c:\UpdateEnlistments.log
Time /T >> c:\UpdateEnlistments.log
set ORIGPATH=%PATH%

REM Put each enlistment to run here
REM call :PerEnlistment <ClientName> <_NTDRIVE> <_NTROOT>
call :PerEnlistment JHARDING-DEV C: \xboxsd
call :PerEnlistment JHARDING-DEV2 C: \x2
call :PerEnlistment JHARDING-DEV3 C: \xblack

REM Done!
goto end

REM
REM This is what is done per enlistment:
REM Set environment variables, call ntenv.cmd, and then syncbuild.cmd
REM

:PerEnlistment
echo Updating %2%3 enlistment... >> c:\UpdateEnlistments.log
set SDCLIENT=%1
set _NTDRIVE=%2
set _NTROOT=%3
set PATH=%_NTDRIVE%%_NTROOT%\public\idw;%_NTDRIVE%%_NTROOT%\public\mstools;%_NTDRIVE%%_NTROOT%\public\tools;%ORIGPATH%
call %_NTDRIVE%%_NTROOT%\public\tools\ntenv.cmd
call c:\bin\syncbuild.cmd


:end