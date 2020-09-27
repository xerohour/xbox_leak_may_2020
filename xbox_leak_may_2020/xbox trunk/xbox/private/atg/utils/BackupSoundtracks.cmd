@echo off
if (%1)==() goto Usage

if NOT EXIST %1 mkdir %1

xbcp /r /s /h /q /t xc:\TDATA\fffe0000\music\*.* %1

goto End

:Usage
echo BackupSoundtracks [LocalDir]

:End