del /q %_NTDRIVE%%_NTROOT%\private\test\lib\i386\DMTEST_VALID.LIB
call %_NTDRIVE%%_NTROOT%\private\test\tools\build\s.cmd
build -DIP
call CopyMedia.bat



