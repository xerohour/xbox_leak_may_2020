FMT_ELEMENT *pfmtelFromDocType(DOCTYPE doctype);
FMT_WINDOW  *pfmtcatFromDocType(DOCTYPE doctype);
FMT_ELEMENT *pfmtelForVarsWnd(UINT);
FMT_WINDOW *pfmtcatForVarsWnd(UINT);

// Text Windows
#define FMTEL_TEXT (0)	// Text
#define FMTEL_SELC (1)	// Selected text
#define FMTEL_BRKP (2)	// Breakpoint
#define FMTEL_CRNT (3)	// Current line

// Call Stack
#define FMTEL_CSHI (2)	// Highlight
#define FMTEL_CSSU (3)	// Superceded code

// Memory Window
#define FMTEL_MMHI (2)	// Highlight

// Register Window
#define FMTEL_RGHI (2)	// Register highlight

// Vars/Watch window.
#define FMTEL_VCHI (2)  // Variables/watch change highlight color.

// Disassembly window.
#define FMTEL_DASM (2)  // Assembly code in disassembly window.

// Number of predefined token classes - from colors.cpp
#define CPREDEFTOKCLS (10)
