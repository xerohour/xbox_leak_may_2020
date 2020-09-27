del obj\err*.txt 

set BUILDDIR=c:\xboxbins\dsp
set UTIL_DIR=..\..\..\tools\internal\bin

::asm56300  -DHALFSAMPLERATE '1' -DSTANDALONE '1' -q -I ..\Include -BObj\reverb.cld reverb_wrapper.asm > Obj\errReverb.txt
asm56300  -DHALFSAMPLERATE '1' -DSTANDALONE '1' -q -I ..\Include -BObj\reverb.cld reverb_wrapper.asm > Obj\errReverb.txt

dsplnk -a -g -q -B -mreverb.map Obj\reverb.cld 

%UTIL_DIR%\cldtobin -noXY Obj\reverb.cld Obj\reverb.bin

%UTIL_DIR%\cldtobin -noXY -asinc -single -byteSwap Obj\reverb.cld Obj\reverb.txt


%UTIL_DIR%\grep -e ERROR -e asm56300 Obj\err*.txt

:: copy the output files to xboxbins (_NT386TREE)

copy obj\reverb.bin %_NT386TREE%\dsp

:: scramble the image file

del %_NT386TREE%\dsp\reverb24k.scr
xcodescr 12345678 obj\reverb.bin %_NT386TREE%\dsp\reverb24k.scr
xcodescr 12345678 obj\reverb.bin ..\bin\reverb24k.scr





