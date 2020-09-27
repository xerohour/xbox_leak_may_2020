
13May98 - a-dennig - The NTBuild directory was added to the Devtools
	  project on 13May98 by a-dennig (Dennis Gallagher x17473)
	  to support conversion of the VB7 build process from a batch
	  file oriented system to one based on NT Build.

	  The files in this directory are the source files for the NT Build
	  executable, build.exe (this executable was checked into devtools at
	  devtools\x86).

	  As of this date, the NT Build executable has NOT been built by anyone
	  on the VB7 team from these source files.

	  The build.exe executable and the source files in this directory were
	  all obtained from billev of the COM+ group.

	  If you build the build.exe from the source files in this directory,
	  you should update this readme.txt file so other will know.

15May98 - a-dennig - I built Build.exe today using VC++ 5.0.  No real
          problems.   It required one include file that wasn't on my
          path.   Once I located the file and added an include path
          to it, the program built fine.

19Jun98 - a-dennig - we replaced build.exe and its sources with new code
          which marioch obtained from patbr.  I've checked the new source
          for build.exe in today but NOTE that no one in the VB7 group
          has built the build.exe from these sources yet.

	  If you build the build.exe from the source files in this directory,
	  you should update this readme.txt file so other will know.

12Mar99 - a-nigelh - Modified buildexe.c to recognise vsautodoc as an error source.

08Jun00	- scotta - Added -n switch, which prepends the processor ID to lines
	  in the log files in multi-proc builds.