@if NOT DEFINED _echo echo off

REM ****************************
REM *** DON'T EDIT THIS FILE ***
REM ****************************

REM *************************************************************************
REM *
REM * SuckNT.cmd
REM *
REM * Defaults:
REM *   SUCKNT_BuildType	- Default build type (Fre or Chk).  Defaults to Fre.
REM *   SUCKNT_Flavor		- Default flavor (Wks, Srv, or Ent).  Defaults to Srv.
REM *   SUCKNT_NoReplace	- If defined sets default to create directories with
REM *						  build number in them instead of replacing files.
REM *	SUCKNT_SymbolsRoot	- Default root directory for symbols.
REM *   SUCKNT_ToolsRoot	- Default root directory for mstools and idw.
REM *						  Defaults to %windir%.
REM *
REM *************************************************************************

setlocal
if "%1" == "-?" goto USAGE
if NOT DEFINED _NTBinDir set _NTBinDir=\nt& echo Defaulting _NTBinDir to \nt

set _ReqBldNum=
set _ReqBuildType=
set _ReqFlavor=
set _SuckSomething=
set _SuckWithSuck=
set _SuckMST=
set _SuckIDW=
set _SuckPub=
set _SuckPubTools=
set _SuckPriv=
set _Replace=
set _CopiedEch=
set _CopiedSuck=
set _NT4Compat=
set _sucknt_test=
set _AltBldNum=
set _BuildType=%SUCKNT_BuildType%
set _Flavor=%SUCKNT_Flavor%
if NOT DEFINED _BuildType set _BuildType=Fre
if NOT DEFINED _Flavor set _Flavor=Srv

REM *************************************************************************
REM * Determine the specific command
REM *************************************************************************

REM Recursively get commands.
:GETCMD
if /i "%1" == "BLD"        	 set _SuckSomething=1& set _SuckMST=1& set _SuckIDW=1& set _SuckPub=1& shift& goto GETCMD
if /i "%1" == "PRIV"       	 set _SuckSomething=1& set _SuckPriv=1& shift& goto GETCMD
if /i "%1" == "PRIVATE"    	 set _SuckSomething=1& set _SuckPriv=1& shift& goto GETCMD
if /i "%1" == "PUB"        	 set _SuckSomething=1& set _SuckPub=1& shift& goto GETCMD
if /i "%1" == "PUBLIC"     	 set _SuckSomething=1& set _SuckPub=1& shift& goto GETCMD
if /i "%1" == "PUBTOOLS"   	 set _SuckSomething=1& set _SuckPubTools=1& shift& goto GETCMD
if /i "%1" == "SYM"        	 set _SuckSomething=1& set _SuckSym=1& shift& goto GETCMD
if /i "%1" == "SYMBOLS"    	 set _SuckSomething=1& set _SuckSym=1& shift& goto GETCMD
if /i "%1" == "IDW"        	 set _SuckSomething=1& set _SuckIDW=1& shift& goto GETCMD
if /i "%1" == "MSTOOLS"    	 set _SuckSomething=1& set _SuckMST=1& shift& goto GETCMD
if /i "%1" == "TOOLS"      	 set _SuckSomething=1& set _SuckMST=1& set _SuckIDW=1& shift& goto GETCMD
if /i "%1" == "FRE"        	 set _BuildType=Fre& shift& goto GETCMD
if /i "%1" == "FREE"       	 set _BuildType=Fre& shift& goto GETCMD
if /i "%1" == "CHK"        	 set _BuildType=Chk& shift& goto GETCMD
if /i "%1" == "CHECK"      	 set _BuildType=Chk& shift& goto GETCMD
if /i "%1" == "ENT"        	 set _Flavor=Ent& shift& goto GETCMD
if /i "%1" == "SRV"        	 set _Flavor=Srv& shift& goto GETCMD
if /i "%1" == "WKS"        	 set _Flavor=Wks& shift& goto GETCMD
if /i "%1" == "/r"         	 set _Replace=1& shift& goto GETCMD
if /i "%1" == "/-r"        	 set _Replace=& shift& goto GETCMD
if /i "%1" == "/4"         	 set _NT4Compat=1& shift& goto GETCMD
if /i "%1" == "/ALT"       	 set _AltBldNum=%2& shift& shift& goto GETCMD
if /i "%1" == "/TOOLSROOT" 	 set SUCKNT_ToolsRoot=%2& shift& shift& goto GETCMD
if /i "%1" == "/SYMBOLSROOT" set SUCKNT_SymbolsRoot=%2& shift& shift& goto GETCMD
if /i "%1" == "/TEST"      	 set _sucknt_test=1& shift& goto GETCMD
if NOT DEFINED _SuckSomething echo *** No known command specified.& goto USAGE

REM Translate commands into components.
if DEFINED _SuckMST      set _SuckWithSuck=1& set _ReqBldNum=1& set _ReqBuildType=1& set _ReqFlavor=1
if DEFINED _SuckIDW      set _SuckWithSuck=1& set _ReqBldNum=1& set _ReqBuildType=1& set _ReqFlavor=1
if DEFINED _SuckPub      set _SuckWithSuck=1& set _ReqBldNum=1& set _ReqBuildType=1
if DEFINED _SuckPubTools set _ReqBldNum=1& set _ReqBuildType=1
if DEFINED _SuckSym      set _SuckWithSuck=1& set _ReqBldNum=1& set _ReqBuildType=1& set _ReqFlavor=1
if DEFINED _SuckPriv     set _ReqBuildType=1

REM *************************************************************************
REM * Determine the build number
REM *************************************************************************

if NOT DEFINED _ReqBldNum goto BLDNUMDONE
set _BldNum=%1
if NOT DEFINED _BldNum echo *** No build number specified.& goto USAGE
Shift
:BLDNUMDONE

REM *************************************************************************
REM * Parse commands that can appear at the end
REM *************************************************************************

:GETCMD2
if /i "%1" == "/r"           set _Replace=1& shift& goto GETCMD2
if /i "%1" == "/-r"          set _Replace=& shift& goto GETCMD2
if /i "%1" == "/4"           set _NT4Compat=1& shift& goto GETCMD2
if /i "%1" == "/ALT"         set _AltBldNum=%2& shift& shift& goto GETCMD2
if /i "%1" == "/TOOLSROOT"   set SUCKNT_ToolsRoot=%2& shift& shift& goto GETCMD2
if /i "%1" == "/SYMBOLSROOT" set SUCKNT_SymbolsRoot=%2& shift& shift& goto GETCMD2
if /i "%1" == "/TEST"        set _sucknt_test=1& shift& goto GETCMD2
if /i "%1" == "FRE"          set _BuildType=Fre& shift& goto GETCMD2
if /i "%1" == "FREE"         set _BuildType=Fre& shift& goto GETCMD2
if /i "%1" == "CHK"          set _BuildType=Chk& shift& goto GETCMD2
if /i "%1" == "CHECK"        set _BuildType=Chk& shift& goto GETCMD2
if /i "%1" == "ENT"          set _Flavor=Ent& shift& goto GETCMD2
if /i "%1" == "SRV"          set _Flavor=Srv& shift& goto GETCMD2
if /i "%1" == "WKS"          set _Flavor=Wks& shift& goto GETCMD2

if NOT DEFINED _Replace if NOT DEFINED SUCKNT_NoReplace set _Replace=1
if NOT DEFINED _AltBldNum set _AltBldNum=%_BldNum%
if NOT DEFINED SUCKNT_ToolsRoot set SUCKNT_ToolsRoot=%WinDir%
if NOT DEFINED SUCKNT_SymbolsRoot set SUCKNT_SymbolsRoot=%WinDir%

REM *************************************************************************
REM * Setup copy commands
REM *************************************************************************

set _XCopyCmd=xcopy /devrksf
set _XCopyNoDirCmd=xcopy /dvrkf
set _CopyCmd=copy
set _Tools_CopyCmd=copy
if NOT DEFINED _NT4Compat (
    set _XCopyCmd=%_XCopyCmd%y
    set _XCopyNoDirCmd=%_XCopyNoDirCmd%y
    set _CopyCmd=%_CopyCmd% /y
    set _Tools_CopyCmd=%_Tools_CopyCmd% /y
)
if DEFINED _sucknt_test (
    set _XCopyCmd=echo %_XCopyCmd%
    set _XCopyNoDirCmd=echo %_XCopyNoDirCmd%
    set _CopyCmd=echo %_CopyCmd%
)

echo.
if DEFINED _ReqBldNum echo *** Preparing to suck the following from build %_BldNum% (%_BuildType% %_Flavor%):
if DEFINED _SuckMST      echo ***   MSTools
if DEFINED _SuckIDW      echo ***   IDW
if DEFINED _SuckPubTools echo ***   Public\Tools
if DEFINED _SuckPub      echo ***   Public
if DEFINED _SuckSym      echo ***   Symbols
if DEFINED _SuckPriv echo *** Preparing to suck the following from the build machine (%_BuildType%):
if DEFINED _SuckPriv     echo ***   Private
if DEFINED _Replace echo *** Replacing existing directories
if NOT DEFINED _Replace echo *** Not replacing existing directories
if NOT "%_SuckMST%%_SuckIDW%" == "" echo *** IDW and MSTools root directory = %SUCKNT_ToolsRoot%
if NOT "%_SuckSym%" == "" echo *** SYMBOLS root directory = %SUCKNT_SymbolsRoot%
if DEFINED _AltBldNum if "%_AltBldNum%" NEQ "%_BldNum%" echo *** Alternate build = %_AltBldNum%
echo.

REM *************************************************************************
REM * Copy PRIVATE directories
REM *************************************************************************

if NOT DEFINED _SuckPriv goto SUCKPRIVDONE

set _PrivMachName=%PROCESSOR_ARCHITECTURE%%_BuildType%
set _PrivShareName=\\%_PrivMachName%\sources

@echo on
for %%d in (developr inc inc\net net\inc ntos\inc windows\inc windows\inc16) do if not exist %_NTBinDir%\private\%%d\slm.ini %_XCopyNoDirCmd% %_PrivShareName%\private\%%d %_NTBinDir%\private\%%d\
%_XCopyNoDirCmd% %_PrivShareName%\private\windows\nlsmsg\winerror.w %_NTBinDir%\private\windows\nlsmsg\
@if NOT DEFINED _echo echo off

:SUCKPRIVDONE

REM *************************************************************************
REM * Copy PUBLIC\TOOLS directory
REM *************************************************************************

if NOT DEFINED _SuckPubTools goto SUCKPUBTOOLSDONE

set _SrcDir=\\ntbuilds\release\usa\%_BldNum%\%PROCESSOR_ARCHITECTURE%\%_BuildType%.pub\tools
set _NTRelicDir=\\ntrelic\usa.%_BldNum%.%PROCESSOR_ARCHITECTURE%\%_BuildType%.pub\tools
set _NTRelic2Dir=\\ntrelic2\usa.%_BldNum%.%PROCESSOR_ARCHITECTURE%\%_BuildType%.pub\tools

echo Checking %_SrcDir%
if NOT EXIST %_SrcDir%\ goto RELICPUBTOOLS
@echo on
%_XCopyCmd% %_SrcDir% %_NTBinDir%\public\tools\
@if NOT DEFINED _echo echo off
goto SUCKPUBTOOLSDONE

:RELICPUBTOOLS
echo Checking %_NTRelicDir%
if NOT EXIST %_NTRelicDir%\ goto RELIC2PUBTOOLS
@echo on
%_XCopyCmd% %_NTRelicDir% %_NTBinDir%\public\tools\
@if NOT DEFINED _echo echo off
goto SUCKPUBTOOLSDONE

:RELIC2PUBTOOLS
echo Checking %_NTRelic2Dir%
if NOT EXIST %_NTRelic2Dir%\ goto NOPUBTOOLS
@echo on
%_XCopyCmd% %_NTRelic2Dir% %_NTBinDir%\public\tools\
@if NOT DEFINED _echo echo off
goto SUCKPUBTOOLSDONE

:NOPUBTOOLS
echo No pubtools found.

:SUCKPUBTOOLSDONE

REM *************************************************************************
REM * Create a command script and execute it
REM *************************************************************************

if NOT DEFINED _SuckWithSuck goto SUCKDONE

set _SrcMach=\\ntblds%%i\usa.%_AltBldNum%.%PROCESSOR_ARCHITECTURE%
set _NTRelic=\\ntrelic\usa.%_AltBldNum%.%PROCESSOR_ARCHITECTURE%
set _NTRelic2=\\ntrelic2\usa.%_AltBldNum%.%PROCESSOR_ARCHITECTURE%

if DEFINED _Replace (
    set _MSTDst=%SUCKNT_ToolsRoot%\mstools
    set _IDWDst=%SUCKNT_ToolsRoot%\idw
    set _PubDst=%_NTBinDir%\public
    set _SymDst=%SUCKNT_SymbolsRoot%\symbols
) else (
    set _MSTDst=%SUCKNT_ToolsRoot%\mstools%_BldNum%
    set _IDWDst=%SUCKNT_ToolsRoot%\idw%_BldNum%
    set _PubDst=%_NTBinDir%\pub%_BldNum%
    set _SymDst=%SUCKNT_SymbolsRoot%\symbols%_BldNum%
)

echo Checking \\ntbuilds\release\usa\%_BldNum%\%PROCESSOR_ARCHITECTURE%\%_BuildType%.%_Flavor%
if EXIST \\ntbuilds\release\usa\%_BldNum%\%PROCESSOR_ARCHITECTURE%\%_BuildType%.%_Flavor%\ (
    set _UseNTRelic=
    set _UseNTRelic2=
) else (
    echo Checking %_NTRelic%
    if EXIST %_NTRelic%\ (
        set _UseNTRelic=1
        set _UseNTRelic2=
    ) else (
        echo Checking %_NTRelic2%
        if EXIST %_NTRelic2%\ (
            set _UseNTRelic=
            set _UseNTRelic2=1
        ) else (
            echo Build %_BldNum% not available for %PROCESSOR_ARCHITECTURE%
            goto DONE
        )
    )
)

set _MSTSrc=%_SrcMach%\%_BuildType%.%_Flavor%\mstools
set _MSTRelic=%_NTRelic%\%_BuildType%.%_Flavor%\mstools
set _MSTRelic2=%_NTRelic2%\%_BuildType%.%_Flavor%\mstools
set _MSTScp=%TEMP%\_suckmst.cmd

set _IDWSrc=%_SrcMach%\%_BuildType%.%_Flavor%\idw
set _IDWRelic=%_NTRelic%\%_BuildType%.%_Flavor%\idw
set _IDWRelic2=%_NTRelic2%\%_BuildType%.%_Flavor%\idw
set _IDWScp=%TEMP%\_suckidw.cmd

set _PubSrc=%_SrcMach%\%_BuildType%.pub
set _PubRelic=%_NTRelic%\%_BuildType%.pub
set _PubRelic2=%_NTRelic2%\%_BuildType%.pub
set _PubScp=%TEMP%\_suckpub.cmd

set _SymSrc=%_SrcMach%\%_BuildType%.%_Flavor%\symbols
set _SymRelic=%_NTRelic%\%_BuildType%.%_Flavor%\symbols
set _SymRelic2=%_NTRelic2%\%_BuildType%.%_Flavor%\symbols
set _SymScp=%TEMP%\_sucksym.cmd
if NOT DEFINED _NT4Compat (
    set _SymSrc=%SymSrc%\retail
    set _SymRelic=%_SymRelic%\retail
    set _SymRelic2=%_SymRelic2%\retail
)

REM
REM Make sure we have the tools we need.
REM
if DEFINED _UseNTRelic (
    set _ToolSrc=\\ntrelic\usa.%_AltBldNum%.%PROCESSOR_ARCHITECTURE%
) else if DEFINED _UseNTRelic2 (
    set _ToolSrc=\\ntrelic2\usa.%_AltBldNum%.%PROCESSOR_ARCHITECTURE%
) else (
    set _ToolSrc=\\ntbuilds\release\usa\%_BldNum%\%PROCESSOR_ARCHITECTURE%
)
set _echsrc=%_ToolSrc%\%_BuildType%.%_Flavor%\idw\ech.exe
set _sucksrc=%_ToolSrc%\%_BuildType%.%_Flavor%\idw\suck.exe
set _echfile=%TEMP%\ech.exe
set _suckfile=%TEMP%\suck.exe
if EXIST %_echsrc% (set _echexists=1) else (set _echexists=)
if EXIST %_sucksrc% (set _suckexists=1) else (set _suckexists=)
if NOT DEFINED _echexists if NOT DEFINED _suckexists NO_FILES
%_Tools_CopyCmd% %_echsrc% %_echfile% >%TEMP%\copyech.log
%_Tools_CopyCmd% %_sucksrc% %_suckfile% >%TEMP%\copysuck.log
set _CopiedEch=1
set _CopiedSuck=1
if %_BldNum% GEQ 1738 (
    set _SuckCmd=suck -pz
) else (
    set _SuckCmd=suck -qz
)

REM
REM Create the output directories if they don't exist.
REM
if DEFINED _SuckMST if NOT exist %_MSTDst% md %_MSTDst%
if DEFINED _SuckIDW if NOT exist %_IDWDst% md %_IDWDst%
if DEFINED _SuckPub if NOT exist %_PubDst% md %_PubDst%
if DEFINED _SuckSym if NOT exist %_SymDst% md %_SymDst%

REM
REM Create the command scripts.
REM
echo setlocal > %_MSTScp%
echo setlocal > %_IDWScp%
echo setlocal > %_PubScp%
echo setlocal > %_SymScp%
echo path %%temp%%;%%path%% >> %_MSTScp%
echo path %%temp%%;%%path%% >> %_IDWScp%
echo path %%temp%%;%%path%% >> %_PubScp%
echo path %%temp%%;%%path%% >> %_SymScp%
echo @echo on >> %_MSTScp%
echo @echo on >> %_IDWScp%
echo @echo on >> %_PubScp%
echo @echo on >> %_SymScp%
%_echfile% %_SuckCmd% %_MSTDst% >> %_MSTScp%
%_echfile% %_SuckCmd% %_IDWDst% >> %_IDWScp%
%_echfile% %_SuckCmd% %_PubDst% >> %_PubScp%
%_echfile% %_SuckCmd% %_SymDst% >> %_SymScp%
set _SourcesFound=
if DEFINED _UseNTRelic (
    %_echfile% %_MSTRelic%>> %_MSTScp%& %_echfile% %_IDWRelic%>> %_IDWScp%& %_echfile% %_PubRelic%>> %_PubScp%& %_echfile% %_SymRelic%>> %_SymScp%
    set _SourcesFound=1
) else if DEFINED _UseNTRelic2 (
    %_echfile% %_MSTRelic2%>> %_MSTScp%& %_echfile% %_IDWRelic2%>> %_IDWScp%& %_echfile% %_PubRelic2%>> %_PubScp%& %_echfile% %_SymRelic2%>> %_SymScp%
    set _SourcesFound=1
) else (
    for /L %%i in (1,1,6) do if exist %_SrcMach%\ set _SourcesFound=1& %_echfile% %_MSTSrc%>> %_MSTScp%& %_echfile% %_IDWSrc%>> %_IDWScp%& %_echfile% %_PubSrc%>> %_PubScp%& %_echfile% %_SymSrc%>> %_SymScp%
    if exist %_IDWRelic% set _SourcesFound=1& %_echfile% %_MSTRelic%>> %_MSTScp%& %_echfile% %_IDWRelic%>> %_IDWScp%& %_echfile% %_PubRelic%>> %_PubScp%& %_echfile% %_SymRelic%>> %_SymScp%
    if exist %_IDWRelic2% set _SourcesFound=1& %_echfile% %_MSTRelic2%>> %_MSTScp%& %_echfile% %_IDWRelic2%>> %_IDWScp%& %_echfile% %_PubRelic2%>> %_PubScp%& %_echfile% %_SymRelic2%>> %_SymScp%
)
%_echfile% ; >> %_MSTScp%
%_echfile% ; >> %_IDWScp%
%_echfile% ; >> %_PubScp%
%_echfile% ; >> %_SymScp%
echo @if NOT DEFINED _echo echo off >> %_MSTScp%
echo @if NOT DEFINED _echo echo off >> %_IDWScp%
echo @if NOT DEFINED _echo echo off >> %_PubScp%
echo @if NOT DEFINED _echo echo off >> %_SymScp%
echo endlocal >> %_MSTScp%
echo endlocal >> %_IDWScp%
echo endlocal >> %_PubScp%
echo endlocal >> %_SymScp%

REM
REM If no sources found, don't do anything.
REM
if NOT DEFINED _SourcesFound (
    echo No source servers found for the requested files.
    goto AFTERSUCK
)

REM
REM Execute the command scripts.
REM
if NOT DEFINED _sucknt_test if DEFINED _SuckMST call %_MSTScp%
if NOT DEFINED _sucknt_test if DEFINED _SuckIDW call %_IDWScp%
if NOT DEFINED _sucknt_test if DEFINED _SuckPub call %_PubScp%
if NOT DEFINED _sucknt_test if DEFINED _SuckSym call %_SymScp%
:AFTERSUCK

if DEFINED _sucknt_test (
    echo.
    echo Temporary files may be found at %TEMP%
    Pause
)

del /q %_MSTScp% %_IDWScp% %_PubScp% %_SymScp%
if EXIST suck.ini del /q suck.ini

if DEFINED _CopiedEch  del /q %_echfile%
if DEFINED _CopiedSuck del /q %_suckfile%
if EXIST %TEMP%\copyech.log del /q %TEMP%\copyech.log
if EXIST %TEMP%\copysuck.log del /q %TEMP%\copysuck.log

:SUCKDONE

goto DONE

REM *************************************************************************
REM * Usage
REM *************************************************************************

:USAGE

echo.
echo SuckNT {commands} [options]
echo.
echo    SuckNT {cmd1} [Fre/Chk]
echo    SuckNT {cmd2} {build#} [Fre/Chk] [/r OR /-r] [/4] [/ALT build#]
echo    SuckNT {cmd3} {build#} [Fre/Chk] [Ent/Srv/Wks] [/r OR /-r] [/4] [/ALT build#]
echo.
echo    Copies files from NT servers.
echo.
echo    {commands} - One or more of the following:
echo        PRIVATE -  {cmd1} Private headers required to build the cluster.
echo        PRIV -     {cmd1} Synonym for PRIVATE.
echo        PUBLIC -   {cmd2} Public headers, libs, etc. from a specific NT build.
echo        PUB -      {cmd2} Synonmym for PUBLIC
echo        PUBTOOLS - {cmd2} Public\tools directory from a specific NT build.
echo        SYMBOLS -  {cmd3} Symbols from a specific NT build.
echo        SYM -      {cmd3} Synonym for SYMBOLS.
echo        MSTOOLS -  {cmd3} MSTools directory from a specific NT build.
echo        IDW -      {cmd3} IDW directory from a specific NT build.
echo        TOOLS -    {cmd3} Same as MSTOOLS and IDW.
echo        BLD -      {cmd3} Same as PUB and TOOLS.
echo    /r -  Replace replace existing directory (default).
echo    /-r - Create build-specific directories
echo       (e.g. idw1733, mstools1733, sym1733, pub1733).
echo       If SUCKNT_NoReplace is defined, defaults to /-r.
echo    /4 - Use NT4-compatible commands.
echo    /ALT build# - Alternate build # for accessing \\NTBLDS# or \\NTRELIC*.
echo    /TOOLSROOT dir - Root for IDW and MSTOOLS.  Defaults to SUCKNT_ToolsRoot
echo       if defined, or WINDIR if not.
echo    /SYMBOLSROOT dir - Root for SYMBOLS.  Defaults to SUCKNT_SymbolsRoot
echo       if defined, or WINDIR if not.
echo    /TEST - Don't copy anything, just test the procedure.
echo.
echo Notes:
echo    If _NTBinDir is not set, defaults to \nt.
echo    Uses multiple threads when possible (all but PRIV).
echo    [Fre/Chk] defaults to Fre
echo    [Ent/Srv/Wks] defaults to Srv
goto DONE

:NO_FILES
if "%_echexists%" == "0" echo %_echsrc% does not exist.
if "%_suckexists%" == "0" echo %_sucksrc% does not exist.
echo SuckNT failed.

:DONE
endlocal
