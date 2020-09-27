@echo off

cd ..

if "%4" == "/a" goto CLEAN_BUILD
if "%5" == "/a" goto CLEAN_BUILD
if "%6" == "/a" goto CLEAN_BUILD
goto SKIP

:CLEAN_BUILD
nmake %1.%2.%3 clean=1

:SKIP
if "%4" == "/browse" goto BROWSE
if "%5" == "/browse" goto BROWSE
if "%6" == "/browse" goto BROWSE
goto BUILD

:BROWSE
nmake %1.%2.%3 dx8=1 browse=1 clean=0
echo Building browser information file...
bscmake /o nvdd32 common\*.sbr
goto END

:BUILD
nmake %1.%2.%3 dx8=1 clean=0

:END
