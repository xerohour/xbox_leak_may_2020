@if "%_echo%" == "" echo off

REM Validate parameters.

set DATE=%1

REM Connect to extranet share
net use Q: \\tkatgfsa01\xdsdirectory
if errorlevel 1 goto nonetaccess

if _%DATE%==_ goto usage
if exist Q:\%DATE% goto direxists

mkdir Q:\nvidia\outgoing\%DATE%

:direxists

REM Copy.
echo Copying Private and Public trees
call dropsrc %_NTDRIVE%%_NTROOT%\private Q:\nvidia\outgoing\%DATE%\private
call dropsrc %_NTDRIVE%%_NTROOT%\public Q:\nvidia\outgoing\%DATE%\public

goto done

:nonetaccess
echo.
echo Couldn't connect to \\tkatgfsa01\xdsdirectory
echo.
goto done

:baddir

echo.
echo Unable to create destination directory: %DATE%
echo.
goto done

:usage

echo.
echo nvidiaprop [destdir]
echo.
echo Copies all XBOX source files from private and public to a directory.
echo Convention is to name them like 20001101 for November 1, 2000.
echo.

:done

net use q: /d /y >NUL

