for /F %%n in (\\xbuilds\release\usa\latest.txt) do set _BUILDVER=%%n
echo %_BUILDVER%
call perl buildmail.pl -b %_BUILDVER%
copy /y checkins.htm \\xbox\bvt\bvtresults.htm
