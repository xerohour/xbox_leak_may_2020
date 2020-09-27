@echo off
REM Builds and copies everything (private harness, media) to XBox and runs it.
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\bat\b
call copymedia
call %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn\bat\r %1
:END
