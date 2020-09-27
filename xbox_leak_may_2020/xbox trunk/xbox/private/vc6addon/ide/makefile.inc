#
# Misc standard rules for VC project
#

!ifdef CMDFILE

.SUFFIXES: .cmd .rcs

!ifndef RCFILE
!error RCFILE must be defined
!endif

!ifndef CMDSRC
CMDSRC=$(CMDFILE)
!endif

$(RCFILE).rc : $O\$(CMDFILE).rcs

$O\$(CMDFILE).rcs: $(CMDSRC).cmd
	cl /EP $(C_DEFINES) $(CMDSRC).cmd > $O\$(CMDSRC).cmd
	cmdcomp $O\$(CMDSRC).cmd
!endif

.SUFFIXES: .bin .tpl

.tpl.bin:
    cmtempl $< $@

.tpl{$(O)}.bin:
    cmtempl $< $@
