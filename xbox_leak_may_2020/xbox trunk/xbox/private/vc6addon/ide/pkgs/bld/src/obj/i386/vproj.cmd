
CMDTABLE IDCT_BLDPACKAGE





COMMAND IDG_PROJECT_SETTINGS IDM_PROJECT_SUBPROJECTS PROJECT_DEPENDENT
	"ProjectDependencies" "D&ependencies..."
	PROMPT "Edits project dependencies\0Dependencies"

COMMAND IDG_PROJECT_SETTINGS IDM_PROJECT_SETTINGS
	"ProjectSettings" "&Settings..." 
	PROMPT "Edits the project build and debug settings\0Settings"

COMMAND IDG_BUILD_SETTINGS IDM_PROJECT_DEFAULT_PROJ PROJECT_DEPENDENT
    "BuildSetActiveConfiguration" "Set Active C&onfiguration..."
    PROMPT "Selects the active project and configuration\0Set Active Configuration"

COMMAND IDG_BUILD_SETTINGS IDM_PROJECT_CONFIGURATIONS PROJECT_DEPENDENT
	"BuildConfigurations" "Con&figurations..."
	PROMPT "Edits the project configurations\0Configurations"

COMMAND IDG_BUILD_SETTINGS IDM_RUN_PROFILE PROJECT_DEPENDENT
	"ToolsProfile" "&Profile..."
	PROMPT "Sets Profile options, profiles the program, and displays profile data\0Profile"


COMMAND IDG_DEBUG_1 IDM_PROJECT_OPENDIALOG NOKEY NOMENU NOBUTTON 
	"" "Open Dialog"
	PROMPT "Open Dialog"







COMMAND IDG_DEBUG_1 IDM_PROJECT_BUILDGRAPH NOKEY DYNAMIC
	"" "&Graph..."
	PROMPT "Show build dependency graph"

COMMAND IDG_DEBUG_1 IDM_PROJECT_WRITE NOKEY DYNAMIC
	"" "&Save Project..."
	PROMPT "Saves buildable project file(s)"

COMMAND IDG_DEBUG_1 IDM_DUMP_HEAP NOKEY DYNAMIC
	"" "&Dump Heap..."
	PROMPT "Creates an Excel spreadsheet of the user heap"

COMMAND IDG_PROJECTOPS_ADD IDM_PROJECT_SETTINGS_POP NOMENU NOKEY
	"" "&Settings..."
	PROMPT "Edits the selected project item build settings"

COMMAND IDG_PROJECTOPS_ADD IDM_INSERT_FILES_POP  NOMENU NOKEY
	"" "Add F&iles..." 
	PROMPT "Inserts existing file(s) into the project" 

COMMAND IDG_BUILD_1 IDM_PROJITEM_COMPILE
	"BuildCompile" "&Compile" 
	PROMPT "Compiles the file\0Compile" 
	GLYPH 0

COMMAND IDG_BUILD_1 IDM_PROJITEM_CCOMPILE NOMENU NOKEY
	"" "&Compile"
	PROMPT "Compiles the selected file"

COMMAND IDG_BUILD_1 IDM_PROJECT_SET_AS_DEFAULT NOMENU NOKEY
	"" "Set as Acti&ve Project"
	PROMPT "Sets this project to be the active project"

COMMAND IDG_BUILD_1 IDM_PROJITEM_BUILD NOMENU
	"Build" "&Build"
	PROMPT "Builds the project\0Build"
	GLYPH 1


COMMAND IDG_BUILD_1 IDM_BUILD_TOGGLE
	"BuildToggleBuild" "&Build"
	PROMPT "Builds the project\0Build"

COMMAND IDG_BUILD_1 IDM_PROJITEM_CBUILD NOMENU NOKEY
    "" "&Build"
    PROMPT "Builds the selected project"

COMMAND IDG_BUILD_1 IDM_PROJITEM_CBUILD_NO_DEPS NOMENU NOKEY
   "" "Build (selection only)"
    PROMPT "Builds the selected project without building project dependencies"

COMMAND IDG_BUILD_1 IDM_PROJITEM_REBUILD_ALL
	"BuildRebuildAll" "&Rebuild All"
	PROMPT "Rebuilds Active Project and all project dependencies\0Rebuild All" 
	GLYPH 2

COMMAND IDG_BUILD_1 IDM_PROJITEM_BATCH_BUILD PROJECT_DEPENDENT
	"BuildBatch" "Batch B&uild..." 
	PROMPT "Builds multiple projects\0Batch Build"

COMMAND IDG_BUILD_1 IDM_PROJECT_STOP_BUILD NOMENU
	"BuildStop" "Stop &Build" 
	PROMPT "Stops the build\0Stop Build" 
	GLYPH 3

COMMAND IDG_BUILD_1 IDM_PROJECT_CLEAN_ACTIVE PROJECT_DEPENDENT
	"BuildClean" "Cl&ean" 
	PROMPT "Deletes intermediate and output files (cleans the project)\0Clean"

COMMAND IDG_BUILD_1 IDM_PROJECT_SCAN_ALL QUERYMENU PROJECT_DEPENDENT
	"BuildUpdateAllDependencies" "Update &All Dependencies..." 
	PROMPT "Updates dependencies for the selected projects\0Update All Dependencies"

COMMAND IDG_CONFIG_COMBO ID_CONFIG_COMBO NOMENU NOKEY HWND HWNDSIZE
    "ConfigurationSelectTool"
    PROMPT "Selects the active configuration\0Select Active Configuration"
    GLYPH 4

COMMAND IDG_BUILD_2 IDM_PROJECT_EXECUTE PROJECT_DEPENDENT
	"BuildExecute" "E&xecute" 
	PROMPT "Executes the program\0Execute Program"
	GLYPH 5

COMMAND IDG_BUILD_1 IDM_PROJECT_CLEAN NOMENU NOKEY
   "" "&Clean (selection only)"
    PROMPT "Deletes intermediate and output files without cleaning project dependencies"






 
COMMAND IDG_PROJECT_SETTINGS IDM_PROJECT_EXPORT PROJECT_DEPENDENT
   "BuildProjectExport" "Export &Makefile..."
    PROMPT "Exports buildable project(s) in external makefile format\0Export Makefile"





COMMAND IDG_TOOLS_REMOTE IDM_PROJECT_TOOL_MECR QUERYMENU
	"UpdateRemoteOutputFile" "Update Remote Output File"
	PROMPT "Updates the remote project output file\0Update Remote Output File"

COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD0 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD1 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD2 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD3 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD4 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD5 NOKEY QUERYMENU ""
COMMAND IDG_TOOLOPS IDM_PROJECT_TOOL_CMD6 NOKEY QUERYMENU ""

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO1 NOMENU NOKEY
	"" "Input"
	PROMPT "Directory of the input file"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO2 NOMENU NOKEY
	"" "Intermediate"
	PROMPT "Intermediate directory"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO3 NOMENU NOKEY
	"" "Output"
	PROMPT "Output directory"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO4 NOMENU NOKEY
	"" "Workspace"
	PROMPT "Workspace directory"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO5 NOMENU NOKEY
	"" "Project"
	PROMPT "Project directory"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO6 NOMENU NOKEY
	"" "Target"
	PROMPT "Directory of the primary target"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO7 NOMENU NOKEY
	"" "Microsoft Developer"
	PROMPT "Directory where Microsoft Developer is installed"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_DIRMACRO8 NOMENU NOKEY
	"" "Remote Target"
	PROMPT "Directory of the remote target"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO1 NOMENU NOKEY
	"" "Workspace Name"
	PROMPT "Name of the workspace file"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO2 NOMENU NOKEY
	"" "Target Path"
	PROMPT "Full path of the primary target"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO3 NOMENU NOKEY
	"" "Target Name"
	PROMPT "Name of the primary target file"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO4 NOMENU NOKEY
	"" "Input Path"
	PROMPT "Full path of the input"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO5 NOMENU NOKEY
	"" "Input Name"
	PROMPT "Name of the input file"

COMMAND IDG_BLDSYS_TOOL IDM_BLDTOOL_FILEMACRO6 NOMENU NOKEY
	"" "Remote Target Path"
	PROMPT "Full path of the remote target"

COMMAND IDG_PROJECT IDM_PROJITEM_OPEN NOMENU NOKEY
	"" "O&pen" 
	PROMPT "Opens an item for editing"

COMMAND IDG_BLDSYS_TOOL IDM_BROWSE_FOR_DEBUG_EXE NOMENU NOKEY
	"" "Browse..."
	PROMPT "Browse for executable"

COMMAND IDG_BLDSYS_TOOL IDM_USE_TESTCONTAINER NOMENU NOKEY
	"" "ActiveX Control Test Container"
	PROMPT "Enters ActiveX Control Test Container executable"

COMMAND IDG_BLDSYS_TOOL IDM_USE_WEBBROWSER NOMENU NOKEY
	"" "Default Web Browser"
	PROMPT "Enters Default Web Browser executable"
