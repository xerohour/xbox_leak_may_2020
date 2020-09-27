// Debug package commands

CMDTABLE IDCT_DBGPACKAGE

COMMAND IDG_DEBUG_GO IDM_RUN_GO
	"DebugGo" "&Go"
	PROMPT "Starts or continues the program\0Go"
	GLYPH 0

// Major group: Debug

COMMAND IDG_DEBUGRUN IDM_RUN_RESTART
	"DebugRestart" "&Restart"
	PROMPT "Restarts the program\0Restart"
	GLYPH 5

COMMAND IDG_DEBUGRUN IDM_RUN_STOPDEBUGGING
	"DebugStopDebugging" "Stop &Debugging"
	PROMPT "Stops debugging the program\0Stop Debugging"
	GLYPH 6

COMMAND IDG_DEBUGRUN IDM_RUN_BREAK
	"DebugBreak" "&Break"
	PROMPT "Stops program execution; breaks into the debugger\0Break Execution"
	GLYPH 18

COMMAND IDG_DEBUGRUN IDM_RUN_TOGGLEBREAK NOMENU
	"DebugToggleBreakpoint" "Insert/Remove Breakpoint"
	PROMPT "Inserts or removes a breakpoint\0Insert/Remove Breakpoint"
	GLYPH 7

COMMAND IDG_DEBUGRUN IDM_RUN_CLEARALLBREAK NOMENU
	"DebugRemoveAllBreakpoints" "Remove All Breakpoints"
	PROMPT "Removes all breakpoints\0Remove All Breakpoints"
	GLYPH 8

COMMAND IDG_DEBUGRUN IDM_RUN_ENABLEBREAK NOMENU
	"DebugEnableBreakpoint" "Enable Breakpoint"
	PROMPT "Enables or disables a breakpoint\0Enable/Disable Breakpoint"
    GLYPH 16

COMMAND IDG_DEBUGRUN IDM_RUN_DISABLEALLBREAK NOMENU
	"DebugDisableAllBreakpoints" "Disable All Breakpoints"
	PROMPT "Disables all breakpoints\0Disable All Breakpoints"
    GLYPH 17

COMMAND IDG_DEBUG_STEPINTO IDM_RUN_TRACEINTO PROJECT_DEPENDENT
	"DebugStepInto" "Step &Into"
	PROMPT "Steps into the next statement\0Step Into"
	GLYPH 1

COMMAND IDG_DEBUGSTEP1 IDM_RUN_STEPOVER
	"DebugStepOver" "Step &Over"
	PROMPT "Steps over the next statement\0Step Over"
	GLYPH 2

COMMAND IDG_DEBUGSTEP1 IDM_RUN_SRC_STEPOVER NOMENU
	"DebugStepOverSource" "Step &Over Source"
	PROMPT "Steps over the next source level statement\0Step Over Source"

COMMAND IDG_DEBUGSTEP1 IDM_RUN_STOPAFTERRETURN
	"DebugStepOut" "Step O&ut"
	PROMPT "Steps out of the current function\0Step Out"
	GLYPH 3

COMMAND IDG_DEBUG_RUNTOCURSOR IDM_RUN_TOCURSOR PROJECT_DEPENDENT
	"DebugRunToCursor" "Run to &Cursor"
	PROMPT "Runs the program to the line containing the cursor\0Run to Cursor"
	GLYPH 4

COMMAND IDG_DEBUGSTEP1 IDM_RUN_SETNEXTSTMT NOMENU
	"DebugSetNextStatement" "Set Next Statement"
	PROMPT "Sets the instruction pointer to the line containing the cursor\0Set Next Statement"
	GLYPH 20

COMMAND IDG_DEBUGSHOW IDM_DEBUG_SHOWIP
	"DebugShowNextStatement" "Show &Next Statement"
	PROMPT "Displays the source line for the instruction pointer\0Show Next Statement"
	GLYPH 19

COMMAND IDG_BREAKPOINTS IDM_RUN_SETBREAK PROJECT_DEPENDENT
	"DebugBreakpoints" "Brea&kpoints..."
	PROMPT "Edits breakpoints in the program\0Breakpoints"

COMMAND IDG_DEBUGSHOW IDM_RUN_QUICKWATCH
	"DebugQuickWatch" "&QuickWatch..."
	PROMPT "Performs immediate evaluation of variables and expressions\0QuickWatch"
	GLYPH 9

COMMAND IDG_DEBUGSHOW IDM_GOTO_DISASSY NOMENU NOKEY
	"" "Go To Disassembly"
	PROMPT "Activates the disassembly window for this instruction"

COMMAND IDG_DEBUGSHOW IDM_GOTO_SOURCE NOMENU NOKEY
      "" "Go To Source"
      PROMPT "Activates the source window for this instruction"

COMMAND IDG_DEBUGSHOW IDM_VIEW_MIXEDMODE NOMENU
	"DebugToggleMixedMode" "Toggle Mixed Mode"
	PROMPT "Switches between the source view and the disassembly view for this instruction\0Toggle Mixed"

COMMAND IDG_DEBUGSHOW IDM_GOTO_CODE NOMENU NOKEY
	"" "Go To Code"
	PROMPT "Activates the stack frame corresponding to this function"

COMMAND IDG_BUILD_DEBUGMENU IDM_DEBUG_ACTIVE
	"ToolsRemoteConnection" "Debugger Remote Co&nnection..."
	PROMPT "Edits remote debug connection settings\0Remote Connection"

COMMAND IDG_DEBUGTHREADS IDM_RUN_EXCEPTIONS
	"DebugExceptions" "&Exceptions..."
	PROMPT "Edits debug actions taken when an exception occurs\0Exceptions"
	GLYPH 22

COMMAND IDG_DEBUGTHREADS IDM_RUN_THREADS
	"DebugThreads" "&Threads..."
	PROMPT "Sets the debuggee's thread attributes\0Threads"
	GLYPH 21

COMMAND IDG_DEBUGTHREADS IDM_RUN_FIBERS
	"DebugFibers" "&Fibers..."
	PROMPT "Activates the fiber selection window\0Fibers"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_WATCH PROJECT_DEPENDENT
	"ActivateWatchWindow" "&Watch"
	PROMPT "Activates the Watch window\0Watch"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_CALLSTACK PROJECT_DEPENDENT
	"ActivateCallStackWindow" "&Call Stack"
	PROMPT "Activates the Call Stack window\0Call Stack"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_MEMORY PROJECT_DEPENDENT
	"ActivateMemoryWindow" "&Memory"
	PROMPT "Activates the Memory window\0Memory"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_VARIABLES PROJECT_DEPENDENT
	"ActivateVariablesWindow" "&Variables"
	PROMPT "Activates the Variables window\0Variables"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_REGISTERS PROJECT_DEPENDENT
	"ActivateRegistersWindow" "&Registers"
	PROMPT "Activates the Registers window\0Registers"

COMMAND IDG_VIEW_DEBUG_WINDOWS IDM_ACTIVATE_DISASSEMBLY PROJECT_DEPENDENT
	"ActivateDisassemblyWindow" "&Disassembly"
	PROMPT "Activates the Disassembly window\0Disassembly"

// Major group: Tools
COMMAND IDG_DEBUGOPTS IDM_DEBUG_RADIX NOMENU
	"DebugHexadecimalDisplay" "Hexadecimal Display"
	PROMPT "Toggles between decimal and hexadecimal format\0Hexadecimal"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_FLOAT NOMENU NOKEY
	"" "Floating Point Registers"
	PROMPT "Shows or hides floating point registers"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_STACKTYPES NOMENU NOKEY
	"" "Parameter Types"
	PROMPT "Shows or hides function parameter types"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_STACKVALUES NOMENU NOKEY
	"" "Parameter Values"
	PROMPT "Shows or hides function parameter values"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_SOURCE NOMENU NOKEY
	"" "Source Annotation"
	PROMPT "Shows or hides source annotation"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_CODEBYTES NOMENU NOKEY
	"" "Code Bytes"
	PROMPT "Shows or hides code bytes"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_MEMBYTE NOMENU NOKEY
	"" "Byte Format"
	PROMPT "Sets byte display format"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_MEMSHORT NOMENU NOKEY
	"" "Short Hex Format"
	PROMPT "Sets short hex display format"

COMMAND IDG_DEBUGOPTS IDM_MEMORY_NEXTFORMAT NOMENU
	"DebugMemoryNextFormat" "Next Memory format"
	PROMPT "Switches the memory window to the next display format\0Memory Next Format"

COMMAND IDG_DEBUGOPTS IDM_MEMORY_PREVFORMAT NOMENU
	"DebugMemoryPrevFormat" "Previous Memory format"
	PROMPT "Switches the memory window to the previous display format\0Memory Previous Format"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_MEMLONG NOMENU NOKEY
	"" "Long Hex Format"
	PROMPT "Sets long hex display format"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_MEMORY_BAR NOMENU NOKEY
	"" "Toolbar"
	PROMPT "Shows or hides the Memory toolbar"

COMMAND IDG_DEBUGOPTS IDM_DBGSHOW_VARS_BAR NOMENU NOKEY
	"" "Toolbar"
	PROMPT "Shows or hides the Variables toolbar"


COMMAND IDG_DEBUGSTEP2 IDM_RUN_TRACEFUNCTION
 	"DebugStepIntoSpecificFunction" "Ste&p Into Specific Function"
 	PROMPT "Steps into the selected function\0Step Into Specific Function"

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_WATCH NOMENU
	"ToggleWatchWindow" "Watch Window"
	PROMPT "Shows or hides the Watch window\0Watch"
	GLYPH 10

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_VARIABLES NOMENU
	"ToggleVariablesWindow" "Variables Window"
	PROMPT "Shows or hides the Variables window\0Variables"
	GLYPH 11

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_REGISTERS NOMENU
	"ToggleRegistersWindow" "Registers Window"
	PROMPT "Shows or hides the Registers window\0Registers"
	GLYPH 12

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_MEMORY NOMENU
	"ToggleMemoryWindow" "Memory Window"
	PROMPT "Shows or hides the Memory window\0Memory"
	GLYPH 13

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_CALLSTACK NOMENU
	"ToggleCallStackWindow" "Call Stack Window"
	PROMPT "Shows or hides the Call Stack window\0Call Stack"
	GLYPH 14

COMMAND IDG_VIEW_WINDOWS IDM_TOGGLE_DISASSEMBLY NOMENU
	"ToggleDisassemblyWindow" "Disassembly Window"
	PROMPT "Shows or hides the Disassembly window\0Disassembly"
	GLYPH 15

COMMAND IDG_DEBUG_ATTACH IDM_RUN_ATTACH_TO_ACTIVE
	"DebugAttachToActive" "&Attach to Process..."
	PROMPT "Attaches to running process\0Attach to Process"

COMMAND IDG_DEBUGRUN IDM_DEBUG_UPDATEIMAGE NOMENU
	"ApplyCodeChanges" "Appl&y Code Changes"
	PROMPT "Applies code changes made to C/C++ source files while debugging\0Apply Code Changes"
	GLYPH 23

COMMAND IDG_DEBUGRUN IDM_DEBUG_UPDATEIMAGE_TOGGLE
	"UpdateImageToggle" "Appl&y Code Changes"
	PROMPT "Applies code changes made to C/C++ source files while debugging\0Apply Code Changes"

COMMAND IDG_DEBUGTHREADS IDM_RUN_FLIPHACK
	"DebugModules" "&Modules..."
	PROMPT "Shows modules currently loaded\0Modules"
