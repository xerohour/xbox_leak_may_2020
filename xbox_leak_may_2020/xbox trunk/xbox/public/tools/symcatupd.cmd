@echo off
setlocal ENABLEDELAYEDEXPANSION
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM ******************************************************************
REM
REM This script updates symbols.cat when the build lab takes backprops
REM for symbol files that are in symbols.cab
REM
REM ******************************************************************

if "%1" == "" goto Usage
REM  Check the command line for /? -? or ?
for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

if "%2" == "" (
    set lang=usa
    set mybinaries=%binaries%
) else (
    set lang=%2
    set mybinaries=%relbins%\%2
)

REM
REM Decide which build we are working on
REM debug vs. retail and i386 vs. alpha
REM

if /i "%PROCESSOR_ARCHITECTURE%" == "x86"   (
    if /i "%lang%" == "NEC_98" (
        set SUBDIR=NEC98
    ) else (
        set SUBDIR=i386
    )
)
if /i "%PROCESSOR_ARCHITECTURE%" == "alpha" (
    set SUBDIR=alpha
)

if /i "%NTDEBUG%" == "ntsd" (
    set FRECHK=Debug
) else (
    set FRECHK=Retail
)


set catalog=%mybinaries%\symbolcd\cd\symbols\%SUBDIR%\%FRECHK%\symbols.cat
set temp.txt=%mybinaries%\symbolcd\%lang%\update.txt

REM
REM Find out which symbol files, if any need to be updated in
REM the catalog file
REM

findstr /il %1 %binaries%\symbolcd\%lang%\symbolcd.txt > %temp.txt%

set Resign=FALSE

for /F "tokens=3 delims=," %%a in (%temp.txt%) do (
    updcat %catalog% -a %binaries%\%%a
    set Resign=TRUE
)

if /i "%Resign%" == "TRUE" (
    echo %catalog% needs to be signed and propped
    goto end
) else (
    del %temp.txt%
    echo No changes were made to %catalog%
    goto errend
)

goto end

:Usage
echo.
echo Usage:  symcatupd ^<file^> [^<language^>]
echo.
echo file     The binary that is being updated.
echo          associated lists and infs
echo cabs     Update the out of date cabs
echo.
echo language Language must be one of the languages in codes.txt.
echo          If language is not given on the command line,
echo          usa is the default.
echo.
goto errend

:errend
endlocal
REM Set errorlevel to 1 in case of errors during execution.
REM Used by the build rule script "symbols.cmd" after calling "symbolcd.cmd".
seterror.exe 1
goto :EOF

:end
endlocal
REM Set errorlevel to 0 when the script finishes successfully.
REM Used by the build rule script "symbols.cmd" after calling "symbolcd.cmd".
seterror.exe 0
goto :EOF

