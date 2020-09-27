@goto Run
:Help
@echo Run performance %3 tests series %2 times saving results of the last %2-1 runs
@echo   %1.txt has the build log
@echo   %1.dat has the performance data for import
@echo   %1.dsk has the disk usage info (files and sizes) for import
@echo .
:Usage
@echo Usage: Testrun.bat  version times batchfile [optional additional switches]
@echo ie testrun.bat v3v3v322 5 v3run -vc_ver:IDE30Batch30Lib30 -cafekey:CAFE_v3v3v3 -Build_Wait:"120,120,60,60" -DBG_Wait:"9,4"
@goto exit
:Run
@if  '%1'=='/?' goto Help
@if  '%1'==''   goto Help
@if  '%2'==''   goto Help
@if  '%3'==''   goto Help
@echo ********* Initialize test run ***************
@copy VC_Data.dat  VC_Data.xxx
@del  vcdisk.txt
@title "%1    First Run"
@call %3.bat -run %4 %5 %6 %7 %8 %9
@echo ********* toss first run's results **********
@del  VC_Data.dat
@del  vcbldlog.txt
@del  vcdbglog.txt
@copy vcdisk.txt   %1.dsk
@rem ********* How many tests should I run? ******
@if  '%2'=='1'  goto one
@if  '%2'=='2'  goto two
@if  '%2'=='3'  goto three
@if  '%2'=='4'  goto four
@if  '%2'=='5'  goto five
@if  '%2'=='6'  goto six
@if  '%2'=='7'  goto seven
@if  '%2'=='8'  goto eight
@if  '%2'=='9'  goto nine
@if  '%2'=='10' goto ten
@if  '%2'=='11' goto eleven
@if  '%2'=='12' goto twelve
@if  '%2'=='13' goto thirteen
@if  '%2'=='14' goto fourteen
@if  '%2'=='15' goto fifteen
@title "%1   16 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:fifteen
@title "%1   15 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:fourteen
@title "%1   14 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:thirteen
@title "%1   13 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:twelve
@title "%1   12 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:eleven
@title "%1   11 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:ten
@title "%1   10 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:nine
@title "%1    9 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:eight
@title "%1    8 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:seven
@title "%1    7 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:six
@title "%1    6 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:five
@title "%1    5 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:four
@title "%1    4 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:three
@title "%1    3 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:two
@title "%1    2 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
:one
@title "%1    1 left"
@call %3.bat -run %4 %5 %6 %7 %8 %9
@echo ******* save testrun results %1.txt,.dat,.dsk **
@copy vcbldlog.txt %1.txt
@copy vcdbglog.txt %1.dbg
@copy VC_Data.dat  %1.dat
@copy VC_Data.xxx  VC_Data.dat
@goto exit
:exit
