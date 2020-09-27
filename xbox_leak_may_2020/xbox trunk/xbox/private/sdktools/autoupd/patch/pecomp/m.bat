@if "%_ECHO%" == "" @echo off

setlocal

set CFLAGS= /Ox /GzyF /Zi /W4 /MD
set LFLAGS= /incremental:no /debugtype:cv,fixup /pdbpath:none /merge:.rdata=.text /opt:nowin98 /opt:icf /opt:ref /release

cl %CFLAGS% pecomp.cpp /link %LFLAGS%

endlocal
