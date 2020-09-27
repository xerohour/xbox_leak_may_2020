RMLIBDIR=$(NV_BASE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\i386

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
