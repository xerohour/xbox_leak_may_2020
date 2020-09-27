if . == .%1 goto usage

rd /s /q %TEMP%\silver%1
mkdir %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\boot\xboxrom.bin %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\xboxkrnl.exe %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\xboxkrnl.pdb %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\recovery.iso %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\devkit\xbdm.dll %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\devkit\xbdm.pdb %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\devkit\cydrive.exe %TEMP%\silver%1\cydrive.dxt
copy \\xbuilds\release\usa\%1\checked\dump\xshell.xbe %TEMP%\silver%1\default.xbe
copy \\xbuilds\release\usa\%1\checked\dump\xshell.exe %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\checked\dump\xshell.pdb %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\lib\d3d8d.lib %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\include\d3d8.h %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\lib\xapilibd.lib %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\lib\xboxkrnl.lib %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\lib\xbdm.lib %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\sdk\xbox\lib\xnetd.lib %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\free\idw\xboxdbg.dll %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\free\idw\imagebld.exe %TEMP%\silver%1
copy \\xbuilds\release\usa\%1\free\idw\gdfimage.exe %TEMP%\silver%1
start /wait winzip %TEMP%\silver%1
goto end

:usage
nvidiasilverzip <bldnumber>

:end
