@echo off
REM Builds and copies everything to XBox and runs it.
REM -------------------------------------------------------------------------
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\bat\b 
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\audharn\bat\r %1

