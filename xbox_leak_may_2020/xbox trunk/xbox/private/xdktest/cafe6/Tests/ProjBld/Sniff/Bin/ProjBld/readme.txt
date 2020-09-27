What does ProjBld sniff do?

1. Create an application project
	- File.New.Project Workspace.Application "Hello"
	- Insert source files {hello.cpp, hello1.cpp, hello.rc}
	- Close the workspace

2. Verify build setting options
	- Open the created application in step 1
	- Set Intermidate directory to EXEInt
	- Set Output directory to EXEOut
	- Add testing.lib to additional library to link
	- Set the Options.Directories binaries to use the fake compiler tool
	- Build the project
	- Verify the result by checking the buildres.out file
	- Close the workspace

3. Verify Subprojects
	- Insert Dynamic-Linking Library "DLL" as a subproject of "Hello"
	- Insert source files for "DLL" project {dll.cpp, hello1.cpp}
	- Set the Options.Directories binaries to use the fake compiler tool
	- Build the project
	- Verify that project DLL is built first by checking the buildres.out file
	- Close the workspace

4. Verify file dependencies
	- Open the created application in step 1
	- Remove the fake compiler tool directory so the real compiler tool will be used for this test case
	- Touch hello1.h
	- Build the project
	- Verify that hello.cpp is rebuilt (by checking hello.obj timestamp)
	- Insert source file foo.cpp
	- Build the project
	- Verify that foo.pp is built
	- Touch foo.h
	- Verify that foo.cpp is rebuilt
	- Finally make sure UpdateAllDependencies works
	- Close the workspace

5. Verify custom build rules at target level
	- Open the created application in step 1
	- Add custom build rule for Win32 Release target
		BuildCommand=echo "TargetName = $(TargetName)" > .\$(OutDir)\echo.out
		OutputFile=$(OutDir)\echo.out
	- Build the project
	- Verify that echo.out file is generated.
	- Close the workspace

6. Load an external makefile - need improvement - no verification at all for this test case
	- Open an external makefile
	- Set Debug option
	- Set Build Cmd
	- Build the project
	- Close the workspace

	
