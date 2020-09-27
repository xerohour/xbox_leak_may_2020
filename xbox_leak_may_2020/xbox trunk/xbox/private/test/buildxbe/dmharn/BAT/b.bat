echo off
REM Builds everything, but doesn't run it.  Good for when you want to debug by starting Visual Studio
REM ------------------------------------------------------------------------------------------------------------
pushd .
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\bat\p %1
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmth8
build -DIP
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1
build -DIP
cd /d %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\goHarness.bat
popd


