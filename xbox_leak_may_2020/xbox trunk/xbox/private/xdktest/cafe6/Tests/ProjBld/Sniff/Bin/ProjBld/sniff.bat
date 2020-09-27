@echo off
echo Running ProjBld sniff test...
cd ..
start cafedrv -stf:ProjBld.stf %1 %2 %3 %4 %5 %6 %7
cd ProjBld
