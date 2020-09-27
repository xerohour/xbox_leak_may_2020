@echo off
SET __TITLEID=000000A2
SET __XBENAME=GRegion3

@rmdir /s /q iso >nul
mkdir iso
mkdir iso\%__TITLEID%
xcopy %_NTBINDIR%\private\test\ui\GameRegions\%__XBENAME%\tdata\*.* iso\%__TITLEID% /S /E /I /H /Y
copy %_NT386TREE%\dump\%__XBENAME%.xbe iso\default.xbe
gdfimage iso .\%__XBENAME%.iso
rmdir /s /q iso
goto end

:end
SET __TITLEID=
SET __XBENAME=
