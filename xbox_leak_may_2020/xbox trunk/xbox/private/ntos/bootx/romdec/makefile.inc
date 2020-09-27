.SUFFIXES: .com .exe .obj .lst .c .asm .def .lnk .inc

{i386\}.asm{obj\i386\}.obj:
    $(386_ASSEMBLER_NAME) -Sa -Fl -c -omf -Fo$(MAKEDIR)\$@ -I. $(INCPATH1) $<

OBJ=obj\i386\romdec.obj

obj\i386\romdec.com: $(OBJ)

obj\i386\romdec.obj: i386\romdec.asm

obj\i386\romdec.com: $(OBJ)
    echo foo
    link16.exe @<<
/tiny /nod /noi +
$(OBJ)
obj\i386\romdec.com



<<
