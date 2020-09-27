# Load the common kernel targets

!INCLUDE $(INCDIR)\kernel.mak
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

VXDOBJS = $(OBJDIR)\vxdstub.obj $(OBJDIR)\nvwatch.obj   $(OBJDIR)\nv10wtch.obj \
        $(OBJDIR)\nv04wtch.obj \
        $(OBJDIR)\osdbg.obj $(OBJDIR)\serial.obj \
        $(OBJDIR)\nv_diag.obj   $(OBJDIR)\nvdsptch.obj \
        $(OBJDIR)\osapi.obj     $(OBJDIR)\os.obj        $(OBJDIR)\osinit.obj \
        $(OBJDIR)\vmmwrap.obj   $(OBJDIR)\cpuid.obj     $(OBJDIR)\nvagp.obj \
        $(OBJDIR)\nvapi.obj     $(OBJDIR)\client.obj    $(OBJDIR)\dload.obj \
		$(OBJDIR)\dload_inflate.obj						$(OBJDIR)\nvdebug.obj \
        $(KERNLIB)              $(KERNLIB_NV4)          $(KERNLIB_NV10) \
        $(KERNLIB_NV20)

#
# VDD files.
#

$(OBJDIR)\nvmini.obj : $(VDDDIR)\nvmini.asm $(VDDINCS)
        set ML= $(AFLAGSVDD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VDDDIR)\$(@B).asm

$(OBJDIR)\nvmini2.obj : $(VDDDIR)\nvmini2.asm $(VDDINCS)
        set ML= $(AFLAGSVDD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VDDDIR)\$(@B).asm

$(OBJDIR)\texsurf.obj : $(VDDDIR)\common\texsurf.c $(VDDINCS) $(RMINCS) $(OSINCS) $(INCDXDDIR)
        $(CCVXD) $(CFLAGSVXD) /I$(INC32DIR) /I$(INCDXDDIR) /I$(INCVPE) /I$(INCDDDIR) /I$(INCMOCOMPDIR) $(VDDDIR)\common\$(@B).c
        $(LST)


#
# OS files.
#
$(OBJDIR)\vxdstub.obj : $(VXDDIR)\vxdstub.asm $(INCVXDDIR)\debug.inc $(INCVXDDIR)\vmm.inc $(INCNV)\vnvrmd.inc
        set ML= $(AFLAGSVXD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VXDDIR)\$(@B).asm

$(OBJDIR)\nv_diag.obj : $(VXDDIR)\nv_diag.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\nvwatch.obj : $(VXDDIR)\nvwatch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\nv04wtch.obj : $(VXDDIR)\NV4\nv04wtch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\NV4\$(@B).c
        $(LST)

$(OBJDIR)\nv10wtch.obj : $(VXDDIR)\NV10\nv10wtch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\NV10\$(@B).c
        $(LST)

$(OBJDIR)\nvdsptch.obj : $(VXDDIR)\nvdsptch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\osapi.obj : $(VXDDIR)\osapi.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) /I$(INCDIR) /I$(INCDDDIR) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\os.obj : $(VXDDIR)\os.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\osdbg.obj : $(VXDDIR)\osdbg.c $(RMINCS)  $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\serial.obj : $(VXDDIR)\serial.c $(RMINCS)  $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\osinit.obj : $(VXDDIR)\osinit.c $(RMINCS) $(OSINCS) $(INCDDDIR)\files.h
        $(CCVXD) $(CFLAGSVXD) /I$(INCDDDIR) $(VXDDIR)\$(@B).c
        $(LST)

$(OBJDIR)\vmmwrap.obj : $(VXDDIR)\vmmwrap.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(@B).c
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
#====================================================================
# build primary mini-VDD
$(BINDIR)\nvmini.vxd : $(VDDDIR)\nvmini.def $(VDDOBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb $(LIB32DIR)\libc.lib
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VDDOBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb $(LIB32DIR)\libc.lib
/OUT:$(BINDIR)\nvmini.vxd
/MAP:$(DBGDIR)\nvmini.map
/DEF:$(VDDDIR)\nvmini.def
<<
		del $(BINDIR)\nvmini.exp
		del $(BINDIR)\nvmini.lib
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /i$(INCNV) /r /fo $(OBJDIR)\nvmini.res $(VDDDIR)\nvmini.rc
        $(ADDRC) $(BINDIR)\nvmini.vxd $(OBJDIR)\nvmini.res

# build secondary mini-VDD
$(BINDIR)\nvmini2.vxd : $(VDDDIR)\nvmini2.def $(VDD2OBJS)
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VDD2OBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb $(LIB32DIR)\libc.lib
/OUT:$(BINDIR)\nvmini2.vxd
/MAP:$(DBGDIR)\nvmini2.map
/DEF:$(VDDDIR)\nvmini2.def
<<
		del $(BINDIR)\nvmini2.exp
		del $(BINDIR)\nvmini2.lib
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /i$(INCNV) /r /fo $(OBJDIR)\nvmini2.res $(VDDDIR)\nvmini2.rc
        $(ADDRC) $(BINDIR)\nvmini2.vxd $(OBJDIR)\nvmini2.res

$(BINDIR)\nvcore.vxd : $(VXDDIR)\nvcore.def $(VXDOBJS)
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nvcore.vxd
/MAP:$(DBGDIR)\nvcore.map
/DEF:$(VXDDIR)\nvcore.def
<<
		del $(BINDIR)\nvcore.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvcore.res $(VXDDIR)\nvcore.rc
        $(ADDRC) $(BINDIR)\nvcore.vxd $(OBJDIR)\nvcore.res

!ELSE
#====================================================================
#	BUILD DEBUG VERSION
# build the primary mini-VDD
$(BINDIR)\nvmini.vxd : $(VDDDIR)\nvmini.def $(VDDOBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb  $(LIB32DIR)\libc.lib
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV /PDB:$(BINDIR)\nvmini.pdb /PDBTYPE:CON @<<
$(VDDOBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb $(LIB32DIR)\libc.lib
/OUT:$(BINDIR)\nvmini.vxd
/MAP:$(DBGDIR)\nvmini.map
/DEF:$(VDDDIR)\nvmini.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) /SOURCE:..\$(VDDDIR)\common;..\$(VDDDIR)\nv4;..\$(VDDDIR);..\$(DRVCMNDIR)\VPE\src;..\$(DRVCMNDIR)\VPE\src\core	\
       /OUTPUT:nvmini.nms ..\$(BINDIR)\nvmini.vxd
       cd ..
       del $(BINDIR)\nvmini.pdb
       del $(BINDIR)\nvmini.exp
       del $(BINDIR)\nvmini.lib
       $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvmini.res $(VDDDIR)\nvmini.rc
       $(ADDRC) $(BINDIR)\nvmini.vxd $(OBJDIR)\nvmini.res

# build the secondary mini-VDD
$(BINDIR)\nvmini2.vxd : $(VDDDIR)\nvmini2.def $(VDD2OBJS)
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV /PDB:$(BINDIR)\nvmini2.pdb /PDBTYPE:CON @<<
$(VDD2OBJS) $(VDDCOBJS) $(OBJDIR)\texsurf.obj $(BINDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb $(LIB32DIR)\libc.lib
/OUT:$(BINDIR)\nvmini2.vxd
/MAP:$(DBGDIR)\nvmini2.map
/DEF:$(VDDDIR)\nvmini2.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) /SOURCE:..\$(VDDDIR);..\$(VDDDIR)	\
       /OUTPUT:nvmini2.nms ..\$(BINDIR)\nvmini2.vxd
       cd ..
       del $(BINDIR)\nvmini2.pdb
       del $(BINDIR)\nvmini2.exp
       del $(BINDIR)\nvmini2.lib
       $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvmini2.res $(VDDDIR)\nvmini2.rc
       $(ADDRC) $(BINDIR)\nvmini2.vxd $(OBJDIR)\nvmini2.res
 
# build the resource manager
$(BINDIR)\nvcore.vxd : $(VXDDIR)\nvcore.def $(VXDOBJS)
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV /PDB:$(BINDIR)\nvcore.pdb /PDBTYPE:CON @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nvcore.vxd
/MAP:$(DBGDIR)\nvcore.map
/DEF:$(VXDDIR)\nvcore.def
<<
        cd $(DBGDIR)
        ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) \
            $(NMSFILELIST) \
            /OUTPUT:nvcore.nms ..\$(BINDIR)\nvcore.vxd
        cd ..
	    del $(BINDIR)\nvcore.pdb
        del $(BINDIR)\nvcore.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvcore.res $(VXDDIR)\nvcore.rc
        $(ADDRC) $(BINDIR)\nvcore.vxd $(OBJDIR)\nvcore.res

!ENDIF

#############################################################################
#                                                                           #
#                                DLL16 files.                               #
#                                                                           #
#############################################################################

#
# 16 bit DLL system API for Resource Manager.
#

!IFDEF RETAIL
# RETAIL Version

$(BINDIR)\nvarch16.dll :  $(OBJDIR)\nvsys.obj $(OBJDIR)\nvarch16.obj $(OBJDIR)\nvrm16.obj $(OBJDIR)\thunk16.obj $(OBJDIR)\nvrm16.res
    set LIB=$(LIB16DIR)
	$(LINK16) /NOD $(DLL16OBJS), $(BINDIR)\nvarch16.dll, $(DBGDIR)\nvarch16.map, oldnames libw sdllcew, $(DLL16DIR)\nvrm16.def;
	$(RC95)	-40 $(OBJDIR)\nvrm16.res $(BINDIR)\nvarch16.dll 
	$(OLDTOOLDIR)\cc16\implib $(LIB16DIR)\nvarch.lib $(BINDIR)\nvarch16.dll

!ELSE
# DEBUG or DEVELOPER Version

$(BINDIR)\nvarch16.dll :  $(OBJDIR)\nvsys.obj $(OBJDIR)\nvarch16.obj $(OBJDIR)\nvrm16.obj $(OBJDIR)\thunk16.obj $(OBJDIR)\nvrm16.res
    set LIB=$(LIB16DIR)
	$(LINK16) /NOD /CODEVIEW /LINENUMBERS /MAP $(DLL16OBJS), $(BINDIR)\nvarch16.dll, $(DBGDIR)\nvarch16.map, oldnames libw sdllcew, $(DLL16DIR)\nvrm16.def;
	$(RC95)	-40 $(OBJDIR)\nvrm16.res $(BINDIR)\nvarch16.dll 
	$(OLDTOOLDIR)\cc16\implib $(LIB16DIR)\nvarch16.lib $(BINDIR)\nvarch16.dll
    cd $(DBGDIR)
    ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) /SOURCE:..\$(DLL16DIR); /SOURCE:..\$(OBJDIR) \
    /OUTPUT:nvarch16.nms ..\$(BINDIR)\nvarch16.dll
    cd ..

!ENDIF

#############################################################################
#                                                                           #
#                                DLL32 files.                               #
#                                                                           #
#############################################################################

#
# 32 bit DLL client API to Resource Manager.
#

#
# Link dependencies.
#
!IFDEF RETAIL
# RETAIL version

$(BINDIR)\nvarch32.dll : $(DLL32DIR)\nvrm32.def $(OBJDIR)\nvapi32.obj $(OBJDIR)\nvarch32.obj $(OBJDIR)\thunk32.obj $(OBJDIR)\nvrm32.res
        set LIB=$(LIB32DIR)
        $(LINK32) $(LINKCPPDBG) @$(DLL32DIR)\nvrm32.lnk
        set LIB=

!ELSE
# DEBUG Version

$(BINDIR)\nvarch32.dll : $(DLL32DIR)\nvrm32.def $(OBJDIR)\nvapi32.obj $(OBJDIR)\nvarch32.obj $(OBJDIR)\thunk32.obj $(OBJDIR)\nvrm32.res
        set LIB=$(LIB32DIR)
        $(LINK32) $(LINKCPPDBG) @$(DLL32DIR)\nvrm32.lnk
        set LIB=
        cd $(DBGDIR)
        ..\$(NMSYM) /TRANSLATE:SOURCE,$(SOURCE_PACKAGING) /SOURCE:..\$(DLL32DIR); /SOURCE:..\$(OBJDIR) \
        /OUTPUT:nvarch32.nms ..\$(BINDIR)\nvarch32.dll
        cd ..

!ENDIF
