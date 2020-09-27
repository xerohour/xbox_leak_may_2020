# Microsoft Developer Studio Project File - Name="ee" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ee - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "ee.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "ee.mak" CFG="ee - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "ee - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "ee - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ee - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\bin\core\EE.dll" /libpath:"..\..\..\..\lib"

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"..\..\bin\core\eed.pdb" /debug /machine:I386 /out:"..\..\bin\core\eed.dll" /libpath:"..\..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ee - Win32 Release"
# Name "ee - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\DbgTestBase.cpp
# End Source File
# Begin Source File

SOURCE=.\eecase.cpp
# End Source File
# Begin Source File

SOURCE=.\eecasepp.cpp
# End Source File
# Begin Source File

SOURCE=.\eespec.cpp
# End Source File
# Begin Source File

SOURCE=.\eesub.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\DbgTestBase.h
# End Source File
# Begin Source File

SOURCE=.\eecase.h
# End Source File
# Begin Source File

SOURCE=.\eecasepp.h
# End Source File
# Begin Source File

SOURCE=.\eespec.h
# End Source File
# Begin Source File

SOURCE=.\eesub.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "target sources"

# PROP Default_Filter ""
# Begin Group "ee"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\base1.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\base1.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\base2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\base2.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\derived1.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\derived1.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\dll.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\dll.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\dll.mak
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\ee.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\ee.mak
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\global.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\global.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\pch.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\pch.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\struct1.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\struct1.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\union1.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee\union1.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Expr Eval PP"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\bin\Core\src\ExprEval PP\ExprEval PP.cpp"
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "ee2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\bin\Core\src\ee2\ee2.mak
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\bin\Core\src\ee2\eetest.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# End Target
# End Project
