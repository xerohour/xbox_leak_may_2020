@if "%_ECHO%" == "" @echo off

setlocal

set OUTTYPE=release
set INCLUDE=%INCLUDE%;.\inc
set LIB=%LIB%;.\lib\%PROCESSOR_ARCHITECTURE%
set BIN=.\bin\%PROCESSOR_ARCHITECTURE%
set OUT=%OUTTYPE%\%PROCESSOR_ARCHITECTURE%

set CFLAGS= /GzyF /Zi /W4 /MD -Fd%OUT%\ -Fe%OUT%\ -Fo%OUT%\
set LFLAGS= /incremental:no /debugtype:cv,fixup /pdbpath:none /merge:.rdata=.text /opt:nowin98 /opt:icf /opt:ref /release

mkdir %OUT% >nul 2>nul

cl /Ox %CFLAGS% sym7.cpp mspdb70.lib version.lib /link %LFLAGS%

copy /y %BIN% %OUT%

endlocal
