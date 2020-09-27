!IFDEF NVIA64
RMLIBDIR=$(BUILD_TREE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\ia64
!ELSE
RMLIBDIR=$(BUILD_TREE)\drivers\resman\arch\nvalloc\winnt4\lib\nv4\i386
!ENDIF

!IF "$(BUILD_DDK_VER)"!="nt5"
RMLIBDIR=$(RMLIBDIR)\$(DDKBUILDENV)
!ENDIF

$(RMLIBDIR)\nv20kern.lib:           \
        $(RMLIBDIR)\dacnv20.lib     \
        $(RMLIBDIR)\dmanv20.lib     \
        $(RMLIBDIR)\fbnv20.lib      \
        $(RMLIBDIR)\fifonv20.lib    \
        $(RMLIBDIR)\grnv20.lib      \
        $(RMLIBDIR)\halnv20.lib     \
        $(RMLIBDIR)\mcnv20.lib      \
        $(RMLIBDIR)\modnv20.lib     \
        $(RMLIBDIR)\mpnv20.lib      \
        $(RMLIBDIR)\vidnv20.lib
                $(LIBRARIAN) $**
