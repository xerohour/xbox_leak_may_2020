########################################################################
# Makefile for DLLAPPX test application program
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

#goal:  incdll.dll powdll.dll dec2.dll dllappx.exe	
goal: dllappx.exe	

!IF "$(CFG)" == ""
CFG=x86
!MESSAGE No configuration specified. Defaulting to x86.
!ENDIF 

!IF "$(CFG)" == "x86"
#incdll.dll: incdll.lnk incdll.obj incdll.def
#	link -out:incdll.dll -dll -implib:"incdll.lib" -def:incdll.def -debug:full -debugtype:CV @incdll.lnk
#	
#incdll.lnk:                   .\dllappx.mak
#	echo incdll.obj  > incdll.lnk
#
#powdll.dll: powdll.lnk powdll.obj powdll.def
#	link -out:powdll.dll -dll -def:powdll.def -debug:full -debugtype:CV @powdll.lnk
#	
#powdll.lnk:                   .\dllappx.mak
#	echo powdll.obj  > powdll.lnk
#
#dec2.dll: dec2.lnk dec2.obj	dec2.def incdll.lib
#	link -out:dec2.dll -dll -implib:"dec2.lib" -def:dec2.def -debug:full -debugtype:CV @dec2.lnk
#	
#dec2.lnk:                   .\dllappx.mak
#	echo dec2.obj  > dec2.lnk
#	echo incdll.lib  >> dec2.lnk

dllappx.exe: dllappx.lnk dllappx.obj dec2.lib dllapp.def dllapp.res
	link -out:dllappx.exe -def:dllapp.def -debug:full -debugtype:CV -pdbtype:sept @dllappx.lnk

dllappx.lnk:                   .\dllappx.mak
	echo dllappx.obj  > dllappx.lnk
	echo dllapp.res  >> dllappx.lnk
	echo dec2.lib  >> dllappx.lnk
	echo user32.lib  >> dllappx.lnk
	echo gdi32.lib  >> dllappx.lnk

dllapp.res:
	rc /l 0x409 /fo"dllapp.res" /d"_DEBUG" dllapp.rc

!ENDIF 

