########################################################################
# Makefile for RETVAL test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

#goal:  testdll.dll retval.exe	
goal: retval.exe	

#testdll.dll: testdll.lnk testdll.obj
#	link -out:testdll.dll -dll -implib:"testdll.lib" -debug:full -debugtype:CV @testdll.lnk
#	
#testdll.lnk:                   .\retval.mak
#	echo testdll.obj  > testdll.lnk

retval.exe: retval.lnk retval.obj lineonly.obj nodebug.obj testdll.lib 
	cl -c -Od -Zd -GX lineonly.cpp 
	cl -c -GX nodebug.cpp 
	link -out:retval.exe -debug:full -debugtype:CV -pdbtype:sept @retval.lnk

retval.lnk:                   .\retval.mak
	echo retval.obj  > retval.lnk
	echo lineonly.obj  >> retval.lnk
	echo nodebug.obj  >> retval.lnk
	echo testdll.lib  >> retval.lnk
	echo user32.lib  >> retval.lnk
	echo gdi32.lib  >> retval.lnk

