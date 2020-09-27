// VShell's package commands

CMDTABLE IDCT_SHELLPACKAGE


// Being commented out because retail build won't build
// due to IDG_DEBUG_1 being ifdef'd under DEBUG in ide\include\cmds.h
// COMMAND IDG_DEBUG_1 IDMY_SET_FONT NOKEY
//      "PrintCommandTable" "Print Command Table"
//      PROMPT "Prints the command tables"


// Major group: File

COMMAND IDG_FILE_NEW ID_FILE_NEW
	"New" "&New..." 
	PROMPT "Creates a new document, project or workspace\0New" 
	GLYPH 0

COMMAND IDG_FILE_NEW_IN_PROJ IDM_FILE_NEW_IN_PROJ
	"FileNewInProject" "&New..." 
	PROMPT "Creates a new document in the project workspace\0New" 

COMMAND IDG_FILEOPS ID_FILE_OPEN
	"FileOpen" "&Open..." 
	PROMPT "Opens an existing document\0Open" 
	GLYPH 1

COMMAND IDG_FILEOPS ID_FILE_CLOSE
	"FileClose" "&Close" 
	PROMPT "Closes the document\0Close"

//COMMAND IDG_FILEOPS IDM_FILE_REVERT
//      "FileRevert" "&Revert..."
//      PROMPT "Reverts the source file to the last saved copy, discarding changes\0Discard Changes"

COMMAND IDG_FILESAVE ID_FILE_SAVE
	 "FileSave" "&Save" 
	 PROMPT "Saves the document\0Save" 
	 GLYPH 2

COMMAND IDG_FILESAVE ID_FILE_SAVE_AS
	"FileSaveAs" "Save &As..." 
	PROMPT "Saves the document with a new name\0Save As"

COMMAND IDG_FILESAVE IDM_FILE_SAVE_ALL
	"FileSaveAll" "Save A&ll" 
	PROMPT "Saves all the open files\0Save All" 
	GLYPH 13

COMMAND IDG_FILESAVE ID_FILE_SAVE_ALL_EXIT NOMENU
	"FileSaveAllExit" "Save All and Exit"
	PROMPT "Saves all the open files and exits Developer Studio\0Save All and Exit"

COMMAND IDG_PRINTMAIN ID_FILE_PAGE_SETUP
	"FilePrintPageSetup" "Page Set&up..." 
	PROMPT "Changes the page layout settings\0Page Layout"

COMMAND IDG_PRINTMAIN ID_FILE_PRINT
	"FilePrint" "&Print..." 
	PROMPT "Prints all or part of the document\0Print"
	GLYPH 8

COMMAND IDG_MRU ID_FILE_MRU_FILE1 NOKEY DYNAMIC
	"" "Recent File List"
	PROMPT "Opens this document"

COMMAND IDG_PROJ_MRU ID_PROJ_MRU_FILE1 NOKEY DYNAMIC QUERYMENU
	"" "Recent Workspace List"
	PROMPT "Opens this workspace"

COMMAND IDG_EXIT ID_APP_EXIT
	"ApplicationExit" "E&xit" 
	PROMPT "Quits the application; prompts to save documents\0Exit"


// Major group: Edit

COMMAND IDG_EDITPROPS IDM_WINDOW_SHOWPROPERTIES
	"Properties" "&Properties" 
	PROMPT "Edits the current selection's properties\0Properties"
	GLYPH 52

COMMAND IDG_UNDO ID_EDIT_UNDO  HWND
	"Undo" "&Undo" 
	PROMPT "Undoes the last action\0Undo" 
	GLYPH 3

COMMAND IDG_UNDO ID_EDIT_REDO  HWND
	"Redo" "&Redo" 
	PROMPT "Redoes the previously undone action\0Redo" 
	GLYPH 4

COMMAND IDG_CLIPBOARD ID_EDIT_CUT  REPEATABLE
	"Cut" "Cu&t" 
	PROMPT "Cuts the selection and moves it to the Clipboard\0Cut" 
	GLYPH 5

COMMAND IDG_CLIPBOARD ID_EDIT_COPY 
	"Copy" "&Copy" 
	PROMPT "Copies the selection to the Clipboard\0Copy" 
	GLYPH 6

COMMAND IDG_CLIPBOARD ID_EDIT_PASTE  REPEATABLE
	 "Paste" "&Paste" 
	 PROMPT "Inserts the Clipboard contents at the insertion point\0Paste" 
	 GLYPH 7

COMMAND IDG_CLIPBOARD ID_EDIT_CLEAR  REPEATABLE
	 "Delete" "&Delete" 
	 PROMPT "Deletes the selection\0Delete"
	 GLYPH 14

COMMAND IDG_SELECTALL ID_EDIT_SELECT_ALL
	 "SelectAll" "Select A&ll" 
	 PROMPT "Selects the entire document\0Select All"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD NOMENU
	"SetRepeatCount" "Set Repeat Count"
	 PROMPT "Sets the numeric argument or repeat count for the next command\0Set Repeat"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD0 NOMENU 
	"SetRepeatCount0" "Set Repeat Count 0"
	PROMPT "Sets the numeric argument or repeat count to 0\0Repeat 0"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD1 NOMENU 
	"SetRepeatCount1" "Set Repeat Count 1"
	PROMPT "Sets the numeric argument or repeat count to 1\0Repeat 1"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD2 NOMENU
	"SetRepeatCount2" "Set Repeat Count 2"
	PROMPT "Sets the numeric argument or repeat count to 2\0Repeat 2"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD3 NOMENU
	"SetRepeatCount3" "Set Repeat Count 3"
	PROMPT "Sets the numeric argument or repeat count to 3\0Repeat 3"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD4 NOMENU
	"SetRepeatCount4" "Set Repeat Count 4"
	PROMPT "Sets the numeric argument or repeat count to 4\0Repeat 4"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD5 NOMENU
	"SetRepeatCount5" "Set Repeat Count 5"
	PROMPT "Sets the numeric argument or repeat count to 5\0Repeat 5"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD6 NOMENU
	"SetRepeatCount6" "Set Repeat Count 6"
	PROMPT "Sets the numeric argument or repeat count to 6\0Repeat 6"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD7 NOMENU
	"SetRepeatCount7" "Set Repeat Count 7"
	PROMPT "Sets the numeric argument or repeat count to 7\0Repeat 7"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD8 NOMENU
	"SetRepeatCount8" "Set Repeat Count 8"
	PROMPT "Sets the numeric argument or repeat count to 8\0Repeat 8"

COMMAND IDG_CLIPBOARD ID_REPEAT_CMD9 NOMENU
	"SetRepeatCount9" "Set Repeat Count 9"
	PROMPT "Sets the numeric argument or repeat count to 9\0Repeat 9"

// Major group: Search

COMMAND IDG_FIND_1 ID_EDIT_FIND_DLG 
	"Find" "&Find..." 
	PROMPT "Finds the specified text\0Find" 
	GLYPH 10

COMMAND IDG_FIND_2 ID_EDIT_REPLACE 
	"FindReplace" "R&eplace..." 
	PROMPT "Replaces the specified text with different text\0Replace"

COMMAND IDG_FIND_2 ID_EDIT_FIND_NEXT NOMENU REPEATABLE
	"FindNext" "Find Next"
	PROMPT "Finds the next occurrence of the specified text\0Find Next" 
	GLYPH 37

COMMAND IDG_FIND_2 ID_EDIT_FIND_PREV NOMENU REPEATABLE
	"FindPrev" "Find Previous"
	PROMPT "Finds the previous occurrence of the specified text\0Find Previous" 
	GLYPH 38

COMMAND IDG_FIND_COMBO ID_EDIT_FIND NOMENU NOKEY HWND HWNDSIZE HWNDENABLE DYNAMIC_CMD_STRING
	""
	PROMPT "Activates the Find tool\0Find" 
	GLYPH 10

COMMAND IDG_GOTO IDM_GOTO_LINE 
	"GoTo" "&Go To..." 
	PROMPT "Moves to a specified location\0Go To"

COMMAND IDG_FIND_2 IDM_EDIT_TOGGLE_CASE NOMENU
    "FindToggleCaseSensitivity" "Case Sensitive Matching"
    PROMPT "Toggles case sensitive matching\0Toggle Case Sensitivity"
	GLYPH 45

COMMAND IDG_FIND_2 IDM_EDIT_TOGGLE_WORD NOMENU
    "FindToggleMatchWord" "Whole Word Matching"
    PROMPT "Toggles whole word matching\0Toggle Match Whole Word"
	GLYPH 44

COMMAND IDG_FIND_2 IDM_EDIT_TOGGLE_RE NOMENU
    "FindToggleRegExpr" "Regular Expression Matching"
    PROMPT "Toggles regular expression matching\0Toggle Reg Expr Matching"
	GLYPH 46

COMMAND IDG_FIND_2 IDM_EDIT_TOGGLE_SEARCH_ALL_DOCS NOMENU
    "FindToggleAllDocs" "Search Across Open Documents"
    PROMPT "Toggles searching across all open documents\0Toggle Searching Across Open Documents"
	GLYPH 47


// Major group: Insert

COMMAND IDG_PROJECT_ADD_INSERT IDM_INSERT_FILES
	"InsertFilesIntoProject" "&Files..." 
	PROMPT "Inserts existing file(s) into project\0Insert Files Into Project" 
	GLYPH 55

COMMAND IDG_PROJECTOPS_ADD IDM_PROJECT_UNLOAD NOMENU NOKEY
	"" "&Unload Project"
	PROMPT "Unloads the selected project from the workspace"

// Major group: Tools

// REVIEW_BRENDANX: For ordering in the menu this remains in shell
//              but they should be in the project package.

COMMAND IDG_VIEW_WINDOWS IDM_ACTIVATE_PROJECT_WORKSPACE
	"ActivateWorkspaceWindow" "Wor&kspace"
	PROMPT "Activates the Workspace window\0Workspace"

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_PROJECT_WORKSPACE NOMENU
	"ToggleWorkspaceWindow" "Workspace Window"
	PROMPT "Shows or hides the Workspace window\0Workspace" 
	GLYPH 41

// the dyanmic placeholder command that is used 
COMMAND IDG_TOOLBARS_LIST IDM_TOOLBARS_LIST DYNAMIC NOKEY NOMENU
	"" "Available Toolbar List" 
	PROMPT "This will be the list of all available toolbars"

// a special customize used at the bottom of the toolbars menu
COMMAND IDG_TOOLBARS_CUSTOMIZE IDM_TOOLBARS_CUSTOMIZE NOKEY NOMENU
	"" "&Customize" 
	PROMPT "Customizes the toolbars"

COMMAND IDG_OPTIONS IDM_CUSTOMIZE
	"ToolsCustomize" "&Customize..." 
	PROMPT "Customizes the Tools menu and toolbars\0Customize"

COMMAND IDG_OPTIONS IDM_TOOLBAR_CUSTOMIZE NOMENU
	"ToolsCustomizeToolbars" "&Customize..."
	PROMPT "Customizes the toolbars\0Customize Toolbars"

COMMAND IDG_OPTIONS IDM_OPTIONS
	"ToolsOptions" "&Options..." 
	PROMPT "Changes options\0Options"

COMMAND IDG_BUTTONOPS IDM_BUTTON_RESET NOMENU NOKEY
	"" "&Reset" 
	PROMPT "Resets the button and its contents to default settings"

COMMAND IDG_BUTTONOPS IDM_BUTTON_DELETE NOMENU NOKEY
	"" "&Delete" 
	PROMPT "Deletes the button"

COMMAND IDG_BUTTONOPS IDM_BUTTON_COPYIMAGE NOMENU NOKEY
	"" "&Copy Button Image" 
	PROMPT "Copies the image of the selected button to the Clipboard"

COMMAND IDG_BUTTONOPS IDM_BUTTON_PASTEIMAGE NOMENU NOKEY
	"" "&Paste Button Image" 
	PROMPT "Pastes the image on the Clipboard onto the selected button"

COMMAND IDG_BUTTONOPS IDM_BUTTON_RESETIMAGE NOMENU NOKEY
	"" "Re&set Button Image" 
	PROMPT "Resets the image of the selected button to the built-in image"

COMMAND IDG_BUTTONOPS IDM_BUTTON_CHOOSEAPPEARANCE NOMENU NOKEY
	"" "Button &Appearance..." 
	PROMPT "Allows modification of image and label for the selected button"

COMMAND IDG_BUTTONOPS IDM_BUTTON_IMAGEONLY NOMENU NOKEY
	"" "&Image Only" 
	PROMPT "Shows the selected button as an image only"

COMMAND IDG_BUTTONOPS IDM_BUTTON_TEXTONLY NOMENU NOKEY
	"" "&Text Only" 
	PROMPT "Shows the selected button as text only"

COMMAND IDG_BUTTONOPS IDM_BUTTON_IMAGETEXT NOMENU NOKEY
	"" "I&mage and Text" 
	PROMPT "Shows the selected button as image and text"

COMMAND IDG_BUTTONOPS IDM_BUTTON_GROUPSTART NOMENU NOKEY
	"" "Begin a &Group" 
	PROMPT "Begins a command group before the selected button"

COMMAND IDG_USERTOOLS IDM_USERTOOLS_BASE[16] QUERYMENU DYNAMIC
	"UserTool%d" "Tool %d"
	PROMPT "Activates user-defined tool %d\0Tool"
	GLYPH 21

COMMAND IDG_CUSTOMMENU IDM_CUSTOMMENU_BASE[16] NOMENU MENU
	"MenuCustom%d" "Custom Menu %d"
	PROMPT "Activates custom menu %d"

COMMAND IDG_OPTIONS ID_CANCEL NOMENU
	"Cancel" "Cancel"
	PROMPT "Hides windows or cancels modes\0Cancel"

// Major group: Window

COMMAND IDG_WINDOWOPS ID_WINDOW_NEW
	"WindowNewWindow" "&New Window" 
	PROMPT "Opens another window for the document\0New Window" 
	GLYPH 18

COMMAND IDG_WINDOWOPS ID_NEXT_PANE NOMENU
	"WindowNextPane" "NextPane"
	PROMPT "Activates the next pane\0Next Pane"

COMMAND IDG_WINDOWOPS ID_PREV_PANE NOMENU
	"WindowPrevPane" "NextPane"
	PROMPT "Activates the previous pane\0Previous Pane"

COMMAND IDG_WINDOWOPS ID_WINDOW_SPLIT
	"WindowSplit" "S&plit" 
	PROMPT "Splits the window into panes\0Split" 
	GLYPH 19

COMMAND IDG_WINDOWOPS IDM_TOGGLE_MDI
	"WindowDockingView" "&Docking View" 
	PROMPT "Toggles the docking feature for the window on and off\0Docking View"

COMMAND IDG_WINDOWOPS ID_WINDOW_HIDE NOMENU
	"WindowHide" "Hi&de" 
	PROMPT "Hides the window\0Hide Window"

COMMAND IDG_WINDOWOPS ID_WINDOW_POPUP_HIDE NOMENU NOKEY
	"" "&Hide" 
	PROMPT "Hides the window"

COMMAND IDG_WINDOWOPS IDM_WINDOW_CLOSE
	"WindowClose" "Cl&ose" 
	PROMPT "Closes the window\0Close Window"
	GLYPH 51

COMMAND IDG_WINDOWNEXT IDM_WINDOW_NEXT
	"WindowNextMDI" "Ne&xt" 
	PROMPT "Activates the next undocked window\0Next Window"
	GLYPH 49

COMMAND IDG_WINDOWNEXT IDM_WINDOW_PREVIOUS
	"WindowPreviousMDI" "Pre&vious" 
	PROMPT "Activates the previous undocked window\0Previous Window"
	GLYPH 50

COMMAND IDG_WINDOWALL ID_WINDOW_CASCADE
	"WindowCascade" "&Cascade" 
	PROMPT "Arranges the windows as overlapping tiles\0Cascade" 
	GLYPH 15

COMMAND IDG_WINDOWALL ID_WINDOW_TILE_HORZ
	"WindowTileHorizontally" "Tile &Horizontally" 
	PROMPT "Arranges the windows as horizontal, nonoverlapping tiles\0Tile Horizontally" 
	GLYPH 16

COMMAND IDG_WINDOWALL ID_WINDOW_TILE_VERT
	"WindowTileVertically" "&Tile Vertically" 
	PROMPT "Arranges the windows as vertical, nonoverlapping tiles\0Tile Vertically" 
	GLYPH 17

COMMAND IDG_WINDOWOPS ID_WINDOW_CLOSE_ALL
	"WindowCloseAll" "Close Al&l" 
	PROMPT "Closes all open windows\0Close All"

COMMAND IDG_VIEW_FULLSCREEN ID_WINDOW_FULLSCREEN
	"ToggleFullScreen" "F&ull Screen"
	PROMPT "Toggles Full Screen mode on and off\0Toggle Full Screen"
	GLYPH 20

COMMAND IDG_WINDOWLIST ID_WINDOW_LIST_DOCS
	"WindowList" "&Windows..."
	PROMPT "Manages the currently open windows\0Window List"
	GLYPH 42


// Major group: Help

COMMAND IDG_HELPGENERAL IDM_HELP_CONTENTS
	"HelpSystemContents" "&Contents" 
	PROMPT "Displays the table of contents for the online documentation\0Contents"

COMMAND IDG_HELPGENERAL ID_IV_SEARCH
	"HelpSystemSearch" "&Search..."
	PROMPT "Searches the online documentation\0Search"
	GLYPH 40

COMMAND IDG_HELPGENERAL ID_HELP98_KEYWORDS
	"HelpSystemIndex" "&Index..."
	PROMPT "Displays the Index to the online documentation\0Index"

COMMAND IDG_HELP_KEYBOARD ID_HELP_KEYBOARD
	"HelpKeyboard" "&Keyboard Map..."
	PROMPT "Displays all keyboard commands\0Keyboard Help"

COMMAND IDG_HELPGENERAL IDM_HELP_EXTHELP QUERYMENU
	"HelpToggleExtensionHelp" "&Use Extension Help" 
	PROMPT "Toggles extension Help for F1 on and off\0Extension Help"

COMMAND IDG_HELPDOCS IDM_HELP_TIPOFTHEDAY QUERYMENU
	"HelpTipOfTheDay" "Ti&p of the Day..." 
	PROMPT "Displays a Tip of the Day\0Tip of the Day"

COMMAND IDG_HELPDOCS IDM_HELP_MSPROD QUERYMENU
	"HelpTechnicalSupport" "&Technical Support"
	PROMPT "Displays support information for Developer Studio\0Technical Support"

COMMAND IDG_MSONTHEWEB IDM_MENU_HELP_MSONTHEWEB MENU
	"HelpXMSOnTheWeb" "Microsoft on the &Web"
	PROMPT "Starts your Web browser and goes to Microsoft Web sites"

COMMAND IDG_HELPABOUT ID_APP_ABOUT
	"HelpAbout" "&About Visual C++" 
	PROMPT "Displays the program information and copyright\0About" 
	GLYPH 9

// Microsoft on the Web submenu
COMMAND IDG_HELP_MSONTHEWEB_NEWS IDM_HELP_MOW_FREESTUFF
	"HelpMOWFreeStuff" "&Free Stuff"
	PROMPT "Opens the Developer Studio 97 free stuff page\0Free Stuff"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_NEWS IDM_HELP_MOW_PRODUCTNEWS
	"HelpMOWProductNews" "&Product News"
	PROMPT "Opens the Developer Studio 97 product news page\0Product News"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_NEWS IDM_HELP_MOW_FAQ
	"HelpMOWFAQ" "Frequently Asked &Questions"
	PROMPT "Opens the Developer Studio 97 FAQ page\0Frequently Asked Questions"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_NEWS IDM_HELP_MOW_ONLINESUPPORT
	"HelpMOWOnlineSupport" "Online &Support"
	PROMPT "Opens the Microsoft online support page\0Online Support"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_INFO IDM_HELP_MOW_DEVONLYHOMEPAGE
	"HelpMOWDevOnly" "&MSDN Online"
	PROMPT "Opens the Developer products home page\0Developers Only"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_INFO IDM_HELP_MOW_SENDFEEDBACK
	"HelpMOWFeedback" "Send Feedbac&k..."
	PROMPT "Opens the Developer products feedback page\0Send Feedback"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_INFO IDM_HELP_MOW_BESTOFTHEWEB
	"HelpMOWBestOfWeb" "&Best of the Web"
	PROMPT "Opens the Best of the Web page\0Best of the Web"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_INFO IDM_HELP_MOW_SEARCHTHEWEB
	"HelpMOWSearchWeb" "Search the &Web"
	PROMPT "Opens the search page\0Search Web"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_INFO IDM_HELP_MOW_WEBTUTORIAL
	"HelpMOWWebTutorial" "Web &Tutorial"
	PROMPT "Opens the Web Tutorial page\0Web Tutorial"
	GLYPH 53

COMMAND IDG_HELP_MSONTHEWEB_HOME IDM_HELP_MOW_MSHOMEPAGE
	"HelpMOWMSHome" "Microsoft &Home Page"
	PROMPT "Opens the Microsoft home page\0Microsoft Home Page"
	GLYPH 53

COMMAND IDG_VIEW_REFRESH IDM_REFRESH
	"Refresh" "Refres&h"
	PROMPT "Refreshes the selection\0Refresh"
	GLYPH 54

#ifdef PERF_MEASUREMENTS
COMMAND IDG_DEBUG_PERF ID_DUMP_MEM 
//COMMAND IDG_DEBUG_2 ID_DUMP_MEM 
//COMMAND IDG_HELPABOUT ID_DUMP_MEM 
	"DumpMem" "&Memory Dump"
	PROMPT "Dump the current working set to a disk file\0DumpMem"
#endif
