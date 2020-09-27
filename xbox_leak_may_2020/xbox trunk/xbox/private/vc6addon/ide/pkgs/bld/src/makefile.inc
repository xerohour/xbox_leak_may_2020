!include $(IDE_ROOT)\makefile.inc

vproj.cpp: $(PACKVERCHECK)

$O\vproj.obj: vproj.cpp

$O\$(RCFILE).res: $(RCFILE).rc $(O)\bld.tlb
