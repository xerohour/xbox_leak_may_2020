What does SDI sniff do?


	SDI sniff accept the following params (in SDI.stf file):
-BPTABLE=<sdibp.def>	name of the connection & breakpoint definition file
-APPNAME=<ODBCMT>	name of the C++ project (supposed to be find in $(APPNAME) subdirectory of this directory) to work on
-APPCMD=<servername /r>	command line parameters for $(APPNAME) app for debug session in test#4
-DEBUGCOUNT=<1>		number of debug sessions in test#4
-LEGO=<0>		bool switch to disable test#4 (enabled by default)


	The connection & breakpoint definition file (SDIBP.DEF by default) defines miltiple connections to the SQL datasources. Refer to the default DIBP.DEF for the syntax description:
		-file datasource (if prepended by DSN=). Its DSN file is built on the fly from its description, or
		-registry datasource (its HKCU\Software\ODBC entries must be registered separately)
	Each connection consist of one line connection description & multiple lines of breakpoint descriptions. Each breakpoint defines:
		-name of stored proc (or table/trigger) where BP shld be set
		-line text on which BP shld be set
		-number of times BP shld be hit during each debug session of test#4

1. Add a database project to the C++ project
	- Open the C++ project specified by $(APPNAME) 
	- File.New.Project Workspace.Database "$(APPNAME)1" add to existing workspace
	- For each connection from connection definition file:
		-insert.New Connection (from .DSN file created on the fly or from registry Data Source)
		-fill the Login dialog appropriately
	- For each BP in connection definition file:
		-open its Stored Proc/Trigger from Data View
		-find the text BP shld be set on, then set BP
		-if BP shld be hit 0 times - disable it
	- Save & Close the workspace

2. Verify SQL breakpoints & build the C++ project
	- Reopen the workspace & reenter passwords for data connections if necessary
	- Verify the BPs entered in test#1 still exists
	- Rebuild C++ project
	- Close the workspace

3. Direct Database Debuging
	- Reopen the workspace & reenter passwords for data connections if necessary
	- For every Stored Proc in which BPs were entered in test#1 (including disabled BPs):
		-open SP from Data View
		-start DDD & step twice
		-run & verify the BP was hit
		-debug.Stop Debygging
	- Close the workspace

4. Mixed C++/SQL debugging
	- Reopen the workspace & reenter passwords for data connections if necessary
	- Set the active project to C++ project
	- Set Debug.Command line params to $(APPCMD)
	- For $(DEBUGCOUNT) number of times:
		-debug.Go
		-check the current IP source line when any BP hit & confirm one of SQL BPs entered in test#1 was hit
		-continue Debug.Go until all BPs from test#1 were hit
		-debug.Stop Debugging
	- Close the workspace
