@echo off
echo Running VRES sniff test...
cd ..
start cafedrv -stf:vres.stf %1 %2 %3 %4 %5 %6 %7
cd vres
