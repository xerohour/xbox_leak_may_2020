@echo off
SET __TITLEID=00001123

@rmdir /s /q iso >nul
mkdir iso
xcopy %_NTBINDIR%\private\test\ui\muconfig\tdata\*.* iso /S /E /I /H /Y
copy %_NT386TREE%\dump\muconfig.xbe iso\default.xbe
attrib -r -s -h iso /S /D
gdfimage iso .\MUConfig.iso
rmdir /s /q iso
goto end

:end
SET __TITLEID=
