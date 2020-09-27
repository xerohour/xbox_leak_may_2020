VSS=\\tkatgfsa01\vss\win32\ss.exe
EDBS=XboxDll.edb XboxDll_A.edb XboxDll_B.edb XboxDll_C.edb
VSS_EDB_DIR="$$/Xbox Localization/SW/VC6_addon/Jpn/EDB/"

Source\IDE\DevDbg.Pkg:  $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\pkgs\Dbg\obj\i386\DevDbg.Pkg
      Copy $? $(@D)

Source\IDE\DevBld.Pkg:  $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\pkgs\bld\src\obj\i386\Devbld.Pkg
      Copy $? $(@D)

Source\XslWz.Awx:       $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\wizards\Xslwiz\obj\i386\XslWz.Awx
      Copy $? $(@D)

Source\IDE\XbgWz.Awx:   $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\wizards\Xbgwiz\obj\i386\XbgWz.Awx
      Copy $? $(@D)

Source\DevShl.Dll:      $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\shell\obj\i386\Devshl.Dll
      Copy $? $(@D)

Source\IDE\DevXbox.Pkg: $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\pkgs\bld\add_ons\Xbox\obj\i386\DevXbox.Pkg
      Copy $? $(@D)

Source\ShXbox.Dll:      $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\debugger\shXbox\osdebug\obj\i386\shXbox.Dll
      Copy $? $(@D)

Source\IDE\MfcApWz.Dll: $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\wizards\appwiz\obj\i386\MfcApWz.Dll
      Copy $? $(@D)

Source\TlXbox.Dll:      $(_NTDRIVE)$(_NTROOT)\Private\VC6AddOn\IDE\debugger\tlxbox\obj\i386\tlxbox.dll
      Copy $? $(@D)

# LocStudio needs to be called once, ignoring the results, so it'll update the .Edb with the
# current path information. It's then called again to do the actual build.

$(EDBS):
      $(VSS) get $(VSS_EDB_DIR)$@ -W -I-

Target\XslWz.Awx:      Pre XboxDLL_C.Edb Source\XslWz.Awx
      Copy XboxDll_C.Edb Source
      -LocStudio\lscmd /U /G %cd%\Source\XboxDLL_C.Edb /S %cd%\Source /T %cd%\Target
      LocStudio\lscmd /U /G %cd%\Source\XboxDLL_C.Edb /S %cd%\Source /T %cd%\Target /O %cd%\Result\Result.Txt
      BinPlace -b Jpn Target\XslWz.Awx

Target\DevShl.Dll:     Pre XboxDLL_B.Edb Source\DevShl.Dll
      Copy XboxDll_B.Edb Source
      -LocStudio\lscmd /U /G %cd%\Source\XboxDLL_B.Edb /S %cd%\Source /T %cd%\Target
      LocStudio\lscmd /U /G %cd%\Source\XboxDLL_B.Edb /S %cd%\Source /T %cd%\Target /O %cd%\Result\Result.Txt
      BinPlace -b Jpn Target\DevShl.Dll

Target\IDE\DevDbg.Pkg Target\IDE\DevBld.Pkg: Pre XboxDLL_A.Edb Source\IDE\DevDbg.Pkg Source\IDE\DevBld.Pkg
      Copy XboxDll_A.Edb Source
      -LocStudio\lscmd /U /G %cd%\Source\XboxDLL_A.Edb /S %cd%\Source /T %cd%\Target
      LocStudio\lscmd /U /G %cd%\Source\XboxDLL_A.Edb /S %cd%\Source /T %cd%\Target /O %cd%\Result\Result.Txt
      BinPlace -b Jpn Target\IDE\DevDbg.Pkg
      BinPlace -b Jpn Target\IDE\DevBld.Pkg

#VCMunge\Jpn\MfcApWz\Bin\MfcApWz.Stb: VCMunge\Jpn\MfcApWz\DllMain.Cpp VCMunge\Jpn\MfcApWz\Res\Script1.Rc
#      cd VCMunge\Jpn\MfcApWz
#      -@md Release > nul 2>&1
#      cl.exe /nologo -I$(BASEDIR)\Public\WSDK\Inc /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" \
#             /Fp".\Release/MfcApwz.pch" /YX /Fo".\Release/" /c "dllmain.cpp"
#      rc.exe /l 0x411 -i$(BASEDIR)\Public\WSDK\Inc -i$(BASEDIR)\Public\WSDK\Inc\ATLMfc \
#             /fo".\Release/script1.res" /i "res" /d "NDEBUG" .\res\script1.rc
#      -@md ..\Bin
#      link.exe /nologo /libpath:$(BASEDIR)\Public\WSDK\Lib /subsystem:windows /dll /incremental:no \
#               /pdb:".\Release/MfcApwz.pdb" /out:"../bin/MfcApwz.stb" ".\Release\dllmain.obj" ".\Release\script1.res"
#      cd ..\..\..

Target\IDE\DevXbox.Pkg Target\IDE\XbgWz.Awx Target\ShXbox.Dll Target\MfcApWz.Dll Target\TlXbox.Dll: Pre \
                                           Source\IDE\DevXbox.Pkg Source\IDE\XbgWz.Awx Source\ShXbox.Dll \
                                           Source\IDE\MfcApWz.Dll Source\TlXbox.Dll XboxDLL.Edb \
                                           VCMunge\Jpn\MfcApWz\Bin\MfcApWz.Stb
      Copy XboxDll.Edb Source
      -LocStudio\lscmd /U /G %cd%\Source\XboxDLL.Edb /S %cd%\Source /T %cd%\Target
      LocStudio\lscmd /U /G %cd%\Source\XboxDLL.Edb /S %cd%\Source /T %cd%\Target /O %cd%\Result\Result.Txt
      Copy Target\IDE\MfcApWz.Dll Target\MfcApWz.Dll
      VCMunge\Tools\rescopy VCMunge\Jpn\Bin\MfcApwz.stb Target\MfcApwz.dll "TEMPLATE":all
      BinPlace -b Jpn Target\IDE\DevXbox.Pkg
      BinPlace -b Jpn Target\IDE\XbgWz.Awx
      BinPlace -b Jpn Target\ShXbox.Dll
      BinPlace -b Jpn Target\MfcApWz.Dll
      BinPlace -b Jpn Target\TlXbox.Dll

!if $(FREEBUILD)
DummyTarget: Target\XslWz.Awx Target\DevShl.Dll Target\IDE\DevDbg.Pkg Target\IDE\DevBld.Pkg Target\IDE\DevXbox.Pkg \
             Target\IDE\XbgWz.Awx Target\ShXbox.Dll Target\MfcApWz.Dll Target\TlXbox.Dll
      Touch DummyTarget
!else
DummyTarget:
!endif

Pre:
      -@md Source\IDE > nul 2>&1
      -@md Target\IDE > nul 2>&1
      -@md Result > nul 2>&1
      -@Del Result\Result.Txt
