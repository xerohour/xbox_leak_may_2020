@echo off
echo Running EDITGO sniff test...
cd ..
start cafedrv -stf:editgo.stf %1 %2 %3 %4 %5 %6 %7
cd editgo
