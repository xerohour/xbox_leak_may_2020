@if '%1'=='/?' goto Help
@if '%1'=='' goto Help
ren vc_perf.dat vc_perf.xxx
call v2key.bat
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
echo ******* toss first run's results **********
del  vc_perf.dat
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
echo ******* Save second run's build log **********
copy vc_bld.txt vc20%1.txt
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
call flush.bat
call ClearLog.bat
sniff_v2 -debug:false  -run -nopost
ren vc_perf.dat  vc20%1.dat
ren vc_perf.xxx  vc_perf.dat
@goto exit
:Help
@echo Run performance tests series 15 times saving results of the last five runs
@echo      VC20####.txt has the build log
@echo      VC20####.dat has the performance data for import
@echo .
:Usage
@echo Usage: V2RunAll.bat  version#
@echo        ie   V2RunAll  4206
@goto exit
:exit
