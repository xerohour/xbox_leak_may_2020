
fximg:
    copy $(_NT386TREE)\$*.bin media

{}.ini{$O}.bin:
    set _XGPIMAGE_DSP_CODE_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\bin
    set _XGPIMAGE_INI_PATH=%%_NTDRIVE%%%%_NTROOT%%\private\windows\directx\dsound\dsound\dsp\ini
    xgpimage $** $*.bin $*.h
    copy $*.bin media
    copy $*.h
!ifndef NO_BINPLACE
    @!binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $(BINPLACE_LC_MISCFLAGS) $@
    @!binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $(BINPLACE_LC_MISCFLAGS) $*.h
!endif

