@echo off
setlocal
call \nt\public\tools\projects.cmd %1
if "%3" == "restore" goto restore
@echo on
@md %2%proj_path%
@attrib -h %proj_path%\slm.ini
copy %_ntdrive%%proj_path%\slm.ini %2%proj_path%
@attrib +h %proj_path%\slm.ini
@echo off
@goto end
:restore
@md %_ntdrive%%proj_path%
copy %2%proj_path%\slm.ini %_ntdrive%%proj_path%
@attrib +h +r %proj_path%\slm.ini
@echo off
@goto end
:end
endlocal
