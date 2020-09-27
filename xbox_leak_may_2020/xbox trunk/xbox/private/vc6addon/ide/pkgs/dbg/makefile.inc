!include $(IDE_ROOT)\makefile.inc

dbg.cpp: $(PACKVERCHECK)

#--------------------------------------------------------------------
#
#  RESOURCE INCLUDE DEPENDENCIES
#
#--------------------------------------------------------------------
#RC_INC_DEP = \
#	$(INC_DIR)\dlgs.h        \
#	$(INC_DIR)\msvc.ver      \
#	$(LANGAPI_ROOT)\include\verstamp.h

#--------------------------------------------------------------------
#
#  RESOURCE DIALOG DEPENDENCIES
#
#--------------------------------------------------------------------
RC_DLG_DEP =
#--------------------------------------------------------------------
#
#  RESOURCE BITMAP DEPENDENCIES
#
#--------------------------------------------------------------------
#BITMAPS_DEP = \
#	$(ICO_DIR)\maintool.bmp \
#	$(ICO_DIR)\wizbar.bmp

#--------------------------------------------------------------------
#
#  RESOURCE ICON DEPENDENCIES
#
#--------------------------------------------------------------------

#ICONS_DEP = \
#    $(ICO_DIR)\source.ico \
#    $(ICO_DIR)\header.ico \
#    $(ICO_DIR)\browse.ico \
#    $(ICO_DIR)\qcerror.ico


$O\$(RCFILE).res: $(RCFILE).rc $O\$(CMDFILE).rcs $(O)\devdbg.tlb \
                           $(RC_DLG_DEP) $(RC_INC_DEP) $(ICONS_DEP) $(BITMAPS_DEP)

#qcqp.h : inc\qcqp.h
#    copy $** $@
