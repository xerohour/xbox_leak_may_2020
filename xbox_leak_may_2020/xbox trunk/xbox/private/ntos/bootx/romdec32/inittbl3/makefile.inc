.SUFFIXES: .com .exe .obj .lst .c .asm .def .lnk .inc

{..\i386\}.asm{obj\i386\}.obj:
    $(386_ASSEMBLER_NAME) -c -omf -Fl -Fo$(MAKEDIR)\$@ -I. $(INCPATH1) $(ASM_DEFINES) $<

OBJ=obj\i386\inittbl.obj

$(BASEDIR)\private\ntos\obj\i386\inittbl_dvt4.bin: $(OBJ)

obj\i386\inittbl.obj: ..\i386\inittbl.asm

$(BASEDIR)\private\ntos\obj\i386\inittbl_dvt4.bin: $(OBJ)
    echo foo
    link16.exe @<<
/tiny /nod /noi +
$(OBJ)
$(BASEDIR)\private\ntos\obj\i386\inittbl_dvt4.bin



<<
!IFDEF _NT386TREE
    binplace -R $(_NTTREE) $(BINPLACE_DBGFLAGS_NT) $(BINPLACE_FLAGS) $@
!ENDIF

