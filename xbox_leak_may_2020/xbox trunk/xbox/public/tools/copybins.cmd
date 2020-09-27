@echo off

if "" == "%_NT386TREE%" goto usage

echo Ready to update your target Xbox's kernel and dashboard from %_NT386TREE%
pause
echo.
xbcp -f -q -y %_NT386TREE%\boot\xboxrom_dvt4.bin xc:\xboxrom.bin
if not "0" == "%errorlevel%" goto end
xbcp -f -q -y %_NT386TREE%\devkit\xbdm.dll xc:\
xbcp -f -q -y %_NT386TREE%\devkit\cydrive.exe xe:\dxt\cydrive.dxt

xbcp -f -q -y %_NT386TREE%\dump\xshell.xbe xy:\
xbcp -f -q -y -r %_NTBINDIR%\private\test\ui\xshell\tdata\ xy:\
xbcp -f -q -y %_NTBINDIR%\private\test\ui\xshell\dashboard.xbx xc:\
echo.
echo Ready to reboot your target Xbox
pause
echo.
xbreboot -c

goto end

:usage
echo usage: copybins (_NT386TREE must be defined)

:end
