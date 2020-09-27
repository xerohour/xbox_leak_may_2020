del Obj\err*.txt 

set BUILDDIR=c:\xboxbins\dsp
set UTIL_DIR=..\..\..\tools\internal\bin

asm56300 -DFX_TEST '1' -a -g -q -I ..\Include -BObj\bootsnd.cld bootsnd.asm > Obj\errBootSnd.txt

%UTIL_DIR%\cldtobin -noXY Obj\bootSnd.cld Obj\bootSnd.bin

%UTIL_DIR%\cldtobin -noXY -asinc -single -byteSwap Obj\bootSnd.cld Obj\bootSnd.txt
copy obj\bootSnd.txt ..\..\dspbootsndcode.h

%UTIL_DIR%\grep -e ERROR -e asm56300 Obj\err*.txt

:: rebuild dsound

cd ..\..\..\boot\
build -cz

cd ..\dsound\dsp\fx





