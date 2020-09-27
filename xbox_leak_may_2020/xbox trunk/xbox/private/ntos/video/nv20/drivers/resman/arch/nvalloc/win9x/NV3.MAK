!MESSAGE Loading NV3 dependencies

MODEDIR   = $(WINDIR)\MODESET\NV3

RMINCS = $(INCDIR)\all_incs.h   $(INCDIR)\arb_fifo.h   $(INCDIR)\au.h 	   \
         $(INCDIR)\cart.h       $(INCDIR)\class.h      $(INCDIR)\control.h \
         $(INCDIR)\cursor.h     $(INCDIR)\dac.h        $(INCDIR)\dma.h	   \
         $(INCDIR)\dmapush.h    $(INCDIR)\event.h      $(INCDIR)\fb.h	   \
         $(INCDIR)\fifo.h       $(INCDIR)\gport.h      $(INCDIR)\gr.h	   \
         $(INCDIR)\mc.h         $(INCDIR)\modular.h    $(INCDIR)\mp.h	   \
         $(INCDIR)\nv3_ref.h    $(INCDIR)\nv_ref.h     $(INCDIR)\nvrm.h	   \
         $(INCDIR)\btree.h      $(INCDIR)\rmode.h      \
         $(INCDIR)\sgs_dac.h    $(INCDIR)\state.h      $(INCDIR)\tmr.h	   \
         $(INCDIR)\tv.h         $(INCDIR)\vblank.h     $(INCDIR)\vga.h	   \
         $(INCDIR)\heap.h       $(INCNV)\os.h          $(INCDIR)\heap.h    \
         $(INCDIR)\btree.h      $(INCNV)\nvhw.h	       $(NVSDKDIR)\inc\nvos.h

VDDOBJS = $(OBJDIR)\nv3.obj 
VDD2OBJS = $(OBJDIR)\nv3mini2.obj		

#
# 32 bit VxD Resource Manager.
#

VXDOBJS = $(OBJDIR)\vxdstub.obj $(OBJDIR)\nv3watch.obj \
	$(OBJDIR)\vid_mode.obj  $(OBJDIR)\nv_diag.obj   $(OBJDIR)\nvdsptch.obj \
	$(OBJDIR)\osapi.obj     $(OBJDIR)\os.obj        $(OBJDIR)\osinit.obj \
	$(OBJDIR)\classobj.obj  $(OBJDIR)\classdel.obj  $(OBJDIR)\class.obj \
	$(OBJDIR)\control.obj \
	$(OBJDIR)\dacinit.obj   $(OBJDIR)\dacstate.obj  $(OBJDIR)\dac.obj \
	$(OBJDIR)\notify.obj    $(OBJDIR)\dmastate.obj  $(OBJDIR)\dma.obj \
	$(OBJDIR)\dmapush.obj \
	$(OBJDIR)\event.obj \
	$(OBJDIR)\fbinit.obj    $(OBJDIR)\fbstate.obj   $(OBJDIR)\fb.obj        \
	$(OBJDIR)\fifoinit.obj  $(OBJDIR)\fifostat.obj  $(OBJDIR)\fifo.obj      \
	$(OBJDIR)\fifoobj.obj \
	$(OBJDIR)\mcinit.obj    $(OBJDIR)\mc.obj        $(OBJDIR)\mcstate.obj   \
	$(OBJDIR)\tmrstate.obj  $(OBJDIR)\time.obj      $(OBJDIR)\tmrinit.obj   \
	$(OBJDIR)\grinit.obj    $(OBJDIR)\grstate.obj   $(OBJDIR)\gr.obj        \
	$(OBJDIR)\grobj.obj     $(OBJDIR)\grval.obj     $(OBJDIR)\grmethd.obj   \
	$(OBJDIR)\grctxt.obj    $(OBJDIR)\mp.obj	$(OBJDIR)\mpinit.obj	\
	$(OBJDIR)\mpstate.obj \
	$(OBJDIR)\state.obj     $(OBJDIR)\btree.obj     $(OBJDIR)\vblank.obj    \
	$(OBJDIR)\modeset.obj	$(OBJDIR)\modehw.obj	$(OBJDIR)\vmmwrap.obj	\
	$(OBJDIR)\heap.obj      $(OBJDIR)\nvapi.obj     $(OBJDIR)\client.obj \
	$(OBJDIR)\class04D.obj  $(OBJDIR)\class04E.obj  $(OBJDIR)\class04F.obj \
	$(OBJDIR)\class050.obj	$(OBJDIR)\class051.obj \
	$(OBJDIR)\class63.obj   $(OBJDIR)\class64.obj   $(OBJDIR)\class65.obj \
	$(OBJDIR)\validate.obj \
	$(OBJDIR)\class54.obj   $(OBJDIR)\class55.obj \
	$(OBJDIR)\class56.obj   $(OBJDIR)\class57.obj   $(OBJDIR)\class61.obj \
	$(OBJDIR)\class62.obj   $(OBJDIR)\class66.obj   $(OBJDIR)\class67.obj   \
	$(OBJDIR)\class68.obj   $(OBJDIR)\class69.obj   $(OBJDIR)\class70.obj   \
	$(OBJDIR)\class71.obj   $(OBJDIR)\class72.obj   $(OBJDIR)\class73.obj   \
	$(OBJDIR)\class74.obj   $(OBJDIR)\class75.obj


#
# VDD files.
#

$(OBJDIR)\nv3.obj : $(VDDDIR)\$(CHIP)\nv3.asm $(VDDINCS)
        set ML= $(AFLAGSVDD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VDDDIR)\$(CHIP)\$(@B).asm

$(OBJDIR)\nv3mini2.obj : $(VDDDIR)\$(CHIP)\nv3mini2.asm $(VDDINCS)
        set ML= $(AFLAGSVDD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VDDDIR)\$(CHIP)\$(@B).asm

#
# OS files.
#

$(OBJDIR)\vxdstub.obj : $(VXDDIR)\$(CHIP)\vxdstub.asm $(INCVXDDIR)\debug.inc $(INCVXDDIR)\vmm.inc $(INCNV)\vnvrmd.inc
        set ML= $(AFLAGSVXD) /Fo$(OBJDIR)\$(@B).obj /Fl$(LSTDIR)\$(@B).lst
        $(ASMVXD) $(VXDDIR)\$(CHIP)\$(@B).asm

$(OBJDIR)\nv_diag.obj : $(VXDDIR)\$(CHIP)\nv_diag.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\nv3watch.obj : $(VXDDIR)\$(CHIP)\nv3watch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\nvdsptch.obj : $(VXDDIR)\$(CHIP)\nvdsptch.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\vid_mode.obj : $(VXDDIR)\$(CHIP)\vid_mode.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) /I$(MODEDIR) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\osapi.obj : $(VXDDIR)\$(CHIP)\osapi.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) /I$(INCDIR) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\os.obj : $(VXDDIR)\$(CHIP)\os.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) /I$(MODEDIR) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\osinit.obj : $(VXDDIR)\$(CHIP)\osinit.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\vmmwrap.obj : $(VXDDIR)\$(CHIP)\vmmwrap.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(VXDDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Video Modeset files.
#

$(OBJDIR)\modeset.obj : $(MODEDIR)\modeset.c $(MODEDIR)\modedata.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(MODEDIR)\$(@B).c
        $(LST)

$(OBJDIR)\modehw.obj : $(MODEDIR)\modehw.c $(RMINCS) $(OSINCS)
        $(CCVXD) $(CFLAGSVXD) $(MODEDIR)\$(@B).c
        $(LST)

#
# RM Kernel files.
#

#
# State files.
#

$(OBJDIR)\state.obj : $(STATEDIR)\$(CHIP)\state.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(STATEDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Object Oriented files.
#

$(OBJDIR)\classdel.obj : $(CLASSDIR)\$(CHIP)\classdel.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(CLASSDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\classobj.obj : $(CLASSDIR)\$(CHIP)\classobj.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(CLASSDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class.obj : $(CLASSDIR)\$(CHIP)\class.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(CLASSDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\control.obj : $(CLASSDIR)\$(CHIP)\control.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(CLASSDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# DMA Engine files.
#

$(OBJDIR)\dma.obj : $(DMADIR)\$(CHIP)\dma.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DMADIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\notify.obj : $(DMADIR)\$(CHIP)\notify.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DMADIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\dmastate.obj : $(DMADIR)\$(CHIP)\dmastate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DMADIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\event.obj : $(DMADIR)\$(CHIP)\event.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DMADIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\dmapush.obj : $(DMADIR)\$(CHIP)\dmapush.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DMADIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Framebuffer files.
#

$(OBJDIR)\fb.obj : $(FBDIR)\$(CHIP)\fb.c $(INCDIR)\nvrm.h $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FBDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\fbstate.obj : $(FBDIR)\$(CHIP)\fbstate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FBDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\fbinit.obj : $(FBDIR)\$(CHIP)\fbinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FBDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# FIFO Engine files.
#

$(OBJDIR)\fifo.obj : $(FIFODIR)\$(CHIP)\fifo.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FIFODIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\fifoobj.obj : $(FIFODIR)\$(CHIP)\fifoobj.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FIFODIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\fifostat.obj : $(FIFODIR)\$(CHIP)\fifostat.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FIFODIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\fifoinit.obj : $(FIFODIR)\$(CHIP)\fifoinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(FIFODIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Graphics Engine files.
#

$(OBJDIR)\gr.obj : $(GRDIR)\$(CHIP)\gr.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grobj.obj : $(GRDIR)\$(CHIP)\grobj.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grval.obj : $(GRDIR)\$(CHIP)\grval.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grmethd.obj : $(GRDIR)\$(CHIP)\grmethd.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grctxt.obj : $(GRDIR)\$(CHIP)\grctxt.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grstate.obj : $(GRDIR)\$(CHIP)\grstate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\grinit.obj : $(GRDIR)\$(CHIP)\grinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(GRDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Heap file
#
$(OBJDIR)\heap.obj: $(HEAPDIR)\$(CHIP)\heap.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(HEAPDIR)\$(CHIP)\$(@B).c
        $(LST)
       
#
# DAC files.
#

$(OBJDIR)\dac.obj : $(DACDIR)\$(CHIP)\dac.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DACDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\dacstate.obj : $(DACDIR)\$(CHIP)\dacstate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DACDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\dacinit.obj : $(DACDIR)\$(CHIP)\dacinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(DACDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Master Control files.
#

$(OBJDIR)\mc.obj : $(MCDIR)\$(CHIP)\mc.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(MCDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\mcstate.obj : $(MCDIR)\$(CHIP)\mcstate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(MCDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\mcinit.obj : $(MCDIR)\$(CHIP)\mcinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(MCDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# Timer Device files.
#

$(OBJDIR)\time.obj : $(TMRDIR)\$(CHIP)\time.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(TMRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\tmrstate.obj : $(TMRDIR)\$(CHIP)\tmrstate.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(TMRDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\tmrinit.obj : $(TMRDIR)\$(CHIP)\tmrinit.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(TMRDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# BTREE files.
#

$(OBJDIR)\btree.obj : $(BTREEDIR)\$(CHIP)\btree.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(BTREEDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# VBLANK files.
#

$(OBJDIR)\vblank.obj : $(VBLANKDIR)\$(CHIP)\vblank.c $(RMINCS)
        $(CCVXD) $(CFLAGSVXD) $(VBLANKDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# MODULAR Class files.
#

$(OBJDIR)\class04D.obj : $(MODDIR)\$(CHIP)\class04D.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class04E.obj : $(MODDIR)\$(CHIP)\class04E.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class04F.obj : $(MODDIR)\$(CHIP)\class04F.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class050.obj : $(MODDIR)\$(CHIP)\class050.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class051.obj : $(MODDIR)\$(CHIP)\class051.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class63.obj : $(MODDIR)\$(CHIP)\class63.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class64.obj : $(MODDIR)\$(CHIP)\class64.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class65.obj : $(MODDIR)\$(CHIP)\class65.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\validate.obj : $(MODDIR)\$(CHIP)\validate.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class54.obj : $(MODDIR)\$(CHIP)\class54.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class55.obj : $(MODDIR)\$(CHIP)\class55.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class56.obj : $(MODDIR)\$(CHIP)\class56.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class57.obj : $(MODDIR)\$(CHIP)\class57.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class61.obj : $(MODDIR)\$(CHIP)\class61.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class62.obj : $(MODDIR)\$(CHIP)\class62.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class66.obj : $(MODDIR)\$(CHIP)\class66.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class67.obj : $(MODDIR)\$(CHIP)\class67.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class68.obj : $(MODDIR)\$(CHIP)\class68.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class69.obj : $(MODDIR)\$(CHIP)\class69.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class70.obj : $(MODDIR)\$(CHIP)\class70.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class71.obj : $(MODDIR)\$(CHIP)\class71.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class72.obj : $(MODDIR)\$(CHIP)\class72.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class73.obj : $(MODDIR)\$(CHIP)\class73.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class74.obj : $(MODDIR)\$(CHIP)\class74.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\class75.obj : $(MODDIR)\$(CHIP)\class75.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MODDIR)\$(CHIP)\$(@B).c
        $(LST)

#
# MEDIA PORT files.
#
$(OBJDIR)\mp.obj : $(MPDIR)\$(CHIP)\mp.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MPDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\mpinit.obj : $(MPDIR)\$(CHIP)\mpinit.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MPDIR)\$(CHIP)\$(@B).c
        $(LST)

$(OBJDIR)\mpstate.obj : $(MPDIR)\$(CHIP)\mpstate.c $(RM_INCS)
        $(CCVXD) $(CFLAGSVXD) $(MPDIR)\$(CHIP)\$(@B).c
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
$(BINDIR)\nv3.vxd : $(VDDDIR)\$(CHIP)\nv3.def $(VDDOBJS) $(VDDCOBJS) $(SDKLIBDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VDDOBJS) $(VDDCOBJS) $(SDKLIBDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb
/OUT:$(BINDIR)\nv3.vxd
/MAP:$(DBGDIR)\nv3.map
/DEF:$(VDDDIR)\$(CHIP)\nv3.def
<<
		del $(BINDIR)\nv3.exp
		del $(BINDIR)\nv3.lib
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /i$(INCNV) /r /fo $(OBJDIR)\nvvdd.res $(VDDDIR)\$(CHIP)\nvvdd.rc
        $(ADDRC) $(BINDIR)\nv3.vxd $(OBJDIR)\nvvdd.res

# build secondary mini-VDD
$(BINDIR)\nv3mini2.vxd : $(VDDDIR)\$(CHIP)\nv3mini2.def $(VDD2OBJS) 
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VDD2OBJS)
/OUT:$(BINDIR)\nv3mini2.vxd
/MAP:$(DBGDIR)\nv3mini2.map
/DEF:$(VDDDIR)\$(CHIP)\nv3mini2.def
<<
		del $(BINDIR)\nv3mini2.exp
		del $(BINDIR)\nv3mini2.lib
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /i$(INCNV) /r /fo $(OBJDIR)\nvvdd2.res $(VDDDIR)\$(CHIP)\nvvdd2.rc
        $(ADDRC) $(BINDIR)\nv3mini2.vxd $(OBJDIR)\nvvdd2.res

$(BINDIR)\nv3rm.vxd : $(VXDDIR)\$(CHIP)\nv3rm.def $(VXDOBJS)
        $(LINKVXD) /VXD /NOD /PDB:NONE @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nv3rm.vxd
/MAP:$(DBGDIR)\nv3rm.map
/DEF:$(VXDDIR)\$(CHIP)\nv3rm.def
<<
		del $(BINDIR)\nv3rm.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvrm.res $(VXDDIR)\$(CHIP)\nvrm.rc
        $(ADDRC) $(BINDIR)\nv3rm.vxd $(OBJDIR)\nvrm.res

!ELSE
#====================================================================
#	BUILD DEBUG VERSION
# build the primary mini-VDD
$(BINDIR)\nv3.vxd : $(VDDDIR)\$(CHIP)\nv3.def $(VDDOBJS) $(VDDCOBJS) $(SDKLIBDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV @<<
$(VDDOBJS) $(VDDCOBJS) $(SDKLIBDIR)\nvrmr0api.lib $(LIB32DIR)\vxdwraps.clb
/OUT:$(BINDIR)\nv3.vxd
/MAP:$(DBGDIR)\nv3.map
/DEF:$(VDDDIR)\$(CHIP)\nv3.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,PACKAGE /SOURCE:..\$(VDDDIR)\common;..\$(VDDDIR)\$(CHIP) \
       /OUTPUT:nv3.nms ..\$(BINDIR)\nv3.vxd
       cd ..
	   del $(BINDIR)\nv3.pdb
       del $(BINDIR)\nv3.exp
       del $(BINDIR)\nv3.lib
       $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvvdd.res $(VDDDIR)\$(CHIP)\nvvdd.rc
       $(ADDRC) $(BINDIR)\nv3.vxd $(OBJDIR)\nvvdd.res
       
# build the secondary mini-VDD
$(BINDIR)\nv3mini2.vxd : $(VDDDIR)\$(CHIP)\nv3mini2.def $(VDD2OBJS) 
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV @<<
$(VDD2OBJS)
/OUT:$(BINDIR)\nv3mini2.vxd
/MAP:$(DBGDIR)\nv3mini2.map
/DEF:$(VDDDIR)\$(CHIP)\nv3mini2.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,PACKAGE /SOURCE:..\$(VDDDIR);..\$(VDDDIR)\$(CHIP) \
       /OUTPUT:nv3mini2.nms ..\$(BINDIR)\nv3mini2.vxd
       cd ..
	   del $(BINDIR)\nv3mini2.pdb
       del $(BINDIR)\nv3mini2.exp
       del $(BINDIR)\nv3mini2.lib
       $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvvdd2.res $(VDDDIR)\$(CHIP)\nvvdd2.rc
       $(ADDRC) $(BINDIR)\nv3mini2.vxd $(OBJDIR)\nvvdd2.res
 
# build the resource manager
$(BINDIR)\nv3rm.vxd : $(VXDDIR)\$(CHIP)\nv3rm.def $(VXDOBJS)
        $(LINKVXD) /VXD /NOD /DEBUG /DEBUGTYPE:CV @<<
$(VXDOBJS)
/OUT:$(BINDIR)\nv3rm.vxd
/MAP:$(DBGDIR)\nv3rm.map
/DEF:$(VXDDIR)\$(CHIP)\nv3rm.def
<<
       cd $(DBGDIR)
       ..\$(NMSYM) /TRANSLATE:SOURCE,PACKAGE /SOURCE:..\$(VXDDIR)\$(CHIP);..\$(MODEDIR);..\$(FBDIR)\$(CHIP);..\$(FIFODIR)\$(CHIP);..\$(DMADIR)\$(CHIP);..\$(CLASSDIR)\$(CHIP);..\$(BTREEDIR)\$(CHIP)\; \
       /SOURCE:..\$(GRDIR)\$(CHIP);..\$(STATEDIR)\$(CHIP);..\$(MCDIR)\$(CHIP);..\$(MODDIR)\$(CHIP);..\$(MPDIR)\$(CHIP);..\$(DACDIR)\$(CHIP);..\$(VBLANKDIR)\$(CHIP);..\$(NVOBJDIR); \
       /SOURCE:..\$(NVARCSRCDIR);..\$(TMRDIR)\$(CHIP);..\$(HEAPDIR)\$(CHIP);\
       /OUTPUT:nv3rm.nms ..\$(BINDIR)\nv3rm.vxd
        cd ..
	    del $(BINDIR)\nv3rm.pdb
		del $(BINDIR)\nv3rm.exp
        $(RC95) /i$(INC32DIR) /i$(INCVXDDIR) /i$(INCVERDIR) /r /fo $(OBJDIR)\nvrm.res $(VXDDIR)\$(CHIP)\nvrm.rc
        $(ADDRC) $(BINDIR)\nv3rm.vxd $(OBJDIR)\nvrm.res
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

$(BINDIR)\nv3sys.dll :  $(OBJDIR)\nvsys.obj $(OBJDIR)\nvarch16.obj $(OBJDIR)\nvrm16.obj $(OBJDIR)\thunk16.obj $(OBJDIR)\nvrm16.res
    set LIB=$(LIB16DIR)
	$(LINK16) /NOD $(DLL16OBJS), $(BINDIR)\nv3sys.dll, $(DBGDIR)\nv3sys.map, oldnames libw sdllcew, $(DLL16DIR)\$(CHIP)\nvrm16.def;
	$(RC95)	-40 $(OBJDIR)\nvrm16.res $(BINDIR)\nv3sys.dll 
	$(TOOLDIR)\cc16\implib $(LIB16DIR)\nvsys.lib $(BINDIR)\nv3sys.dll

!ELSE
# DEBUG or DEVELOPER Version

$(BINDIR)\nv3sys.dll :  $(OBJDIR)\nvsys.obj $(OBJDIR)\nvarch16.obj $(OBJDIR)\nvrm16.obj $(OBJDIR)\thunk16.obj $(OBJDIR)\nvrm16.res
    set LIB=$(LIB16DIR)
	$(LINK16) /NOD /CODEVIEW /LINENUMBERS /MAP $(DLL16OBJS), $(BINDIR)\nv3sys.dll, $(DBGDIR)\nv3sys.map, oldnames libw sdllcew, $(DLL16DIR)\$(CHIP)\nvrm16.def;
	$(RC95)	-40 $(OBJDIR)\nvrm16.res $(BINDIR)\nv3sys.dll 
	$(TOOLDIR)\cc16\implib $(LIB16DIR)\nvsys.lib $(BINDIR)\nv3sys.dll
    cd $(DBGDIR)
    ..\$(NMSYM) /TRANSLATE:SOURCE,PACKAGE /SOURCE:..\$(DLL16DIR)\$(CHIP) /SOURCE:..\$(OBJDIR) \
    /OUTPUT:nv3sys.nms ..\$(BINDIR)\nv3sys.dll
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

$(BINDIR)\nv3api.dll : $(DLL32DIR)\$(CHIP)\nvrm32.def $(OBJDIR)\nvapi32.obj $(OBJDIR)\nvarch32.obj $(OBJDIR)\thunk32.obj $(OBJDIR)\nvrm32.res
        set LIB=$(LIB32DIR)
        $(LINK32) $(LINKCPPDBG) @$(DLL32DIR)\$(CHIP)\nvrm32.lnk
        set LIB=

!ELSE
# DEBUG Version

$(BINDIR)\nv3api.dll : $(DLL32DIR)\$(CHIP)\nvrm32.def $(OBJDIR)\nvapi32.obj $(OBJDIR)\nvarch32.obj $(OBJDIR)\thunk32.obj $(OBJDIR)\nvrm32.res
        set LIB=$(LIB32DIR)
        $(LINK32) $(LINKCPPDBG) @$(DLL32DIR)\$(CHIP)\nvrm32.lnk
        set LIB=
        cd $(DBGDIR)
        ..\$(NMSYM) /TRANSLATE:SOURCE,PACKAGE /SOURCE:..\$(DLL32DIR)\$(CHIP) /SOURCE:..\$(OBJDIR) \
        /OUTPUT:nv3api.nms ..\$(BINDIR)\nv3api.dll
        cd ..

!ENDIF
