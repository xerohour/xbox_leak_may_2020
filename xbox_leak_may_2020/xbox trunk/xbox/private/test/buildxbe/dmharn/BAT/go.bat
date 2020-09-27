@echo off
REM Builds libs in subdirectories
REM -----------------------------------------------------------
echo DELETING LIBS
del /s *.lib
echo BUILDING FILES
build -DIP


