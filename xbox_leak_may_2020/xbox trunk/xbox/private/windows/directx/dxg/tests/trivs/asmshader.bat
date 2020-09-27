@echo off
REM Run vertex shader assembler

out shader.h
..\..\tools\xsasm\obj\i386\xsasm -h -l -b shader.nvv
