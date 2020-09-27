@echo off
if not "%1" == "" SET _XBOXMACHINE=-x %1
if "%1" == "" SET _XBOXMACHINE=
xbmkdir %_XBOXMACHINE% xc:\tdata\a7049955
xbmkdir %_XBOXMACHINE% xc:\tdata\103be6d2
xbcp -q -y %_XBOXMACHINE% %_NT386TREE%\dump\harness.xbe xc:\devkit
xbcp -q -y %_XBOXMACHINE% %_NT386TREE%\dump\hwtest.xbe xc:\devkit
xbcp -q -y %_XBOXMACHINE% %_NT386TREE%\xboxtest\testini.ini xc:\tdata\a7049955