$(BASEDIR)\private\ntos\obj\i386\romdec32.bin: ..\r16\obj\i386\rstartup.com ..\r32\obj\i386\romdec32.exe
    rdbld ..\r32\obj\i386\romdec32.exe ..\r16\obj\i386\rstartup.com $(BASEDIR)\private\ntos\obj\i386\romdec32.bin
!IFDEF _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!ENDIF


# Hack to get around a MP build problem where romdec32.exe isn't linked by
# the time the above build dependency rule is evaluated.
..\r32\obj\i386\romdec32.exe:
    sleep 1
