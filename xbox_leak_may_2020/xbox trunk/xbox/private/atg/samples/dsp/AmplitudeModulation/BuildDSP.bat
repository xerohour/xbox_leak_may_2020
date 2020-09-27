REM Set up our parameters
set UTIL_DIR=..\bin
set OBJ_DIR=obj
set EFFECT=AmplitudeModulation

REM Ensure our output directory exists and delete old error file
if NOT EXIST %OBJ_DIR% mkdir %OBJ_DIR%
del %OBJ_DIR%\errors.txt 

REM Assemble effect - add -DSTEREO for a stereo version
asm56300 -DSTANDALONE '1' -q -I ..\Include -B%OBJ_DIR%\%EFFECT%.cld %EFFECT%.asm > errors.txt

REM Link effect
dsplnk -a -g -q -b %OBJ_DIR%\%EFFECT%.cld

REM Generate .bin file
%UTIL_DIR%\cldtobin -noXY %OBJ_DIR%\%EFFECT%.cld %OBJ_DIR%\%EFFECT%.bin

REM Generate new scrambled image from .bin file
del %OBJ_DIR%\%EFFECT%.scr
xcodescr 12345678 %OBJ_DIR%\%EFFECT%.bin %OBJ_DIR%\%EFFECT%.scr





