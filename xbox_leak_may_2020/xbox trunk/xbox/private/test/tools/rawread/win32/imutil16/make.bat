@echo off
md debug > nul
md release > nul
del debug\*.*
del release\*.*
nmake debug=1
if errorlevel 1 goto end
for %%i in (*.obj *.sbr *.dll *.lib) do move %%i debug
nmake debug=0
if errorlevel 1 goto end
for %%i in (*.obj *.sbr *.dll *.lib) do move %%i release
:end
