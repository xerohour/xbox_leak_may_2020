del /q %_NTDRIVE%%_NTROOT%\private\test\lib\i386\TRANS.LIB
call d:\util\batch\s.cmd

:XBOX
 cd XBox
 build -DIP
 cd..
REM goto END

:WIN
 cd Win
 build -DIP
 cd..
 goto END


:END
 cd..
