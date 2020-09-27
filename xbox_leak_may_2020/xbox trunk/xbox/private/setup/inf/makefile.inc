#############################################################################
#
#   MAKEFILE.INC
#
#   Common rules for the setup INF files
#
#############################################################################


# Suffixes used by this makefile
.SUFFIXES : .inx .inf


# Definitions
# Base switches for C preprocessor
#
# /EP - Run through the preprocessor only
# /W4 - Warning level 4
# /nologo - Don't display compiler logo
# /c - Don't build an EXE
CFLAGS_BASE=/EP /W4 /nologo /c

TARGETCOPY_CMD = $(TARGETCOPY_CMD:.exe=.inf)

{..\}.inx{$(_OBJ_DIR)\$(TARGET_DIRECTORY)\}.inf:
    $(CC) $(CFLAGS_BASE) $(STD_DEFINES) $(TARGET_DBG_DEFINES) $(C_DEFINES) $(INCPATH0) $< > $*.tmp
    uniq -u $*.tmp > $@
    del $*.tmp
    $(TARGETCOPY_CMD)

{.}.inx{$(_OBJ_DIR)\$(TARGET_DIRECTORY)\}.inf:
    $(CC) $(CFLAGS_BASE) $(STD_DEFINES) $(TARGET_DBG_DEFINES) $(C_DEFINES) $(INCPATH0) $< > $*.tmp
    uniq -u $*.tmp > $@
    del $*.tmp
    $(TARGETCOPY_CMD)

$(_OBJ_DIR)\$(TARGET_DIRECTORY)\$(TARGETNAME).inf: $(PRIVATE_INC_PATH)\xboxverp.h

$(O)\xdk.sed : ..\xdk.sdx
        $(C_PREPROCESSOR_NAME) $(C_PREPROCESSOR_FLAGS:-DNT =) $** > $@

$(O)\$(TARGETNAME).exe : $(O)\xdk.sed $(O)\xdk.inf
        cd $(O)
        iexpressbld.exe /n /M xdk.sed
        cd ..\..
