# Microsoft Visual C++ Generated NMAKE File, Format Version 20046
# MSVCPRJ: version 2.0.4068
# ** DO NOT EDIT **

# TARGTYPE "Win32 Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE No configuration specified.  Defaulting to Debug.
!ENDIF 

!IF "$(CFG)" != "Debug" && "$(CFG)" != "Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "LAYOUT.MAK" CFG="Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Debug" (based on "Win32 Console Application")
!MESSAGE "Release" (based on "Win32 Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Debug"

ALL : .\LAYOUT.exe .\LAYOUT.bsc

# ADD CPP /nologo /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /Zi /YX /Od /D "_DEBUG" /D "_CONSOLE" /FR /Fp"LAYOUT.pch"\
 /Fd"LAYOUT.pdb" /c 
CPP_OBJS=
CPP_SBRS=
# SUBTRACT BASE RSC /l 0x0
# ADD RSC /d "_DEBUG"
# SUBTRACT RSC /l 0x0
RSC_PROJ=/l 0x409 /fo"LAYOUT.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"LAYOUT.bsc" 
BSC32_SBRS= \
	.\layout.sbr

.\LAYOUT.bsc :  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD LINK32 ehcrt.lib netapi32.lib  /NOLOGO /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=ehcrt.lib netapi32.lib  /NOLOGO /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:"LAYOUT.exe" 
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\layout.obj

.\LAYOUT.exe :  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Release"

# PROP Output_Dir "RelObjs"
# PROP Intermediate_Dir "RelObjs"

ALL : .\RelObjs\LAYOUT.exe .\RelObjs\LAYOUT.bsc

# ADD CPP /nologo /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /YX /O2 /D "NDEBUG" /D "_CONSOLE" /FR"RelObjs/"\
 /Fp"RelObjs/LAYOUT.pch" /Fo"RelObjs/" /c 
CPP_OBJS=.\RelObjs/
CPP_SBRS=.\RelObjs/
# SUBTRACT BASE RSC /l 0x0
# ADD RSC /d "NDEBUG"
# SUBTRACT RSC /l 0x0
RSC_PROJ=/l 0x409 /fo"RelObjs/LAYOUT.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"RelObjs\LAYOUT.bsc" 
BSC32_SBRS= \
	.\RelObjs\layout.sbr

.\RelObjs\LAYOUT.bsc :  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD LINK32 ehcrt.lib netapi32.lib  /NOLOGO /machine:$(PROCESSOR_ARCHITECTURE)
LINK32_FLAGS=ehcrt.lib netapi32.lib  /NOLOGO /machine:$(PROCESSOR_ARCHITECTURE)\
 /OUT:"RelObjs\LAYOUT.exe" 
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\RelObjs\layout.obj

.\RelObjs\LAYOUT.exe :  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\layout.c

!IF  "$(CFG)" == "Debug"

.\layout.obj .\layout.sbr :  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Release"

.\RelObjs\layout.obj .\RelObjs\layout.sbr :  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
