pushd .


REM ----- Sync up --------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\sync


REM ----- Build app and copy it to XBox --------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\B

REM ---- Copy ALL media to XBox (only need to do this once!) -----
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\COPYMEDIA

REM ------------- Copy required repro case to XBox ---------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\P BVTAUDIO

REM ------------- Run the test (optional) ------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\R

popd