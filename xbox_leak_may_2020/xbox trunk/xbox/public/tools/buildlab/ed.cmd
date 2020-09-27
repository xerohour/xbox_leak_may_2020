@echo off
if "%1"=="" goto usage

call sd edit %1
start "TextPad" "C:\Program Files\TextPad 4\TextPad.exe" %1
goto end

:usage
echo    %0 [file]
echo    sd edit file and open in TextPad
echo.

:end
