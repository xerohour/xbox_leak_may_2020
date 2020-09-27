if . == .%1 goto usage

rd /s /q %TEMP%\dvtfull%1
mkdir %TEMP%\dvtfull%1
mkdir %TEMP%\dvtfull%1\lib
mkdir %TEMP%\dvtfull%1\include

copy \\xbuilds\release\usa\%1\black\boot\xboxrom.bin %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\boot\xboxrom_1024.bin %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\xboxkrnl.exe %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\xboxkrnl.pdb %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\rec_blk.iso %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\devkit\xbdm.dll %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\devkit\xbdm.pdb %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\devkit\cydrive.exe %TEMP%\dvtfull%1\cydrive.dxt
copy \\xbuilds\release\usa\%1\black\dump\default.* %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\dump\benmark.* %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black\dump\tricpp.* %TEMP%\dvtfull%1

xcopy /y /e \\xbuilds\release\usa\%1\black\public\xdk\inc %TEMP%\dvtfull%1\include
del %TEMP%\dvtfull%1\include\xmetal.h
del %TEMP%\dvtfull%1\include\xrl.h
del %TEMP%\dvtfull%1\include\xonline.h

xcopy /y /e \\xbuilds\release\usa\%1\black\public\xdk\lib %TEMP%\dvtfull%1\lib
xcopy /y /e \\xbuilds\release\usa\%1\black_free\public\xdk\lib %TEMP%\dvtfull%1\lib
del %TEMP%\dvtfull%1\lib\voxcodec*.lib
del %TEMP%\dvtfull%1\lib\imaadpcm*.lib

copy \\xbuilds\release\usa\%1\black_free\mstools\kdextx86.dll %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black_free\idw\xbflash.exe %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black_free\idw\xboxdbg.dll %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black_free\idw\imagebld.exe %TEMP%\dvtfull%1
copy \\xbuilds\release\usa\%1\black_free\idw\gdfimage.exe %TEMP%\dvtfull%1
start /wait winzip %TEMP%\dvtfull%1
goto end

:usage
nvidiadvtfull <bldnumber>

:end
