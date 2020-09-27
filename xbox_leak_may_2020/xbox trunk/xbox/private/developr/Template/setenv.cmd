@echo off
REM This file is executed whenever you started a Razzle Window

REM Add projects you use here, this will make ntsync work from any directory
set NTPROJECTS=private public ntos sdktools directx

REM Make sure you get the advantages of a multiprocessor build
if "%NUMBER_OF_PROCESSORS%" GTR "1" set BUILD_MULTIPROCESSOR=1

REM Symbol and binary directories
set _NT386TREE=%_NTDRIVE%%_NTROOT%bins
set _NT_SYMBOL_PATH=%_NT386TREE%\symbols\dump;%_NT386TREE%\symbols\retail

REM Tell the build process to make debug files
set NTDBGFILES=1

REM Debug build by default. To do a retail build, do set NTDEBUG=ntsdnodbg
set NTDEBUG=ntsd

REM Enable source-level debugging
set NTDEBUGTYPE=both

REM Xbox is set to 115200 by default
set _NT_DEBUG_BAUD_RATE=115200

REM Set your own default COM port here
REM set _NT_DEBUG_PORT=com2
