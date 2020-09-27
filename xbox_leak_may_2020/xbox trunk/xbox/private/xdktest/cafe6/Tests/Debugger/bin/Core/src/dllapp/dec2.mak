########################################################################
# Makefile for DEC2.DLL (DLLAPPX test application program)
#
# Build rules

.SUFFIXES: .c .obj .lib

#       .c = CMerge source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

# CC_FLAGS = -LDd -MLd -Od -ZI -D"WIN32"
CC_FLAGS = -Od -Zi -D"WIN32"

OBJDIR = debug
TESTDIR = $(MAKEDIR)
DLLINIT = DllEntryPoint

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.cxx{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: dec2.dll	

!IF "$(CFG)" == ""
CFG=x86
!MESSAGE No configuration specified. Defaulting to x86.
!ENDIF 

!IF "$(CFG)" == "x86"
dec2.dll: dec2.lnk dec2.obj	dec2.def incdll.lib
#	link -out:dec2.dll -dll -implib:"dec2.lib" -def:dec2.def -debug:notmapped,full -debugtype:CV @dec2.lnk
	link -out:dec2.dll -dll -implib:"dec2.lib" -def:dec2.def -debug:notmapped,full -debugtype:CV -entry:DllEntryPoint -subsystem:windows -pdbtype:sept @dec2.lnk
	
dec2.lnk:                   .\dec2.mak
	echo msvcrt.lib > dec2.lnk
	echo kernel32.lib >> dec2.lnk
	echo user32.lib >> dec2.lnk
	echo dec2.obj  >> dec2.lnk
	echo incdll.lib  >> dec2.lnk

!ENDIF 


