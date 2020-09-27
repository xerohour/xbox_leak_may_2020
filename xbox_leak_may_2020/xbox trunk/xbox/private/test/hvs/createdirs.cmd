@echo off

REM *************************************************************************
REM Copyright (C) Microsoft Corporation.  All rights reserved.
REM 
REM Abstract:
REM 
REM     This batch file creates the appropriate directory structure for
REM     the HVS Launcher and all of its tests.  This will allow these
REM     programs to be compiled in to an Xbox disc image, or simply
REM     copied over to an Xbox to be executed from the XDK Launcher.
REM 
REM *************************************************************************

REM
REM Clean up existing directories if they exist
REM
 if EXIST %_NT386TREE%\xboxtest\hvs Echo Removing Existing Directory (%_NT386TREE%\xboxtest\hvs)...
 if EXIST %_NT386TREE%\xboxtest\hvs rmdir /s /q %_NT386TREE%\xboxtest\hvs >nul

REM
REM Create directory structure
REM
 Echo Creating the directories...
 if NOT EXIST %_NT386TREE%\xboxtest mkdir %_NT386TREE%\xboxtest >nul
 mkdir %_NT386TREE%\xboxtest\hvs >nul
 mkdir %_NT386TREE%\xboxtest\hvs\media >nul
 mkdir %_NT386TREE%\xboxtest\hvs\tests >nul
 mkdir %_NT386TREE%\xboxtest\hvs\roms >nul
 mkdir %_NT386TREE%\xboxtest\hvs\symbols >nul
 mkdir %_NT386TREE%\xboxtest\hvs\docs >nul

REM
REM Copy the UI
REM
 Echo Copying the UI...
 copy /Y %_NT386TREE%\dump\hvslaunch.xbe  %_NT386TREE%\xboxtest\hvs\default.xbe >nul
 copy /Y %_NT386TREE%\dump\hvslaunch.pdb  %_NT386TREE%\xboxtest\hvs\symbols\default.pdb >nul

REM
REM Copy the tests
REM
 Echo Copying the Tests...
 REM (Disk Read)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskRead\diskRead.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskRead\diskRead.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskRead\diskRead.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\diskRead.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\diskRead.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

 REM (Read File)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\readFile\readFile.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\readFile\readFile.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\readFile\readFile.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\readFile.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\readFile.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

 REM (Disk Write)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskWrite\diskWrite.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskWrite\diskWrite.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\diskWrite\diskWrite.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\diskWrite.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\diskWrite.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

 REM (Reboot)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\reboot\reboot.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\reboot\reboot.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\reboot\reboot.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\reboot.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\reboot.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

 REM (Mem Check)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\memcheck\memcheck.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\memcheck\memcheck.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\memcheck\memcheck.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\memcheck.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\memcheck.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

 REM (CD Rip)
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\CDRip\cdrip.ini %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\CDRip\cdrip.cfg %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NTDRIVE%%_NTROOT%\private\test\hvs\CDRip\cdrip.txt %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\CDRip.xbe %_NT386TREE%\xboxtest\hvs\tests >nul
 copy /Y %_NT386TREE%\dump\CDRip.pdb %_NT386TREE%\xboxtest\hvs\symbols >nul

REM
REM Copy the ROMs
REM
 Echo Copying the ROMS...
 copy /Y %_NT386TREE%\boot\xboxrom*.bin %_NT386TREE%\xboxtest\hvs\roms >nul

REM
REM Copy the Media
REM
 Echo Copying the Media...
 copy %_NTDRIVE%%_NTROOT%\private\test\hvs\launcher\default.cfg %_NT386TREE%\xboxtest\hvs\default.cfg >nul
 xcopy %_NTDRIVE%%_NTROOT%\private\test\hvs\launcher\media\* %_NT386TREE%\xboxtest\hvs\media /E /Y /R >nul

REM
REM Copy Documentation
REM
 Echo Copying Documentation...
 copy "%_NTDRIVE%%_NTROOT%\private\test\hvs\launcher\HVS Launcher Documentation.doc" "%_NT386TREE%\xboxtest\hvs\docs\HVS Launcher Documentation.doc" >nul
 copy "%_NTDRIVE%%_NTROOT%\private\test\hvs\docs\titleIDs.txt" "%_NT386TREE%\xboxtest\hvs\docs\titleIDs.txt" >nul
