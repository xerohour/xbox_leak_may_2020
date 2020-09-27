#
# Standard inference rule for generating dsp files.
# Also includes rule for generating DSP scratch image
#

.SUFFIXES: .dsp .dsx .ini .bin .rdf .xpr

{}.dsx{$O}.dsp:
    m4 -DPROJNAME=$(PROJNAME) -DTARGETNAME=$(TARGETNAME) -DTITLEID=$(TITLEID) -DCOMMONSOURCEFILELIST= -DRELEASELIBS= -DDEBUGLIBS= $** > $(O)\tmp.m4
    m4 -DPOUND=# $(O)\tmp.m4 > $@
    del $(O)\tmp.m4
!ifndef NO_BINPLACE
    @!binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $(BINPLACE_LC_MISCFLAGS) $@
!endif

dsstdfx:
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


.rdf{$O}.xpr:
    bundler $** -o $*.xpr -h $*.h -e $*.err
    copy $*.xpr media
    copy $*.h

C_DEFINES=$(C_DEFINES) -D_USE_XGMATH