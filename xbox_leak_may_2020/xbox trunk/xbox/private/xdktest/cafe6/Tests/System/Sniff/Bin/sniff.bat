@echo off
echo Running SYS sniff test...
cd ..
start cafedrv -stf:sys.stf %1 %2 %3 %4 %5 %6 %7
cd sys
