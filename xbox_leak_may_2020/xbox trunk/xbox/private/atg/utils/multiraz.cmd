@echo off
set SDCLIENT=%1
set _NTDRIVE=%2
set _NTROOT=%3
if NOT (%4) == () set _NT_DEBUG_PORT=%4
if NOT (%5) == () set XBOXIP=%5
if (%6) == (RETAIL) set NTDEBUG=ntsdnodbg
if (%6) == (RELEASE) set NTDEBUG=release

set PATH=%PATH%;%_NTDRIVE%%_NTROOT%\private\atg\utils
call %_NTDRIVE%%_NTROOT%\public\tools\razzle.cmd
