@echo off
setlocal enableextensions

rem Copyright (c) 1998 Microsoft Corporation
rem Module Name: p-status.cmd
rem Abstract: list private source in current directory
rem Authors: Benjamin Vierck (benv), Lloyd Johnson (a-lloydj)

if defined verbose echo on 
if not defined verbose set OUTP=^>nul 2^>^&1

for %%a in (/ -) do if "%1"=="%%a?" goto usage
if "%1"=="?" goto usage

set private_dir=pri_bld
set ThisScript=%0
set comment=%computername% %username%

rem -- verify current directory is SLM'd
if NOT exist SLM.INI goto err_noslmini

status -x %private_dir%\

goto end

:usage
echo %0
echo  Files that were modified in this directory for the private
echo  build are displayed when this command is executed.
goto end

:err_noslmini
echo Error: %ThisScript% Aborting. Private files cannot be located for this directory.
goto end

:end
endlocal
