
clean: 
       -del /q $(CRT_FILES1) 2>nul
       -del /q $(CRT_FILES2) 2>nul
       -del /q $(CRT_FILES3) 2>nul

#
# Inference rules.  
#

.SUFFIXES:.h

{..\}.h{$(CRT_INC_PATH)}.h:
   ..\compfile  $** $@

{.\sys}.h{$(CRT_INC_PATH)\sys}.h:
   ..\compfile  $** $@

$(CRT_INC_PATH)\conio.h: ..\conio.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\direct.h: ..\direct.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\dos.h: ..\dos.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\io.h: ..\io.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\malloc.h: ..\malloc.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\process.h: ..\process.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\stdio.h: ..\stdio.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\stdlib.h: ..\stdlib.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\tchar.h: ..\tchar.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\time.h: ..\time.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\wchar.h: ..\wchar.x
   hsplit -ts noxbox -o tmp.x nul -i $** 
   ..\compfile  tmp.x $@
   del tmp.x

$(CRT_INC_PATH)\algorithm : ..\algorithm
	..\compfile $** $@

$(CRT_INC_PATH)\bitset : ..\bitset
	..\compfile $** $@

$(CRT_INC_PATH)\cassert : ..\cassert
	..\compfile  $** $@

$(CRT_INC_PATH)\cctype : ..\cctype
	..\compfile  $** $@

$(CRT_INC_PATH)\cerrno : ..\cerrno
	..\compfile  $** $@

$(CRT_INC_PATH)\cfloat : ..\cfloat
	..\compfile  $** $@

$(CRT_INC_PATH)\ciso646 : ..\ciso646
	..\compfile  $** $@

$(CRT_INC_PATH)\climits : ..\climits
	..\compfile  $** $@

$(CRT_INC_PATH)\clocale : ..\clocale
	..\compfile  $** $@

$(CRT_INC_PATH)\cmath : ..\cmath
	..\compfile  $** $@

$(CRT_INC_PATH)\complex : ..\complex
	..\compfile  $** $@

$(CRT_INC_PATH)\csetjmp : ..\csetjmp
	..\compfile  $** $@

$(CRT_INC_PATH)\csignal : ..\csignal
	..\compfile  $** $@

$(CRT_INC_PATH)\cstdarg : ..\cstdarg
	..\compfile  $** $@

$(CRT_INC_PATH)\cstddef : ..\cstddef
	..\compfile  $** $@

$(CRT_INC_PATH)\cstdio : ..\cstdio
	..\compfile  $** $@

$(CRT_INC_PATH)\cstdlib : ..\cstdlib
	..\compfile  $** $@

$(CRT_INC_PATH)\cstring : ..\cstring
	..\compfile  $** $@

$(CRT_INC_PATH)\ctime : ..\ctime
	..\compfile  $** $@

$(CRT_INC_PATH)\cwctype : ..\cwctype
	..\compfile  $** $@

$(CRT_INC_PATH)\deque : ..\deque
	..\compfile  $** $@

$(CRT_INC_PATH)\exception : ..\exception
	..\compfile  $** $@

$(CRT_INC_PATH)\fstream : ..\fstream
	..\compfile  $** $@

$(CRT_INC_PATH)\functional : ..\functional
	..\compfile  $** $@

$(CRT_INC_PATH)\iomanip : ..\iomanip
	..\compfile  $** $@

$(CRT_INC_PATH)\ios : ..\ios
	..\compfile  $** $@

$(CRT_INC_PATH)\iosfwd : ..\iosfwd
	..\compfile  $** $@

$(CRT_INC_PATH)\iostream : ..\iostream
	..\compfile  $** $@

$(CRT_INC_PATH)\istream : ..\istream
	..\compfile  $** $@

$(CRT_INC_PATH)\iterator : ..\iterator
	..\compfile  $** $@

$(CRT_INC_PATH)\limits : ..\limits
	..\compfile  $** $@

$(CRT_INC_PATH)\list : ..\list
	..\compfile  $** $@

$(CRT_INC_PATH)\locale : ..\locale
	..\compfile  $** $@

$(CRT_INC_PATH)\map : ..\map
	..\compfile  $** $@

$(CRT_INC_PATH)\memory : ..\memory
	..\compfile  $** $@

$(CRT_INC_PATH)\new : ..\new
	..\compfile  $** $@

$(CRT_INC_PATH)\numeric : ..\numeric
	..\compfile  $** $@

$(CRT_INC_PATH)\ostream : ..\ostream
	..\compfile  $** $@

$(CRT_INC_PATH)\queue : ..\queue
	..\compfile  $** $@

$(CRT_INC_PATH)\set : ..\set
	..\compfile  $** $@

$(CRT_INC_PATH)\sstream : ..\sstream 
	..\compfile  $** $@

$(CRT_INC_PATH)\stack : ..\stack
	..\compfile  $** $@

$(CRT_INC_PATH)\stdexcept : ..\stdexcept
	..\compfile  $** $@

$(CRT_INC_PATH)\streambuf : ..\streambuf
	..\compfile  $** $@

$(CRT_INC_PATH)\string : ..\string
	..\compfile  $** $@

$(CRT_INC_PATH)\strstream : ..\strstream
	..\compfile  $** $@

$(CRT_INC_PATH)\typeinfo : ..\typeinfo
	..\compfile  $** $@

$(CRT_INC_PATH)\utility : ..\utility
	..\compfile  $** $@

$(CRT_INC_PATH)\valarray : ..\valarray
	..\compfile  $** $@

$(CRT_INC_PATH)\vector : ..\vector
	..\compfile  $** $@

$(CRT_INC_PATH)\xcomplex : ..\xcomplex
	..\compfile  $** $@

$(CRT_INC_PATH)\xiosbase : ..\xiosbase
	..\compfile  $** $@

$(CRT_INC_PATH)\xlocale : ..\xlocale
	..\compfile  $** $@

$(CRT_INC_PATH)\xlocinfo : ..\xlocinfo
	..\compfile  $** $@

$(CRT_INC_PATH)\xlocmon : ..\xlocmon
	..\compfile  $** $@

$(CRT_INC_PATH)\xlocnum : ..\xlocnum
	..\compfile  $** $@

$(CRT_INC_PATH)\xloctime : ..\xloctime
	..\compfile  $** $@

$(CRT_INC_PATH)\xmemory : ..\xmemory
	..\compfile  $** $@

$(CRT_INC_PATH)\xstddef : ..\xstddef
	..\compfile  $** $@

$(CRT_INC_PATH)\xstring : ..\xstring
	..\compfile  $** $@

$(CRT_INC_PATH)\xtree : ..\xtree
	..\compfile  $** $@

$(CRT_INC_PATH)\xutility : ..\xutility
	..\compfile  $** $@


