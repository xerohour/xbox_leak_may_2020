@echo off

rem figure total number of functions
@echo Total number of functions:
qgrep -e "#{\\footnote" cafehlp.rtf > fnccnt.out
wc -l fnccnt.out

rem figure number of functions undocumented
@echo Number of undocumented functions:
qgrep -e "appropriate component owner" cafehlp.rtf > unfnccnt.out
wc -l unfnccnt.out
