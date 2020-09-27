########################################################################
# Makefile for INCDLL.DLL (DLLAPPX test application program)
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

goal:  incdll.dll	

!IF "$(CFG)" == ""
CFG=x86
!MESSAGE No configuration specified. Defaulting to x86.
!ENDIF 

!IF "$(CFG)" == "x86"
incdll.dll: incdll.lnk incdll.obj incdll.def
	link -out:incdll.dll -dll -implib:"incdll.lib" -def:incdll.def -debug:full -debugtype:CV -pdbtype:sept @incdll.lnk
	
incdll.lnk:                   .\incdll.mak
	echo incdll.obj  > incdll.lnk

!ENDIF 


