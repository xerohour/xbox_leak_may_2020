@if '%1'=='/?' goto Help
@if '%1'=='' goto Help
ren vc_perf.dat vc_perf.xxx
call v3key.bat
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
echo ******* toss first run's results **********
del  vc_perf.dat
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
echo ******* Save second run's build log **********
copy vc_bld.txt vc30%1.txt
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
call flush.bat
call ClearLog.bat
sniff -debug:false -run -nopost
ren vc_perf.dat  vc30%1.dat
ren vc_perf.xxx  vc_perf.dat
@goto exit
:Help
@echo Run performance tests series Six times saving results of the last five runs
@echo      VC30####.txt has the build log
@echo      VC30####.dat has the performance data for import
@echo .
:Usage
@echo Usage: V3RunAll.bat  version#
@echo        ie   V3RunAll  4306
@goto exit
:exit
