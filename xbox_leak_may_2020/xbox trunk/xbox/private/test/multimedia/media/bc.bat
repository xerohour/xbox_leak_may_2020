@echo off
REM Builds and copies everything, but doesn't run it.  Good for when you want to debug by starting Visual Studio
REM ------------------------------------------------------------------------------------------------------------
call CopyMedia.bat
call b.bat %1

:END


