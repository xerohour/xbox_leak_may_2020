@echo off
set ERROR=No Error
set XBENAME=DMHARN
if "%1" == "" goto NOPARAM
if not exist %_NT386TREE%\dump\%XBENAME%.xbe goto NOXBE
set DESTDIR=\\xbox\usr\DanHaff\DMHarn\%1
echo set CURDIR=%DESTDIR%>Install.cmd
type install.txt>>install.cmd
mkdir %DESTDIR%
mkdir %DESTDIR%\media
xcopy %_NT386TREE%\dump\%XBENAME%.xbe %DESTDIR%
xcopy %_NT386TREE%\dump\%XBENAME%.pdb %DESTDIR%
xcopy %_NT386TREE%\dump\%XBENAME%.sym %DESTDIR%
xcopy install.cmd %DESTDIR%
xcopy /s ..\media\*.* %DESTDIR%\media
xcopy %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\dmharn.ini %DESTDIR%
echo YOUR INSTALL POINT IS %DESTDIR%\Install.cmd

goto END

:NOPARAM
echo Need to specify the destination directory name, fool.
goto END

:NOXBE
echo What is wrong with you?  %_NT386TREE%\dump\%XBENAME%.xbe does not exist.  
goto END


:END