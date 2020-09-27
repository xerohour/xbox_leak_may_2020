    set _XGPIMAGE_DSP_CODE_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\bin
    set _XGPIMAGE_INI_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\ini

del %_NT386TREE%\defaultscratchimg.bin

xgpimage %1.ini %_NT386TREE%\defaultscratchimg.bin c:\xboxbins\%1.h

