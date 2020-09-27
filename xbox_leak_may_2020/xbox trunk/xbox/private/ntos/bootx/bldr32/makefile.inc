$(BASEDIR)\private\ntos\obj\i386\xboxbldr.bin: obj\i386\bldr32.exe
    bldrbld obj\i386\bldr32.exe $(BASEDIR)\private\ntos\obj\i386\xboxbldr.bin
!IFDEF _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!ENDIF

