########################################################################
# Makefile for NAMSPACE test application program

!CMDSWITCHES +i

CC = cl

# xbox doesnt do -ZI
# CC_FLAGS = -Od -ZI 
CC_FLAGS = -Od -Zi -Zvc6

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: namspace.xbe	

namspace.xbe: namspace.exe
	imagebld /debug namspace.exe -out:namspace.xbe

namspace.exe: namspace.lnk namspace.obj
	link -out:namspace.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @namspace.lnk

namspace.lnk:                   .\namspace.mak
	echo namspace.obj  > namspace.lnk
	echo xapilib.lib  >> namspace.lnk
	echo xboxkrnl.lib  >> namspace.lnk
!IF "$(PCODE)" == "yes"
	echo pcode.lib  >> namspace.lnk
!ENDIF



