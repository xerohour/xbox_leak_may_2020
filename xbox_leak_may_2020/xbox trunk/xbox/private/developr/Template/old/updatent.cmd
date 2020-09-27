@if NOT defined _echo echo off

REM ****************************
REM *** DON'T EDIT THIS FILE ***
REM ****************************

if "%1" == "" goto usage
if "%2" == "" goto usage
if "%3" == "" (
    set _Type=srv
) else (
    set _Type=%3
)
if /i NOT "%2" == "fre" if /i NOT "%2" == "chk" goto usage
if /i NOT "%_Type%" == "ent" if /i NOT "%_Type%" == "srv" if /i NOT "%_Type%" == "wks" goto usage

:try_again
echo Searching for \\ntbuilds\release\usa\%1\%processor_architecture%\%2.%_Type%\winnt32
if exist \\ntbuilds\release\usa\%1\%processor_architecture%\%2.%_Type%\winnt32 goto start_winnt32
sleep 300
goto try_again

:start_winnt32
@echo on
\\ntbuilds\release\usa\%1\%processor_architecture%\%2.%_Type%\winnt32\winnt32 /#8:usa\%1\%processor_architecture%\%2.%_Type% %4
goto done

:usage
echo UpdateNT build fre\chk [ent\srv\wks] [opt args]
:done
