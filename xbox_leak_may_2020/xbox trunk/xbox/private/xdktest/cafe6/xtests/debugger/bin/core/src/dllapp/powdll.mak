########################################################################
# Makefile for POWDLL.DLL (DLLAPPX test application program)
#
# Build rules

.SUFFIXES: .c .obj .lib

#       .c = CMerge source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI -D"WIN32"

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.cxx{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: powdll.dll	

!IF "$(CFG)" == ""
CFG=x86
!MESSAGE No configuration specified. Defaulting to x86.
!ENDIF 

!IF "$(CFG)" == "x86"
powdll.dll: powdll.lnk powdll.obj powdll.def
	link -out:powdll.dll -dll -def:powdll.def -debug:full -debugtype:CV -pdbtype:sept @powdll.lnk
	
powdll.lnk:                   .\powdll.mak
	echo powdll.obj  > powdll.lnk

!ENDIF 


