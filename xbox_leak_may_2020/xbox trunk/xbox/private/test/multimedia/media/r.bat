@echo off
REM Runs the current set of tests on the XBox.
REM -----------------------------------------
if not "%1" == "" call p %1
xbreboot xe:\MEDIA.XBE

:END



