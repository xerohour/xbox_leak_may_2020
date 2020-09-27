pushd .

REM ---------- Sync test files------------------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\BAT\sync

REM ----- Build app and copy it to XBox --------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\BAT\B

REM ---- Copy ALL media to XBox (only need to do this once!) -----
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\BAT\COPYMEDIA

REM ------------- Copy required repro case to XBox ---------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\BAT\P HarnessBVT

REM ------------- Run the test (optional) ------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\BAT\R

popd