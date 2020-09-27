$(BASEDIR)\private\ntos\obj\i386\xpreldr.bin: obj\i386\preldr32.exe
    bldrbld -p obj\i386\preldr32.exe $(BASEDIR)\private\ntos\obj\i386\xpreldr.bin
!IFDEF _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!ENDIF

