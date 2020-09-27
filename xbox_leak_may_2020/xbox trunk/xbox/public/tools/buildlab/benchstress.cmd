@echo off
::
:: update XSS to latest without needing recovery CD
:: start stress on all the machines on this xlab
::
:: machine naming: xlab##N
:: com ports start at com3
::
:: To start stress on machines 1 to 15
:: first run benchpath to set the paths to today's build
:: start benchstress 1 15

pushd .
if "%1"=="/?" goto usage
if "%1"=="" goto usage
if "%2"=="" goto usage
if "%1"=="-e" (
    shift
    set rec_ext=1
)

set bench=01
set _xsconfig=1
set recovery=internal
set source=\\xbuilds\release\usa
set k_htype=chk_chk
set copymedia=1
set pause=0
set testini=Random

set last=%2
set n=%1

rem hack a for loop, because a set doesn't take effect within the code block

:forloop
set next=0
set xboxname=xlab%bench%%n%
set /A comn=%n% + 2
set comport=com%comn%

if exist exclude.txt (
    for /f %%e in (exclude.txt) do if "%%e"=="%xboxname%" set next=1
)
if "%next%"=="1" goto guard

call xbrecover %xboxname%

echo %xboxname%  %comport% %buildver%
setlocal
rem only use of of the next two calls
rem the first one will start stress
rem the second will only open the debugger

call \\xstress\stress\stress.cmd
REM call \\xstress\stress\xskd.cmd %XboxName% %ComPort% %Source% %BuildVer% %K_HType% 0
REM xbsetcfg -x %xboxname% -d
REM xbreboot -x %xboxname% -c

endlocal
:guard
set /a n=%n% +1
if %n% LEQ %last% goto forloop
goto end

:usage
echo Usage: benchstress {start} {end}
echo.

:end
popd
