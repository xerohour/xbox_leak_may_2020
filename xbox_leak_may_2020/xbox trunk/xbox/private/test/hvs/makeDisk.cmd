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
 call %_NTDRIVE%%_NTROOT%\private\test\hvs\createdirs.cmd

REM
REM Create the ISO image
REM
 echo.
 gdfimage %_NT386TREE%\xboxtest\hvs %_NT386TREE%\xboxtest\hvs.iso


echo.
echo.
echo *************************************************************************
echo Done. Image located in %_NT386TREE%\xboxtest\hvs.iso
echo *************************************************************************
@echo on