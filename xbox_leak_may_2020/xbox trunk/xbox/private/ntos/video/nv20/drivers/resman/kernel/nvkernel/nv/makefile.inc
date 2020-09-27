!IFDEF NVIA64
RMLIBDIR=$(BUILD_TREE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\ia64
!ELSE
RMLIBDIR=$(BUILD_TREE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\i386
!ENDIF

!IF "$(BUILD_DDK_VER)"!="nt5"
RMLIBDIR=$(RMLIBDIR)\$(DDKBUILDENV)
!ENDIF

$(RMLIBDIR)\nvkernel.lib:           \
        $(RMLIBDIR)\btree.lib       \
        $(RMLIBDIR)\class.lib       \
        $(RMLIBDIR)\dac.lib         \
        $(RMLIBDIR)\devinit.lib     \
        $(RMLIBDIR)\dma.lib         \
        $(RMLIBDIR)\fb.lib          \
        $(RMLIBDIR)\fifo.lib        \
        $(RMLIBDIR)\gr.lib          \
        $(RMLIBDIR)\hal.lib         \
        $(RMLIBDIR)\heap.lib        \
        $(RMLIBDIR)\mc.lib          \
        $(RMLIBDIR)\modular.lib     \
        $(RMLIBDIR)\mp.lib          \
        $(RMLIBDIR)\state.lib       \
        $(RMLIBDIR)\tmr.lib         \
        $(RMLIBDIR)\vblank.lib      \
        $(RMLIBDIR)\vid.lib
                $(LIBRARIAN) $**
