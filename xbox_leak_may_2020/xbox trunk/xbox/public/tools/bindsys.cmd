@echo off
if NOT "%Verbose%" == "" echo on
if NOT "%_echo%" == ""   echo on
setlocal

REM  Don't bind on checked build machines:
if /i "%ntdebug%" == "" goto OK
if /i NOT "%ntdebug%" == "ntsdnodbg" goto EOF
:OK

REM ---------------------------------------------------------------------------
REM    BINDSYS.CMD - bind the NT binaries
REM
REM ---------------------------------------------------------------------------

if "%bindrive%" == "" goto UseNTTREE
if "%binroot%" == "" goto UseNTTREE
goto UseBinDriveRoot
:UseNTTREE
if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
echo PROCESSOR_ARCHITECTURE not defined.
goto EOF

:X86
set binplacedir=%_nt386tree%
if DEFINED IA64 set binplacedir=%_ntia64tree%
goto GotBinplaceDir

:ALPHA
set binplacedir=%_ntalphatree%
if DEFINED AXP64 set binplacedir=%_ntaxp64tree%
goto GotBinplaceDir

:UseBinDriveRoot
set binplacedir=%bindrive%%binroot%

:GotBinplaceDir
pushd %binplacedir%

REM ------------------------------------------------
REM  Exclude these crypto binaries:
REM ------------------------------------------------

set Excludeexe=-x ntoskrnl.exe -x ntkrnlmp.exe -x ntkrnlpa.exe -x ntkrpamp.exe

set Excludedll=-x dssbase.dll -x rsabase.dll -x gpkcsp.dll
set Excludedll=%Excludedll% -x slbcsp.dll -x slbcom.dll -x slbkygen.dll -x sspagg.dll
set Excludedll=%Excludedll% -x schannel.dll -x schnl128.dll -x sch128c.dll -x schnlc.dll
set Excludedll=%Excludedll% -x disdnci.dll  -x disdnsu.dll
set Excludedll=%Excludedll% -x scrdaxp.dll -x scrdenrl.dll -x scrdsign.dll -x scrdx86.dll
set Excludedll=%Excludedll% -x xenraxp.dll -x xenroll.dll -x xenrsign.dll -x xenrx86.dll
set Excludedll=%Excludedll% -x kerberos.dll -x msv1_0.dll

if exist System32\*.dll     Bind %Excludedll% -u -s Symbols\system32 -p System32 System32\*.dll
if exist System32\*.exe     Bind %Excludeexe% -u -s Symbols\system32 -p System32 System32\*.exe
if exist System32\*.com     Bind            % -u -s Symbols\system32 -p System32 System32\*.com
if exist *.dll              Bind %Excludedll% -u -s Symbols\retail -p System32 *.dll
if exist *.exe              Bind %Excludeexe% -u -s Symbols\retail -p System32 *.exe
if exist *.cpl              Bind %Excludedll% -u -s Symbols\retail -p System32 *.cpl
if exist *.com              Bind              -u -s Symbols\retail -p System32 *.com
if exist *.ocx              Bind              -u -s Symbols\retail -p System32 *.ocx
if exist *.ax               Bind              -u -s Symbols\retail -p System32 *.ax
if exist *.scr              Bind              -u -s Symbols\retail -p System32 *.scr
if exist *.tsp              Bind              -u -s Symbols\retail -p System32 *.tsp
if exist *.drv              Bind              -u -s Symbols\retail -p System32 *.drv
if exist MSTOOLS\*.dll      Bind %Excludedll% -u -s Symbols\mstools -p MSTools;System32 MSTOOLS\*.dll
if exist MSTOOLS\*.exe      Bind %Excludeexe% -u -s Symbols\mstools -p MSTools;System32 MSTOOLS\*.exe
if exist MSTOOLS\*.com      Bind              -u -s Symbols\mstools -p MSTools;System32 MSTOOLS\*.com
if exist IDW\*.dll          Bind %Excludedll% -u -s Symbols\idw  -p IDW;MSTools;System32 IDW\*.dll
if exist IDW\*.exe          Bind %Excludeexe% -u -s Symbols\idw  -p IDW;MSTools;System32 IDW\*.exe
if exist IDW\*.com          Bind              -u -s Symbols\idw  -p IDW;MSTools;System32 IDW\*.com
if exist Dump\*.dll         Bind %Excludedll% -u -s Symbols\dump -p Dump;IDW;MSTools;System32 Dump\*.dll
if exist Dump\*.exe         Bind %Excludeexe% -u -s Symbols\dump -p Dump;IDW;MSTools;System32 Dump\*.exe
if exist Dump\*.com         Bind              -u -s Symbols\dump -p Dump;IDW;MSTools;System32 Dump\*.com

popd
goto EOF


REM ------------------------------------------------
REM  Display Usage:
REM ------------------------------------------------
:Usage

echo.
echo Usage:  bindsys.cmd
echo.
echo binds the NT binaries. Run during congeal.
echo.

:EOF
endlocal
