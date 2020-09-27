@if "%_ECHO%" == "" @echo off

setlocal

set OUTTYPE=debug
set INCLUDE=%INCLUDE%;.\inc
set LIB=%LIB%;.\lib\%PROCESSOR_ARCHITECTURE%
set BIN=.\bin\%PROCESSOR_ARCHITECTURE%
set OUT=%OUTTYPE%\%PROCESSOR_ARCHITECTURE%

set CFLAGS= /DDEBUG=1 /GzyF /Zi /W4 /ML -Fd%OUT%\ -Fe%OUT%\ -Fo%OUT%\
set LFLAGS= /incremental:no /debugtype:cv /pdbpath:none

mkdir %OUT% >nul 2>nul

cl /Od %CFLAGS% sym7.cpp mspdb70.lib version.lib /link %LFLAGS%

copy /y %BIN% %OUT%

endlocal
