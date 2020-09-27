!include $(IDE_ROOT)\makefile.inc

# ---------------------------------------------------------------------------
# Set up dependance on package versioning file
# ---------------------------------------------------------------------------
..\mfcappwz.cpp: $(PACKVERCHECK)

# ---------------------------------------------------------------------------
# General macros
# ---------------------------------------------------------------------------

APPWIZ_OBJDIR=$(APPWIZ_ROOT)\\$(O)


# ---------------------------------------------------------------------------
# Rules for building $(NEW_RC_FILE) "mfcappwz.rc_"
# ---------------------------------------------------------------------------

TPL_NOLOC_FILES=*.*
NEW_RC_FILE=$(APPWIZ_OBJDIR)\\mfcappwz.rc_

mfcappwz.rc : $(NEW_RC_FILE)
$(NEW_RC_FILE) : $(TPL_NOLOC_DIR)\$(TPL_NOLOC_FILES)
    cd $(TPL_NOLOC_DIR)
    echo // Code templates                                                       > $*.tm2
    @for %%f in ($(TPL_NOLOC_FILES)) do echo %%f TEMPLATE "$(TPL_NOLOC_DIR)\\%%f" >> $*.tm2
    fixwhack < $*.tm2 > $@
    cd $(APPWIZ_ROOT)


# ---------------------------------------------------------------------------
# Build the resource DLL
# ---------------------------------------------------------------------------

LINKFLAGS_RES = -dll -noentry -machine:$(TARGET_DIRECTORY) -debug:none -release
NEW_LANG_FILE=$(APPWIZ_OBJDIR)\\$(APPWIZ_LANG).rc_
LANG_RC_FILE=$(APPWIZ_ROOT)\\$(APPWIZ_LANG).rc
TPL_LOC_FILES= *.*

$(LANG_RC_FILE): $(NEW_LANG_FILE)

$(NEW_LANG_FILE): $(TPL_LOC_DIR)\$(TPL_LOC_FILES) $(TPL_TAG_DIR)\$(TPL_LOC_FILES)
    cd $(TPL_LOC_DIR)
    echo // Resource templates                                                >  $*.tm2
    @for %%f in ($(TPL_LOC_FILES)) do echo %%f TEMPLATE "$(TPL_LOC_DIR)\\%%f" >> $*.tm2
    cd $(APPWIZ_ROOT)
    cd $(TPL_TAG_DIR)
    echo // Tagged resource templates                                         >>  $*.tm2
    @for %%f in ($(TPL_LOC_FILES)) do echo %%f TEMPLATE_TAG "$(TPL_TAG_DIR)\\%%f" >> $*.tm2
    fixwhack < $*.tm2 > $@
    cd $(APPWIZ_ROOT)


$O\$(APPWIZ_LANG).res: $(LANG_RC_FILE)

$(RESDLL_NAME): $O\$(APPWIZ_LANG).res
    $(LINKER) $(LINKFLAGS_RES) $O\$(APPWIZ_LANG).res


# ---------------------------------------------------------------------------
# rules for generating a .def file from hell
# ---------------------------------------------------------------------------

!if "$(DEBUG)"=="0"
DESCRIPTION="APPWIZ_DLL"
DLL=dllretail
!else
DESCRIPTION="APPWIZD_DLL"
DLL=dlldebug
!endif

SHARE=shared

!ifdef REDEFPASS

$O\$(TARGETNAME).def:
    -del /f/q $O\$(TARGETNAME).def
    echo LIBRARY $(TARGETNAME) > $@

!else

$(DLLDEF): $O\$(TARGETNAME).map
#   strip the map file to symbols leave 1 space at the beginning of line
    maptweak $** $O\tmp.1
#   filter out what we don't want to export (blank at start of each line)
    qgrep -v -f $(APPWIZ_ROOT)\filter.no $O\tmp.1 > $O\tmp.3
    qgrep < $O\tmp.3 -f $(APPWIZ_ROOT)\filter.yes > $O\tmp.4
    sort < $O\tmp.4 > $O\tmp.2
    type $O\tmp.2 > $O\$(TARGETNAME).tmp
    #-del $O\tmp.1 $O\tmp.2 $O\tmp.3 $O\tmp.4
#   build the real .DEF file
    echo LIBRARY $(TARGETNAME)           > $@
    echo DESCRIPTION $(DESCRIPTION)     >> $@
    echo EXPORTS                        >> $@
    type canned.exp     >> $@
!if "$(DEBUG)"=="0"
    genord 2 < $O\$(TARGETNAME).tmp >> $@
!else
    type $O\$(TARGETNAME).tmp >> $@
!endif
    #-del $O\$(TARGETNAME).tmp

$O\$(TARGETNAME).map: $(LIBRARY_OBJS)
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).lib
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).exp
    -del $(DLLDEF)
    set LINKER_FLAGS=-map:$@
    set NOLINK=
    $(MAKE) MAKEDLL=1 REDEFPASS=1 LINKER_NOREF=1
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).lib
    -del $(TARGETPATHLIB)\$(TARGET_DIRECTORY)\$(TARGETNAME).exp

!endif
