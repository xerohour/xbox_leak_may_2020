@echo off
setlocal enableextensions

rem Copyright (c) 1998 Microsoft Corporation
rem Module Name: p-ssync.cmd
rem Abstract: Ssync to private bits
rem Authors: Benjamin Vierck (benv), Lloyd Johnson (a-lloydj)

if defined verbose echo on 
if not defined verbose set OUTP=^>nul 2^>^&1

for %%a in (/ -) do if "%1"=="%%a?" goto usage
if "%1"=="?" goto usage

set private_dir=pri_bld
set ThisScript=%0
set comment=%computername% %username%

set files=%*
if "%1"=="" set files=*.*

rem -- verify files exist & cmd-line args
set missing_files=
for %%a in (%files%) do if NOT exist %%~na%%~xa set missing_files=%missing_files% %%~na%%~xa
if defined missing_files goto err_filesmissing

rem -- check for private build directory under current directory
ssync -f %private_dir% %OUTP%
if ERRORLEVEL 1 goto err_noprbld

rem -- verify current directory is SLM'd
if NOT exist SLM.INI goto err_noslmini

rem -- ssync the files in private dir
pushd %private_dir%
ssync -f %files%
if ERRORLEVEL 1 goto err_nossync

rem -- remove forward slashes
set files=%files:/=%

rem -- create list of files (from private dir) as fast as possible
set res_files=
for /f %%a in ('dir /b %files%') do set res_files=%res_files% %%a
popd

for %%a in (%res_files%) do call :ssyncfiles %%a
goto end

:ssyncfiles
if not exist %1 goto skiprocheck

:check_ro
dir /ar /b %1 %outp%
if ERRORLEVEL 1 goto err_notro

:skiprocheck

xcopy /kr %private_dir%\%1 %1 %OUTP%
goto :EOF

:usage
echo %0 ^<file1^> ^<file2^> ...
echo  Files are ssync from private build tree in current directory
echo  Note: wildcards ARE supported in the arguments for this command.
goto end

:err_nossync
popd
goto end

:err_filesmissing
echo %ThisScript% cannot find the following files:
for %%a in (%missing_files%) do @echo %%a
goto end

:err_notro
echo Error: The file %1 is not read-only. %ThisScript% did not overwrite %1 current directory.
goto :EOF

:err_noslmini
echo Error: %ThisScript% Aborting. Private files cannot be checked into non-SLM'd projects.
goto end

:err_noprbld
echo Error: %ThisScript% did not find any private build files here.
goto end

:err_nofiles
goto :EOF

:err_filesmissing
echo Error: Aborting. No files ssync'd. %ThisScript% cannot find the following files:
for %%a in (%missing_files%) do @echo %%a
echo Note: Command line arguments are not supported.
goto end

:end
endlocal
