/////////////////////////////////////////////////////////////////////////////
// cafehelp.h
//
// email	date		change
// briancr	10/21/94	created
//
// copyright 1994 Microsoft


// RTF filename
CString strListFile = "cafehelp.lst";
CString strRTFFile = "cafehelp.rtf";

// comment block constants
const CString cstrBeginComment = "BEGIN_HELP_COMMENT";
const CString cstrEndComment = "END_HELP_COMMENT";
const CString cstrFunction = "Function:";
const CString cstrDescription = "Description:";
const CString cstrReturn = "Return:";
const CString cstrParam = "Param:";

// prototypes
void Error(LPCSTR szMsg, ...);
void Warning(LPCSTR szMsg, ...);
void Debug(LPCSTR szMsg, ...);
BOOL ProcessCmdLine(int argc, char **argv);
void Usage(void);

BOOL ReadFileList(CString strFilename, CStringList &listFiles);
void RTFHeader(CStdioFile &file);
void ProcessCommentBlocks(CString strFilename, CStdioFile &file);
void RTFFunction(CString strLine, CString strFilename, int nLine, CStdioFile &file);
void RTFDescription(CString strLine, CString strFilename, int nLine, CStdioFile &file);
void RTFReturn(CString strLine, CString strFilename, int nLine, CStdioFile &file);
void RTFParamHdr(CString strLine, CString strFilename, int nLine, CStdioFile &file);
void RTFParam(CString strLine, CString strFilename, int nLine, CStdioFile &file);
void RTFEndOfFunction(CString strLine, CString strFilename, int nLine, CStdioFile &file);
CString EliminateLeadingChars(CString str, CString strSet);
CString EliminateTrailingChars(CString str, CString strSet);
void RTFEnd(CStdioFile &file);


// RTF header
CString cstrRTFHeader[] = { "{\\rtf1\\ansi \\deff0\\deflang1033{\\fonttbl{\\f0\\froman\\fcharset0\\fprq2 Tms Rmn{\\*\\falt Times New Roman};}{\\f2\\fswiss\\fcharset0\\fprq2 Helv{\\*\\falt Helvetica};}{\\f8\\froman\\fcharset0\\fprq2 Times;}{\\f7\\fswiss\\fcharset0\\fprq2 MS Sans Serif;}",
							"{\\f9\\fswiss\\fcharset0\\fprq2 Helvetica;}}{\\colortbl;\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red255\\green255\\blue255;\\red0\\green0\\blue128;",
							"\\red0\\green128\\blue128;\\red0\\green128\\blue0;\\red128\\green0\\blue128;\\red128\\green0\\blue0;\\red128\\green128\\blue0;\\red128\\green128\\blue128;\\red192\\green192\\blue192;}{\\stylesheet{\\nowidctlpar \\fs20 \\snext0 Normal;}{\\s1\\sb240\\nowidctlpar \\b\\f2\\ul ",
							"\\sbasedon0\\snext0 heading 1;}{\\s2\\sb120\\nowidctlpar \\b\\f2 \\sbasedon0\\snext0 heading 2;}{\\s3\\li360\\nowidctlpar \\b \\sbasedon0\\snext17 heading 3;}{\\s4\\li360\\nowidctlpar \\ul \\sbasedon0\\snext17 heading 4;}{\\s5\\li720\\nowidctlpar \\b\\fs20 \\sbasedon0\\snext17 ",
							"heading 5;}{\\s6\\li720\\nowidctlpar \\fs20\\ul \\sbasedon0\\snext17 heading 6;}{\\s7\\li720\\nowidctlpar \\i\\fs20 \\sbasedon0\\snext17 heading 7;}{\\s8\\li720\\nowidctlpar \\i\\fs20 \\sbasedon0\\snext17 heading 8;}{\\s9\\li720\\nowidctlpar \\i\\fs20 \\sbasedon0\\snext17 ",
							"heading 9;}{\\*\\cs10 \\additive Default Paragraph Font;}{\\*\\cs15 \\additive\\f0\\fs16\\up6\\lang1033 \\sbasedon10 footnote reference;}{\\s16\\nowidctlpar \\fs20 \\sbasedon0\\snext16 footnote text;}{\\s17\\li720\\nowidctlpar \\fs20 \\sbasedon0\\snext17 Normal Indent;}{",
							"\\s18\\fi-240\\li480\\sb80\\nowidctlpar\\tx480 \\f9 \\sbasedon0\\snext18 nscba;}{\\s19\\fi-240\\li240\\sa20\\nowidctlpar \\f9 \\sbasedon0\\snext19 j;}{\\s20\\li480\\sa20\\nowidctlpar \\f9 \\sbasedon0\\snext20 ij;}{\\s21\\sb80\\sa20\\nowidctlpar \\f9 \\sbasedon0\\snext21 btb;}{",
							"\\s22\\fi-240\\li2400\\sb20\\sa20\\nowidctlpar \\f9\\fs20 \\sbasedon0\\snext22 ctcb;}{\\s23\\fi-240\\li480\\sa40\\nowidctlpar\\tx480 \\f9 \\sbasedon0\\snext23 ns;}{\\s24\\sa120\\nowidctlpar \\f9\\fs28 \\sbasedon0\\snext24 TT;}{\\s25\\fi-240\\li2400\\sa20\\nowidctlpar \\f9 ",
							"\\sbasedon0\\snext25 crtj;}{\\s26\\fi-240\\li480\\nowidctlpar\\tx480 \\f9 \\sbasedon0\\snext26 nsca;}{\\s27\\sa20\\nowidctlpar \\f9 \\sbasedon0\\snext27 bt;}{\\s28\\li240\\sb120\\sa40\\nowidctlpar \\f9 \\sbasedon0\\snext28 Hf;}{\\s29\\li240\\sb120\\sa40\\nowidctlpar \\f9 ",
							"\\sbasedon0\\snext29 Hs;}{\\s30\\li480\\sb120\\sa40\\nowidctlpar \\f9 \\sbasedon0\\snext30 RT;}{\\s31\\fi-2160\\li2160\\sb240\\sa80\\nowidctlpar\\tx2160 \\f9 \\sbasedon0\\snext31 c;}{\\s32\\li2160\\sa20\\nowidctlpar \\f9 \\sbasedon0\\snext32 ct;}{\\s33\\li240\\sa20\\nowidctlpar \\f9 ",
							"\\sbasedon0\\snext33 it;}{\\s34\\li480\\nowidctlpar \\f9\\fs20 \\sbasedon0\\snext34 nsct;}{\\s35\\fi-160\\li400\\sb80\\sa40\\nowidctlpar \\f9 \\sbasedon0\\snext35 nscb;}{\\s36\\fi-2640\\li2880\\sb120\\sa40\\nowidctlpar\\brdrb\\brdrs\\brdrw15 \\brdrbtw\\brdrs\\brdrw15 \\tx2880 \\f9 ",
							"\\sbasedon0\\snext36 HC2;}{\\s37\\fi-2640\\li2880\\sb120\\sa20\\nowidctlpar\\tx2880 \\f9 \\sbasedon0\\snext37 C2;}{\\s38\\fi-240\\li2400\\sa20\\nowidctlpar \\f9\\fs20 \\sbasedon0\\snext38 ctc;}{\\s39\\li2160\\sb160\\nowidctlpar \\f9 \\sbasedon0\\snext39 crt;}{",
							"\\s40\\li480\\sb20\\sa40\\nowidctlpar \\f9 \\sbasedon0\\snext40 or;}{\\s41\\fi-259\\li360\\sb40\\sa40\\nowidctlpar\\tx360 \\f7\\fs20 \\sbasedon0\\snext41 Ln1;}{\\s42\\li115\\sb80\\sa80\\nowidctlpar \\f7\\fs20 \\sbasedon0\\snext0 *Intro;}{\\s43\\li115\\sb80\\sa80\\keepn\\nowidctlpar \\b\\f7 ",
							"\\sbasedon3\\snext42 *Title;}{\\s44\\fi-245\\li360\\sb80\\nowidctlpar \\f7\\fs20 \\snext44 *Jl;}{\\s45\\li360\\sb40\\sa40\\nowidctlpar \\f7\\fs20 \\snext0 Lp1;}{\\s46\\fi-1800\\li1915\\sb60\\sl-240\\slmult0\\nowidctlpar\\tx1915 \\f7\\fs20 \\sbasedon0\\snext46 Tph;}{",
							"\\s47\\li115\\sb120\\sa80\\nowidctlpar \\b\\f7\\fs20 \\snext41 Proch;}{\\*\\cs48 \\additive\\super \\sbasedon10 endnote reference;}}{\\info{\\title AFXCORE}{\\subject MFC Core RTF Help}{\\author AFX}{\\creatim\\yr1992\\mo10\\dy7\\hr11\\min4}{\\version1}{\\edmins12}{\\nofpages0}",
							"{\\nofwords0}{\\nofchars0}{\\vern49221}}\\widowctrl\\ftnbj\\aenddoc\\makebackup\\hyphcaps0 \\fet0\\sectd \\linex576\\endnhere {\\*\\pnseclvl1\\pnucrm\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl2\\pnucltr\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl3",
							"\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxta .}}{\\*\\pnseclvl4\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxta )}}{\\*\\pnseclvl5\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl6\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}",
							"{\\*\\pnseclvl7\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl8\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}{\\*\\pnseclvl9\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb (}{\\pntxta )}}\\pard\\plain \\sl240\\slmult0\\widctlpar ",
							"\\fs20 \n",
							"" };

// Function
const CString cstrBeginFunc = "{\\cs15\\fs16\\up6 ";
const CString cstrBeginIDFunc = "#{\\footnote \\pard\\plain \\sl240\\slmult0\\widctlpar \\fs20 {\\cs15\\fs16\\up6 #} "; // + id
const CString cstrEndIDFunc = "}\n";
const CString cstrBeginIndexFunc = "K{\\footnote \\pard\\plain \\s16\\widctlpar \\fs20 {\\cs15\\fs16\\up6 K} "; // + index entry text (class; func)
const CString cstrEndIndexFunc = "}}{\\f8\\fs22  }\n";
const CString cstrBeginTopicFunc = "{\\cs15\\fs16\\up6 ${\\footnote \\pard\\plain \\s16\\widctlpar \\fs20 {\\cs15\\fs16\\up6 $} "; // + topic entry text (class::func)
const CString cstrEndTopicFunc = "}}{\\f8\\fs22  }\n";
const CString cstrBeginTextFunc = "{\\f8\\fs32 "; // + ret class::func(param)
const CString cstrEndTextFunc = "}\n";

// Description
const CString cstrBeginDesc = "{\\f8\\fs22 \\par \\par "; // + description
const CString cstrEndDesc = "\\par \\par }\n";

// Return
const CString cstrBeginReturn = "{\\b\\f8\\fs22 Return value: \\par }{\\f8\\fs22 \\par }{\\f8\\fs22 "; // + return desc
const CString cstrEndReturn = "\\par \\par }\n";

// Parameters
const CString cstrParamHdr = "{\\b\\f8\\fs22 Parameters: \\par }{\\f8\\fs22 \\par }\n";
const CString cstrBeginParamName = "{\\i\\f8\\fs22 "; // + parameter name
const CString cstrEndParamName = "}\n";
const CString cstrBeginParamDesc = "{\\f8\\fs22 : "; // + parameter desc
const CString cstrEndParamDesc = "\\par \\par }\n";

// End of function
const CString cstrBeginFuncRef = "{\\b\\f8\\fs22 Reference: }{\\f8\\fs22 "; // + filename, line num
const CString cstrEndFuncRef = "\\par }";
const CString cstrEndFunc = "{\\f8\\fs22 \\par \\page }\n";

// RTF end
const CString cstrRTFEnd = "}\n";



/* RTF header

{\rtf1\ansi \deff0\deflang1033{\fonttbl{\f0\froman\fcharset0\fprq2 Tms Rmn{\*\falt Times New Roman};}{\f2\fswiss\fcharset0\fprq2 Helv{\*\falt Helvetica};}{\f7\fswiss\fcharset0\fprq2 MS Sans Serif;}
{\f9\fswiss\fcharset0\fprq2 Helvetica;}}{\colortbl;\red0\green0\blue0;\red0\green0\blue255;\red0\green255\blue255;\red0\green255\blue0;\red255\green0\blue255;\red255\green0\blue0;\red255\green255\blue0;\red255\green255\blue255;\red0\green0\blue128;
\red0\green128\blue128;\red0\green128\blue0;\red128\green0\blue128;\red128\green0\blue0;\red128\green128\blue0;\red128\green128\blue128;\red192\green192\blue192;}{\stylesheet{\nowidctlpar \fs20 \snext0 Normal;}{\s1\sb240\nowidctlpar \b\f2\ul 
\sbasedon0\snext0 heading 1;}{\s2\sb120\nowidctlpar \b\f2 \sbasedon0\snext0 heading 2;}{\s3\li360\nowidctlpar \b \sbasedon0\snext17 heading 3;}{\s4\li360\nowidctlpar \ul \sbasedon0\snext17 heading 4;}{\s5\li720\nowidctlpar \b\fs20 \sbasedon0\snext17 
heading 5;}{\s6\li720\nowidctlpar \fs20\ul \sbasedon0\snext17 heading 6;}{\s7\li720\nowidctlpar \i\fs20 \sbasedon0\snext17 heading 7;}{\s8\li720\nowidctlpar \i\fs20 \sbasedon0\snext17 heading 8;}{\s9\li720\nowidctlpar \i\fs20 \sbasedon0\snext17 
heading 9;}{\*\cs10 \additive Default Paragraph Font;}{\*\cs15 \additive\f0\fs16\up6\lang1033 \sbasedon10 footnote reference;}{\s16\nowidctlpar \fs20 \sbasedon0\snext16 footnote text;}{\s17\li720\nowidctlpar \fs20 \sbasedon0\snext17 Normal Indent;}{
\s18\fi-240\li480\sb80\nowidctlpar\tx480 \f9 \sbasedon0\snext18 nscba;}{\s19\fi-240\li240\sa20\nowidctlpar \f9 \sbasedon0\snext19 j;}{\s20\li480\sa20\nowidctlpar \f9 \sbasedon0\snext20 ij;}{\s21\sb80\sa20\nowidctlpar \f9 \sbasedon0\snext21 btb;}{
\s22\fi-240\li2400\sb20\sa20\nowidctlpar \f9\fs20 \sbasedon0\snext22 ctcb;}{\s23\fi-240\li480\sa40\nowidctlpar\tx480 \f9 \sbasedon0\snext23 ns;}{\s24\sa120\nowidctlpar \f9\fs28 \sbasedon0\snext24 TT;}{\s25\fi-240\li2400\sa20\nowidctlpar \f9 
\sbasedon0\snext25 crtj;}{\s26\fi-240\li480\nowidctlpar\tx480 \f9 \sbasedon0\snext26 nsca;}{\s27\sa20\nowidctlpar \f9 \sbasedon0\snext27 bt;}{\s28\li240\sb120\sa40\nowidctlpar \f9 \sbasedon0\snext28 Hf;}{\s29\li240\sb120\sa40\nowidctlpar \f9 
\sbasedon0\snext29 Hs;}{\s30\li480\sb120\sa40\nowidctlpar \f9 \sbasedon0\snext30 RT;}{\s31\fi-2160\li2160\sb240\sa80\nowidctlpar\tx2160 \f9 \sbasedon0\snext31 c;}{\s32\li2160\sa20\nowidctlpar \f9 \sbasedon0\snext32 ct;}{\s33\li240\sa20\nowidctlpar \f9 
\sbasedon0\snext33 it;}{\s34\li480\nowidctlpar \f9\fs20 \sbasedon0\snext34 nsct;}{\s35\fi-160\li400\sb80\sa40\nowidctlpar \f9 \sbasedon0\snext35 nscb;}{\s36\fi-2640\li2880\sb120\sa40\nowidctlpar\brdrb\brdrs\brdrw15 \brdrbtw\brdrs\brdrw15 \tx2880 \f9 
\sbasedon0\snext36 HC2;}{\s37\fi-2640\li2880\sb120\sa20\nowidctlpar\tx2880 \f9 \sbasedon0\snext37 C2;}{\s38\fi-240\li2400\sa20\nowidctlpar \f9\fs20 \sbasedon0\snext38 ctc;}{\s39\li2160\sb160\nowidctlpar \f9 \sbasedon0\snext39 crt;}{
\s40\li480\sb20\sa40\nowidctlpar \f9 \sbasedon0\snext40 or;}{\s41\fi-259\li360\sb40\sa40\nowidctlpar\tx360 \f7\fs20 \sbasedon0\snext41 Ln1;}{\s42\li115\sb80\sa80\nowidctlpar \f7\fs20 \sbasedon0\snext0 *Intro;}{\s43\li115\sb80\sa80\keepn\nowidctlpar \b\f7 
\sbasedon3\snext42 *Title;}{\s44\fi-245\li360\sb80\nowidctlpar \f7\fs20 \snext44 *Jl;}{\s45\li360\sb40\sa40\nowidctlpar \f7\fs20 \snext0 Lp1;}{\s46\fi-1800\li1915\sb60\sl-240\slmult0\nowidctlpar\tx1915 \f7\fs20 \sbasedon0\snext46 Tph;}{
\s47\li115\sb120\sa80\nowidctlpar \b\f7\fs20 \snext41 Proch;}{\*\cs48 \additive\super \sbasedon10 endnote reference;}}{\info{\title AFXCORE}{\subject MFC Core RTF Help}{\author AFX}{\creatim\yr1992\mo10\dy7\hr11\min4}{\version1}{\edmins12}{\nofpages0}
{\nofwords0}{\nofchars0}{\vern49221}}\widowctrl\ftnbj\aenddoc\makebackup\hyphcaps0 \fet0\sectd \linex576\endnhere {\*\pnseclvl1\pnucrm\pnstart1\pnindent720\pnhang{\pntxta .}}{\*\pnseclvl2\pnucltr\pnstart1\pnindent720\pnhang{\pntxta .}}{\*\pnseclvl3
\pndec\pnstart1\pnindent720\pnhang{\pntxta .}}{\*\pnseclvl4\pnlcltr\pnstart1\pnindent720\pnhang{\pntxta )}}{\*\pnseclvl5\pndec\pnstart1\pnindent720\pnhang{\pntxtb (}{\pntxta )}}{\*\pnseclvl6\pnlcltr\pnstart1\pnindent720\pnhang{\pntxtb (}{\pntxta )}}
{\*\pnseclvl7\pnlcrm\pnstart1\pnindent720\pnhang{\pntxtb (}{\pntxta )}}{\*\pnseclvl8\pnlcltr\pnstart1\pnindent720\pnhang{\pntxtb (}{\pntxta )}}{\*\pnseclvl9\pnlcrm\pnstart1\pnindent720\pnhang{\pntxtb (}{\pntxta )}}\pard\plain \sl240\slmult0\widctlpar 
\fs20 

*/

/* Function

{\cs15\fs16\up6 
#{\footnote \pard\plain \sl240\slmult0\widctlpar \fs20 {\cs15\fs16\up6 #} id_for_entry}
K{\footnote \pard\plain \s16\widctlpar \fs20 {\cs15\fs16\up6 K} Index entry goes here (class <semi> func)}}{\f2  }{\cs15\fs16\up6 
${\footnote \pard\plain \s16\widctlpar \fs20 {\cs15\fs16\up6 $} Entry name goes here (class::func)}}{\f2  }
{\b\f2\fs24 Class::Func(params)}

*/

/* Description

{\f2 
\par 
\par The description of the function goes here.
\par 
\par }

*/

/* Return

{\b\f2 Return value:
\par }{\f2 
\par }{\f2 The description of the return value goes here.
\par 
\par 
\par }

*/

/* Parameters

{\b\f2 Parameters:
\par }{\f2 
\par }{\i\f2 paramName}{\f2 : The description of the parameter goes here
\par 
\par 
\par }

*/

/* End of comment

{\f2 \par \page }

*/

/* RTF End

}

*/
