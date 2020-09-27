!include $(IDE_ROOT)\makefile.inc

$O\$(RCFILE).res: $O\$(CMDFILE).rcs $(RCFILE).rc2 $(RCFILE).rc $(O)\shell.tlb

!if "$(DEBUG)"=="0"
DESCRIPTION="VSHELL_DLL"
DLL=dllretail
!else
DESCRIPTION="VSHELLD_DLL"
DLL=dlldebug
!endif

SHARE=shared

initexit.cpp: $(PACKVERCHECK)

!ifdef BUILD_SHELL_REDEF

# BUILD_SHELL_REDEF is defined.  Build the .def file with maptweak.
#  If VC_BUILD_MACHINE is defined, check the file in.  Otherwise, just
#  copy it to the platform subdir.

! ifdef REDEFPASS

$(PACKVERCHECK): $(DLLDEF)
#   delete the shell/package compatibility check header file
    if exist $(PACKVERCHECK) attrib -r $(PACKVERCHECK)
    if exist $(PACKVERCHECK) del $(PACKVERCHECK)
#   regenerate the shell/package compatibility check header file
    type shellver.a1 > $(PACKVERCHECK)
    shellver >> $(PACKVERCHECK)
	echo. >> $(PACKVERCHECK)

$(DLLDEF):
    -del $(DLLDEF)
    echo LIBRARY $(TARGETNAME) > $(DLLDEF)
    type $(SHARE).def >> $(DLLDEF)

! else

$(DLLDEF): $O\$(TARGETNAME).map
#   strip the map file to symbols  leave 1 space at the beginning of line
    maptweak $** $O\tmp.1
#   filter out what we don't want to export (blank at start of each line)
    qgrep -v -f regen.f2 $O\tmp.1 > $O\tmp.3
    sort < $O\tmp.3 > $O\tmp.2
#   start with canned
    type regen.a1 > $O\$(TARGETNAME).tmp
    type $O\tmp.2 >> $O\$(TARGETNAME).tmp
    -del $O\tmp.1 $O\tmp.2 $O\tmp.3
#   build the real .DEF file
    echo LIBRARY $(TARGETNAME)          > $@
    echo DESCRIPTION $(DESCRIPTION)    >> $@
    type $(SHARE).def                  >> $@
!  if "$(DEBUG)"=="0"
	genord 16 < $O\$(TARGETNAME).tmp >> $@
!  else
    type $O\$(TARGETNAME).tmp >> $@
!  endif
    -del $O\$(TARGETNAME).tmp

$O\$(TARGETNAME).map: $(LIBRARY_OBJS)
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).lib
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).exp
    -del $(DLLDEF)
    set LINKER_FLAGS=-map:$@
    set NOLINK=
    $(MAKE) -$(MAKEFLAGS) MAKEDLL=1 REDEFPASS=1 LINKER_NOREF=1
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).lib
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).exp

! endif

!else

$(DLLDEF): defs\$(TARGET_DIRECTORY)\$(TARGETNAME).def
    copy $** $@

!endif
