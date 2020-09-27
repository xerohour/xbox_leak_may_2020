#
# Makefile to build all interfaces required for RAD Env packages
#

!IF "$(CFG)" == ""
CFG=idl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to idl - Win32 Debug.
!ENDIF 


#
# Target macros
#


!if "$(VSROOT)" == ""
!error Environment Variable VSROOT not set
!endif

CURVER=V6
OUTDIR=$(VSROOT)\src\vc\ide\include
SHELLIDL=$(VSROOT)\src\common\idl\vs
VSEEIDL=$(VSROOT)\src\common\idl\vsee
VCIDL=$(VSROOT)\src\vc\ide\idl
IDLLIB=$(VSROOT)\src\vc\ide\lib\i386
SHAREDVCIDL=$(VSROOT)\src\common\idl\vc
!IF  "$(CFG)" == "idl - Win32 Debug"
VSDROPINCLUDES=$(VSBUILT)\Debug\inc
!else
VSDROPINCLUDES=$(VSBUILT)\Retail\tlb
!endif
!IF  "$(CFG)" == "idl - Win32 Debug"
PATH = $(PATH);$(VSBUILT)\Debug\bin\i386;
!else
PATH = $(PATH);$(VSBUILT)\Retail\bin\i386;
!endif

MIDL=midl.exe
MIDLFLAGS=/I $(SHAREDVCIDL) /I $(SHELLIDL) /I $(VSEEIDL) /I $(VSROOT)\src\common\inc /I $(VSDROPINCLUDES) /I $(MSDEV)\include /I $(VSBUILT)\Debug\inc /I $(OUTDIR) /D_MIDL_USER_MARSHAL_DISABLED /iid $(VCIDL)\$(INTDIR)\$(@B).c /h $(OUTDIR)\$(@B).h /no_format_opt /dlldata $(VCIDL)\$(INTDIR)\dlldata.c

VCIDLSRC=$(VCIDL)\$(INTDIR)\vc.c \
    $(VCIDL)\$(INTDIR)\projbld.c \
    $(VCIDL)\$(INTDIR)\bined.c \
    $(VCIDL)\$(INTDIR)\vcclsvw.c \
    $(VCIDL)\$(INTDIR)\ncb.c \
    $(VCIDL)\$(INTDIR)\respkg.c \
	$(VCIDL)\shell.c \
	$(VCIDL)\$(INTDIR)\exposed.c
.SUFFIXES: .idl

#
# Inference Rules
#

#{$(SHELLIDL)}.idl{$(OUTDIR)}.h:
#	cd $(OUTDIR)
#    $(MIDL) $(MIDLFLAGS) $<
#
#{$(SHELLIDL)}.idl{$(OUTDIR)}.tlb:
#	cd $(OUTDIR)
#    $(MIDL) $(MIDLFLAGS) /tlb $(OUTDIR)\$(@B).tlb $<

#{$(VSEEIDL)}.idl{$(OUTDIR)}.h:
# 	cd $(OUTDIR)
#   $(MIDL) $(MIDLFLAGS) $<

#{$(VCIDL)}.idl{$(OUTDIR)}.tlb:
#	cd $(OUTDIR)
#    $(MIDL) $(MIDLFLAGS) /tlb $(OUTDIR)\$(@B).tlb $<

{$(VCIDL)}.idl{$(OUTDIR)}.h:
	cd $(OUTDIR)
    $(MIDL) $(MIDLFLAGS) $<

{$(SHAREDVCIDL)}.idl{$(OUTDIR)}.h:
	cd $(OUTDIR)
    $(MIDL) $(MIDLFLAGS) $<

#
# Targets
#

ALL: $(OUTDIR)\vc.tlb \
	 $(OUTDIR)\vc.h \
     $(OUTDIR)\bined.h \
	 $(OUTDIR)\respkg.h \
     $(OUTDIR)\vcclsvw.h \
     $(OUTDIR)\ncb.h \
     $(OUTDIR)\projbld.h \
	 $(OUTDIR)\exposed.h \
	 $(VCIDLSRC) \
	 vcidl.lib 


vcidl.lib : $(VCIDLSRC)
    cl /c /Zl /I $(VSROOT)\src\vc\ide\include /I$(VSBUILT)\Debug\inc /I $(VSROOT)\src\common\inc $(VCIDLSRC)
	link /lib *.obj /out:$(IDLLIB)\vcidl.lib

$(OUTDIR)\projbld.h		$(VCIDL)\$(INTDIR)\projbld.c		: $(VCIDL)\projbld.idl
$(OUTDIR)\vc.tlb $(OUTDIR)\vc.h $(VCIDL)\$(INTDIR)\vc.c		: $(VCIDL)\vc.idl
$(OUTDIR)\vcclsvw.h		$(VCIDL)\$(INTDIR)\vcclsvw.c		: $(VCIDL)\vcclsvw.idl
$(OUTDIR)\bined.h		$(VCIDL)\$(INTDIR)\bined.c			: $(VCIDL)\bined.idl
$(OUTDIR)\respkg.h		$(VCIDL)\$(INTDIR)\respkg.c			: $(VCIDL)\respkg.idl
$(OUTDIR)\ncb.h			$(VCIDL)\$(INTDIR)\ncb.c			: $(VCIDL)\ncb.idl
$(OUTDIR)\exposed.h		$(VCIDL)\$(INTDIR)\exposed.c		: $(VCIDL)\exposed.idl







