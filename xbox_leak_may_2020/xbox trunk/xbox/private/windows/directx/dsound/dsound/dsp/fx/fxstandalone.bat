del obj\err*.txt 

set UTIL_DIR=..\..\..\tools\internal\bin

asm56300 -DSTANDALONE '1' -q -I ..\Include -BObj\%1.cld %1.asm > Obj\err%1.txt

dsplnk -a -g -q -B Obj\%1.cld

%UTIL_DIR%\cldtobin -noXY Obj\%1.cld Obj\%1.bin

%UTIL_DIR%\cldtobin -noXY -asinc -single -byteSwap Obj\%1.cld Obj\%1.txt


%UTIL_DIR%\grep -e ERROR -e asm56300 Obj\err*.txt

:: copy the output files to xboxbins (_NT386TREE)

copy obj\%1.bin %_NT386TREE%\dsp

:: scramble the image file

del %_NT386TREE%\dsp\%1.scr
xcodescr 12345678 obj\%1.bin "%XDK%\source\DSOUND\dsp\bin\%1.scr"
xcodescr 12345678 obj\%1.bin ..\bin\%1.scr





