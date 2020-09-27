########################################################################
# Makefile for EE test application program

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

goal: ee.exe	

ee.exe: ee.lnk ee.obj dll.lib base1.obj base2.obj global.obj derived1.obj pch.obj struct1.obj union1.obj
	cl -c -Od -Zi -GX -YX pch.cpp 
	link -out:ee.exe -debug:full -debugtype:CV -pdbtype:sept @ee.lnk

ee.lnk:                   .\ee.mak
	echo ee.obj  > ee.lnk
	echo dll.lib  >> ee.lnk
	echo base1.obj  >> ee.lnk
	echo base2.obj  >> ee.lnk
	echo derived1.obj  >> ee.lnk
	echo global.obj  >> ee.lnk
	echo pch.obj  >> ee.lnk
	echo struct1.obj  >> ee.lnk
	echo union1.obj   >> ee.lnk


