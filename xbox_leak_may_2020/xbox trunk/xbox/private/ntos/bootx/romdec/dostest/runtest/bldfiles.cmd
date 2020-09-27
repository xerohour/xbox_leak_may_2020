@echo off
if '%1'==''  goto usage
if exist *.org  del *.org
if exist *.enc  del *.enc
if exist *.dec  del *.dec
..\enc\obj\i386\enc.exe %1
goto end
:usage
echo Creates a bunch of files with the size of bootloader from the given file
echo usage: bldfiles filename
:end