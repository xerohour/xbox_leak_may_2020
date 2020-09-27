obj\i386\dvdkey$(D).xbe : obj\i386\dvdkey$(D).exe
    dvdbld 0x0101 1 obj\i386\dvdkey$(D).exe obj\i386\dvdkey$(D).xbe
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) obj\i386\dvdkey$(D).exe
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) obj\i386\dvdkey$(D).xbe
!endif

obj\i386\dvdkey1$(D).bin : obj\i386\dvdkey$(D).xbe
    dvdbld 0x0101 1 obj\i386\dvdkey$(D).xbe obj\i386\dvdkey1$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif

obj\i386\dvdkey2$(D).bin : obj\i386\dvdkey1$(D).bin
    dvdbld 0x0101 2 obj\i386\dvdkey1$(D).bin obj\i386\dvdkey2$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif

obj\i386\dvdkey3$(D).bin : obj\i386\dvdkey1$(D).bin
    dvdbld 0x0101 3 obj\i386\dvdkey1$(D).bin obj\i386\dvdkey3$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif

obj\i386\dvdkey4$(D).bin : obj\i386\dvdkey1$(D).bin
    dvdbld 0x0101 4 obj\i386\dvdkey1$(D).bin obj\i386\dvdkey4$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif

obj\i386\dvdkey5$(D).bin : obj\i386\dvdkey1$(D).bin
    dvdbld 0x0101 5 obj\i386\dvdkey1$(D).bin obj\i386\dvdkey5$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif

obj\i386\dvdkey6$(D).bin : obj\i386\dvdkey1$(D).bin
    dvdbld 0x0101 6 obj\i386\dvdkey1$(D).bin obj\i386\dvdkey6$(D).bin
!ifdef _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!endif
