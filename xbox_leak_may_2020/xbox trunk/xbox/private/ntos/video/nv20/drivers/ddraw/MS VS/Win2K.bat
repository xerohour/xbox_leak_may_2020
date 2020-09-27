@echo off

set RELEASE_BRANCH=main

set BUILD_TOOLS_DIR=%BUILDDIR%\tools
set BUILD_NVSDK_DIR=%BUILDDIR%\%RELEASE_BRANCH%\sdk\nvidia
set BUILD_SDK_DIR=%BUILDDIR%\tools\sdk
set BUILD_DDK_DIR=%BUILDDIR%\tools\ddk
set BUILD_DDK_VER=nt5

cd %BUILDDIR%\%RELEASE_BRANCH%\drivers\display\winnt4

if "%1" == "/a" goto CLEAN_BUILD
if "%2" == "/a" goto CLEAN_BUILD
if "%3" == "/a" goto CLEAN_BUILD
if "%4" == "/a" goto CLEAN_BUILD
if "%5" == "/a" goto CLEAN_BUILD
if "%6" == "/a" goto CLEAN_BUILD
if "%7" == "/a" goto CLEAN_BUILD
if "%8" == "/a" goto CLEAN_BUILD
if "%9" == "/a" goto CLEAN_BUILD

call blddisp %1 %2 %3 %4 %5 %6 %7 %8 %9
goto BROWSE

:CLEAN_BUILD
call blddisp %1 clean %2 %3 %4 %5 %6 %7 %8 %9

if "%1" == "/browse" goto BROWSE
if "%2" == "/browse" goto BROWSE
if "%3" == "/browse" goto BROWSE
if "%4" == "/browse" goto BROWSE
if "%5" == "/browse" goto BROWSE
if "%6" == "/browse" goto BROWSE
if "%7" == "/browse" goto BROWSE
if "%8" == "/browse" goto BROWSE
if "%9" == "/browse" goto BROWSE
goto END

:BROWSE
Echo Building browser information file...
cd %BUILDDIR%\%RELEASE_BRANCH%\drivers\ddraw
BSCMAKE /o nvdd32 common\*.sbr

:END
