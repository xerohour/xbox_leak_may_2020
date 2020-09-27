@rem *** Remember to have Cafe keys set for correct paths etc...***
@goto Run
:Help
@echo Run performance tests series
@echo .
:Usage
@echo Usage: V3series.bat  APPWIZ-or-SPY-or-ALL IDE_ver# BatchTools_ver# [-switches]
@echo        ie   V3series APPWIZ 4306 4301
@echo        ie   V3series SPY    4306 4301 -foo
@echo        note: test must be in ALL UPPER CASE ie "SPY" not "Spy" or "spy".
@echo        note: version numbers must be 4 characters or less.
@goto exit
:Run
@if '%1'=='/?'     goto Help
@if '%1'==''       goto Help
@if '%2'==''       goto Help
@if '%3'==''       goto Help
@
@if '%1'=='APPWIZ' goto All
@if '%1'=='SPY'    goto SPY
@if '%1'=='ALL'    goto All
@
:All
call testrun.bat v3v3v333 10 v3run -Test:"APPWIZ" -vc_ver:IDE%2Batch%3Lib30CRT30MFC30 -cafekey:CAFE_v3v3v3   -Build_Wait:"120,120,60,60" -DBG_Wait:"30,15" %4
copy c:\msvc20\mfc\include\afx.crt c:\msvc20\mfc\include\afx.h
call testrun.bat v3v3v332 10 v3run -Test:"APPWIZ" -vc_ver:IDE%2Batch%3Lib30CRT30MFC20 -cafekey:CAFE_v3v3v332 -Build_Wait:"120,120,60,60" -DBG_Wait:"30,15" %4
copy c:\msvc20\mfc\include\afx.200 c:\msvc20\mfc\include\afx.h
call testrun.bat v3v3v322 10 v3run -Test:"APPWIZ" -vc_ver:IDE%2Batch%3Lib30CRT20MFC20 -cafekey:CAFE_v3v3v322 -Build_Wait:"120,120,60,60" -DBG_Wait:"30,15" %4
call testrun.bat v3v3v222 10 v3run -Test:"APPWIZ" -vc_ver:IDE%2Batch%3Lib20CRT20MFC20 -cafekey:CAFE_v3v3v2   -Build_Wait:"120,120,60,60" -DBG_Wait:"30,15" %4
call testrun.bat v3v2v222 10 v3run -Test:"APPWIZ" -vc_ver:IDE%2Batch20Lib20CRT20MFC20 -cafekey:CAFE_v3v2v2   -Build_Wait:"120,120,60,60" -DBG_Wait:"30,15" %4
:SPY
@if '%1'=='APPWIZ' goto exit
call testrun.bat s3s3s333 3  v3run -Test:"SPY"    -vc_ver:IDE%2Batch%3Lib30CRT30MFC30 -cafekey:CAFE_v3v3v3   -Build_Wait:"1200,90,240,180" -DBG_Wait:"60,30" %4
copy c:\msvc20\mfc\include\afx.crt c:\msvc20\mfc\include\afx.h
call testrun.bat s3s3s332 3  v3run -Test:"SPY"    -vc_ver:IDE%2Batch%3Lib30CRT30MFC20 -cafekey:CAFE_v3v3v332 -Build_Wait:"700,90,240,180" -DBG_Wait:"60,30" %4
copy c:\msvc20\mfc\include\afx.200 c:\msvc20\mfc\include\afx.h
call testrun.bat s3s3s322 3  v3run -Test:"SPY"    -vc_ver:IDE%2Batch%3Lib30CRT20MFC20 -cafekey:CAFE_v3v3v322 -Build_Wait:"700,90,240,180" -DBG_Wait:"60,30" %4
call testrun.bat s3s3s222 3  v3run -Test:"SPY"    -vc_ver:IDE%2Batch%3Lib20CRT20MFC20 -cafekey:CAFE_v3v3v2   -Build_Wait:"700,90,240,180" -DBG_Wait:"60,30" %4
call testrun.bat s3s2s222 3  v3run -Test:"SPY"    -vc_ver:IDE%2Batch20Lib20CRT20MFC20 -cafekey:CAFE_v3v2v2   -Build_Wait:"700,90,240,180" -DBG_Wait:"60,30" %4
@goto exit
:exit
