@echo off
cd /d %_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\test
xbcp -f -x dereksx %_NT386TREE%\dump\testds.xbe xe:\
call cpymedia.bat
xbreboot -c xe:\testds.xbe