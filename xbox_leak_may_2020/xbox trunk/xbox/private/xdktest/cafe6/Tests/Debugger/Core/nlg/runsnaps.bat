if "%testos%"=="NT4" set logext=NT4
if "%testos%"=="NT5" set logext=NT5
if "%testos%"=="Win95" set logext=W95
if "%testos%"=="Win98" set logext=W98

if (%1)==(DEBUG) set DebugOrRetail=1
if (%1)==(DEBUG) set LOGTYPE=D

if (%1)==(RETAIL) set DebugOrRetail=0
if (%1)==(RETAIL) set LOGTYPE=R

if (%1)==(debug) set DebugOrRetail=1
if (%1)==(debug) set LOGTYPE=D

if (%1)==(retail) set DebugOrRetail=0
if (%1)==(retail) set LOGTYPE=R

REM default is DEBUG
if (%1)==() set DebugOrRetail=1
if (%1)==() set LOGTYPE=D

goto executionsnaps
echo Running Breakpoints Snap tests...
set logname=bp%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\bp.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:executionsnaps
echo Running Debugger Execution Snaps...
set logname=Exe%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\exe.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:callstacksnaps
echo Running Debugger Callstack Snaps...
set logname=stack%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\stack.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:dissasembly
echo Running Debugger dissasembly Snaps...
set logname=disasm%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\disasm.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:Memory
echo Running Debugger Memory Snaps...
set logname=mem%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\mem.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:Namespace
echo Running Debugger Namespace Snaps...
set logname=name%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\namespac.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:NonLocalGoto
echo Running Debugger NonLocalGoto Snaps...
set logname=nlg%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\nlg.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:ReturnValues
echo Running Debugger ReturnValues Snaps...
set logname=retval%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\retval.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:Templates
echo Running Debugger Templates Snaps...
set logname=tmplates%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\tmplates.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:VariablesWindow
echo Running Debugger VariablesWindow Snaps...
set logname=varswnd%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\varswnd.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:ExpressionEvaluation
echo Running Debugger ExpressionEvaluation Snaps...
set logname=EE%logext%%LOGTYPE%.txt
start /w cafedrv /stf=..\tests\debugger\core\bin\ee.stf -RUN -DEBUG=%DebugOrRetail% -persist=0
attrib -r %logname%
del %logname%
move results.log %logname%

:end
