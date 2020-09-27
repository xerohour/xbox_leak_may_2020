				How to run the DBG Filename test
				================================

This runs just like any other CAFE v3 test. There are two DLLs checked into
WinRel and WinDebug directories. The .STF files are in the ..\v3\filename (root)
directory. All the sources and projects that are necessary for this test to run is in ..\WinDebug\src
and ..\WinRel\src directories.

This is a generic test that driven by a data file named 'Names.TXT'. This contains
all filenames that need to be tested. You can add interesting file/path names to this file. This file is
supposed to be a growing file. If one finds a interesting case please add that case to this file. This
way every time this test is run your case is executed automatically. Note that a separate Names.Txt
files are checked-in to the ..\Winrel\src and ..\windebug\src directories. So, if you want your case to
be executed in both debug and release versions please add that to both .txt files. This file can not
contain empty lines. You can use the '#' in the first column to make a particular line a comment line.
See 'Names.TXT' for examples.

For every filename in the 'Names.TXT' file, the following tests were performed:

1) Create a new appwizz app with one source file with the specified name.
	If a leading pathname exist, it will create this new project in this specified path.
2) Build project
3) Set breakpoints on the file with the specified filename.
4) Debug.Go
5) Check Registers window test
6) Check Callstack window test
7) Set stepping mode
8) Quickwatch test
9) Memory test

