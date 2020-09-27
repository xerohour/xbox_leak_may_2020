# Microsoft Visual C++ Generated NMAKE File, Format Version 20052
# ** DO NOT EDIT **

$$IF(TARGET_INTEL)
$$IF(PROJTYPE_EXTDLL)
# TARGTYPE "$$platform_long$$ Dynamic-Link Library" 0x102
$$ELSE //!PROJTYPE_EXTDLL
# TARGTYPE "$$platform_long$$ Application" 0x101
$$ENDIF //PROJTYPE_EXTDLL
$$ENDIF //TARGET_INTEL
$$IF(TARGET_MIPS)
$$IF(PROJTYPE_EXTDLL)
# TARGTYPE "$$platform_long$$ Dynamic-Link Library" 0x502
$$ELSE //!PROJTYPE_EXTDLL
# TARGTYPE "$$platform_long$$ Application" 0x501
$$ENDIF //PROJTYPE_EXTDLL
$$ENDIF //TARGET_MIPS
$$IF(TARGET_ALPHA)
$$IF(PROJTYPE_EXTDLL)
# TARGTYPE "$$platform_long$$ Dynamic-Link Library" 0x502
$$ELSE //!PROJTYPE_EXTDLL
# TARGTYPE "$$platform_long$$ Application" 0x501
$$ENDIF //PROJTYPE_EXTDLL
$$ENDIF //TARGET_ALPHA
$$IF(TARGET_MAC)
# TARGTYPE "Macintosh Application" 0x301
$$ENDIF //TARGET_MAC

!IF "$(CFG)" == ""
$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
CFG=$$platform_short$$ $$Debug$$
!MESSAGE No configuration specified. Defaulting to $$platform_short$$ $$Debug$$.
$$ELSE
CFG=Macintosh $$Debug$$
!MESSAGE No configuration specified. Defaulting to Macintosh $$Debug$$.
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA
!ENDIF 

$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
!IF "$(CFG)" != "$$platform_short$$ $$Debug$$" && "$(CFG)" != "$$platform_short$$ $$Release$$"
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA
$$IF(TARGET_MAC)
!IF "$(CFG)" != "Macintosh $$Debug$$" && "$(CFG)" != "Macintosh $$Release$$"
$$ENDIF //TARGET_MAC
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
!MESSAGE NMAKE /f "SAMP.MAK" CFG="$$platform_short$$ $$Debug$$"
$$ELIF(TARGET_MAC)
!MESSAGE NMAKE /f "SAMP.MAK" CFG="Macintosh $$Debug$$"
$$ELSE
!MESSAGE NMAKE /f "SAMP.MAK" CFG="$$Debug$$"
$$ENDIF
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
$$IF(PROJTYPE_EXTDLL)
!MESSAGE "$$platform_short$$ $$Debug$$" (based on "$$platform_long$$ Dynamic-Link Library")
!MESSAGE "$$platform_short$$ $$Release$$" (based on "$$platform_long$$ Dynamic-Link Library")
$$ELSE  //!PROJTYPE_EXTDLL
!MESSAGE "$$platform_short$$ $$Debug$$" (based on "$$platform_long$$ Application")
!MESSAGE "$$platform_short$$ $$Release$$" (based on "$$platform_long$$ Application")
$$ENDIF //PROJTYPE_EXTDLL
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA
$$IF(TARGET_MAC)
!MESSAGE "Macintosh $$Debug$$" (based on "Macintosh Application")
!MESSAGE "Macintosh $$Release$$" (based on "Macintosh Application")
$$ENDIF //TARGET_MAC
!MESSAGE 
!ERROR Invalid configuration specified.
$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
!ENDIF
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA
$$IF(TARGET_MAC)
!ENDIF
$$ENDIF //TARGET_MAC

################################################################################
# Begin Project
# PROP MARKED_FOR_SAVE 1
# PROP HAS_NO_VCP_FILE 1
$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
# PROP TARGET_LAST_SCANNED "$$platform_short$$ $$Debug$$"
$$ELSE
# PROP TARGET_LAST_SCANNED "Macintosh $$Debug$$"
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA
$$IF(AUTOMATION)
MTL=MkTypLib.exe
$$ENDIF

$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
!IF  "$(CFG)" == "$$platform_short$$ $$Debug$$"

$$IF(MFCDLL)
# PROP BASE Use_MFC 6
$$ELSE
# PROP BASE Use_MFC 5
$$ENDIF //DLL
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
$$IF(MFCDLL)
# PROP Use_MFC 6
$$ELSE
# PROP Use_MFC 5
$$ENDIF //DLL
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"

ALL : .\WinDebug\$$root$$.exe .\WinDebug\$$root$$.bsc

$$IF(AUTOMATION)
# ADD BASE MTL /nologo /D "_DEBUG" /win32  
# ADD MTL /nologo /D "_DEBUG" /win32  
MTL_PROJ=/nologo /D "_DEBUG" /win32  
$$ENDIF //AUTOMATION	  
RSC=rc.exe
CPP=$$cc$$.exe
BSC32=bscmake.exe 
LINK32=$$linker$$.exe

$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\WinDebug\$$frame_ifile$$.sbr \
$$IF(MDICHILD)
	.\WinDebug\$$child_frame_ifile$$.sbr \
$$ENDIF
	.\WinDebug\$$view_ifile$$.sbr \
$$IF(CRecordView)
	.\WinDebug\$$recset_ifile$$.sbr \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\WinDebug\$$cntritem_ifile$$.sbr \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\WinDebug\$$ipframe_ifile$$.sbr \
	.\WinDebug\$$srvritem_ifile$$.sbr \
$$ENDIF
	.\WinDebug\$$doc_ifile$$.sbr \
$$ELSE //!!PROJTYPE_DLG
	$$dlg_ifile$$.sbr \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\WinDebug\$$root$$.sbr 


.\WinDebug\$$root$$.bsc : $(BSC32_SBRS)
	$(BSC32) @<<
 $(BSC32_FLAGS) $(BSC32_SBRS)
<<


# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib $$EXTRA_DEBUG_LIBRARIES$$ /NOLOGO /INCREMENTAL:YES /DEBUG /MACHINE:$$cpu$$ /SUBSYSTEM:windows
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib $$EXTRA_DEBUG_LIBRARIES$$ /NOLOGO /INCREMENTAL:YES /DEBUG /MACHINE:$$cpu$$ /SUBSYSTEM:windows

$$IF(PROJTYPE_EXTDLL)
# ADD BASE LINK32 /DLL /DEF:$$root$$.def /IMPLIB:$$root$$.lib
# ADD LINK32 /DLL /DEF:$$root$$.def /IMPLIB:$$root$$.lib
$$ENDIF //PROJTYPE_EXTDLL
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\WinDebug\stdafx.obj \
	.\WinDebug\$$root$$.obj \
$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\WinDebug\$$frame_ifile$$.obj \
$$IF(MDICHILD)
	.\WinDebug\$$child_frame_ifile$$.obj \
$$ENDIF
	.\WinDebug\$$view_ifile$$.obj \
$$IF(CRecordView)
	.\WinDebug\$$recset_ifile$$.obj \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\WinDebug\$$cntritem_ifile$$.obj \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\WinDebug\$$ipframe_ifile$$.obj \
	.\WinDebug\$$srvritem_ifile$$.obj \
$$ENDIF
	.\WinDebug\$$doc_ifile$$.obj \
$$ELSE //!!PROJTYPE_DLG
	.\WinDebug\$$dlg_ifile$$.obj \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\WinDebug\$$root$$.res

.\WinDebug\$$root$$.exe : $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "$$platform_short$$ $$Release$$"

$$IF(MFCDLL)
# PROP BASE Use_MFC 6
$$ELSE
# PROP BASE Use_MFC 5
$$ENDIF //DLL
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
$$IF(MFCDLL)
# PROP Use_MFC 6
$$ELSE
# PROP Use_MFC 5
$$ENDIF //DLL
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"

ALL : .\WinRel\$$root$$.exe .\WinRel\$$root$$.bsc

$$IF(AUTOMATION)
# ADD BASE MTL /nologo /D "NDEBUG" /win32  
# ADD MTL /nologo /D "NDEBUG" /win32  
MTL_PROJ=/nologo /D "NDEBUG" /win32  
$$ENDIF //AUTOMATION
RSC=rc.exe
CPP=$$cc$$.exe
BSC32=bscmake.exe 
LINK32=$$linker$$.exe
$$IF(MFCDLL)
# ADD BASE RSC /l$$LANGID$$ /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l$$LANGID$$ /d "NDEBUG" /d "_AFXDLL"
$$ELSE //!DLL
# ADD BASE RSC /l$$LANGID$$ /d "NDEBUG" 
# ADD RSC /l$$LANGID$$ /d "NDEBUG" 
$$ENDIF //DLL
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c 
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c 

$$IF(MFCDLL)
# ADD BASE CPP /D "_AFXDLL" /MD
# ADD CPP /D "_AFXDLL" /MD
CPP_PROJ=/nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR"WinRel/" /MD\
 /Fp"WinRel/$$root$$.pch" /Yu"stdafx.h" /Fo"WinRel/" /c
$$ELSE //!DLL
# ADD BASE CPP /MT
# ADD CPP /MT
CPP_PROJ=/nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FR"WinRel/" /MT\
 /Fp"WinRel/$$root$$.pch" /Yu"stdafx.h" /Fo"WinRel/" /c
$$ENDIF //DLL
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
BSC32_FLAGS=/nologo /o"WinRel\$$root$$.bsc" 
BSC32_SBRS= \
	.\WinRel\stdafx.sbr \
$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\WinRel\$$frame_ifile$$.sbr \
$$IF(MDICHILD)
	.\WinRel\$$child_frame_ifile$$.sbr \
$$ENDIF
	.\WinRel\$$view_ifile$$.sbr \
$$IF(CRecordView)
	.\WinRel\$$recset_ifile$$.sbr \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\WinRel\$$cntritem_ifile$$.sbr \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\WinRel\$$ipframe_ifile$$.sbr \
	.\WinRel\$$srvritem_ifile$$.sbr \
$$ENDIF
	.\WinRel\$$doc_ifile$$.sbr \
$$ELSE //!!PROJTYPE_DLG
	.\WinRel\$$dlg_ifile$$.sbr \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\WinRel\$$root$$.sbr 


.\WinRel\$$root$$.bsc :  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib $$EXTRA_DEBUG_LIBRARIES$$ /NOLOGO /INCREMENTAL:YES /MACHINE:$$cpu$$ /SUBSYSTEM:windows
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib $$EXTRA_DEBUG_LIBRARIES$$ /NOLOGO /INCREMENTAL:YES /MACHINE:$$cpu$$ /SUBSYSTEM:windows

$$IF(PROJTYPE_EXTDLL)
# ADD BASE LINK32 /DLL /DEF:$$root$$.def /IMPLIB:$$root$$.lib
# ADD LINK32 /DLL /DEF:$$root$$.def /IMPLIB:$$root$$.lib
$$ENDIF //PROJTYPE_EXTDLL
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\WinRel\stdafx.obj \
	.\WinRel\$$root$$.obj \
$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\WinRel\$$frame_ifile$$.obj \
$$IF(MDICHILD)
	.\WinRel\$$child_frame_ifile$$.obj \
$$ENDIF
	.\WinRel\$$view_ifile$$.obj \
$$IF(CRecordView)
	.\WinRel\$$recset_ifile$$.obj \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\WinRel\$$cntritem_ifile$$.obj \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\WinRel\$$ipframe_ifile$$.obj \
	.\WinRel\$$srvritem_ifile$$.obj \
$$ENDIF
	.\WinRel\$$doc_ifile$$.obj \
$$ELSE //!!PROJTYPE_DLG
	.\WinRel\$$dlg_ifile$$.obj \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\WinRel\$$root$$.res

.\WinRel\$$root$$.exe :  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ENDIF
$$ENDIF //TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA

$$IF(TARGET_MAC)
!IF  "$(CFG)" == "Macintosh $$Debug$$"

# PROP BASE Use_MFC 5
# PROP BASE Output_Dir "MacDebug"
# PROP BASE Intermediate_Dir "MacDebug"
# PROP Use_MFC 5
# PROP Output_Dir "MacDebug"
# PROP Intermediate_Dir "MacDebug"

ALL : .\MacDebug\$$root$$.exe .\MacDebug\$$root$$.bsc .\$$root$$.trg

$$IF(AUTOMATION)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac
$$ENDIF //AUTOMATION
RSC=rc.exe
CPP=$$cc$$.exe
BSC32=bscmake.exe 
LINK32=$$linker$$.exe
MRC=mrc.exe
MFILE32=mfile.exe
# ADD BASE RSC /d "_DEBUG" /d "_MAC" /d "_68K_" /r
# ADD RSC /d "_DEBUG" /d "_MAC" /d "_68K_" /r
# ADD BASE CPP /nologo /c /W3 /AL /Od /Zi /FR /Q68s /Q68m /GX /D "_WINDOWS" /D "_DEBUG" /D_MBCS /D_MAC /D_68K_ /Yu"stdafx.h" 
# ADD CPP /nologo /c /W3 /AL /Od /Zi /FR /Q68s /Q68m /GX /D "_WINDOWS" /D "_DEBUG" /D_MBCS /D_MAC /D_68K_ /Yu"stdafx.h"
CPP_OBJS=.\MacDebug/
CPP_SBRS=.\MacDebug/
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /D "_MAC" /D "_DEBUG"
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE_FILES= \
	.\MacDebug\$$ROOT$$.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
BSC32_FLAGS=/nologo /o"MacDebug\$$root$$.bsc" 
BSC32_SBRS= \
	.\MacDebug\stdafx.sbr \
$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\MacDebug\$$frame_ifile$$.sbr \
$$IF(MDICHILD)
	.\MacDebug\$$child_frame_ifile$$.sbr \
$$ENDIF
	.\MacDebug\$$view_ifile$$.sbr \
$$IF(CRecordView)
	.\MacDebug\$$recset_ifile$$.sbr \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\MacDebug\$$cntritem_ifile$$.sbr \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\MacDebug\$$ipframe_ifile$$.sbr \
	.\MacDebug\$$srvritem_ifile$$.sbr \
$$ENDIF
	.\MacDebug\$$doc_ifile$$.sbr \
$$ELSE //!!PROJTYPE_DLG
	$$dlg_ifile$$.sbr \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\MacDebug\$$root$$.sbr 


.\MacDebug\$$root$$.bsc : $(BSC32_SBRS)
	$(BSC32) @<<
 $(BSC32_FLAGS) $(BSC32_SBRS)
<<

# ADD BASE LINK32 wlm.rsc commdlg.rsc /debug /map /NOLOGO /MAC:creator="$$CREATOR$$" /INCREMENTAL:yes /PDB:"MacDebug\$$ROOT$$.pdb" /MACHINE:M68K
# ADD LINK32 wlm.rsc commdlg.rsc /debug /map /NOLOGO /MAC:creator="$$CREATOR$$" /INCREMENTAL:yes /PDB:"MacDebug\$$ROOT$$.pdb" /MACHINE:M68K
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\MacDebug\stdafx.obj \
	.\MacDebug\$$root$$.obj \
$$IF(!PROJTYPE_EXTDLL)
$$IF(!PROJTYPE_DLG)
	.\MacDebug\$$frame_ifile$$.obj \
$$IF(MDICHILD)
	.\MacDebug\$$child_frame_ifile$$.obj \
$$ENDIF
	.\MacDebug\$$view_ifile$$.obj \
$$IF(CRecordView)
	.\MacDebug\$$recset_ifile$$.obj \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\MacDebug\$$cntritem_ifile$$.obj \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\MacDebug\$$ipframe_ifile$$.obj \
	.\MacDebug\$$srvritem_ifile$$.obj \
$$ENDIF
	.\MacDebug\$$doc_ifile$$.obj \
$$ELSE //!!PROJTYPE_DLG
	.\MacDebug\$$dlg_ifile$$.obj \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\MacDebug\$$root$$.rsc \
	.\MacDebug\$$r_file$$.rsc

.\MacDebug\$$root$$.exe : $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Macintosh $$Release$$"

# PROP BASE Use_MFC 5
# PROP BASE Output_Dir "MacRel"
# PROP BASE Intermediate_Dir "MacRel"
# PROP Use_MFC 5
# PROP Output_Dir "MacRel"
# PROP Intermediate_Dir "MacRel"

ALL : .\MacRel\$$root$$.exe .\MacRel\$$root$$.bsc

$$IF(AUTOMATION)
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
$$ENDIF //AUTOMATION
RSC=rc.exe
CPP=$$cc$$.exe
BSC32=bscmake.exe 
LINK32=$$linker$$.exe
MRC=mrc.exe
MFILE32=mfile.exe
# ADD BASE RSC /d "_MAC" /d "_68K_" /d "NDEBUG" /r
# ADD RSC /d "_MAC" /d "_68K_" /d "NDEBUG" /r
RSC_PROJ=/fo"MacRel\$$root$$.res" /d "NDEBUG"
# ADD BASE CPP /nologo /c /W3 /AL /O2 /D "NDEBUG" /D "_WINDOWS" /FR /Q68s /GX /D_MBCS /D_MAC /D_68K_ /Yu"stdafx.h" 
# ADD CPP /nologo /c /W3 /AL /O2 /D "NDEBUG" /D "_WINDOWS" /FR /Q68s /GX /D_MBCS /D_MAC /D_68K_ /Yu"stdafx.h" 
CPP_OBJS=.\MacRel/
CPP_SBRS=.\MacRel/
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /D "_MAC" /D "NDEBUG"
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE_FILES= \
	.\MacRel\$$ROOT$$.EXE
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
BSC32_FLAGS=/nologo /o"MacRel\$$root$$.bsc" 
BSC32_SBRS= \
	.\MacRel\stdafx.sbr \
$$IF(PROJTYPE_EXTDLL)
$$ELSE
$$IF(!PROJTYPE_DLG)
	.\MacRel\$$frame_ifile$$.sbr \
$$IF(MDICHILD)
	.\MacRel\$$child_frame_ifile$$.sbr \
$$ENDIF
	.\MacRel\$$view_ifile$$.sbr \
$$IF(CRecordView)
	.\MacRel\$$recset_ifile$$.sbr \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\MacRel\$$cntritem_ifile$$.sbr \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\MacRel\$$ipframe_ifile$$.sbr \
	.\MacRel\$$srvritem_ifile$$.sbr \
$$ENDIF
	.\MacRel\$$doc_ifile$$.sbr \
$$ELSE //!!PROJTYPE_DLG
	.\MacRel\$$dlg_ifile$$.sbr \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\MacRel\$$root$$.sbr 


.\MacRel\$$root$$.bsc :  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

# ADD BASE LINK32 wlm.rsc commdlg.rsc /map /NOLOGO /MAC:creator="$$CREATOR$$" /PDB:"MacRel\$$ROOT$$.pdb" /MACHINE:M68K
# ADD LINK32 wlm.rsc commdlg.rsc /map /NOLOGO /MAC:creator="$$CREATOR$$" /PDB:"MacRel\$$ROOT$$.pdb" /MACHINE:M68K
DEF_FLAGS=
DEF_FILE=
LINK32_OBJS= \
	.\MacRel\stdafx.obj \
	.\MacRel\$$root$$.obj \
$$IF(PROJTYPE_EXTDLL)
$$ELSE
$$IF(!PROJTYPE_DLG)
	.\MacRel\$$frame_ifile$$.obj \
$$IF(MDICHILD)
	.\MacRel\$$child_frame_ifile$$.obj \
$$ENDIF
	.\MacRel\$$view_ifile$$.obj \
$$IF(CRecordView)
	.\MacRel\$$recset_ifile$$.obj \
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	.\MacRel\$$cntritem_ifile$$.obj \
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\MacRel\$$ipframe_ifile$$.obj \
	.\MacRel\$$srvritem_ifile$$.obj \
$$ENDIF
	.\MacRel\$$doc_ifile$$.obj \
$$ELSE //!!PROJTYPE_DLG
	.\MacRel\$$dlg_ifile$$.obj \
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
	.\MacRel\$$root$$.rsc \
	.\MacRel\$$r_file$$.rsc

.\MacRel\$$root$$.exe :  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(DEF_FLAGS) $(LINK32_OBJS)
<<

!ENDIF
$$ENDIF //TARGET_MAC

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

SOURCE=.\stdafx.cpp
DEP_STDAF=\
	.\stdafx.h

# ADD BASE CPP /Yc"stdafx.h" 
# SUBTRACT BASE CPP /Yu 
# ADD CPP /Yc"stdafx.h" 
# SUBTRACT CPP /Yu 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\
$$root$$.cpp
DEP_ROOT_C= \
$$IF(PROJTYPE_EXTDLL)
$$ELSE
$$IF(!PROJTYPE_DLG)
	.\$$frame_hfile$$.h\
$$IF(MDICHILD)
	.\$$child_frame_hfile$$.h\
$$ENDIF
	.\$$doc_hfile$$.h\
	.\$$view_hfile$$.h\
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\$$ipframe_hfile$$.h\
$$ENDIF
$$ELSE //!!PROJTYPE_DLG
	.\$$dlg_hfile$$.h\
$$ENDIF //!PROJTYPE_DLG
	.\$$root$$.h
$$ENDIF //!PROJTYPE_EXTDLL

# End Source File
$$IF(PROJTYPE_EXTDLL)
$$ELSE
$$IF(!PROJTYPE_DLG)
################################################################################
# Begin Source File

SOURCE=.\$$frame_ifile$$.cpp
DEP_MAINF=\
	.\$$root$$.h\
	.\$$frame_hfile$$.h

# End Source File
$$IF(MDICHILD)
################################################################################
# Begin Source File

SOURCE=.\$$child_frame_ifile$$.cpp
DEP_CHILD=\
	.\$$root$$.h\
	.\$$child_frame_hfile$$.h

# End Source File
$$ENDIF //MDICHILD
################################################################################
# Begin Source File

SOURCE=.\$$doc_ifile$$.cpp
DEP_MAKDO=\
	.\$$root$$.h\
$$IF(CONTAINER || CONTAINER_SERVER)
	.\$$cntritem_hfile$$.h\
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\$$srvritem_hfile$$.h\
$$ENDIF
	.\$$doc_hfile$$.h

# End Source File
################################################################################
# Begin Source File

SOURCE=.\$$view_ifile$$.cpp
DEP_MAKVI=\
	.\$$root$$.h\
	.\$$doc_hfile$$.h\
$$IF(CONTAINER || CONTAINER_SERVER)
	.\$$cntritem_hfile$$.h\
$$ENDIF
$$IF(CRecordView)
	.\$$recset_hfile$$.h\
$$ENDIF
	.\$$view_hfile$$.h

# End Source File
$$IF(CRecordView)
################################################################################
# Begin Source File

SOURCE=.\$$recset_ifile$$.cpp
DEP_RECSET=\
	.\stdafx.h\
	.\$$root$$.h\
	.\$$recset_hfile$$.h

# End Source File
$$ENDIF //CRecordView
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
################################################################################
# Begin Source File

SOURCE=.\$$ipframe_ifile$$.cpp
DEP_IPFRAME=\
	.\stdafx.h\
	.\$$root$$.h\
	.\$$ipframe_hfile$$.h

# End Source File
################################################################################
# Begin Source File

SOURCE=.\$$srvritem_ifile$$.cpp
DEP_SRVR=\
	.\stdafx.h\
	.\$$root$$.h\
	.\$$doc_hfile$$.h\
	.\$$srvritem_hfile$$.h

# End Source File
$$ENDIF	//SERVERS
$$IF(CONTAINER || CONTAINER_SERVER)
################################################################################
# Begin Source File

SOURCE=.\$$cntritem_ifile$$.cpp
DEP_CNTR= \
	.\stdafx.h\
	.\$$root$$.h\
	.\$$doc_hfile$$.h\
	.\$$cntritem_hfile$$.h

# End Source File
$$ENDIF //CONTAINERS
$$ELSE //!!PROJTYPE_DLG
################################################################################
# Begin Source File

SOURCE=.\$$dlg_ifile$$.cpp
DEP_DLG=\
	.\$$root$$.h\
	.\$$dlg_hfile$$.h

# End Source File
$$ENDIF //!PROJTYPE_DLG
$$ENDIF //!PROJTYPE_EXTDLL
################################################################################
# Begin Source File

SOURCE=.\$$root$$.rc
DEP_MAK_R= \
$$IF(!PROJTYPE_EXTDLL)
	.\res\$$root$$.ico \
$$IF(PROJTYPE_MDI)
	.\res\$$doc_ifile$$.ico \
$$ENDIF
$$IF(TOOLBAR)
	.\res\toolbar.bmp \
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	.\res\itoolbar.bmp \
$$ENDIF //SERVERS
$$ENDIF //TOOLBAR
$$ENDIF //!PROJTYPE_EXTDLL
	.\res\$$root$$.rc2

# End Source File
$$IF(TARGET_MAC)
################################################################################
# Begin Source File
SOURCE=.\$$r_file$$.r

$$IF(TARGET_INTEL || TARGET_MIPS || TARGET_ALPHA)
!IF "$(CFG)" == "$$platform_short$$ $$Debug$$"

# PROP BASE EXCLUDE_FROM_BUILD 1
# PROP EXCLUDE_FROM_BUILD 1

!ELSEIF  "$(CFG)" == "$$platform_short$$ $$Release$$"

# PROP BASE EXCLUDE_FROM_BUILD 1
# PROP EXCLUDE_FROM_BUILD 1

!ENDIF
$$ENDIF //MIPS/INTEL/ALPHA

# End Source File
$$ENDIF //TARGET_MAC
$$IF(VERBOSE)
################################################################################
# Begin Source File
SOURCE=.\readme.txt

# End Source File
$$ENDIF //VERBOSE
$$IF(AUTOMATION)
################################################################################
# Begin Source File
SOURCE=.\$$root$$.odl

# End Source File
$$ENDIF //AUTOMATION
# End Group
# End Project
################################################################################
