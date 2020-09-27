# Load the common kernel targets

!INCLUDE $(INCDIR)\akernel.mak
!MESSAGE Loading NVRM dependencies

VDDOBJS = $(OBJDIR)\nvmini.obj
VDD2OBJS = $(OBJDIR)\nvmini2.obj

!IFDEF EXCLUDE_SRCS
SOURCE_PACKAGING = NOPACKAGE
!ELSE
SOURCE_PACKAGING = PACKAGE
!ENDIF

#
# 32 bit VxD Resource Manager.
#

VXDOBJS = $(OBJDIR)\avxdstub.obj \
	$(OBJDIR)\vmmwrap.obj	\
	$(OBJDIR)\nvauapi.obj     \
    $(KERNLIB)              $(OBJDIR)\aosinit.obj   $(OBJDIR)\aos.obj \
	$(OBJDIR)\aosapi.obj


#
# OS files.
#
$(OBJDIR)\avxdstub.obj : $(VXDDIR)\avxdstub.asm $(INCVXDDIR)\debug.inc $(INCVXDDIR)\vmm.inc $(INCNV)\vnvrmd.inc
        set ML= $(AFLAGSVXD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VXDDIR)\$(@B).asm


$(OBJDIR)\vmmwrap.obj : $(VXDDIR)\vmmwrap.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

#
# audio os files
#

$(OBJDIR)\aosinit.obj : $(APUVXDDIR)\aosinit.c $(RMINCS) $(OSINCS) $(INCDDDIR)\files.h
        $(CCVXD) $(CFLAGSVXD) /I$(INCDDDIR) $(APUVXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\aosapi.obj : $(APUVXDDIR)\aosapi.c $(RMINCS) $(OSINCS) $(INCDDDIR)\files.h
        $(CCVXD) $(CFLAGSVXD) /I$(INCDDDIR) $(APUVXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\aos.obj : $(APUVXDDIR)\aos.cpp $(RMINCS) $(OSINCS) $(INCDDDIR)\files.h
        $(CCVXD) $(CFLAGSVXD) /I$(INCDDDIR) $(APUVXDDIR)\$(@B).cpp
        $(LST)

#
# Link dependencies.
#

##############################################################################
#                                                                           #
#                                VxD files.                                 #
#                                                                           #
#############################################################################
!IFDEF RETAIL

$(BINDIR)\nvacore.vxd : $(VXDDIR)\nvacore.def $(VXDOBJS)
        $(LINKVXD) /NOD /VXD /PDB:NONE @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nvacore.vxd
/MAP:$(DBGDIR)\nvacore.map
/DEF:$(VXDDIR)\nvacore.def
<<
		del $(BINDIR)\nvacore.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvacore.res $(VXDDIR)\nvcore.rc
        $(ADDRC) $(BINDIR)\nvacore.vxd $(OBJDIR)\nvacore.res

!ELSE

$(BINDIR)\nvacore.vxd : $(VXDDIR)\nvacore.def $(VXDOBJS)
        $(LINKVXD) /VXD /NOD /DEBUG  /MAP /DEBUGTYPE:CV /PDB:$(BINDIR)\nvacore.pdb /PDBTYPE:CON @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nvacore.vxd
/DEF:$(VXDDIR)\nvacore.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) \
	   /SOURCE:..\$(VXDDIR);	\
	   /SOURCE:..\$(INCVXDDIR);	\
       /SOURCE:..\$(NVARCSRCDIR) \
       /SOURCE:..\$(NVARCINCDIR) \
       /SOURCE:..\$(APUVXDDIR)	\
       /SOURCE:..\$(KERNAPUDIR) \
       /SOURCE:..\$(DSPPROGDIR) \
	   /SOURCE:..\$(AUDIOFXDIR)	\
       /SOURCE:..\$(INCDIR)		\
       /OUTPUT:nvacore.nms ..\$(BINDIR)\nvacore.vxd
        cd ..
	    del $(BINDIR)\nvacore.pdb
		del $(BINDIR)\nvacore.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvacore.res $(VXDDIR)\nvcore.rc
        $(ADDRC) $(BINDIR)\nvacore.vxd $(OBJDIR)\nvacore.res

!ENDIF

