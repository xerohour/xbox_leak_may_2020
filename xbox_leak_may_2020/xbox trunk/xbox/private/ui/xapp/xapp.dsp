# Microsoft Developer Studio Project File - Name="xapp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xapp - Win32 DX8 Unicode Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xapp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xapp.mak" CFG="xapp - Win32 DX8 Unicode Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xapp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 DX8 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 DX8 Debug" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 DX8 Unicode" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "xapp - Win32 DX8 Unicode Hybrid" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xapp - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "ddk" /D "NDEBUG" /D "_MBCS" /D "_DSHOW" /D "_DX7" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xapp - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "_MBCS" /D "_DSHOW" /D "_DX7" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "xapp - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xapp___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "xapp___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xapp___Win32_Unicode_Debug"
# PROP Intermediate_Dir "xapp___Win32_Unicode_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"std.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_DSHOW" /D "_DX7" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 d3dxd.lib ddraw.lib d3dim.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "xapp - Win32 DX8 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xapp___Win32_DX8_Unicode_Debug"
# PROP BASE Intermediate_Dir "xapp___Win32_DX8_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xapp___Win32_DX8_Unicode_Debug"
# PROP Intermediate_Dir "xapp___Win32_DX8_Unicode_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_DSHOW" /D "WIN32" /D "_WINDOWS" /D "_DX7" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /Yu"std.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_DX8" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /D "_RAVISENTDVD" /Yu"std.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "xapp - Win32 DX8 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xapp___Win32_DX8_Debug"
# PROP BASE Intermediate_Dir "xapp___Win32_DX8_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xapp___Win32_DX8_Debug"
# PROP Intermediate_Dir "xapp___Win32_DX8_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DX8" /Yu"std.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "_DX8" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "xapp - Win32 DX8 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xapp___Win32_DX8_Unicode"
# PROP BASE Intermediate_Dir "xapp___Win32_DX8_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xapp___Win32_DX8_Unicode"
# PROP Intermediate_Dir "xapp___Win32_DX8_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DX8" /Yu"std.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "ddk" /D "RELEASE" /D "UNICODE" /D "_UNICODE" /D "_DX8" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "RELEASE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "xapp - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xapp___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "xapp___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xapp___Win32_Unicode_Release"
# PROP Intermediate_Dir "xapp___Win32_Unicode_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "ddk" /D "NDEBUG" /D "_MBCS" /D "_DSHOW" /D "WIN32" /D "_WINDOWS" /D "_DX7" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /Yu"std.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "ddk" /D "NDEBUG" /D "_MBCS" /D "_DSHOW" /D "_DX7" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xapp - Win32 DX8 Unicode Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xapp___Win32_DX8_Unicode_Hybrid"
# PROP BASE Intermediate_Dir "xapp___Win32_DX8_Unicode_Hybrid"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xapp___Win32_DX8_Unicode_Hybrid"
# PROP Intermediate_Dir "xapp___Win32_DX8_Unicode_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /O2 /I "ddk" /D "RELEASE" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DX8" /Yu"std.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Ot /Ow /Ob2 /I "ddk" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_DX8" /D "WIN32" /D "_WINDOWS" /D "_AUDIO" /D "_HTTP" /D "_CDPLAYER" /D "_DVDPLAYER" /Yu"std.h" /FD /c
# SUBTRACT CPP /Ox /Og /Oi
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "RELEASE"
# ADD RSC /l 0x409 /d "RELEASE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xapp - Win32 Release"
# Name "xapp - Win32 Debug"
# Name "xapp - Win32 Unicode Debug"
# Name "xapp - Win32 DX8 Unicode Debug"
# Name "xapp - Win32 DX8 Debug"
# Name "xapp - Win32 DX8 Unicode"
# Name "xapp - Win32 Unicode Release"
# Name "xapp - Win32 DX8 Unicode Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\3dsbin.cpp
# End Source File
# Begin Source File

SOURCE=.\ActiveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Array.cpp
# End Source File
# Begin Source File

SOURCE=.\ArrayProps.cpp
# End Source File
# Begin Source File

SOURCE=.\Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioEffects.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioPump.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioVisualizer.cpp
# End Source File
# Begin Source File

SOURCE=.\Background.cpp
# End Source File
# Begin Source File

SOURCE=.\Browser.cpp
# End Source File
# Begin Source File

SOURCE=.\ByteArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\cdda.cpp
# End Source File
# Begin Source File

SOURCE=.\Class.cpp
# End Source File
# Begin Source File

SOURCE=.\Cloth.cpp
# End Source File
# Begin Source File

SOURCE=.\Compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyDest.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyGames.cpp
# End Source File
# Begin Source File

SOURCE=.\CSS.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Date.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\DefUse.cpp
# End Source File
# Begin Source File

SOURCE=.\DeltaField.cpp
# End Source File
# Begin Source File

SOURCE=.\Disc.cpp
# End Source File
# Begin Source File

SOURCE=.\DotField.cpp
# End Source File
# Begin Source File

SOURCE=.\Drive.cpp
# End Source File
# Begin Source File

SOURCE=.\DSound.cpp
# End Source File
# Begin Source File

SOURCE=.\DSStream.cpp
# End Source File
# Begin Source File

SOURCE=.\dvd.cpp
# End Source File
# Begin Source File

SOURCE=.\dvd2.cpp
# End Source File
# Begin Source File

SOURCE=.\DynTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Effect.cpp
# End Source File
# Begin Source File

SOURCE=.\FallOff.cpp
# End Source File
# Begin Source File

SOURCE=.\fft.cpp
# End Source File
# Begin Source File

SOURCE=.\Files.cpp
# End Source File
# Begin Source File

SOURCE=.\FileUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Fly.cpp
# End Source File
# Begin Source File

SOURCE=.\Fog.cpp
# End Source File
# Begin Source File

SOURCE=.\FontCache.cpp
# End Source File
# Begin Source File

SOURCE=.\gif.cpp
# End Source File
# Begin Source File

SOURCE=.\Ground.cpp
# End Source File
# Begin Source File

SOURCE=.\Group.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Hotspot.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\HTML.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\http.cpp
# End Source File
# Begin Source File

SOURCE=.\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\IndexedSets.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpolator.cpp
# End Source File
# Begin Source File

SOURCE=.\ISO639.cpp
# End Source File
# Begin Source File

SOURCE=.\Joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\LAN.cpp
# End Source File
# Begin Source File

SOURCE=.\Layer.cpp
# End Source File
# Begin Source File

SOURCE=.\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\Lerper.cpp
# End Source File
# Begin Source File

SOURCE=.\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\Lexer.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\Locale.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\lzss.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\Math.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxMat.cpp
# End Source File
# Begin Source File

SOURCE=.\Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\MemUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\MovieTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\Navigator.cpp
# End Source File
# Begin Source File

SOURCE=.\NavInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\ntiosvc.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\Particle2.cpp
# End Source File
# Begin Source File

SOURCE=.\ParticleFountain.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderHTML.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Runner.cpp
# End Source File
# Begin Source File

SOURCE=.\SavedGameGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSaver.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Shape.cpp
# End Source File
# Begin Source File

SOURCE=.\Smoke.cpp
# End Source File
# Begin Source File

SOURCE=.\smooth.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\StarField.cpp
# End Source File
# Begin Source File

SOURCE=.\std.cpp
# ADD CPP /Yc"std.h"
# End Source File
# Begin Source File

SOURCE=.\String.cpp
# End Source File
# Begin Source File

SOURCE=.\surfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSensor.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\Tokenize.cpp
# End Source File
# Begin Source File

SOURCE=.\Tristrip.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Viewpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterField.cpp
# End Source File
# Begin Source File

SOURCE=.\Windows.cpp
# End Source File
# Begin Source File

SOURCE=.\xapp.rc
# End Source File
# Begin Source File

SOURCE=.\xcdplay.cpp
# End Source File
# Begin Source File

SOURCE=.\xip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\app_icon.ico
# End Source File
# Begin Source File

SOURCE=.\res\app_icon.ico
# End Source File
# Begin Source File

SOURCE=.\flare0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flare0.bmp
# End Source File
# Begin Source File

SOURCE=.\flare1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flare1.bmp
# End Source File
# Begin Source File

SOURCE=.\flare2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flare2.bmp
# End Source File
# Begin Source File

SOURCE=.\flare3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flare3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ground2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\particle.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Notes.txt
# End Source File
# End Target
# End Project
