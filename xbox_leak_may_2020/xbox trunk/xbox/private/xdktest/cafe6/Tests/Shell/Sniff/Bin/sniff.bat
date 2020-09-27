@echo off
echo Running VSHELL sniff test...
cd ..
start cafedrv -stf:vshell.stf %1 %2 %3 %4 %5 %6 %7
cd vshell
