@echo off

for /f %%n in (\\xbuilds\release\usa\latest.txt) do set latest=%%n

Echo Copying \\xbuilds\release\usa\%latest%\XDKSetup%latest%.exe ...
copy \\xbuilds\release\usa\%latest%\XDKSetup%latest%.exe %temp%\xdksetup.exe

start %temp%\XDKSetup.exe
REM Wait for the app to come up
sleep 2
cscript /nologo sendkeys.wsf "Microsoft XBox SDK" {ENTER}
cscript /nologo sendkeys.wsf "Microsoft XBox SDK" {ENTER}
cscript /nologo sendkeys.wsf "Microsoft XBox SDK" {ENTER}
