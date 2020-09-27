@echo off
echo Running EDITOR sniff test...
cd ..
start cafedrv -stf:editor.stf %1 %2 %3 %4 %5 %6 %7
cd editor
