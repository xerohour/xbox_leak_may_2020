@echo off
if "%1"=="/?" goto usage
if "%1"=="-?" goto usage
if "%1"=="?" goto usage

setlocal
for /f %%n in (\\xbuilds\release\usa\latest.txt) do set latest=%%n

if "%1"=="" (
    set _NT386TREE=\\xbuilds\release\usa\%latest%\checked
) else set _NT386TREE=\\xbuilds\release\usa\%latest%\%1
call copybins.cmd
endlocal
goto end

:usage
echo "copylatest [checked|free|black|main]"
echo "checked is default"

:end
