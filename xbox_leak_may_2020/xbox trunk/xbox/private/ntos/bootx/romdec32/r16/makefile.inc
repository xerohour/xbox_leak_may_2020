.SUFFIXES: .com .exe .obj .lst .c .asm .def .lnk .inc

{..\i386\}.asm{obj\i386\}.obj:
    $(386_ASSEMBLER_NAME) -c -omf -Fl -Fo$(MAKEDIR)\$@ -I. $(INCPATH1) $<

OBJ=obj\i386\rstartup.obj

obj\i386\rstartup.com: $(OBJ)

obj\i386\rstartup.obj: ..\i386\rstartup.asm

obj\i386\rstartup.com: $(OBJ)
    echo foo
    link16.exe @<<
/tiny /nod /noi +
$(OBJ)
obj\i386\rstartup.com



<<
