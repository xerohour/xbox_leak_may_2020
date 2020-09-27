!MESSAGE Loading Kernel dependencies

RMINCS = $(INCDIR)\nvrm.h		$(INCDIR)\nv_papu.h		$(INCDIR)\nv_uap.h \
		 $(INCDIR)\audiohw.h	$(INCDIR)\ahal.h		$(INCDIR)\nv_uep.h \
		 $(AUDIOFXDIR)\AudioFx.h

#
# add INCLUDE directory where all the DSP programs are stored
#
CVXDINCS = $(CVXDINCS) /I$(INCVC50DIR) /I$(DSPPROGDIR) /I$(AUDIOFXDIR) /I$(AUDIOFXDRIVERDIR) /I$(KERNAPUDIR) /I$(INCDSTYPESDIR) /I$(INCDSDRIVERSDIR)

#
# Kernel Chip-Independent Resource Manager Objects.
#
KERNLIB = $(BINDIR)\nvkern.lib
KERNOBJS =	\
	$(OBJDIR)\chalvoiceproc.obj $(OBJDIR)\ahal.obj $(OBJDIR)\chalmcp1.obj \
	$(OBJDIR)\cvpmethod.obj	$(OBJDIR)\ccontextdma.obj $(OBJDIR)\chalglobalproc.obj \
	$(OBJDIR)\chalextendedproc.obj $(OBJDIR)\cepmethod.obj $(OBJDIR)\chalbase.obj  \
	$(OBJDIR)\cgpscratchmanager.obj $(OBJDIR)\cscratchdma.obj $(OBJDIR)\cgpmethod.obj \
	$(OBJDIR)\cgpdspprogram.obj $(OBJDIR)\cscratchmanager.obj $(OBJDIR)\ccommandq.obj \
	$(OBJDIR)\cepscratchmanager.obj $(OBJDIR)\cepdspprogram.obj \
	$(OBJDIR)\audiofx.obj  $(OBJDIR)\util.obj $(OBJDIR)\dsp_util.obj
#     $(OBJDIR)\nullmodule_util.obj $(OBJDIR)\dsp_binaries.obj \
#	$(OBJDIR)\ampmod_util.obj $(OBJDIR)\chorus_util.obj $(OBJDIR)\compressor_util.obj \
#	$(OBJDIR)\delay_util.obj $(OBJDIR)\distortion_util.obj $(OBJDIR)\dmamodule_util.obj \
#	$(OBJDIR)\echo_util.obj \
#	$(OBJDIR)\equalizer_util.obj $(OBJDIR)\flange_util.obj $(OBJDIR)\iiir_util.obj \
#	$(OBJDIR)\iir1_util.obj $(OBJDIR)\iir2_util.obj $(OBJDIR)\iir2hz_lowpass.obj \
#	$(OBJDIR)\limiter_util.obj $(OBJDIR)\mixer_util.obj \
#	$(OBJDIR)\oscillator_util.obj $(OBJDIR)\reverb_util.obj


#
# AUDIO FILES     
#

$(OBJDIR)\chalbase.obj: $(KERNAPUDIR)\chalbase.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF


$(OBJDIR)\chalextendedproc.obj: $(KERNAPUDIR)\chalextendedproc.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF


$(OBJDIR)\chalglobalproc.obj: $(KERNAPUDIR)\chalglobalproc.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF


$(OBJDIR)\ccontextdma.obj: $(KERNAPUDIR)\ccontextdma.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\chalvoiceproc.obj: $(KERNAPUDIR)\chalvoiceproc.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cvpmethod.obj: $(KERNAPUDIR)\cvpmethod.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cepmethod.obj: $(KERNAPUDIR)\cepmethod.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\ahal.obj: $(KERNAPUDIR)\ahal.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\chalmcp1.obj: $(KERNAPUDIR)\chalmcp1.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cgpscratchmanager.obj: $(KERNAPUDIR)\cgpscratchmanager.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cscratchdma.obj: $(KERNAPUDIR)\cscratchdma.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cgpmethod.obj: $(KERNAPUDIR)\cgpmethod.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cgpdspprogram.obj: $(KERNAPUDIR)\cgpdspprogram.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cscratchmanager.obj: $(KERNAPUDIR)\cscratchmanager.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\ccommandq.obj: $(KERNAPUDIR)\ccommandq.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cepscratchmanager.obj: $(KERNAPUDIR)\cepscratchmanager.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\cepdspprogram.obj: $(KERNAPUDIR)\cepdspprogram.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(KERNAPUDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(KERNAPUDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# AudioFX files
#

$(OBJDIR)\util.obj: $(AUDIOFXDIR)\util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\reverb_util.obj: $(AUDIOFXDIR)\reverb_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\oscillator_util.obj: $(AUDIOFXDIR)\oscillator_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\nullmodule_util.obj: $(AUDIOFXDIR)\nullmodule_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mixer_util.obj: $(AUDIOFXDIR)\mixer_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\limiter_util.obj: $(AUDIOFXDIR)\limiter_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\iir2hz_lowpass.obj: $(AUDIOFXDIR)\iir2hz_lowpass.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\iir2_util.obj: $(AUDIOFXDIR)\iir2_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\iir1_util.obj: $(AUDIOFXDIR)\iir1_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\iiir_util.obj: $(AUDIOFXDIR)\iiir_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\flange_util.obj: $(AUDIOFXDIR)\flange_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\equalizer_util.obj: $(AUDIOFXDIR)\equalizer_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\ampmod_util.obj: $(AUDIOFXDIR)\ampmod_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\audiofx.obj: $(AUDIOFXDIR)\audiofx.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\chorus_util.obj: $(AUDIOFXDIR)\chorus_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\compressor_util.obj: $(AUDIOFXDIR)\compressor_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\delay_util.obj: $(AUDIOFXDIR)\delay_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\distortion_util.obj: $(AUDIOFXDIR)\distortion_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dmamodule_util.obj: $(AUDIOFXDIR)\dmamodule_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dsp_binaries.obj: $(AUDIOFXDIR)\dsp_binaries.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dsp_util.obj: $(AUDIOFXDIR)\dsp_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\echo_util.obj: $(AUDIOFXDIR)\echo_util.cpp $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(AUDIOFXDIR)\$(@B).cpp
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(AUDIOFXDIR)\$(@B).cpp
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF



#
# Intermediate libraries.
# 
$(BINDIR)\nvkern.lib : $(KERNOBJS)
    $(LIB32) /SUBSYSTEM:WINDOWS /OUT:$(BINDIR)\nvkern.lib @<<
$(KERNOBJS)
<<
