DSP: mixbinsImage.bin

mixbinsImage.bin: ..\$*.ini
    set _XGPIMAGE_DSP_CODE_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\bin
    set _XGPIMAGE_INI_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\ini
    xgpimage $** $(_NT386TREE)\$*.bin ..\$*.h
