echo off
REM Builds everything, but doesn't run it.  Good for when you want to debug by starting Visual Studio
REM ------------------------------------------------------------------------------------------------------------
pushd .
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\bat\p %1
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmth8
build -DIP
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1
build -DIP
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dsound
build -DIP
cd /d %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\goHarness.bat
popd


