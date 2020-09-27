@echo off
REM Regenerate shader assembler source files.

sd edit shadeasm.h
c:\temp\byacc -p d3dxasm_ -l shadeasm.y
del shadeasm.h
ren Y_TAB.C shadeasm.h
