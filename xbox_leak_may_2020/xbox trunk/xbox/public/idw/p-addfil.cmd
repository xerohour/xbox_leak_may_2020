@echo off
setlocal enableextensions

rem Copyright (c) 1998 Microsoft Corporation
rem Module Name: p-addfil.cmd
rem Abstract: Check out private bits
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

rem -- verify current & pri_bld directory SLMeness
if NOT exist SLM.INI goto err_noslmini
if exist %private_dir%\slm.ini goto :skip_pbadd

rem -- make & addfile pri_bld
:addfile_pribld
md %private_dir% %OUTP%
addfile -c "%comment%- add private tree" %private_dir%
if ERRORLEVEL 1 goto end

rem -- Main Loop; check out files
:skip_pbadd
for %%a in (%files%) do call :add_file %%a

goto end

:add_file
if not exist %private_dir%\%1 xcopy /y %1 %private_dir%\ %OUTP%
if not exist %private_dir%\%1 xcopy %1 %private_dir%\ %OUTP%
if not exist %private_dir%\%1 goto err_nocopy

addfile -c "%comment%- add %1 to private" %private_dir%\%1
if ERRORLEVEL 1 goto err_addfile
attrib +r %1
goto :EOF

:err_nocopy
echo Error: %ThisScript% was unable to copy %1 into the private build tree.
goto :EOF

:err_addfile
goto :EOF

:usage
echo %0 ^<file1^> ^<file2^> ...
echo  This adds files into the private build.
echo  Note: wildcards are not supported in arguments.
goto end


:err_nofile
echo Error: %ThisScript% was unable to find %1 in the current directory.
goto EOF:

:err_badout
echo Error: %ThisScript% was unable to check out %1 in the private build tree.
goto :EOF

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
