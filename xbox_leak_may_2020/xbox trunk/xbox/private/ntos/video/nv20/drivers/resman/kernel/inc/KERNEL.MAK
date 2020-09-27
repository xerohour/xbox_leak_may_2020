!MESSAGE Loading Kernel dependencies

RMINCS = $(INCDIR)\nvrm.h        $(INCDIR)\nv4\nv4_ref.h $(INCDIR)\nv10\nv10_ref.h  \
         $(INCDIR)\nv_ref.h      $(INCDIR)\state.h       $(INCDIR)\class.h       \
         $(INCDIR)\dma.h         $(INCDIR)\fifo.h        $(INCDIR)\fb.h          \
         $(INCDIR)\dac.h         $(INCNV)\os.h           $(INCDIR)\tmr.h         \
         $(INCDIR)\vga.h         $(INCDIR)\arb_fifo.h    $(INCDIR)\cursor.h 	 \
         $(INCDIR)\dmapush.h     $(INCDIR)\event.h       $(INCDIR)\fifo.h	 \
         $(INCDIR)\mc.h          $(INCDIR)\modular.h     $(INCDIR)\mp.h 	 \
         $(INCDIR)\smp.h         $(INCDIR)\control.h     \
         $(INCDIR)\tv.h          $(INCDIR)\vblank.h      $(INCDIR)\all_incs.h	 \
         $(INCDIR)\heap.h        $(INCDIR)\btree.h 	 	 $(INCNV)\nvhw.h         \
         $(INCDIR)\i2c.h         $(NVSDKDIR)\inc\nvos.h  \
         $(INCDIR)\gr.h		     $(INCDIR)\nv4\nv4_gr.h  $(INCDIR)\nv10\nv10_gr.h \
         $(INCDIR)\edid.h        $(VESADIR)\vesadata.h	 \
         $(INCDIR)\devinit.h     $(INCDIR)\nvromdat.h	 $(INCDIR)\nvhal.h \
         $(INCDIR)\nv4\nv4_hal.h $(INCDIR)\nv10\nv10_hal.h $(INCDIR)\vid.h \
         $(INCDIR)\mvision.h     $(INCDIR)\nv10\cr11_ref.h $(INCDIR)\nv10\cr11help.h

#
# SOURCE lines for DEBUG Kernel nmsym command.
#
# The DFLTKERNSRCDIRS var should keep the resultant
# .nms at or around 4M to avoid problems with loading
# at boot in softice.  The other vars (KERN+NV*KERN)
# can be used in combination to get a more complete set
# of files for a particular chip.
#
DFLTKERNSRCDIRS =   \
..\$(DACDIR)\nv;\
/SOURCE:..\$(DEVINITDIR)\nv;\
/SOURCE:..\$(FBDIR)\nv;\
/SOURCE:..\$(FIFODIR)\nv;\
/SOURCE:..\$(GRDIR)\nv;\
/SOURCE:..\$(HEAPDIR)\nv;\
/SOURCE:..\$(MCDIR)\nv;\
/SOURCE:..\$(MODDIR)\nv;\
/SOURCE:..\$(VBLANKDIR)\nv;\
/SOURCE:..\$(DACDIR)\nv4;\
/SOURCE:..\$(GRDIR)\nv4;\
/SOURCE:..\$(MCDIR)\nv4;\
/SOURCE:..\$(DACDIR)\nv10;\
/SOURCE:..\$(GRDIR)\nv10;\
/SOURCE:..\$(MCDIR)\nv10;\
/SOURCE:..\$(DACDIR)\nv20;\
/SOURCE:..\$(GRDIR)\nv20;\
/SOURCE:..\$(MCDIR)\nv20

KERNSRCDIRS  =      \
..\$(CLASSDIR)\nv;\
/SOURCE:..\$(DACDIR)\nv;\
/SOURCE:..\$(DEVINITDIR)\nv;\
/SOURCE:..\$(DMADIR)\nv;\
/SOURCE:..\$(FBDIR)\nv\
/SOURCE:..\$(FIFODIR)\nv\
/SOURCE:..\$(GRDIR)\nv;\
/SOURCE:..\$(HALDIR)\nv;\
/SOURCE:..\$(HEAPDIR)\nv;\
/SOURCE:..\$(MCDIR)\nv;\
/SOURCE:..\$(MODDIR)\nv;\
/SOURCE:..\$(MPDIR)\nv\
/SOURCE:..\$(STATEDIR)\nv;\
/SOURCE:..\$(TMRDIR)\nv;\
/SOURCE:..\$(VBLANKDIR)\nv\
/SOURCE:..\$(VIDEODIR)\nv

NV4KERNSRCDIRS  =   \
..\$(DACDIR)\nv4;\
/SOURCE:..\$(DMADIR)\nv4;\
/SOURCE:..\$(FBDIR)\nv4;\
/SOURCE:..\$(FIFODIR)\nv4;\
/SOURCE:..\$(GRDIR)\nv4;\
/SOURCE:..\$(HALDIR)\nv4;\
/SOURCE:..\$(MCDIR)\nv4;\
/SOURCE:..\$(MODDIR)\nv4\
/SOURCE:..\$(MPDIR)\nv4\
/SOURCE:..\$(VIDEODIR)\nv4

NV10KERNSRCDIRS =   \
..\$(DACDIR)\nv10;\
/SOURCE:..\$(DMADIR)\nv10;\
/SOURCE:..\$(FBDIR)\nv10;\
/SOURCE:..\$(FIFODIR)\nv10;\
/SOURCE:..\$(GRDIR)\nv10;\
/SOURCE:..\$(HALDIR)\nv10;\
/SOURCE:..\$(MCDIR)\nv10;\
/SOURCE:..\$(MODDIR)\nv10;\
/SOURCE:..\$(MPDIR)\nv10;\
/SOURCE:..\$(VIDEODIR)\nv10

NV20KERNSRCDIRS =   \
..\$(DACDIR)\nv20;\
/SOURCE:..\$(DMADIR)\nv20;\
/SOURCE:..\$(FBDIR)\nv20;\
/SOURCE:..\$(FIFODIR)\nv20;\
/SOURCE:..\$(GRDIR)\nv20;\
/SOURCE:..\$(HALDIR)\nv20;\
/SOURCE:..\$(MCDIR)\nv20;\
/SOURCE:..\$(MODDIR)\nv20;\
/SOURCE:..\$(MPDIR)\nv20;\
/SOURCE:..\$(VIDEODIR)\nv20

#
# Kernel Chip-Independent Resource Manager Objects.
#
KERNLIB = $(BINDIR)\nvkern.lib
KERNOBJS =	\
	$(OBJDIR)\classobj.obj  $(OBJDIR)\classdel.obj  $(OBJDIR)\class.obj \
	$(OBJDIR)\control.obj   $(OBJDIR)\i2c.obj       $(OBJDIR)\dacedid.obj  \
	$(OBJDIR)\dacinit.obj   $(OBJDIR)\dacstate.obj  $(OBJDIR)\dac.obj \
    $(OBJDIR)\dacmode.obj   $(OBJDIR)\dactv.obj     $(OBJDIR)\dacfp.obj \
    $(OBJDIR)\vesadata.obj  $(OBJDIR)\nvde.obj                              \
	$(OBJDIR)\notify.obj    $(OBJDIR)\dmastate.obj  $(OBJDIR)\dma.obj \
	$(OBJDIR)\event.obj \
	$(OBJDIR)\fbinit.obj    $(OBJDIR)\fbstate.obj   $(OBJDIR)\fb.obj        \
	$(OBJDIR)\fifoinit.obj  $(OBJDIR)\fifostat.obj  $(OBJDIR)\fifo.obj      \
	$(OBJDIR)\fifoobj.obj	\
	$(OBJDIR)\mc.obj        $(OBJDIR)\mcstate.obj   $(OBJDIR)\mcpower.obj\
	$(OBJDIR)\tmrstate.obj  $(OBJDIR)\time.obj      $(OBJDIR)\tmrinit.obj   \
	$(OBJDIR)\grstate.obj   $(OBJDIR)\gr.obj        \
	$(OBJDIR)\grobj.obj     $(OBJDIR)\grval.obj     $(OBJDIR)\grmethd.obj   \
    $(OBJDIR)\haleng.obj    $(OBJDIR)\halinit.obj   $(OBJDIR)\halrm.obj  \
    $(OBJDIR)\halwrap.obj \
	$(OBJDIR)\mp.obj        $(OBJDIR)\mpobj.obj     $(OBJDIR)\mpstate.obj \
	$(OBJDIR)\state.obj     $(OBJDIR)\btree.obj     $(OBJDIR)\vblank.obj    \
	$(OBJDIR)\heap.obj \
    $(OBJDIR)\video.obj     $(OBJDIR)\videoobj.obj \
	$(OBJDIR)\class050.obj	$(OBJDIR)\class051.obj \
    $(OBJDIR)\class012.obj  $(OBJDIR)\class017.obj  $(OBJDIR)\class018.obj  \
    $(OBJDIR)\class019.obj  $(OBJDIR)\class01C.obj  $(OBJDIR)\class01D.obj  \
    $(OBJDIR)\class01E.obj  $(OBJDIR)\class01F.obj  $(OBJDIR)\class021.obj  \
    $(OBJDIR)\class030.obj  \
    $(OBJDIR)\class036.obj  $(OBJDIR)\class037.obj  $(OBJDIR)\class038.obj  \
    $(OBJDIR)\class039.obj  $(OBJDIR)\class042.obj  $(OBJDIR)\class043.obj  \
    $(OBJDIR)\class044.obj  $(OBJDIR)\class046.obj  $(OBJDIR)\class047.obj  \
    $(OBJDIR)\class048.obj  \
    $(OBJDIR)\class049.obj  $(OBJDIR)\class04A.obj  $(OBJDIR)\class04B.obj  \
    $(OBJDIR)\class04D.obj  $(OBJDIR)\class04E.obj  $(OBJDIR)\class04F.obj \
    $(OBJDIR)\class052.obj  \
    $(OBJDIR)\class053.obj  $(OBJDIR)\class054.obj  $(OBJDIR)\class055.obj  \
    $(OBJDIR)\class056.obj  \
    $(OBJDIR)\class057.obj  $(OBJDIR)\class058.obj  $(OBJDIR)\class059.obj  \
    $(OBJDIR)\class05A.obj  $(OBJDIR)\class05B.obj  $(OBJDIR)\class05C.obj  \
    $(OBJDIR)\class05D.obj  $(OBJDIR)\class05E.obj  $(OBJDIR)\class05F.obj  \
    $(OBJDIR)\class060.obj  $(OBJDIR)\class061.obj  $(OBJDIR)\class063.obj  \
    $(OBJDIR)\class064.obj  $(OBJDIR)\class065.obj  $(OBJDIR)\class066.obj  \
    $(OBJDIR)\class067.obj  \
    $(OBJDIR)\class072.obj  $(OBJDIR)\class076.obj  $(OBJDIR)\class077.obj	\
    $(OBJDIR)\class07A.obj  $(OBJDIR)\class07B.obj  $(OBJDIR)\class07C.obj  \
    $(OBJDIR)\class062.obj  $(OBJDIR)\class088.obj  $(OBJDIR)\class089.obj	\
    $(OBJDIR)\class08A.obj  \
    $(OBJDIR)\class093.obj  $(OBJDIR)\class094.obj  $(OBJDIR)\class095.obj	\
    $(OBJDIR)\class096.obj  $(OBJDIR)\class09F.obj  \
    $(OBJDIR)\class63.obj   $(OBJDIR)\class64.obj   $(OBJDIR)\class65.obj \
    $(OBJDIR)\classcmn.obj	\
    $(OBJDIR)\devinit.obj	$(OBJDIR)\nv04mem.obj	$(OBJDIR)\nv04mtbl.obj \
    $(OBJDIR)\nv05mem.obj	$(OBJDIR)\nv05mtbl.obj \
    $(OBJDIR)\nv10mem.obj   $(OBJDIR)\nv10mtbl.obj  $(OBJDIR)\nv15mem.obj \
    $(OBJDIR)\bios.obj\
    $(OBJDIR)\mvision.obj   

#
# Kernel NV4 Resource Manager Objects.
#
KERNLIB_NV4 = $(BINDIR)\nv04kern.lib
KERNOBJS_NV4 =	\
	$(OBJDIR_NV4)\fbnv04.obj      $(OBJDIR_NV4)\dacnv04.obj     $(OBJDIR_NV4)\fifonv04.obj \
    $(OBJDIR_NV4)\grnv04.obj      $(OBJDIR_NV4)\grcxnv04.obj    $(OBJDIR_NV4)\grmdnv04.obj \
    $(OBJDIR_NV4)\halnv04.obj 	  $(OBJDIR_NV4)\mcnv04.obj      $(OBJDIR_NV4)\c047nv04.obj \
    $(OBJDIR_NV4)\c04Dnv04.obj    $(OBJDIR_NV4)\c04Env04.obj 	$(OBJDIR_NV4)\c04Fnv04.obj \
    $(OBJDIR_NV4)\c63nv04.obj     $(OBJDIR_NV4)\c64nv04.obj     $(OBJDIR_NV4)\c65nv04.obj  \
	$(OBJDIR_NV4)\mpnv04.obj      $(OBJDIR_NV4)\dmanv04.obj     $(OBJDIR_NV4)\vidnv04.obj

#
# Kernel NV10 Resource Manager Objects.
#
KERNLIB_NV10 = $(BINDIR)\nv10kern.lib
KERNOBJS_NV10 =	\
	$(OBJDIR_NV10)\fbnv10.obj     $(OBJDIR_NV10)\dacnv10.obj    $(OBJDIR_NV10)\fifonv10.obj \
	$(OBJDIR_NV10)\grnv10.obj     $(OBJDIR_NV10)\grcxnv10.obj   $(OBJDIR_NV10)\grmdnv10.obj \
    $(OBJDIR_NV10)\halnv10.obj    $(OBJDIR_NV10)\mcnv10.obj     $(OBJDIR_NV10)\c047nv10.obj \
	$(OBJDIR_NV10)\c04Dnv10.obj   $(OBJDIR_NV10)\c04Env10.obj 	$(OBJDIR_NV10)\c04Fnv10.obj \
    $(OBJDIR_NV10)\c07Anv10.obj   $(OBJDIR_NV10)\c63nv10.obj    $(OBJDIR_NV10)\c64nv10.obj  \
    $(OBJDIR_NV10)\c65nv10.obj 	  $(OBJDIR_NV10)\mpnv10.obj     $(OBJDIR_NV10)\dmanv10.obj  \
    $(OBJDIR_NV10)\vidnv10.obj

#
# Kernel NV20 Resource Manager Objects.
#
KERNLIB_NV20 = $(BINDIR)\nv20kern.lib
KERNOBJS_NV20 =	\
    $(OBJDIR_NV20)\dacnv20.obj  $(OBJDIR_NV20)\dmanv20.obj  \
    $(OBJDIR_NV20)\fbnv20.obj   $(OBJDIR_NV20)\fifonv20.obj \
    $(OBJDIR_NV20)\grnv20.obj   $(OBJDIR_NV20)\grcxnv20.obj \
    $(OBJDIR_NV20)\grmdnv20.obj $(OBJDIR_NV20)\halnv20.obj  \
    $(OBJDIR_NV20)\mcnv20.obj   $(OBJDIR_NV20)\mpnv20.obj   \
    $(OBJDIR_NV20)\vidnv20.obj  \
    $(OBJDIR_NV20)\c04dnv20.obj $(OBJDIR_NV20)\c04env20.obj \
    $(OBJDIR_NV20)\c04fnv20.obj $(OBJDIR_NV20)\c047nv20.obj \
    $(OBJDIR_NV20)\c07anv20.obj $(OBJDIR_NV20)\c63nv20.obj  \
    $(OBJDIR_NV20)\c64nv20.obj  $(OBJDIR_NV20)\c65nv20.obj


#
# RM Kernel files.
#

#
# State files.
#

$(OBJDIR)\state.obj : $(STATEDIR)\nv\state.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(STATEDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(STATEDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# Object Oriented files.
#

$(OBJDIR)\classdel.obj : $(CLASSDIR)\nv\classdel.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(CLASSDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(CLASSDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\classobj.obj : $(CLASSDIR)\nv\classobj.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(CLASSDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(CLASSDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class.obj : $(CLASSDIR)\nv\class.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(CLASSDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(CLASSDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\control.obj : $(CLASSDIR)\nv\control.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(CLASSDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(CLASSDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\nvde.obj : $(CLASSDIR)\nv\nvde.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(CLASSDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(CLASSDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# DMA Engine files.
#

$(OBJDIR)\dma.obj : $(DMADIR)\nv\dma.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DMADIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\notify.obj : $(DMADIR)\nv\notify.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DMADIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dmastate.obj : $(DMADIR)\nv\dmastate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DMADIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\event.obj : $(DMADIR)\nv\event.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DMADIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\dmanv04.obj : $(DMADIR)\nv4\dmanv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(DMADIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\dmanv10.obj : $(DMADIR)\nv10\dmanv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(DMADIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\dmanv20.obj : $(DMADIR)\nv20\dmanv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(DMADIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DMADIR)\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# Framebuffer files.
#

$(OBJDIR)\fb.obj : $(FBDIR)\nv\fb.c $(INCDIR)\nvrm.h $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FBDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\fbstate.obj : $(FBDIR)\nv\fbstate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FBDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\fbinit.obj : $(FBDIR)\nv\fbinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FBDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\fbnv04.obj : $(FBDIR)\nv4\fbnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(FBDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv4\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\fbnv10.obj : $(FBDIR)\nv10\fbnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(FBDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv10\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\fbnv20.obj : $(FBDIR)\nv20\fbnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(FBDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(FBDIR)\nv20\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

#
# FIFO Engine files.
#

$(OBJDIR)\fifo.obj : $(FIFODIR)\nv\fifo.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FIFODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\fifoobj.obj : $(FIFODIR)\nv\fifoobj.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FIFODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\fifostat.obj : $(FIFODIR)\nv\fifostat.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FIFODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\fifoinit.obj : $(FIFODIR)\nv\fifoinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(FIFODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\fifonv04.obj : $(FIFODIR)\nv4\fifonv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(FIFODIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv4\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\fifonv10.obj : $(FIFODIR)\nv10\fifonv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(FIFODIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv10\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\fifonv20.obj : $(FIFODIR)\nv20\fifonv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(FIFODIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(FIFODIR)\nv20\$(@B).c
#	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

#
# Graphics Engine files.
#

$(OBJDIR)\gr.obj : $(GRDIR)\nv\gr.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(GRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\grobj.obj : $(GRDIR)\nv\grobj.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(GRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\grval.obj : $(GRDIR)\nv\grval.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(GRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\grmethd.obj : $(GRDIR)\nv\grmethd.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(GRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\grcxnv04.obj : $(GRDIR)\nv4\grcxnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(GRDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\grcxnv10.obj : $(GRDIR)\nv10\grcxnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(GRDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\grcxnv20.obj : $(GRDIR)\nv20\grcxnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(GRDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\grmdnv04.obj : $(GRDIR)\nv4\grmdnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(GRDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\grmdnv10.obj : $(GRDIR)\nv10\grmdnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(GRDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\grmdnv20.obj : $(GRDIR)\nv20\grmdnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(GRDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\grnv04.obj : $(GRDIR)\nv4\grnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(GRDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\grnv10.obj : $(GRDIR)\nv10\grnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(GRDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\grnv20.obj : $(GRDIR)\nv20\grnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(GRDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\grstate.obj : $(GRDIR)\nv\grstate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(GRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(GRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# Heap file
#
$(OBJDIR)\heap.obj: $(HEAPDIR)\nv\heap.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(HEAPDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD) -i=..\common\inc
	$(CCVXD) $(LGROUP) $(HEAPDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
#
# DAC files.
#

$(OBJDIR_NV4)\dacnv04.obj : $(DACDIR)\nv4\dacnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(DACDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\dacnv10.obj : $(DACDIR)\nv10\dacnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(DACDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF


$(OBJDIR_NV20)\dacnv20.obj : $(DACDIR)\nv20\dacnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(DACDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\dac.obj : $(DACDIR)\nv\dac.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dacstate.obj : $(DACDIR)\nv\dacstate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dacinit.obj : $(DACDIR)\nv\dacinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\i2c.obj : $(DACDIR)\nv\i2c.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dacedid.obj : $(DACDIR)\nv\dacedid.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mvision.obj : $(DACDIR)\nv\mvision.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dacmode.obj : $(DACDIR)\nv\dacmode.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dactv.obj : $(DACDIR)\nv\dactv.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\dacfp.obj : $(DACDIR)\nv\dacfp.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DACDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DACDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
#
# VESA files.
#

$(OBJDIR)\vesadata.obj : $(VESADIR)\vesadata.c $(VESADIR)\vesadata.h
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(VESADIR)\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(VESADIR)\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
#
# Master Control files.
#

$(OBJDIR)\mc.obj : $(MCDIR)\nv\mc.c $(RMINCS) $(INCVERDIR)\nvver.h
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MCDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mcstate.obj : $(MCDIR)\nv\mcstate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MCDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mcpower.obj : $(MCDIR)\nv\mcpower.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MCDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\mcnv04.obj : $(MCDIR)\nv4\mcnv04.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MCDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\mcnv10.obj : $(MCDIR)\nv10\mcnv10.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MCDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\mcnv20.obj : $(MCDIR)\nv20\mcnv20.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MCDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MCDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

#
# Timer Device files.
#

$(OBJDIR)\time.obj : $(TMRDIR)\nv\time.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(TMRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(TMRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\tmrstate.obj : $(TMRDIR)\nv\tmrstate.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(TMRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(TMRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\tmrinit.obj : $(TMRDIR)\nv\tmrinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(TMRDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(TMRDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# BTREE files.
#

$(OBJDIR)\btree.obj : $(BTREEDIR)\nv\btree.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(BTREEDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(BTREEDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# VBLANK files.
#

$(OBJDIR)\vblank.obj : $(VBLANKDIR)\nv\vblank.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(VBLANKDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(VBLANKDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# MODULAR Class files.
#

$(OBJDIR_NV4)\c63nv04.obj : $(MODDIR)\nv4\c63nv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c63nv10.obj : $(MODDIR)\nv10\c63nv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c63nv20.obj : $(MODDIR)\nv20\c63nv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c64nv04.obj : $(MODDIR)\nv4\c64nv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c64nv10.obj : $(MODDIR)\nv10\c64nv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c64nv20.obj : $(MODDIR)\nv20\c64nv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c65nv04.obj : $(MODDIR)\nv4\c65nv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c65nv10.obj : $(MODDIR)\nv10\c65nv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c65nv20.obj : $(MODDIR)\nv20\c65nv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class012.obj : $(MODDIR)\nv\class012.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class017.obj : $(MODDIR)\nv\class017.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class018.obj : $(MODDIR)\nv\class018.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class019.obj : $(MODDIR)\nv\class019.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class01C.obj : $(MODDIR)\nv\class01C.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class01D.obj : $(MODDIR)\nv\class01D.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class01E.obj : $(MODDIR)\nv\class01E.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class01F.obj : $(MODDIR)\nv\class01F.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class021.obj : $(MODDIR)\nv\class021.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class030.obj : $(MODDIR)\nv\class030.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class036.obj : $(MODDIR)\nv\class036.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class037.obj : $(MODDIR)\nv\class037.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class038.obj : $(MODDIR)\nv\class038.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class039.obj : $(MODDIR)\nv\class039.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class042.obj : $(MODDIR)\nv\class042.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class043.obj : $(MODDIR)\nv\class043.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class044.obj : $(MODDIR)\nv\class044.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class046.obj : $(MODDIR)\nv\class046.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class047.obj : $(MODDIR)\nv\class047.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c047nv04.obj : $(MODDIR)\nv4\c047nv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c047nv10.obj : $(MODDIR)\nv10\c047nv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c047nv20.obj : $(MODDIR)\nv20\c047nv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class048.obj : $(MODDIR)\nv\class048.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\nv\$(@B).obj
!ENDIF

$(OBJDIR)\class049.obj : $(MODDIR)\nv\class049.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class04A.obj : $(MODDIR)\nv\class04A.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class04B.obj : $(MODDIR)\nv\class04B.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class04D.obj : $(MODDIR)\nv\class04D.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c04Dnv04.obj : $(MODDIR)\nv4\c04Dnv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c04Dnv10.obj : $(MODDIR)\nv10\c04Dnv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c04Dnv20.obj : $(MODDIR)\nv20\c04Dnv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class04E.obj : $(MODDIR)\nv\class04E.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c04Env04.obj : $(MODDIR)\nv4\c04Env04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c04Env10.obj : $(MODDIR)\nv10\c04Env10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c04Env20.obj : $(MODDIR)\nv20\c04Env20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class04F.obj : $(MODDIR)\nv\class04F.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV4)\c04Fnv04.obj : $(MODDIR)\nv4\c04Fnv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MODDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c04Fnv10.obj : $(MODDIR)\nv10\c04Fnv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c04Fnv20.obj : $(MODDIR)\nv20\c04Fnv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class050.obj : $(MODDIR)\nv\class050.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class051.obj : $(MODDIR)\nv\class051.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class052.obj : $(MODDIR)\nv\class052.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class053.obj : $(MODDIR)\nv\class053.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class054.obj : $(MODDIR)\nv\class054.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class055.obj : $(MODDIR)\nv\class055.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class056.obj : $(MODDIR)\nv\class056.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class057.obj : $(MODDIR)\nv\class057.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class058.obj : $(MODDIR)\nv\class058.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class059.obj : $(MODDIR)\nv\class059.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05A.obj : $(MODDIR)\nv\class05A.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05B.obj : $(MODDIR)\nv\class05B.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05C.obj : $(MODDIR)\nv\class05C.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05D.obj : $(MODDIR)\nv\class05D.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05E.obj : $(MODDIR)\nv\class05E.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class05F.obj : $(MODDIR)\nv\class05F.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class060.obj : $(MODDIR)\nv\class060.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class061.obj : $(MODDIR)\nv\class061.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class063.obj : $(MODDIR)\nv\class063.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class064.obj : $(MODDIR)\nv\class064.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class065.obj : $(MODDIR)\nv\class065.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class066.obj : $(MODDIR)\nv\class066.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class067.obj : $(MODDIR)\nv\class067.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class072.obj : $(MODDIR)\nv\class072.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class076.obj : $(MODDIR)\nv\class076.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class077.obj : $(MODDIR)\nv\class077.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class07A.obj : $(MODDIR)\nv\class07A.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\c07Anv10.obj : $(MODDIR)\nv10\c07Anv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MODDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\c07anv20.obj : $(MODDIR)\nv20\c07anv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MODDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\class07B.obj : $(MODDIR)\nv\class07B.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class07C.obj : $(MODDIR)\nv\class07C.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class062.obj : $(MODDIR)\nv\class062.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class088.obj : $(MODDIR)\nv\class088.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class089.obj : $(MODDIR)\nv\class089.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class08A.obj : $(MODDIR)\nv\class08A.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class093.obj : $(MODDIR)\nv\class093.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class094.obj : $(MODDIR)\nv\class094.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class095.obj : $(MODDIR)\nv\class095.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class096.obj : $(MODDIR)\nv\class096.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class09F.obj : $(MODDIR)\nv\class09F.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class63.obj : $(MODDIR)\nv\class63.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class64.obj : $(MODDIR)\nv\class64.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\class65.obj : $(MODDIR)\nv\class65.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\classcmn.obj : $(MODDIR)\nv\classcmn.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MODDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MODDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# MEDIA PORT files.
#
$(OBJDIR_NV4)\mpnv04.obj : $(MPDIR)\nv4\mpnv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(MPDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\mpnv10.obj : $(MPDIR)\nv10\mpnv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(MPDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\mpnv20.obj : $(MPDIR)\nv20\mpnv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(MPDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\mp.obj : $(MPDIR)\nv\mp.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MPDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mpobj.obj : $(MPDIR)\nv\mpobj.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MPDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\mpstate.obj : $(MPDIR)\nv\mpstate.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(MPDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(MPDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# Video engine files
#
$(OBJDIR_NV4)\vidnv04.obj : $(VIDEODIR)\nv4\vidnv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(VIDEODIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(VIDEODIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\vidnv10.obj : $(VIDEODIR)\nv10\vidnv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(VIDEODIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(VIDEODIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\vidnv20.obj : $(VIDEODIR)\nv20\vidnv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(VIDEODIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(VIDEODIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\video.obj : $(VIDEODIR)\nv\video.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(VIDEODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(VIDEODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\videoobj.obj : $(VIDEODIR)\nv\videoobj.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(VIDEODIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(VIDEODIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

#
# Device Initialize (POST) files
#
$(OBJDIR)\devinit.obj: $(DEVINITDIR)\nv\devinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\nv04mem.obj: $(DEVINITDIR)\nv\nv04mem.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\nv04mtbl.obj: $(DEVINITDIR)\nv\nv04mtbl.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\nv05mem.obj: $(DEVINITDIR)\nv\nv05mem.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\nv05mtbl.obj: $(DEVINITDIR)\nv\nv05mtbl.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\nv10mem.obj: $(DEVINITDIR)\nv\nv10mem.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\nv10mtbl.obj: $(DEVINITDIR)\nv\nv10mtbl.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\nv15mem.obj: $(DEVINITDIR)\nv\nv15mem.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
$(OBJDIR)\bios.obj: $(DEVINITDIR)\nv\bios.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(DEVINITDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(DEVINITDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF
       
#
# HAL files
#
$(OBJDIR_NV4)\halnv04.obj : $(HALDIR)\nv4\halnv04.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV4) $(HALDIR)\nv4\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV4)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv4\$(@B).c
	$(LST_NV4)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV4)\$(@B).obj
!ENDIF

$(OBJDIR_NV10)\halnv10.obj : $(HALDIR)\nv10\halnv10.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV10) $(HALDIR)\nv10\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV10)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv10\$(@B).c
	$(LST_NV10)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV10)\$(@B).obj
!ENDIF

$(OBJDIR_NV20)\halnv20.obj : $(HALDIR)\nv20\halnv20.c $(RM_INCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT_NV20) $(HALDIR)\nv20\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD_NV20)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv20\$(@B).c
	$(LST_NV20)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR_NV20)\$(@B).obj
!ENDIF

$(OBJDIR)\haleng.obj : $(HALDIR)\nv\haleng.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(HALDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\halinit.obj : $(HALDIR)\nv\halinit.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(HALDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\halrm.obj : $(HALDIR)\nv\halrm.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(HALDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv\$(@B).c
	$(LST)
	$(LIB) $(LIB32DIR)\nvrm.lib -+$(OBJDIR)\$(@B).obj
!ENDIF

$(OBJDIR)\halwrap.obj: $(HALDIR)\nv\halwrap.c $(RMINCS)
!IFNDEF WIN31
        $(CCVXD) $(CFLAGSVXD_OPT) $(HALDIR)\nv\$(@B).c
        $(LST)
!ELSE
	@SET WCC386=$(CFLAGSVXD)
	$(CCVXD) $(LGROUP) $(HALDIR)\nv\$(@B).c
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
    
$(BINDIR)\nv04kern.lib : $(KERNOBJS_NV4)
    $(LIB32) /SUBSYSTEM:WINDOWS /OUT:$(BINDIR)\nv04kern.lib @<<
$(KERNOBJS_NV4)
<<

$(BINDIR)\nv10kern.lib : $(KERNOBJS_NV10)
    $(LIB32) /SUBSYSTEM:WINDOWS /OUT:$(BINDIR)\nv10kern.lib @<<
$(KERNOBJS_NV10)
<<

$(BINDIR)\nv20kern.lib : $(KERNOBJS_NV20)
    $(LIB32) /SUBSYSTEM:WINDOWS /OUT:$(BINDIR)\nv20kern.lib @<<
$(KERNOBJS_NV20)
<<
