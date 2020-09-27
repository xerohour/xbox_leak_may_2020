@if NOT DEFINED _echo echo off

REM ****************************
REM *** DON'T EDIT THIS FILE ***
REM ****************************

setlocal
if "%1" == "" goto usage
if not "%2" == "" (set procarch=%2) else (set procarch=%processor_architecture%)

for /L %%i in (1,1,7) do if exist \\ntblds%%i\usa.%1.%procarch%\ echo \\ntblds%%i\usa.%1.%procarch%
if exist \\ntrelic\usa.%1.%procarch%\ echo \\ntrelic\usa.%1.%procarch%
goto done

:usage
echo "findpub <build#> [architecture]"

:done
endlocal
