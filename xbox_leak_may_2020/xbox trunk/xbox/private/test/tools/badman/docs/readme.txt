----------------
WHAT BADMAN DOES
----------------

BadMan is a tool which can be used for good and bad parameter testing, and
for gathering performance data for different API's. Input is taken from a text
script. Output goes to the screen, or to a rats-style logfile if the logging
option is selected. Note that rats-style logging can also be directed to the
screen.

Good and Bad Parameter testing includes exception detection and handling,
verification of the value of GetLastError(), and Verification of the results
from the API Call.

----------------
HOW BADMAN WORKS
----------------

BadMan is complex, but the methodology of BadMan is simple. BadMan first
reads in a list of the data types which are used by NT. Next, it reads in
type case names and their descriptions. A type casename , (case name for short)
usually looks something like 'DWORD_VALID_GLOBALALLOC_FLAGS'. The case name
has a corresponding case number. These case numbers are used to define the
action of 'Setup' and 'Cleanup' functions for each data type.

Setup and Cleanup functions exist for each data type which was read in to
badman. They are responsible for creating a variable of the type they are
for, which will be passed back to the BadMan test engine, and used in a
test.  For instance, the DWORD_VALID_GLOBALALLOC_FLAGS case number would
be sent to a routine called DWORDSetup(), which would return a DWORD
which was a set of valid GlobalAlloc() flags. Sometimes Setup functions
allocate system resources which need to be freed up after the variable has
been used. This is done by the corresponding Cleanup function. Setup and
Cleanup functions are contained in a DLL, SetCln.dll, which is loaded and
used by BadMan. Setup and Cleanup functions are located in BadMan\src\SetClean.

Each API has a validation routine, which can be modified by the user to
perform testing for Valid parameter combinations in BadMan. These functions
are defined one per API. These are located in the BadMan\src\Validate directory.


So here are the actions performed by BadMan:

    1) Load in libraries linked at link-time:
	NTLOG.DLL	    -- Rats-style logging functions
	STATW32.DLL	    -- Performance Statistics functions
	TIMERW32.DLL	    -- Performance Timing functions
    2) read in the list of Data Types currently known to BadMan (*.BMD)
    3) read in the list of Case Names and descriptions. (*.BMH)
    4) load the required un-linked libraries:
	SetCln.DLL	    -- Setup and Cleanup functions
	Validate.DLL	    -- API Result Validation routines
    5) Process the specified script (usually has the .BMS suffix).
	Read an entry
	    Locate all setup and cleanup functions for parameter types
	    Locate the API in its DLL
	    For all selected combinations of parameters
		Setup parameters corresponding to current cases
		Call API (with timing and convergence, if requested)
		Check GetLastError().
		Call Validation routine for API, if requested
		Cleanup the API's results, if routine exists
		Cleanup parameters corresponding to current cases
		Verify results match expected, log them.
    6) Unlink to SetCln.DLL and Validate.DLL, and exit


=================
HOW TO USE BADMAN
=================

Before using BadMan, you need to make sure of two things.

    o The BM_ROOT variable is set to the location of your badman root directory.
      For instance, I type this line:
	    set BM_ROOT=c:\BadMan
    o Your path includes the BadMan binary directory. This is important because
      the executable and DLL's are in that directory. Under Mips, you might
      make it
	    set path=%path%;c:\BadMan\bin\mips;


BadMan has more options than you can shake a stick at, so the best approach
to learning how to use BadMan is to look at other component's tests, copying
an entry out of one of their scripts, and modifying it. By copying one of
their tests, you're guaranteeing that there are cases for all the datatypes
used by the API.

At this point, you might want to skip ahead to "GENERATING NEW TESTS WITH
BADMAN" for an explanation of some of the terminology which will be used.

The best compact source of information for Badman is the help message which
comes up in response to

    BadMan -?

Options include:

o   Specifying the name of the script to run. This is the first parameter to
    BadMan. If the first parameter is a flag (preceded by a '-'), or if no
    parameters are specified, then the file TEST.BMS is tested.

o   Specifying whether good, bad or both types of parameter testing are to
    be done. -g indicates good parameter testing, and -b indicates bad.
    If both flags are set, then both types are done. The default is for
    only bad parameter testing to be done.

o   Specifying whether it is to be considered a success or a failure when
    an API call with invalid parameters generates an exception. By default,
    it is considered a failure, but the -e flag indicates that exceptions
    are considered a success.

o   Specify the TYPE of testing to be done. Normally Good and Bad testing
    is done in a linear form. Linear form means that each the default for each
    parameter is the first good value. Then one (and only one!) of the
    parameters is changed to either a different good parameter, or a bad
    parameter.

    Suppose we have three parameters, each with two good and three bad cases:
	P1 = G1 G2 B1 B2 B3
	P2 = G3 G4 B4 B5 B6
	P3 = G5 G6 B7 B8 B9
    Linear good parameter testing would have four cases:
	(G1,G3,G5), (G2,G3,G5), (G2,G4,G5), and (G1,G3,G6).
    Linear bad parameter testing would have nine cases:
	(B1,G3,G5), (B2,G3,G5), (B3,G3,G5),
	(G1,B4,G5), (G1,B5,G5), (G1,B6,G5),
	(G1,G2,B7), (G1,G2,B8), and (G1,G2,B9)
    Linear testing of both good and bad parameters would result in a total
    of 13 cases, the sum of good and bad testing.

    Non-Linear (permutative) testing can be selected with the -p flag.
    Permutative testing tests all possible combinations of the selected
    parameters. For good testing, it tests all permutations of good
    parameters, so for the previous example would result in 2*2*2 = 8
    cases. For bad testing, it tests all permutations of bad parameters, so
    would result in 3*3*3= 27 cases. For both good AND bad parameter testing,
    it tests all permutations of all cases specified, or in this case
    5*5*5 = 125 cases. You should be careful when specifying this flag.

o   Specify that documentation should be generated by using the -x flag.
    This will parse the script, printing out API prototypes, case names
    for the parameters with +'s or -'s around the parm name indicating
    whether it is considered a good or bad parameter for that call, and
    a variation number. Specifying the -x flag shuts off testing, which must
    be explicitly enabled with the -t flag if desired concurrently.

o   Explicitly specify that testing is to occur by using the -t flag. This
    is a default option that can be shut off by specifying some of the
    other flags, but can be explicitly re-enabled.

o   Parse for correctness of a script file ONLY, by using the -c flag.
    This causes the script file to be read, and error messages printed
    for any syntax errors in the script.

o   Generate performance data by specifying the -f flag. Performance data
    is timed with the Timerw32 DLL, and tallied with the Stat DLL.
    Minimum and maximum iterations for convergence of timing data can be
    set immediatly after the the -f flag by using the -min and -max
    subflags. Note that since the first variation for each script entry
    is guaranteed to be good, performance data for each API in a script
    could be generated by invoking badman with
	    badman test.bms -f -g -v 1

o   Select a single API to be tested. If a script file contains tests for
    multiple API's, but results from only one of them is desired, that
    API can be selected with the -a flag, followed by the name of the API.

o   Select a variation number to be tested with the -v flag, followed by
    the # of the variation. This can be used in conjunction with the -a
    flag to execute a single test variation, or by itself. Performance data
    may only be desired for a single good case for each API. If so, this could
    be obtained by specifying
	    badman ScriptName -g -f -v 1
    Good parm combinations are always first in variation numbering for an
    API, so variation #1 is guaranteed to be good parameter testing.

o   Specify that the validation routines are to be called for all tests.
    The validation routines by default are empty, but may be filled with
    verification routines and error detection to provide more thorough
    testing of return results from API's. This is done with the -y flag.

o   Request rats-style logging with the -l flag. This defaults to logging
    all loglevels to a logfile of the same name as the script file, with
    the extension .LOG.  Existing logs of that name are renamed as with
    RATS. Immediately after the -l flag, the output log file can be named
    with the -o flag followed by a file name, and the logging level can
    be specified by the -level flag, followed by flags in quotiation marks
    and parenthesis, logical-or'd together. badman -? gives a list of all
    possible flags. Example:
	    badman -l -o Fish.Log -level "(TLS_LOGALL|TLS_REFRESH)"


Experimentation will yield the most fruitful results.



================================
GENERATING NEW TESTS WITH BADMAN
================================


----------------------
WRITING BADMAN SCRIPTS
----------------------

The script is the most important part of a badman test suite. It is possible
in some cases to implement basic testing for an API by writing a script,
without requiring any programming or compilation of files.


--------------------
Basic Script Options
--------------------

A basic BadMan Script entry for GlobalAlloc() is listed between the dashed
lines.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HANDLE GlobalAlloc(DWORD dwFlags,DWORD dwBytes) = 0
  dwFlags:
    Good: DWORD_VALID_GLOBALALLOC_FLAGS
     Bad: DWORD_INVALID_GLOBALALLOC_FLAGS DWORD_SPURIOUS_GLOBALALLOC_FLAGS
	  DWORD_VALID_AND_SPURIOUS_GLOBALALLOC_FLAGS
  dwBytes:
    Good: DWORD_ONE_K DWORD_ONE_MEG
     Bad: DWORD_THREE_GIG
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

This entry is composed of two parts: A function prototype, and case
specifications for each parameter.

The function prototype consists of the type of the API, the API name,
API argument types and names for each argument, and (optionally) the expected
return value for a failed call to that API. This can either be a number, or
the name of one of the arguments for the API.  Note that if the expected
return value is anything EXCEPT a particular number, '!=' may be used instead
of '='.

The case specifications is a list of the parameter names, along with case
names which correspond to good and bad values for that parameter. The case
names are organized by the first letter of the type, or by the third letter,
if the type begins with the letters "lp". Case names for DWORD's, for instance,
are located in the file BadMan\inc\D_CASES.BMH.

There is a tool, GD, which will allow you to see Case names and comments without
looking in the .BMH files. To use it to see cases for a type, say 'DWORD',
type
    GD DWORD
GD has several options which are explained when you invoke it with the -? flag.

The spacing of script files doesn't matter. If you want your case names one to
a line, you can put them that way. If you want the keywords (denoted by the ':'
after them) on the same line, you can do that, as long as the ORDER of the
keywords doesn't change. Also, case names can be separated by either white
space or commas.

You must specify at least one good and one bad case name for each parameter.

A script entry is terminated by either the beginning of another script entry,
or the end of the script file.

When writing scripts, you may comment them by using the standard 'C' comment
operators: /* and */.  Everything between those two pairs of symbols will be
ignored by BadMan.


-----------------------
Advanced Script Options
-----------------------

Other commands can be used in the script, to exploit some of the features of
BadMan.


Pointers in function prototypes
-------------------------------

Some function prototypes use the indirection operator ('*') rather than a
pre-defined type to delineate pointers. Such a prototype might look like

    VOID FishFood(PVOID *Bait) = 0

since a PVOID is not the same as a PVOID *, pointers are handled differently.
For each '*' in front of a variable name, 'P_' is prepended to the type once.
So, for the Bait parameter above, the type name would be modified to P_PVOID.
If Bait were '**Bait' instead of '*Bait', then the type name would be
P_P_PVOID, and so on. Case names for these parameters should be modified
accordingly, ie P_P_PVOID_MYCASENAME.


GetLastError() codes
--------------------

You can set the GetLastError() codes expected when a given bad case is
tested. This code can be either an integer, or a name which is defined in a
.BMH file in the Badman\inc directory. You set the code for case names which
you want it set for in your script, by using the '=' sign. For instance,
For instance,

    hMem:
	Good: HANDLE_VALID_GMEM_BLOCK  HANDLE_UNLOCKED_GMEM_BLOCK
	 Bad: HANDLE_FREED_BLOCK
	      HANDLE_DISCARDED_BLOCK=ERROR_INVALIDBLOCK
	      HANDLE_RANDOM = 5 HANDLE_FISH

The rules are the same for the organization of case names, except now it might
be less confusing if comma's were used to separate case names rather than
spaces. GetLastError return codes are optional, and may be specified for each
bad case as desired.


The Omit: keyword
-----------------

Some parameters are of types which make it very unlikely that anything except
the cases which you specifically list as good will be good. For instance,
if half a dozen components have already been implemented, and you're testing
an API which require a handle to a thread, the chances are that all the cases
which have been defined by other BadMan users for HANDLES are NOT handles to
threads, and thus are bad cases for your API.

In such a case, rather than specifying Bad: cases for the parameter, you
can specify that you want BadMan to consider ALL cases for that type to
be bad cases, minus the ones that you've already declared as good cases,
or that you tell it to Omit: from the list of bad cases. Speaking abstractly,
suppose I have type FISH, and for FISH there are fifty cases already
defined, FISH_C1 through FISH_C50.  Additionally, you've created three
cases. FISH_MYGOOD, which is good, FISH_MYBAD which is bad, and FISH_MYFOOT
which can be either good or bad, depending upon circumstances. Then your entry
for the parameter fOod, of type FISH, would look like

    fOod:
	Good: FISH_MYGOOD
	Omit: FISH_MYFOOT

This would be functionally equivalent to

    fOod:
	Good: FISH_MYGOOD
	Bad: FISH_MYBAD FISH_C1 FISH_C2 ... FISH_C50

with the added bonus that any FISH_ cases which were made in the future
would automatically be included into the list of BAD cases. If it turned
out they WEREN'T bad cases, they could simply be added to the Omit list
as needed.


The EndCase: keyword
--------------------

[NOTE: BadMan will not give an error with this keyword, but currently ignores it.]

Some API's have multiple parameters for which certain values may be
incompatible. BadMan allows you to specify groups of cases which
may be used together, without having to redefine other cases for parameters
in the API which may not be affected by the dependency.

This is supported through the use of the EndCase: keyword.

Suppose you have an API "Cod" which has five parameters, each of type
FISH. Suppose further that parameters three and five can only be present
in certain combinations. A Definition for Cod which illustrates the use of
the EndCase: keyword follows.


FISH Cod(FISH p1, FISH p2, FISH p3, FISH p4, FISH p5)
    p1: Good: FISH_C1  Bad: FISH_C2
    p2: Good: FISH_C1  Bad: FISH_C2
    p3: Good: FISH_C1  Bad: FISH_C2
    p4: Good: FISH_C1  Bad: FISH_C2
    p5: Good: FISH_C1  Bad: FISH_C2
    EndCase:
    p3: Good: FISH_C3  Bad: FISH_C4 FISH C5
    p5: Good: FISH_C11 Bad: FISH_C19
    EndCase:
    p3: Good: FISH_C29 FISH_C31 Omit: FISH_C1 FISH_C2
    p5: Good: FISH_C50 Bad: FISH_C49


Note that there is no EndCase: after the last case. The EndCase: can be
put there, or omitted, as the parser will assume the case is ended when it
sees a new API type.

The case specifiers are parsed as follows: The first time each parameter
has a case list specified, those cases are made into defaults. If there
are cases afterward, it will do the following:

l:  Test the current case, substituting Parameter specifications involved
    in the current case for defaults.

    Read in default case
    execute testing of the default case
    while (another case exists)
      read in the specifications for that case
      substitute the parameter specifications for that case for default specs.
      test the case


So three sets of Cases would be tested: one with the original set of
five parameter case specifications, one with default specifications for
p1, p2, and p4, and with

    p3: Good: FISH_C3  Bad: FISH_C4 FISH C5
    p5: Good: FISH_C11 Bad: FISH_C19

and one set with default specifications for p1, p2, and p4, and with

    p3: Good: FISH_C29 FISH_C31 Omit: FISH_C1 FISH_C2
    p5: Good: FISH_C50 Bad: FISH_C49


The Depend: and EndDepend: keywords
-----------------------------------

There can be complex levels of parameter combination where you might
want to change one parameter from the default, then with that change
active, change another parameter or set of parameters several times.
The depend-enddepend keyword pair allow you to do this. Everything
between the depend: and enddepend: keywords is processed as described in
the example for the description of EndCase:, except that all parameter case
sets active when the depend: keyword was seen become temporary defaults.
The keyword EndDepend: has an implicit EndCase: within it.

A pseudo-code example for an API with five parameters:

    p1: Good: C1  Bad: C2
    p2: Good: C3  Bad: C4
    p3: Good: C5  Bad: C6
    P4: Good: C7  Bad: C8
    p5: Good: C9  Bad: C10 EndCase:
    p2: Good: C11  Bad: C12
	Depend:
	    p4: Good: C13  Bad: C14 EndCase:
	    p3: Good: C15  Bad: C16
		Depend:
		    p4: Good: C17  Bad: C18
		EndDepend:
	    p5: Good: C19  Bad: C20 EndCase:
	    p4: Good: C21  Bad: C22
	EndDepend:
    p2: Good: C23  Bad: C24
    p5: Good: C25  Bad: C26

This is going to result in six sets of cases:

	  p1	      p2	  p3	      p4	  p5
       Good Bad	   Good Bad    Good Bad	   Good Bad    Good Bad
      ---------------------------------------------------------
(1)	C1  C2	    C3	C4	C5  C6	    C7	C8	C9  C10
(2)	C1  C2	    C11 C12	C5  C6	    C13 C14	C9  C10
(3)	C1  C2	    C11 C12	C15 C16	   C17	C18	C9  C10
(4)	C1  C2	    C11 C12	C5  C6	    C7	C8	C19 C20
(5)	C1  C2	    C11 C12	C5  C6	    C21 C22	C9  C10
(6)	C1  C2	    C23 C24	C5  C6	    C7	C8	C25 C26


This follows basic rules for determining case sets. It assumes that one
case set has just been read in and processed, and we're starting to read
a second one.

    1) A case set is ended by reading in an EndCase: Statement
    2) A case set is ended by encountering an End-of-file or a new API
       test specification.
    3) A case set is ended when one or more EndDepend: keywords are read.
    4) Case sets displace the defaults for parameter cases.
    5) Before reading a new case set (and excluding the ORIGINAL defaults)
	a) All elements from the previous case set which are at a more
	   nested Depend: level are removed from the defaults.
	b) All elements from the previous case set which are at the current
	   Depend: nesting level are removed from the defaults.

Dependency lists are the most complicated feature of BadMan scripts, and should
not be used unless understood completely. When properly used, they can save
a significant amount of space in the script file. The example above would
save what is considered a significant amount of space.


------------------
Creating New Cases
------------------

Every API has parameter requirements which, for thorough testing, require
new cases to be generated for it.

Information for test cases is contained in two places. Case names and
their descriptions are contained in the file BADMAN\INC\x_CASES.BMH.  The
source code responsible for generating a value for the case resides in
the file BADMAN\SRC\SETCLEAN\x_SETCLN.C.  In both of these cases 'x' should
be substituted with the first letter of the datatype, or the third letter in
the case that the datatype begins with "lp". Cases which begin with a series
of 'P_''s are found in their actual type name, for instance P_P_INT would make
x = I.

The BADMAN\SRC\SETCLEAN directory also contains other files, usually on a
component by component basis, which house functions called by the various
x_SETCLN.C files.


Creating New Cases for existing datatypes
-----------------------------------------

To create a new case, you must first check out the x_CASES.BMH and x_SETCLN.C
files which house the datatype for that case. You must also check out the
DLL BADMAN\BIN\*\SETCLN.DLL.  DO NOT CHECK ANY FILES BACK
IN UNTIL CHANGES HAVE BEEN SUCCESSFULLY COMPILED AND TESTED.  After checking
out the files, you need to think of a name for the case. edit the x_CASES.BMH
file, and find the section which has other cases of this datatype. These are
clearly marked by large comment banners. At the bottom of the set of cases
for your type, Add a new case. The new case should have a name which begins
with the type and an underscore, and should be #define'd to be the next
case number. The cases in the x_CASES.BMH file MUST remain in numerical order
for each type.

When adding a new case definition, a format consistent with the existing
case definitions should be maintained. See the example for adding a handle
to a fish, below.

After adding the case name and number to the .BMH file, the code to generate
and destroy the case should be added to the appropriate Setup and Cleanup
functions in x_SETCLN.C. if T is the datatype of the case to be added, then
the Setup function is TSetup(), and the Cleanup function is TCleanup().

Setup and Cleanup functions are large switch statements. Setup functions have an
entry for each Case name for that datatype. Cleanup functions only require an
entry for a Case name if a resource needs to be destroyed, but it is good
programming style to put an entry in which consists of a break; statement.

To add a new Case C of type T to a setup function,
    o Go to the bottom of the TSetup() function	in the appropriate
      x_SETCLN.C file.
    o insert the Case just before the default: case for the switch, using
      the case name.

When inserting Setup and Cleanup code for creating a case, there are several
things to remember:

    (1) Standard variations of printf() don't work, since this is in a DLL.

	To print Error messages, use the ErrorPrint() function, which
	has three user arguments: Function name; Case number; and error message.
	the first two arguments should be hConOut and hLog. A sample call would
	look like
	    ErrorPrint(hConOut,hLog,"HANDLESetup",HANDLE_FISH,"My error.");
	ErrorPrint messages will be printed to the screen, and to the
	rats-style logfile, if one is active.

	To print normally, use the supplied SCPrintf function. This is the
	same as printf, but requires the inital argument hConOut.

	To print to the rats-style logfile, if active, use the tlLog
	function and the supplied hLog handle. hLog will be NULL if
	logging is not active, and this should be tested for before printing
	to the log. See documentation on NTLOG for more information.

    (2) If you allocate a resource in Setup, you need to save a pointer to that
	resource, or to a structure which contains information the corresponding
	cleanup function can use to deallocate the resource. ScInfo is
	provided for this purpose. A typical assignment would be
	    if ((pMyStruct = malloc(sizeof(MyStructType))) == NULL) {
	      ErrorPrint(hConOut,hLog,"FISH",C1,"Unable to allocate cleanup.");
	      }
	    else {
	      /* fill the structure */
	      }
	    *SCInfo = (LPVOID) pMyStruct;
	The pointer will be tracked, and passed in to the cleanup function
	for its use.

    (3) The variable "Ret" is usually used to return results, and
	is of the same datatype as the case. Ret will sometimes have
	hungarian notation for the type prepended to it.

    (4) NEVER return from inside the switch. This is bad programming
	style. The return() statement at the bottom of the Setup and
	Cleanup functions should be the only exit from the routines.

If you have a number of similar cases, it might be wise to group them
together in the Setup and Cleanup functions, and pass the parameters on
to your own functions. These functions need to be put into separate files
in the project, and ADDFILE'd to the project. They also need to be inserted
into the SOURCES file for the SETCLEAN directory, and any include files you
generate should be put in BADMAN\INC.  The same rules apply for checking
in these files as apply to the SetCln and Cases files. If you check in code
which doesn't work, you are in DEEP DOO-DOO!


Creating New Cases for non-existant datatypes
---------------------------------------------

If you haven't read the section on creating new cases for existing datatypes,
go back and read it before proceding.

A tool, GenSetCl, has been supplied to simplify generating the function and
header stubs for a new datatype.

GenSetCl processes either a file containing a list of types, or a single
type specified on the command line. If no output file is specified, it will
create x_SETCLN.C and x_CASES.BMH files for every letter x which starts a
typename in the file. If types which are pointers are specified, they should
have their pointers appended to their name. For instance, for a pointer to
a pointer to char, the entry should be 'char**'.

GenSetCl has options for specifying input, output, and .DEF file information.
For more information on it, invoke it with the -? flag.


To create a template for a set of new datatypes:

    (1) Make a temporary directory to hold the datatypes.
    (2) Create a text file containing the datatypes you want function stubs
	for, one line per type.
    (3) Look in BadMan\src\setclean\setcln.def.  Find the greatest ordinal
	number (number after the @) associated with an export name.  We'll call
	this number n.
    (4) Invoke GenSetCl. Suppose the file with data types in it is dtype.txt.
		gensetcl dtype.txt -n n
	Remember that n is the greatest ordinal number from (3)
    (5) Look at the files which have been generated, and make certain they look
	reasonable.
    (6) For each x_SetCln.C file, check out the corresponding file in
	BadMan\src\SetClean.  For each x_cases.bmh file, check out the
	corresponding file in BadMan\inc. Check out
	BadMan\src\SetClean\SetCln.def.
    (7) Integrate the function stubs into the project files, and build the
	project to make certain their are no mistakes.
    (8) If there are no mistakes, check the project files back in.


SAMPLE addition of Case
-----------------------

Suppose you wanted to add a case for generating a handle to a fish.
First, you would take a look at the file H_CASES.BMH.  At the time this readme
was written, the handles section looked like

/************************************
H   H   A   N   N DDD   L     EEEEE 
H   H  A A  NN  N D  D  L     E     
HHHHH AAAAA N N N D   D L     EEEE  
H   H A   A N  NN D   D L     E     
H   H A   A N   N DDDD  LLLLL EEEEE 
************************************/

	/* These are handles I defined for Memory management (JohnMil) */

		// A Random number instead of a handle
#define HANDLE_RANDOM 1
 .
 .
 .

		// A freed Global Memory block
#define HANDLE_FREED_GMEM_BLOCK 12

		// A Null Handle
#define HANDLE_NULL 13

	/* End of the handles I defined for Memory Management... (JohnMil) */



IT IS ESSENTIAL THAT A SINGLE LINE COMMENT BE PLACED ABOVE EACH CASE
DEFINITION. This comment is used as a description of the parameter when
printing out test documentation, and error messages. The comment should be a
good explanation of what the case is. The H_CASES.BMH file also doubles as
an include file to H_SETCLN.C. The value which the case is defined to MUST be
an integer.  For this case, You would add something like


		// A Handle to a great white whale (I KNOW it's a mammal)
#define HANDLE_TO_FISH 14


then save the file.

The code to generate the case must then be added to the setup and cleanup
functions. This is covered above.


-----------------------------
MODIFYING VALIDATION ROUTINES
-----------------------------

Validation takes place if the -y flag is specified in BadMan, immediately
after each API call. It is intended to provide a means of verifying
characteristics of the return value of an API call.

Validation routines are located in the BADMAN\SRC\VALIDATE directory, and
consist of files of the format x_VALID.C, where 'x' is the first letter of
the API to validate. The validation function for an API A would be
AValidate(). It's parameters include:

o   VarInfo[].	VarInfo[i].next has a pointer to the definition of the case
		used for parameter #i. VarInfo[i].next->Symbol is the text
		for the define, VarInfo[i].next->Comment is the text of
		the comment associated with the define (if any), and
		VarInfo[i].next->Case is the # associated with it.

o   dwParms[]	dwParms[i] is the actual parameter passed in as parameter
		#i in the API call, and can be typecast to the correct type
		for that parameter.

o   ParmGoodBad[]  ParmGoodBad[i] contains information on whether parameter
		#i is good or bad. either CLOPT_GOOD or CLOPT_BAD will be
		the value of it, depending.

o   dwRetVal	This is the return value for the API call.

o   hLog	This is the handle (if any) to be used for Rats-style
		logging. It will be NULL if logging isn't enabled.

o   hConOut	Since printf() doesn't work in DLL's, this is a handle
		to the main console, which will allow ValPrintf to print
		to the screen.


Validation routines do not return anything to the BadMan test engine.

In order to modify a validation routine, you must
    o	Check out the x_VALID.C file in BADMAN\SRC\VALIDATE, where x
	is the first letter of the API name.
    o	Check out BADMAN\BIN\*\VALIDATE.DLL
    o	Find the correct validation routine, and modify it.
    o	Build Validate.DLL, and make certain it works. Then check in
	the changes.

Format and depth of validation is left to the user.

Note that for new API's, the tool GenValid can be used. Its format and use
are very similar to GenSetCl. Refer to the documentation under 'Creating New
Cases for non-existant datatypes', and the output supplied by GenValid -? for
more information.


---------------------------
CLEANING UP AFTER API TESTS
---------------------------

Some API's create or modify system resources in a way which needs to be
cleaned up afterwards. For instance, testing GlobalAlloc() will result
in a Global memory block being allocated in each instance where the API call
succeeds. Over the course of a large number of tests, this can use up
enough system resources to affect test results.

SetCln contains functions which allow you to clean up the results of an API
test call. The functions are named xAPICleanup(), where 'x' is actually the
name of the API. For instance, the function called after GlobalAlloc() is
tested is GlobalAllocAPICleanup().

These functions are to be created as needed. They are passed the return
value of the API, an array of parameters with which the API was called,
and handles to the Logfile (if one exists) and the console. The standard
DLL printing routines are present in a new form, ErrorPrint() and APrintf().
Look at the prototypes in BadMan\src\APIClean\APICln.c for the exact
format.

Functions in APICln.c are responsible for checking to see if the value in
dwRet is valid before they take their actions. For instance, the routine
for GlobalAllocAPICleanup() first checks to make certain ((HANDLE) dwRet)
is not null.

Whenever a function is added to the APICln.C file, it must also be
exported via the APICln.DEF file in the same directory. As always, don't
check in results unless they have been built and verified.
