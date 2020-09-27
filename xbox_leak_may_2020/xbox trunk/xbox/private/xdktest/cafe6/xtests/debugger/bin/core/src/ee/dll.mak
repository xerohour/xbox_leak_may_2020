  ########################################################################
# Makefile for DLL.DLL (EE test application program)
#

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI -D"WIN32"

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

goal: dll.dll	

dll.dll: dll.lnk dll.obj 
	link -out:dll.dll -dll -implib:"dll.lib" -debug:full -debugtype:CV -pdbtype:sept @dll.lnk
	
dll.lnk:                   .\dll.mak
	echo dll.obj  > dll.lnk


