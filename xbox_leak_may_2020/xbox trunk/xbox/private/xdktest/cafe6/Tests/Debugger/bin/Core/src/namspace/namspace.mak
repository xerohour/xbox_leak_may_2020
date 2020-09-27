########################################################################
# Makefile for NAMSPACE test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   namspace.exe

namspace.exe: namspace.lnk namspace.obj
	link -out:namspace.exe -debug:full -debugtype:CV -pdbtype:sept @namspace.lnk

namspace.lnk:                   .\namspace.mak
	echo namspace.obj  > namspace.lnk
!IF "$(PCODE)" == "yes"
	echo pcode.lib  >> namspace.lnk
!ENDIF



