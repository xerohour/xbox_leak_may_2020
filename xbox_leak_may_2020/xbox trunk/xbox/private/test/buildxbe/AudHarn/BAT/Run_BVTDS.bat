pushd .

REM ---------- Sync test files------------------------------------
cd /d %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn
sd sync ...
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dsound
sd sync ...

REM ----- Build app and copy it to XBox --------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\B

REM ---- Copy ALL media to XBox (only need to do this once!) -----
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\COPYMEDIA

REM ------------- Copy required repro case to XBox ---------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\P BVTDS

REM ------------- Run the test (optional) ------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\BAT\R

popd