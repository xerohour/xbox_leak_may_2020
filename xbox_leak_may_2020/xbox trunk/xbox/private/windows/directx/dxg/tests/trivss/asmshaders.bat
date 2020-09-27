@echo off
REM Run vertex shader assembler
out shader.h
..\..\tools\xsasm\obj\i386\xsasm -h -l -b shader.nvv
out stateShader.h
..\..\tools\xsasm\obj\i386\xsasm -v -h -l -b stateShader.nvv
