@echo **************************************************************
@echo COMPONENT OWNERS: PLEASE KEEP THIS UP TO DATE
@echo Copies XBox OS files from current directory into NT directory.
@echo Usage: updatexb [winnt dir path]
@echo **************************************************************
@echo on
if "%1" == "" goto setdefault
set xboxroot=%1
goto copyfiles

:setdefault
set xboxroot=c:\xboxos
goto copyfiles

:copyfiles
copy ntoskrnl.exe %xboxroot%\system32
copy *.sys %xboxroot%\system32\drivers
copy boot\*.* %xboxroot%\system32
copy dump\default.xbe %xboxroot%\system32
copy devkit\xbdm.dll %xboxroot%\system32
copy dump\nv.dll %xboxroot%\system32

@rem copy xbox test harness
copy dump\harness.xbe %xboxroot%\system32\default.xbe
xcopy xboxtest\testini.ini c:\tdata\a7049955\