    set _XGPIMAGE_DSP_CODE_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\bin
    set _XGPIMAGE_INI_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\ini

del %_NT386TREE%\defaultscratchimg.bin
mkdir %_NT386TREE%\dsp 1>nul
xgpimage defaultScratchImg.ini %_NT386TREE%\defaultscratchimg.bin %_NT386TREE%\dsp\default.h

del %_NT386TREE%\dsstdfx.bin
xgpimage dsstdfx.ini %_NT386TREE%\dsstdfx.bin %_NTDRIVE%%_NTROOT%\public\sdk\inc\dsstdfx.h

