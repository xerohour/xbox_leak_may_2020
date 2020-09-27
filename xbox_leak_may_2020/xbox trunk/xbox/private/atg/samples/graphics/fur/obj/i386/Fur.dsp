# Microsoft Developer Studio Project File - Name="Fur" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=Fur - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Fur.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Fur.mak" CFG="Fur - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Fur - Xbox Release" (based on "Xbox Application")
!MESSAGE "Fur - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "Fur - Xbox Release"


# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zvc6 /O2 /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /c
# ADD CPP /nologo /W3 /GX /Zvc6 /O2 /I "..\..\Common\include" /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib /nologo /incremental:no /machine:I386 /subsystem:xbox  /opt:ref /fixed:no /debugtype:vc6
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib  /nologo /incremental:no /machine:I386 /subsystem:xbox  /opt:ref /fixed:no /debugtype:vc6
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0xffff0602" /testname:"Fur" /stack:0x10000 /INITFLAGS:0x00000000 /TITLEIMAGE:titleimage.xbx  /out:"Release/Fur.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Zvc6 /Od /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Zvc6 /Od /I "..\..\Common\include" /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox  /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib  /nologo /incremental:no /debug /machine:I386 /subsystem:xbox  /fixed:no /debugtype:vc6
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testid:"0xffff0602" /testname:"Fur" /stack:0x10000 /debug /INITFLAGS:0x00000000 /TITLEIMAGE:titleimage.xbx  /out:"Debug/Fur.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Fur - Xbox Release"
# Name "Fur - Xbox Debug"

# Begin Group "Resources"

# PROP Default_Filter "*.rdf"
# Begin Source File

SOURCE=.\font.rdf

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build
ProjDir=.
InputPath=.\font.rdf
InputName=font

BuildCmds= \
    bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\font.rdf
InputName=font

BuildCmds= \
    bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\gamepad.rdf

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build
ProjDir=.
InputPath=.\gamepad.rdf
InputName=gamepad

BuildCmds= \
    bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\gamepad.rdf
InputName=gamepad

BuildCmds= \
    bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Source File

SOURCE=.\Fur.cpp
# End Source File
# Begin Source File

SOURCE=.\XBFur.h
# End Source File
# Begin Source File

SOURCE=.\XBFur.cpp
# End Source File
# Begin Source File

SOURCE=.\XBFurMesh.h
# End Source File
# Begin Source File

SOURCE=.\XBFurMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\Mipmap.h
# End Source File
# Begin Source File

SOURCE=.\Mipmap.cpp
# End Source File
# Begin Source File

SOURCE=.\OctoSphere.h
# End Source File
# Begin Source File

SOURCE=.\OctoSphere.cpp
# End Source File
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\src\XBApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\Xfvf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBHelp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XBMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XObj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XParse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XBResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\XBSound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBUtil.cpp
# End Source File
# End Group
# Begin Group "Media"

# PROP Default_Filter "vsh;psh;m;abc;tga;bmp;dds"
# Begin Source File

SOURCE=.\media\shaders\Mipmap.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\Mipmap.psh
InputName=Mipmap

BuildCmds= \
	xsasm -nologo -v -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\Mipmap.psh
InputName=Mipmap

BuildCmds= \
	xsasm -nologo -v -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media\shaders\comp2.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\comp2.psh
InputName=comp2

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\comp2.psh
InputName=comp2

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media\shaders\comp3.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\comp3.psh
InputName=comp3

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\comp3.psh
InputName=comp3

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media\shaders\furfade0.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade0.psh
InputName=furfade0

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade0.psh
InputName=furfade0

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media\shaders\furfade1.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade1.psh
InputName=furfade1

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade1.psh
InputName=furfade1

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\media\shaders\furfade2.psh

!IF  "$(CFG)" == "Fur - Xbox Release"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade2.psh
InputName=furfade2

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\media\shaders\furfade2.psh
InputName=furfade2

BuildCmds= \
	xsasm -nologo -l $(InputPath) $(OutDir)\$(InputName).xpu $(OutDir)\$(InputName).xsc $(ProjDir)\$(InputName).inl \

"$(ProjDir)\$(InputName).inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Media\Shaders\fin.vsh

!IF  "$(CFG)" == "Fur - Xbox Release"

USERDEP__FIN_V="Media\Shaders\eyelighthalf.vsh"	"Media\Shaders\hairlighting.vsh"	"Media\Shaders\wind.vsh"	
# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\Media\Shaders\fin.vsh
InputName=fin

BuildCmds= \
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu "" $(OutDir)\$(InputName)_wind1_local1_self1.lst \
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu "" $(OutDir)\$(InputName)_wind1_local1_self0.lst \
	xsasm -nologo -l -D WIND                   -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu "" $(OutDir)\$(InputName)_wind1_local0_self1.lst \
	xsasm -nologo -l -D WIND                                     $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu "" $(OutDir)\$(InputName)_wind1_local0_self0.lst \
	xsasm -nologo -l         -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu "" $(OutDir)\$(InputName)_wind0_local1_self1.lst \
	xsasm -nologo -l         -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu "" $(OutDir)\$(InputName)_wind0_local1_self0.lst \
	xsasm -nologo -l                           -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu "" $(OutDir)\$(InputName)_wind0_local0_self1.lst \
	xsasm -nologo -l                                             $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu "" $(OutDir)\$(InputName)_wind0_local0_self0.lst \
	

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

USERDEP__FIN_V="Media\Shaders\eyelighthalf.vsh"	"Media\Shaders\hairlighting.vsh"	"Media\Shaders\wind.vsh"	
# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\Media\Shaders\fin.vsh
InputName=fin

BuildCmds= \
	xsasm -nologo -l -Od -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu "" $(OutDir)\$(InputName)_wind1_local1_self1.lst \
	xsasm -nologo -l -Od -D WIND -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu "" $(OutDir)\$(InputName)_wind1_local1_self0.lst \
	xsasm -nologo -l -Od -D WIND                   -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu "" $(OutDir)\$(InputName)_wind1_local0_self1.lst \
	xsasm -nologo -l -Od -D WIND                                     $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu "" $(OutDir)\$(InputName)_wind1_local0_self0.lst \
	xsasm -nologo -l -Od         -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu "" $(OutDir)\$(InputName)_wind0_local1_self1.lst \
	xsasm -nologo -l -Od         -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu "" $(OutDir)\$(InputName)_wind0_local1_self0.lst \
	xsasm -nologo -l -Od                           -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu "" $(OutDir)\$(InputName)_wind0_local0_self1.lst \
	xsasm -nologo -l -Od                                             $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu "" $(OutDir)\$(InputName)_wind0_local0_self0.lst \
	

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Media\Shaders\fur.vsh

!IF  "$(CFG)" == "Fur - Xbox Release"

USERDEP__FIN_V="Media\Shaders\eyelighthalf.vsh"	"Media\Shaders\hairlighting.vsh"	"Media\Shaders\wind.vsh"	
# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\Media\Shaders\fur.vsh
InputName=fur

BuildCmds= \
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu "" $(OutDir)\$(InputName)_wind1_local1_self1.lst \
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu "" $(OutDir)\$(InputName)_wind1_local1_self0.lst \
	xsasm -nologo -l -D WIND                   -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu "" $(OutDir)\$(InputName)_wind1_local0_self1.lst \
	xsasm -nologo -l -D WIND                                     $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu "" $(OutDir)\$(InputName)_wind1_local0_self0.lst \
	xsasm -nologo -l         -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu "" $(OutDir)\$(InputName)_wind0_local1_self1.lst \
	xsasm -nologo -l         -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu "" $(OutDir)\$(InputName)_wind0_local1_self0.lst \
	xsasm -nologo -l                           -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu "" $(OutDir)\$(InputName)_wind0_local0_self1.lst \
	xsasm -nologo -l                                             $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu "" $(OutDir)\$(InputName)_wind0_local0_self0.lst \
	

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Fur - Xbox Debug"

USERDEP__FIN_V="Media\Shaders\eyelighthalf.vsh"	"Media\Shaders\hairlighting.vsh"	"Media\Shaders\wind.vsh"	
# Begin Custom Build - xsasm $(InputPath)
ProjDir=.
InputPath=.\Media\Shaders\fur.vsh
InputName=fur

BuildCmds= \
	xsasm -nologo -l -Od -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu "" $(OutDir)\$(InputName)_wind1_local1_self1.lst \
	xsasm -nologo -l -Od -D WIND -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu "" $(OutDir)\$(InputName)_wind1_local1_self0.lst \
	xsasm -nologo -l -Od -D WIND                   -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu "" $(OutDir)\$(InputName)_wind1_local0_self1.lst \
	xsasm -nologo -l -Od -D WIND                                     $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu "" $(OutDir)\$(InputName)_wind1_local0_self0.lst \
	xsasm -nologo -l -Od         -D LOCAL_LIGHTING -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu "" $(OutDir)\$(InputName)_wind0_local1_self1.lst \
	xsasm -nologo -l -Od         -D LOCAL_LIGHTING                   $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu "" $(OutDir)\$(InputName)_wind0_local1_self0.lst \
	xsasm -nologo -l -Od                           -D SELF_SHADOWING $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu "" $(OutDir)\$(InputName)_wind0_local0_self1.lst \
	xsasm -nologo -l -Od                                             $(InputPath) $(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu "" $(OutDir)\$(InputName)_wind0_local0_self0.lst \
	

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind1_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local1_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self1.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\Media\Shaders\$(InputName)_wind0_local0_self0.xvu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Media\Shaders\eyelighthalf.vsh
# End Source File
# Begin Source File

SOURCE=.\Media\Shaders\hairlighting.vsh
# End Source File
# Begin Source File

SOURCE=.\Media\Shaders\wind.vsh
# End Source File
# End Group
# End Target
# End Project

