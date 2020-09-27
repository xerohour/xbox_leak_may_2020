RMLIBDIR=$(NV_BASE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\i386

$(RMLIBDIR)\nv04kern.lib:           \
        $(RMLIBDIR)\dacnv04.lib     \
        $(RMLIBDIR)\dmanv04.lib     \
        $(RMLIBDIR)\fbnv04.lib      \
        $(RMLIBDIR)\fifonv04.lib    \
        $(RMLIBDIR)\grnv04.lib      \
        $(RMLIBDIR)\halnv04.lib     \
        $(RMLIBDIR)\mcnv04.lib      \
        $(RMLIBDIR)\modnv04.lib     \
        $(RMLIBDIR)\mpnv04.lib      \
        $(RMLIBDIR)\vidnv04.lib
                $(LIBRARIAN) $**
