@echo off
if not "%_echo%" == "" echo on
if not "%verbose%"=="" echo on
setlocal ENABLEEXTENSIONS

REM ---------------------------------------------------------------------------
REM    NTENLIST.CMD  -  enlist in projects associated with NT.
REM
REM    Parameters: NTENLIST [project1] [project2] [projectX] ..
REM                where [projectX] is an NT project to enlist in.
REM
REM         for convenience, you may set up an environment variable
REM         (for example, NTPROJECTS) which contains all the projects
REM         you wish to enlist in, and simply type NTENLIST %%NTPROJECTS%%
REM
REM ---------------------------------------------------------------------------

for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

for %%a in (./ .-) do if /i ".%1." == "%%aFORCE." set _NTENLIST_FORCE=1&shift

REM ------------------------------------------------
REM  Set default Variables for script:
REM ------------------------------------------------

if "%_ENLISTOPTIONS%" == "" set _ENLISTOPTIONS=v
if "%_NTDRIVE%" == "" set _NTDRIVE=C:
if "%_NTROOT%" == "" set _NTROOT=\nt
if "%_projects_cmd%" == "" set _projects_cmd=projects.cmd

cd /d %_NTDRIVE%\
REM  If public enlistment already exists, skip to private projects:
if EXIST %_NTROOT%\public\slm.ini goto DoEnlist

REM ------------------------------------------------
REM  Create Public enlistment:
REM ------------------------------------------------
:EnlistPubs

if "%_NT_PUBLIC_SERVER%" == "" set _NT_PUBLIC_SERVER=\\orville\razzle
if "%_NT_PUBLIC_PROJECT%" == "" set _NT_PUBLIC_PROJECT=public
md %_NTROOT%
cd %_NTROOT%
md public
md private
cd %_NTROOT%\public
echo Enlisting in %_NTROOT%\public project
enlist -%_ENLISTOPTIONS% -c -s %_NT_PUBLIC_SERVER% -p %_NT_PUBLIC_PROJECT%
path %PATH%;%_NTROOT%\public\tools

REM ------------------------------------------------
REM  Create Private enlistments:
REM ------------------------------------------------
:DoEnlist

if "%1" == "" goto Done
call %_projects_cmd% %1
shift
if "%proj_path%" == "" goto BadProject
if "%_NTENLIST_FORCE%"=="1" goto skip_check
if EXIST %proj_path%\slm.ini goto AlreadyEnlisted
:skip_check
if EXIST %proj_path%\nul goto GotDir
mkdir %proj_path%
if ERRORLEVEL 1 goto BadPath

:GotDir
cd /d %proj_path% 2>nul
if ERRORLEVEL 1 goto badpath
echo Enlisting in %project% project in %proj_path%
enlist -%_ENLISTOPTIONS% -s %slm_root% -p %project%
goto DoEnlist

:BadProject
echo Invalid project name - %1
goto DoEnlist

:BadPath
echo Unable to create or change to %proj_path% for %project% project
goto DoEnlist

:AlreadyEnlisted
echo You are already enlisted in %project% project in %proj_path%
goto DoEnlist

REM ------------------------------------------------
REM  Display Usage:
REM ------------------------------------------------
:Usage

echo.
echo  NTENLIST.CMD - enlists you in the specified NT Project(s).
echo.
echo     Usage: NTENLIST [/FORCE] [project1] [project2] [projectX] ..
echo            where [projectX] is an NT project to enlist in.
echo.
echo            for convenience, you may set up an environment variable
echo            (for example, NTPROJECTS) which contains all the projects
echo            you wish to enlist in, and simply type NTENLIST %%NTPROJECTS%%
echo.
echo            /FORCE will force re-enlisting an already enlisted project
echo.

:Done
endlocal
