@echo off
setlocal enableextensions

rem Copyright (c) 1998 Microsoft Corporation
rem Module Name: p-in.cmd
rem Abstract: Check in private bits
rem Authors: Benjamin Vierck (benv), Lloyd Johnson (a-lloydj)

if defined verbose echo on 
if not defined verbose set OUTP=^>nul 2^>^&1

if "%1"=="" goto usage
for %%a in (/ -) do if "%1"=="%%a?" goto usage
if "%1"=="?" goto usage

set private_dir=pri_bld
set ThisScript=%0
set comment=%computername% %username%
set files=%*

rem -- verify files exist & cmd-line args
set missing_files=
for %%a in (%files%) do if NOT exist %%~na%%~xa set missing_files=%missing_files% %%~na%%~xa
if defined missing_files goto err_filesmissing

rem -- verify current directory is SLM'd
if NOT exist SLM.INI goto err_noslmini

rem -- main file loop
for %%a in (%files%) do call :checkin %%a

goto end

rem -- check for pri_bld dir & pri_bld\<file>
:checkin
if not exist %private_dir%\slm.ini goto err_nopout
if not exist %private_dir%\%1 goto err_noprivatefiles

xcopy /uq %1 %private_dir%\ %OUTP%
in -c "%comment%- %1 checked into private tree" %private_dir%\%1
attrib +r %1 %OUTP%
goto :EOF

:usage
echo %0 ^<file1^> ^<file2^> ...
echo  Listed files will be checked into the private build tree.
echo  Note: wildcards are not supported in arguments.
echo.
goto end

:err_noprivatefiles
goto :EOF

:err_nopout
echo Error: Cannot check file into private SLM structure.  Either use P-OUT to check out the file
echo or use P-ADDFIL to add new files to the structure.
goto end

:err_filesmissing
echo Error: Aborting. No files checked in. %ThisScript% cannot find the following files:
for %%a in (%missing_files%) do @echo %%a
echo Note: Command line arguments are not supported.
goto end

:err_noslmini
echo Error: %ThisScript% Aborting. Private files cannot be checked into non-SLM'd projects.
goto end

:end
endlocal
