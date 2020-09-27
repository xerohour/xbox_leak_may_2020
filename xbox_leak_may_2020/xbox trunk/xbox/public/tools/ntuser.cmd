@echo off
if not "%_echo%" == "" echo on
if not "%verbose%"=="" echo on

REM If no drive has been specified for the NT development tree, assume
REM C:. To override this, place a SET _NTDRIVE=X: in your CONFIG.SYS
REM
REM If we're called by a remote debugger window, %1 will be "DebugRemote"
REM so set the environment and get rid of %1

set DEBUG_REMOTE=
if /i "%1" == "DebugRemote" set DEBUG_REMOTE=RemoteDebug&& shift
if "%_NTDRIVE%" == "" set _NTDRIVE=C:
if NOT "%USERNAME%" == "" goto skip1
echo.
echo !!! Error - USERNAME environment variable not set !!!
echo.
goto done
:skip1

REM  This command file is either invoked by NTENV.CMD during the startup of
REM  a Razzle screen group, or it is invoked directly by a developer to
REM  switch developer environment variables on the fly. If invoked with
REM  no argument, it will restore the original developer's environment (as
REM  remembered by the NTENV.CMD command file).  Otherwise the argument is
REM  a developer's email name and that developer's environment is established.

if NOT "%1" == "" set USERNAME=%1
if "%_NTUSER%" == "" goto skip2

REM  if invoked by a debug remote, don't set aliases that confuse the debugger

if "%DEBUG_REMOTE%" == "RemoteDebug" goto skip2
if "%1" == "" if "%USERNAME%" == "%_NTUSER%" alias src /d
if "%1" == "" set USERNAME=%_NTUSER%
:skip2

REM  Most tools look for .INI files in the INIT environment variable, so set
REM  it. MS WORD looks in MSWNET of all places.

set INIT=%_NTBINDIR%\private\developr\%USERNAME%
set MSWNET=%INIT%

REM Load CUE with the standard public aliases and the developer's private ones

if "%DEBUG_REMOTE%" == "RemoteDebug" goto skip4
if "%_NTUSER%" == "" goto skip3

REM Initialize user settable NT nmake environment variables

set NTPROJECTS=public
set NT386FLAGS=
set NTMIPSFLAGS=
set NTCPPFLAGS=
if "%NTDEBUG%" == "" set NTDEBUG=ntsd
set BUILD_OPTIONS=
set 386_OPTIMIZATION=
set 386_WARNING_LEVEL=
set SDPORT=XSRC:1666
if "%SDCLIENT%" == "" set SDCLIENT=%COMPUTERNAME%
alias src > nul
if NOT errorlevel 1 goto skip4
if EXIST %INIT%_dbg.pub alias -f %INIT%\dbg.pub
alias -f %_NTBINDIR%\private\developr\cue.pub -f %_NTBINDIR%\private\developr\ntcue.pub -f %INIT%\cue.pri
goto skip4

:skip3

alias src > nul
if errorlevel 1 goto skip4
alias -f %_NTBINDIR%\private\developr\cue.pub -f %INIT%\cue.pri

:skip4

REM Load the developer's private environment initialization (keyboard rate,
REM screen size, colors, environment variables, etc).
REM Pass on "RemoteDebug" if ntuser.cmd was invoked by dbgntenv.cmd:

set BUILD_OPTIONS=

call %INIT%\setenv.cmd %DEBUG_REMOTE%

if "%NTDEBUG%" == "release" goto release

if "%NTDEBUG%" == "ntsdnodbg" goto free

:checked
set NODEVKIT=
goto done

:free
REM
REM enable building of optional directories
REM
set BUILD_OPTIONS=%BUILD_OPTIONS% ltcg profile
set NODEVKIT=
goto done

:release
REM
REM set release (free and no devkit) options
REM
set BUILD_OPTIONS=%BUILD_OPTIONS% ltcg ~dm
set NTDEBUG=ntsdnodbg
set NODEVKIT=1

:done
echo Current user is now %USERNAME%

:end
