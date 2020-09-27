del Obj\err*.txt 

set BUILDDIR=c:\xboxbins\dsp
set UTIL_DIR=..\..\..\tools\internal\bin

asm56300 -DFX_START_ROUTINE 'StartEcho' -DFX_FILENAME 'echo.asm' -DFX_TEST '1' -a -g -q -I ..\Include -BObj\defaultEngine.cld defaultEngine.asm > Obj\errDefaultEngine.txt


%UTIL_DIR%\cldtobin -noXY Obj\defaultEngine.cld Obj\defaultEngine.bin

%UTIL_DIR%\cldtobin -noXY -asinc -single -byteSwap Obj\defaultEngine.cld Obj\defaultEngine.txt
copy obj\defaultEngine.txt ..\..\dspexeccode.h

%UTIL_DIR%\grep -e ERROR -e asm56300 Obj\err*.txt

:: rebuild dsound

cd ..\..
touch gpdsp.cpp
build -z

cd dsp\fx





