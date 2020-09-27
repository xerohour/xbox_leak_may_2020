!MESSAGE Loading Kernel dependencies

RMINCS = $(INCDIR)\nvrm.h       $(INCDIR)\nv_papu.h    $(INCDIR)\nv_uap.h \
		 $(INCDIR)\audiohw.h	$(INCDIR)\ahal.h $(INCDIR)\nv_uep.h

#
# SOURCE line for DEBUG Kernel nmsym
#
KERNSRCDIRS =	\
	/SOURCE:..\$(KERNAPUDIR)\mcpx;..\$(HALDIR)\mcp1; 

#
# Kernel Chip-Independent Resource Manager Objects.
#
KERNLIB = $(BINDIR)\nvkern.lib
KERNOBJS =	\
	$(OBJDIR)\chalvoiceproc.obj $(OBJDIR)\ahal.obj $(OBJDIR)\chalmcp1.obj \
	$(OBJDIR)\cmcp1method.obj	$(OBJDIR)\ccontextdma.obj $(OBJDIR)\chalglobalproc.obj \
	$(OBJDIR)\chalextendedproc.obj $(OBJDIR)\cepmethod.obj $(OBJDIR)\chalbase.obj

 
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

$(OBJDIR)\cmcp1method.obj: $(KERNAPUDIR)\cmcp1method.cpp $(RMINCS)
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

#
# Intermediate libraries.
# 
$(BINDIR)\nvkern.lib : $(KERNOBJS)
    $(LIB32) /SUBSYSTEM:WINDOWS /OUT:$(BINDIR)\nvkern.lib @<<
$(KERNOBJS)
<<
