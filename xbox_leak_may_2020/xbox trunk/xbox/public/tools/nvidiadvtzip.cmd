if . == .%1 goto usage

rd /s /q %TEMP%\dvt%1
mkdir %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\boot\xboxrom.bin %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\xboxkrnl.exe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\xboxkrnl.pdb %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\recovery.iso %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\devkit\xbdm.dll %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\devkit\xbdm.pdb %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\devkit\cydrive.exe %TEMP%\dvt%1\cydrive.dxt
copy \\xbuilds\release\usa\%1\checked\dump\default.* %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\dump\benmark.* %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\dump\tricpp.* %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\dump\hwharness.xbe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\dump\hwharness.exe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\dump\hwharness.pdb %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\d3d8d.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\inc\d3d8.h %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\inc\d3d8types.h %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\dsoundd.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\inc\dsound.h %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\xapilibd.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\xboxkrnl.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\xbdm.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\checked\public\xdk\lib\xnetd.lib %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\mstools\kdextx86.dll %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\idw\xbflash.exe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\idw\xbsetcfg.exe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\idw\xboxdbg.dll %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\idw\imagebld.exe %TEMP%\dvt%1
copy \\xbuilds\release\usa\%1\free\idw\gdfimage.exe %TEMP%\dvt%1
start /wait winzip %TEMP%\dvt%1
rd /s /q %TEMP%\dvt%1
goto end

:usage
nvidiadvtzip <bldnumber>

:end
