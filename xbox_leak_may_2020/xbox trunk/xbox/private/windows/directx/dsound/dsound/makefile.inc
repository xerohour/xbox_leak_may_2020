DSP: dsstdfx.bin defaultscratchimg.bin

$(XDK_LIB_PATH)\$(TARGETNAME).lib: $(SDK_LIB_PATH)\$(TARGETNAME).lib
    copy $** $@

dsstdfx.bin defaultscratchimg.bin: dsp\ini\$*.ini
    set _XGPIMAGE_DSP_CODE_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\bin
    set _XGPIMAGE_INI_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\ini
    xgpimage $** $(_NT386TREE)\$*.bin $(SDK_INC_PATH)\$*.h
    copy $(SDK_INC_PATH)\$*.h $(XDK_INC_PATH)\$*.h
