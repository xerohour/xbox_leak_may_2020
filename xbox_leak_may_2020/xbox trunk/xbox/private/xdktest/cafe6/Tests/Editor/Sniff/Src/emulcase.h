///////////////////////////////////////////////////////////////////////////////
//	emulcase.h
//
//	Created by :			
//		GeorgeCh
//
//	Description :
//		Declaration of the Edsnif01TestSet Class
//

#ifndef __EMULCASE_H__
#define __EMULCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// enum Editor {VC2,VC3,Brief,Epsilon};

// Edit stuff
#define BackEpsilonParagraph 	0        
#define BackEpsilonSentence		1        
#define CapitalizeEpsilonWord	2        
#define ColumnSelect			3        
#define CursorBackWord			4        
#define CursorBeginDocument		5        
#define CursorBeginLine			6        
#define CursorBeginWindow		7        
#define CursorBriefEnd			8        
#define CursorBriefHome			9        
#define CursorDown		 		10       
#define CursorEndDocument		11       
#define CursorEndLine			12       
#define CursorEndWindow			13       
#define CursorForwardWord		14       
#define CursorHome				15       
#define CursorLeft				16       
#define CursorLeftWindow		17       
#define CursorPageDown			18       
#define CursorPageUp			19       
#define CursorRight				20       
#define CursorRightWindow		21       
#define CursorUp				22       
#define DeleteBlankLines        23      
#define DeleteHorizontalSpace   24      
#define DeleteNextWord          25      
#define DeletePreviousWord      26      
#define DeleteToBeginningOfLine 27      
#define DeleteToEndOfLine       28      
#define EditCopy                29      
#define EditCut                 30      
#define EditCutLine             31      
#define EditDelete              32      
#define EditDeleteLine          33      
#define EditLowerCase           34      
#define EditPaste               35      
#define EditProperties          36      
#define EditRedo                37      
#define EditRedoChanges         38      
#define EditSelectAll           39      
#define EditUndo                40      
#define EditUndoChanges         41      
#define EditUpperCase           42      
#define ForwardEpsilonParagraph 43      
#define ForwardEpsilonSentence  44      
#define HighlightEpsilonRegion  45      
#define IndentEpsilonRegion     46      
#define IndentLines             47      
#define KillEpsilonRegion       48      
#define KillEpsilonSentence     49      
#define LineSelect              50      
#define LowerCaseEpsilonWord    51      
#define MarkEpsilonParagraph    52      
#define StreamSelectExclusive   53      
#define StreamSelectInclusive   54      
#define SwapAncorPoint          55      
#define TabifyLines             56      
#define ToggleOverstrike        57      
#define ToggleTabDisplay        58      
#define TransposeCharacters     59
#define TransposeLines          60
#define TransposeWords          61
#define UnindentLines           62
#define UntabifyLines           63
#define UpperCaseEpsilonWord    64
#define EditBookmarks           65
#define NextDef                 66
#define PreviousDef             67
#define SearchAgain             68
#define SearchClearMarks        69
#define SearchCondDown          70
#define SearchCondUp            71
#define Bookmark10              72
#define Bookmark1               73
#define Bookmark2               74
#define Bookmark3               75
#define Bookmark4               76
#define Bookmark5               77
#define Bookmark6               78
#define Bookmark7               79
#define Bookmark8               80
#define Bookmark9               81
#define DropEpsMark             82
#define dFind                    83
#define FindNext                84
#define FindNextWord            85
#define FindPrev                86
#define FindPrevWord            87
#define GoTo                    88
#define GoToDef                 89
#define GoToTag                 90
#define GoToFile                91
#define GoToRef                 92
#define MatchBrace              93
#define NextMark                94
#define NextEpsMark             95
#define NextErrorTag            96
#define PreviousMark            97 
#define PreviousTag             98 
#define Replace                 99 
#define ToggleMark              100
#define SearchTool              101
#define SelectCondDown          102
#define SelectCondUp            103
#define ToggleCase              104
#define ToggleRE                105
#define ToggleSearchForWord     106
                                
// Add one to array subscript for 0
                                
static LPCTSTR KeyCommand[107][8] = { 
//                               VC2         VC3         Brief,      Epsilon,     C_VC2,      C_VC3,      C_Brief,    C_Epsilon
/* BackEpsilonParagraph     */  {"%{UP}",    "%{UP}",    "",         "(%[)",      "",         "",         "",         "%{UP}"},   
/* BackEpsilonSentence      */  {"^%{LEFT}", "^%{LEFT}", "",         "(%A)",      "",         "",         "",         "^{UP}"},   
/* CapitalizeEpsilonWord    */  {"",         "",         "",         "(%C)",      "",         "",         "",         ""},        
/* ColumnSelect             */  {"^+{F8}",   "^+{F8}",   "%C",       "(^X)(+3)",  "",         "",         "",         ""},      
/* CursorBackWord           */  {"^{LEFT}",  "^{LEFT}",  "^{LEFT}",  "(%B)",      "",         "",         "^+{LEFT}", "^{LEFT}"}, 
/* CursorBeginDocument      */  {"^{HOME}",  "^{HOME}",  "^{PGUP}",  "(%<)",      "",         "",         "",         "^{HOME}"}, 
/* CursorBeginLine          */  {"",         "",         "",         "",          "",         "",         "",         ""},        
/* CursorBeginWindow        */  {"",         "",         "^{HOME}",  "(%,)",      "",         "",         "",         "{HOME}"},  
/* CursorBriefEnd           */  {"",         "",         "{END}",    "",          "",         "",         "",         ""},        
/* CursorBriefHome          */  {"",         "",         "{HOME}",   "",          "",         "",         "",         ""},        
/* CursorDown               */  {"{DOWN}",   "{DOWN}",   "{DOWN}",   "^N",        "",         "",         "",         "{DOWN}"},      
/* CursorEndDocument        */  {"^{END}",   "^{END}",   "^{PGDN}",  "(%>)",      "",         "",         "",         "^{END}"},  
/* CursorEndLine            */  {"{END}",    "{END}",    "",         "(%{RIGHT})","",         "",         "",         "^E"},      
/* CursorEndWindow          */  {"",         "",         "^{END}",   "(%.)"      ,"",         "",         "",         "{END}"},   
/* CursorForwardWord        */  {"^{RIGHT}", "^{RIGHT}", "^{RIGHT}", "(%F)"      ,"",         "",         "",         "^{RIGHT}"},
/* CursorHome               */  {"{HOME}",   "{HOME}",   "",         "%{LEFT}"   ,"",         "",         "",         "^A"},        
/* CursorLeft               */  {"{LEFT}",   "{LEFT}",   "{LEFT}",   "^B"        ,"",         "",         "+{LEFT}",  "{LEFT}"},        
/* CursorLeftWindow         */  {"",         "",         "+{HOME}",  ""          ,"",         "",         "",         ""},        
/* CursorPageDown           */  {"{PGDN}",   "{PGDN}",   "{PGDN}",   "{PGDN}"    ,"",         "",         "",         "^V"},        
/* CursorPageUp             */  {"{PGUP}",   "{PGUP}",   "{PGUP}",   "{PGUP}"    ,"",         "",         "",         "%V"},        
/* CursorRight              */  {"{RIGHT}",  "{RIGHT}",  "{RIGHT}",  "^f"        ,"",         "",         "",         "{RIGHT}"},        
/* CursorRightWindow        */  {"",         "",         "+{END}",   ""          ,"",         "",         "",         ""},        
/* CursorUp                 */  {"{UP}",     "{UP}",     "{UP}",     "^P"        ,"",         "",         "",         "{UP}"},        
/* DeleteBlankLines         */  {"",         "",         "",         "(^X)(^O)"  ,"",         "",         "",         ""},
/* DeleteHorizontalSpace    */  {"",         "",         "",         "%\\"       ,"",         "",         "",         ""},
/* DeleteNextWord           */  {"^{DEL}",   "^{DEL}",   "%{BS}",    "%D"        ,"",         "",         "",         ""},        
/* DeletePreviousWord       */  {"^{BS}",    "^{BS}",    "^{BS}",    "%{BS}"     ,"",         "",         "",         ""},        
/* DeleteToBeginningOfLine  */  {"",         "",         "^K",       ""          ,"",         "",         "",         ""},
/* DeleteToEndOfLine        */  {"",         "",         "%K",       ""          ,"",         "",         "",         ""},
/* EditCopy                 */  {"^C",       "^C",       "{NUMPAD+}","%W"        ,"^{INS}",   "^{INS}",   "^{INS}",   "^C"},
/* EditCut                  */  {"^X",       "^X",       "{NUMPAD-}","+{DEL}"    ,"+{DEL}",   "+{DEL}",   "+{DEL}",   ""},
/* EditCutLine              */  {"^Y",       "^Y",       "",         "^K"        ,"",         "",         "",         ""},
/* EditDelete               */  {"{DEL}",    "{DEL}",    "{DEL}",    "^D"        ,"",         "",         "",         "{DEL}"},
/* EditDeleteLine           */  {"",         "",         "%D",       ""          ,"",         "",         "",         ""},
/* EditLowerCase            */  {"^U",       "^U",       "^{DOWN}",  ""          ,"",         "",         "",         ""},
/* EditPaste                */  {"^V",       "^V",       "{INS}",    "^V"        ,"+{INS}",   "+{INS}",   "+{INS}",   "^Y"},
/* EditProperties           */  {"%{ENTER}", "%{ENTER}", "%{ENTER}", "(%{ENTER})","",         "",         "",         ""},
/* EditRedo                 */  {"^A",       "^A",       "^U",       "(^X)(R)"   ,"",         "",         "^A",       "{F10}"},
/* EditRedoChanges          */  {"",         "",         "",         "(^X)(^R)"  ,"",         "",         "",         "^{F10}"},
/* EditSelectAll            */  {"",         "",         "",         ""          ,"",         "",         "",         ""},
/* EditUndo                 */  {"^Z",       "^Z",       "{NUMPAD*}","(^X)(U)"   ,"%{BS}",    "%{BS}",    "%U",       "{F9}"},
/* EditUndoChanges          */  {"",         "",         "",         "(^X)(^U)"  ,"",         "",         "",         "^{F9}"},
/* EditUpperCase            */  {"^+U",      "^+U",      "^{UP}",    ""          ,"",         "",         "",         ""},
/* ForwardEpsilonParagraph  */  {"%{DOWN}",  "%{DOWN}",  "",         "(%])"      ,"",         "",         "",         "%{DOWN}"},
/* ForwardEpsilonSentence   */  {"^%{RIGHT}","^%{RIGHT}","",         "(%E)"      ,"",         "",         "",         "^{DOWN}"},
/* HighlightEpsilonRegion   */  {"^H",       "^H",       "",         "(^X)(^H)"  ,"",         "",         "",         ""},
/* IndentEpsilonRegion      */  {"^%I",      "^%I",      "",         ""          ,"",         "",         "",         "^%{BS}"},
/* IndentLines              */  {"",         "",         "",         ""          ,"",         "",         "",         ""},
/* KillEpsilonRegion        */  {"^%w",      "^%w",      "",         "^w"        ,"",         "",         "",         ""},
/* KillEpsilonSentence      */  {"^%K",      "^%K",      "",         "%K"        ,"",         "",         "",         ""},
/* LineSelect               */  {"^%{F8}",   "^%{F8}",   "%L",       ""          ,"",         "",         "",         ""},
/* LowerCaseEpsilonWord     */  {"",         "",         "",         "(%L)"      ,"",         "",         "",         ""},
/* MarkEpsilonParagraph     */  {"",         "",         "",         "%H"        ,"",         "",         "",         ""},
/* StreamSelectExclusive    */  {"^+A",      "^+A",      "%A",       "%@"        ,"",         "",         "",         "^2"},
/* StreamSelectInclusive    */  {"^+I",      "^+I",      "%M",       ""          ,"",         "",         "",         ""},
/* SwapAncorPoint           */  {"^+X",      "^+X",      "^+X",      "(^X)(^X)"  ,"",         "",         "",         ""},
/* TabifyLines              */  {"^+T",      "^+T",      "",         "(^X)(%T)"  ,"",         "",         "",         ""},
/* ToggleOverstrike         */  {"{INS}",    "{INS}",    "%I",       "{INS}"     ,"",         "",         "",         ""},
/* ToggleTabDisplay         */  {"^%T",      "^%T",      "^%T",      ""          ,"",         "",         "",         ""},
/* TransposeCharacters      */  {"",         "",         "",         "^T"        ,"",         "",         "",         ""},
/* TransposeLines           */  {"",         "",         "",         "(^X)(^T)"  ,"",         "",         "",         ""},
/* TransposeWords           */  {"",         "",         "",         "(%T)"      ,"",         "",         "",         ""},
/* UnindentLines            */  {"",         "",         "",         ""          ,"",         "",         "",         ""},
/* UntabifyLines            */  {"^+ ",      "^+ ",      "",         "(^X)(%I)"  ,"",         "",         "",         ""},
/* UpperCaseEpsilonWord     */  {"",         "",         "",         "(%U)"      ,"",         "",         "",         ""},
/* EditBookmarks            */  {"",         "",         "%J",       "(^X)(/)"   ,"",         "",         "",         "(^X)(J)"},
/* NextDef                  */  {"+{NUMPAD+}","+{NUMPAD+}","^{NUMPAD+}",""       ,"",         "",         "",         ""},
/* PreviousDef              */  {"+{NUMPAD-}","+{NUMPAD-}","",       ""          ,"",         "",         "",         ""},
/* SearchAgain              */  {"",         "",         "+{F5}",    ""          ,"",         "",         "",         ""},
/* SearchClearMarks         */  {"",         "",         "",         ""          ,"",         "",         "",         ""},
/* SearchCondDown           */  {"^.",       "^.",       "^.",       ""          ,"",         "",         "",         ""},
/* SearchCondUp             */  {"^,",       "^,",       "^,",       ""          ,"",         "",         "",         ""},
/* Bookmark10               */  {"",         "",         "%0",       ""          ,"",         "",         "",         ""},
/* Bookmark1                */  {"",         "",         "%1",       ""          ,"",         "",         "",         ""},
/* Bookmark2                */  {"",         "",         "%2",       ""          ,"",         "",         "",         ""},
/* Bookmark3                */  {"",         "",         "%3",       ""          ,"",         "",         "",         ""},
/* Bookmark4                */  {"",         "",         "%4",       ""          ,"",         "",         "",         ""},
/* Bookmark5                */  {"",         "",         "%5",       ""          ,"",         "",         "",         ""},
/* Bookmark6                */  {"",         "",         "%6",       ""          ,"",         "",         "",         ""},
/* Bookmark7                */  {"",         "",         "%7",       ""          ,"",         "",         "",         ""},
/* Bookmark8                */  {"",         "",         "%8",       ""          ,"",         "",         "",         ""},
/* Bookmark9                */  {"",         "",         "%9",       ""          ,"",         "",         "",         ""},
/* DropEpsMark              */  {"",         "",         "",         "%/"        ,"",         "",         "",         ""},
/* Find                     */  {"%{F3}",    "%{F3}",    "%{F3}",    "%{F3}"     ,"",         "",         "",         ""},
/* FindNext                 */  {"{F3}",     "{F3}",     "",         ""          ,"",         "",         "",         ""},
/* FindNextWord             */  {"^{F3}",    "^{F3}",    "",         ""          ,"",         "",         "",         ""},
/* FindPrev                 */  {"+{F3}",    "+{F3}",    "+{F3}",    "+{F3}"     ,"",         "",         "",         ""},
/* FindPrevWord             */  {"^+{F3}",   "^+{F3}",   "",         ""          ,"",         "",         "",         ""},
/* GoTo                     */  {"^G",       "^G",       "^G",       "(^X)(G)"   ,"",         "",         "",         ""},
/* GoToDef                  */  {"%{F1}",    "%{F1}",    "{F11}",    "(^X)(,)"   ,"{F11}",    "{F11}",    "",         "%{F1}"},
/* GoToTag                  */  {"",         "",         "",         ""          ,"",         "",         "",         ""},
/* GoToFile                 */  {"^+G",      "^+G",      "^+G",      "^+G"       ,"",         "",         "",         ""},
/* GoToRef                  */  {"%{F1}",    "%{F1}",    "%{F1}",    "%{F1}"     ,"+{F11}",   "+{F11}",   "+{F11}",   "+{F11}"},
/* MatchBrace               */  {"^]",       "^M",       "^]",       ""          ,"^+]",      "^+M",      "^M",       ""},
/* NextMark                 */  {"{F2}",      "{F2}",    "",         ""          ,"",         "",         "",         ""},
/* NextEpsMark              */  {"",          "",        "",         "%J"        ,"",         "",         "",         ""},
/* NextErrorTag             */  {"{F4}",      "{F4}",    "^N",       "(^X)(^N)"  ,"",         "",         "",         ""},
/* PreviousMark             */  {"+{F2}",     "+{F2}",   "",         ""          ,"",         "",         "",         ""},
/* PreviousTag              */  {"+{F4}",     "+{F4}",   "+{F4}",    "(^X)(^P)"  ,"",         "",         "",         "+{F4}"},
/* Replace                  */  {"",          "",        "",         ""          ,"",         "",         "",         ""},
/* ToggleMark               */  {"^{F2}",     "^{F2}",   "",         ""          ,"",         "",         "",         ""},
/* SearchTool               */  {"%A",        "%A",      "^f",       ""          ,"^f",       "^f",       "",         ""},
/* SelectCondDown           */  {"^+.",       "^+.",     "^+.",      ""          ,"",         "",         "",         ""},
/* SelectCondUp             */  {"^+,",       "^+,",     "^+,",      ""          ,"",         "",         "",         ""},
/* ToggleCase               */  {"",          "",        "^{F5}",    ""          ,"",         "",         "",         ""},
/* ToggleRE                 */  {"",          "",        "^{F6}",    ""          ,"",         "",         "",         ""},
/* ToggleSearchForWord      */  {"",          "",        "",         ""          ,"",         "",         "",         ""},
};


///////////////////////////////////////////////////////////////////////////////
//	Sniff test classes

#endif //__EMULCASE_H__
