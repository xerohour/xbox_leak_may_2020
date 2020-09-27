del obj\err*.txt 

set BUILDDIR=c:\xboxbins\dsp
set UTIL_DIR=..\..\..\tools\internal\bin

asm56300 -D%2 '1' -DSTANDALONE '1' -q -I ..\Include -BObj\%1.cld %1.asm > Obj\err%1.txt

dsplnk -a -g -q -B Obj\%1.cld

%UTIL_DIR%\cldtobin -noXY Obj\%1.cld Obj\%1_%2.bin

%UTIL_DIR%\cldtobin -noXY -asinc -single -byteSwap Obj\%1.cld Obj\%1_%2.txt


%UTIL_DIR%\grep -e ERROR -e asm56300 Obj\err*.txt

:: copy the output files to xboxbins (_NT386TREE)

copy obj\%1_%2.bin %_NT386TREE%\dsp
copy obj\%1_%2.txt %_NT386TREE%\dsp

:: scramble the image file

xcodescr 12345678 obj\%1_%2.bin %_NT386TREE%\dsp\%1_%2.scr
xcodescr 12345678 obj\%1_%2.bin ..\bin\%1_%2.scr







