@if "%_echo%" == "" echo off

REM Validate parameters.

set SRC=%1
set DEST=%2

if _%SRC%==_ goto usage
if not exist %SRC% goto nosrcdir

if _%DEST%==_ goto usage

if not exist %DEST% mkdir %DEST%
if errorlevel 1 goto baddestdir

REM Copy.

xcopy /S /C %SRC% %DEST% /EXCLUDE:%_NTDRIVE%\%_NTROOT%\public\tools\dropsrc.exclude | tee %TEMP%\dropsrc.log

findstr /B /L /V %SRC% %TEMP%\dropsrc.log > nul

goto done

:nosrcdir

echo.
echo Bad source directory: %SRC%
echo.

goto done

:baddestdir

echo.
echo Unable to create destination directory: %DEST%
echo.

:usage

echo.
echo dropsrc [source directory] [destination directory]
echo.
echo Copies all XBOX source files from an enlistment into another directory except
echo those that cannot be exposed outside of MS.
echo.

:done

