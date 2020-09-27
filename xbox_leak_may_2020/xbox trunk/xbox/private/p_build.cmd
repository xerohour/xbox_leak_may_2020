@echo off
setlocal enableextensions

rem Copyright (c) 1998 Microsoft Corporation
rem Module Name: p_build.cmd
rem Abstract: build private bits
rem Authors: Benjamin Vierck (benv), Lloyd Johnson (a-lloydj)

if defined verbose echo on 
if not defined verbose set OUTP=^>nul 2^>^&1

rem WARNING - DO NOT USE ALIASES IN THIS SCRIPT!

rem Set alternate path for private binaries
if "%PROCESSOR_ARCHITECTURE%" == "x86" set _NT386TREE=%_NTDRIVE%\bin.alt
if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" set _NTALPHATREE=%_NTDRIVE%\bin.alt

%_NTDRIVE%

goto skipssyncexample
rem The list of directories to p-ssync are manually inserted into
rem the "ssyncdirs" environment variable.  Do NOT use hard paths.
rem e.g.
set ssyncdirs=
set ssyncdirs=%ssyncdirs% %_NTROOT%\private\redist\ptools
:skipssyncexample

set ssyncdirs=

for %%a in (%ssyncdirs%) do pushd %%a&call p-ssync&popd

if /I NOT "%COMPUTERNAME%"=="NTXPRIME" goto end


goto endexample
rem Example BUILD Instructions

rem BenV - NT Build Team - New Memory Management project
cd /d %_NTROOT%\private\redist\ptools
build -cZ

rem generated binaries: ntoskrnl.exe, ntkrnlmp.exe
:endexample

rem BUILD Instructions


:end
rem Reset path for binaries
if "%PROCESSOR_ARCHITECTURE%" == "x86" set _NT386TREE=%BINARIES%
if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" set _NTALPHATREE=%BINARIES%

endlocal
