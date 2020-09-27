rem Release today's build

rem move build to release share
rem move /y f:\pvtrelease\usa\latest.txt f:\release\usa\latest.txt
rem move f:\pvtrelease\usa\%_BUILDVER% f:\release\usa\%_BUILDVER%

rem Allow Users to Execute Setup
@rem resource kit must be installed for cacls
cacls f:\release\usa\%_BUILDVER%\*.exe /E /G "XBUILDS\Xbuilds-Release":R

rem add symbols to symsrv
rem to access, set _NT_SYMBOL_PATH=symsrv*symsrv.dll*\\xbuilds\symbols
call symstore add /r /p /f \\xbuilds\release\usa\%_BUILDVER%\checked\*.pdb /s \\xbuilds\symbols /t Xbox /v "%_BUILDVER% CHK"
call symstore add /r /p /f \\xbuilds\release\usa\%_BUILDVER%\free\*.pdb    /s \\xbuilds\symbols /t Xbox /v "%_BUILDVER% FRE"

rem delete refs with call symstore del /i <transaction id> /s d:\symbols
rem transaction ids can be found in \\xbuilds\symbols\000admin\server.txt

rem store symbols for future generations
rem this will take about 20 minutes
cd /d %XBLD_REL%\%_BUILDVER%
call wzzip -rp \\xdb01\symbolarchive\%_BUILDVER%_symbols.zip *.pdb
