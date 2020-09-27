@echo off

REM *************************************************************************
REM Copyright (C) Microsoft Corporation.  All rights reserved.
REM 
REM Abstract:
REM 
REM     This batch file copies the necessary files required to make a stand
REM     alone CD to run the Hardware Verification Suite tests.
REM 
REM *************************************************************************



REM
REM Create directory structure
REM
 call createdirs.cmd

SET __HVS_XBOXNAME=%1
IF "" == "%1" SET __HVS_XBOXNAME=your xbox
SET __HVS_XBOX=-x %__HVS_XBOXNAME%
if "" == "%1" SET __HVS_XBOX=

REM
REM Delete any existing files from the Xbox
REM
 Echo.
 Echo Removing any existing files from the Xbox...
 xbdel %__HVS_XBOX=% /r /s /h /f xe:\hvs >nul

REM
REM Copy the files to the xbox
REM
 Echo Copying the files to the Xbox...
 xbcp %__HVS_XBOX=% /q /r /s /h /t /f %_NT386TREE%\xboxtest\hvs xe:\ >nul

REM
REM Clear any file attributes that could cause problems
REM
 Echo Clearing any file attributes...
 xbattrib %__HVS_XBOX=% /r /s /h -ro -hid xe:\hvs

goto END

:END

echo.
echo *************************************************************************
echo Done. Files copied over to %__HVS_XBOXNAME%.  Files located in xe:\hvs
echo *************************************************************************

SET __HVS_XBOXNAME=
SET __HVS_XBOX=

@echo on