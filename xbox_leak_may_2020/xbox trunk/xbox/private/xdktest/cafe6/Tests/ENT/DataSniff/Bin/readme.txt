What does Data sniff do?


	Data sniff accept the following params (in data.stf file):
-DSN=<sql.def>		name of the connection definition file
-DESIGNTABLE=<sales>	name of the table which would be created & modified in Designers test
-DESIGNERS=<0>		bool switch to enable the Designers test (disabled by default)

	The connection definition file (SQL.DEF by default) defines miltiple connections to the SQL datasources. Refer to the default SQL.DEF for the syntax description. Each connection can be:
		-file datasource (if prepended by DSN=). Its DSN file is built on the fly from its description, or
		-registry datasource (its HKCU\Software\ODBC entries must be registered separately)
	Each connection consist of one line connection description & multiple lines of objecsts descriptions of types:
		TABLE - table name & its owner
		TRIGGER - trigger name & the table it's on
		DIAGRAM - diagram name & the tables in it
		VIEW - view name & its owner
		ADHOC - file name of the ODBC query (shld have SQL extention)
		PROC - stored procedure & its parameters for execution
		NEWPROC - name of the new stored procedure, its parameters & file name where its text resides (shld be TSQ extention)



1. Create a database project
	- File.New.Project Workspace.Database "Data Base Proj"
	- For each connection from connection definition file:
		-insert.New Connection (from .DSN file created on the fly or from registry Data Source)
		-fill the Login dialog appropriately
	- Save & Close the workspace

2. Designers test
	- Open the workspace & reenter password if necessary
	- Open & execute the SQL script specified by -DESIGNTABLE subsuite param (sales.sql by default, .SQL extention is always assumed). The script should create & populate the correct table which is the subject of this test: default name is sales$(MACHINE) - look into sales.sql file to see what the safe script shld do.
2a - new diagram:
	- Create new diagram
	- Drag the created table (default sales$(MACHINE)) onto its surface
	- Expand the table grid to show the column properties
	- Add new column of type char
	- Save the new diagram (default name "sales$(MACHINE) diagram")
	- Verify the Data View displays the column added to the table
2b - new view:
	- Create new view (the query document opens)
	- Set focus on the Diagram pane
	- Drag the table (default sales$(MACHINE)) onto Diagram pane
	- Select all (*) column for the query
	- Run the query & verify the number of rows is the same as the amount of data populated the table from SQL script
	- Save the new view (default name "sales$(MACHINE) view")
	- Verify the Data View displays the new view
2c - new query:
	- Create new local query (the query document opens)
	- Set focus on the Diagram pane
	- Drag the table (default sales$(MACHINE)) onto Diagram pane
	- Select all (*) column for the query
	- Add GROUP BY clause to the query
	- Run the query & verify it displays the number of records which populated the table from SQL script
	- Save the local (default name "sales$(MACHINE) query")
	- Verify the query was added to the File View
	- Save & Close the workspace

3. Verify Non-text objects (TABLE, DIAGRAM, VIEW)
	- Open the workspace & reenter password if necessary
	- For each object in connection definition file:
		-find the object in Data View & check its properties
		-open the object as a default document (query from tables & views)
		-for DIAGRAM: check the existance of tables specified in object description
		-for TABLE & VIEW:
			change the query by adding "Group by" option
			verify the syntax of the new query
			run the query (TODO & verify the result is the number of original rows)
	- Close the workspace

4. Verify text objects (PROC, NEWPROC, TRIGGER, ADHOC)
	- Open the workspace & reenter password if necessary
	- For each object in connection definition file:
		-find the object in Data View & check its properties
		-for existing objects (PROC, TRIGGER) open the object as a text document
		-for non-existing objects (NEWPROC, ADHOC)
			delete the old object from Data View if exists in database
			open the script file (.SQL or .TSQ) as specified in object description
			run the script file (in NEWPROC case, it shld create an SP in Data View)
		-for stored procedures (PROC & NEWPROC) run it with the params specified in object description
	- Close the workspace

	
