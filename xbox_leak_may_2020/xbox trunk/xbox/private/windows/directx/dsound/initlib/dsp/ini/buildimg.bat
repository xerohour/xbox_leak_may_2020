@echo off
set _XGPIMAGE_DSP_CODE_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\bin
set _XGPIMAGE_INI_PATH=%_NTDRIVE%%_NTROOT%\private\windows\directx\dsound\dsound\dsp\ini
xgpimage %1.ini %_NT386TREE%\%1.bin %_NTBINDIR%\public\sdk\inc\%1.h