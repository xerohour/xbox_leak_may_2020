########################################################################
# Makefile for EE test application program

!CMDSWITCHES +i

CC = cl

#xbox CC_FLAGS = -GX -Od -ZI -D"WIN32"
CC_FLAGS = -GX -Od -Zi -D"WIN32" -Zvc6

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

goal: ee.xbe

ee.xbe: ee.exe
	imagebld -debug ee.exe -out:ee.xbe	

#xbox ee.exe: ee.lnk ee.obj dll.lib base1.obj base2.obj global.obj derived1.obj pch.obj struct1.obj union1.obj
ee.exe: ee.lnk ee.obj base1.obj base2.obj global.obj derived1.obj pch.obj struct1.obj union1.obj
	cl -c -Od -Zi -GX -YX pch.cpp 
	link -out:ee.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no -incremental:no @ee.lnk

ee.lnk:                   .\ee.mak
	echo ee.obj  > ee.lnk
 	echo xapilib.lib  >> ee.lnk
 	echo xboxkrnl.lib  >> ee.lnk
# 	echo dll.lib  >> ee.lnk
	echo base1.obj  >> ee.lnk
	echo base2.obj  >> ee.lnk
	echo derived1.obj  >> ee.lnk
	echo global.obj  >> ee.lnk
	echo pch.obj  >> ee.lnk
	echo struct1.obj  >> ee.lnk
	echo union1.obj   >> ee.lnk

