@call flush.bat
@call Cleanall.bat
start /w cafedrv -stf:editgo.stf -debug:0 -flush -views:0 -hotkey_thread:0 -debug_thread:0 -newapp -close %1 %2 %3 %4 %5 %6 %7 %8 %9
