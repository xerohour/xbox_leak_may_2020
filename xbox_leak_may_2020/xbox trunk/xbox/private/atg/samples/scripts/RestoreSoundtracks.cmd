@echo off
if (%1)==() goto Usage

if NOT EXIST %1 echo %1 doesn't exist & goto End
if NOT EXIST %1\ST.DB echo %1 doesn't contain a soundtrack database & goto End

xbcp /r /s /h /q /t %1\*.* xc:\TDATA\fffe0000\music

goto End

:Usage
echo RestoreSoundtracks [LocalDir]

:End