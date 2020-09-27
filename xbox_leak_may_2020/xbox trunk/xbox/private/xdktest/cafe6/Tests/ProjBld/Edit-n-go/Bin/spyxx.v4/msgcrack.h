//-----------------------------------------------------------------------------
//  MsgCrack.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//
//  Message cracker header.
//
//  Note that there are three different classes of crackers.  The first
//  is called a message cracker.  This is called to format a line in a
//  message stream view.  It is passed a MSGSTREAMDATA pointer to the
//  message (which is immediately followed by any additional data, if
//  there is any defined for that message), and it writes to a
//  CMsgStream output stream, which adds a line to a CMsgLog window
//  when EndLine is called on it.  This cracker type must return
//  TRUE if it writes to the stream.  This return value is used by the
//  formatting code to determine if it needs to add a comma before
//  printing out the raw wParam and lParam values, for instance.
//  Based on the users output filters, these crackers may not even
//  get called (like if they only want to see raw parameters).
//
//  The second kind of a cracker is called a return value cracker,
//  and it is exactly the same except that it is called to format
//  a return value type message.
//
//  The third kind of cracker is called a parameter cracker.  It is
//  passed a slightly different kind of structure (PMSGSTREAMDATA2)
//  that has a pointer to the additional data instead of it being
//  tacked on the end, and it is passed a CMsgParmStream output
//  stream.  This cracker is called when the user is looking at
//  properties for a message, and the wParam/lParam and additional
//  data needs to be formatted and placed into a listbox in the
//  Property inspector.  The output stream will cause one line in
//  the listbox to be added when EndLine is called.  This kind of
//  cracker is not quite as performance sensitive as the previous
//  ones, because a single message is being inspected at a time,
//  instead of a potential flood of messages being logged in a view.
//
//  There are a number of useful macros below to use in the crackers,
//  but you must be careful to only use them in the appropriate type of
//  cracker function.  The message crackers and return value crackers can
//  generally share most output type macros, but these macros must not
//  be used in a message parameter cracker, for instance.
//
//  Revision History:
//
//  04/23/93 byrond  Created
//
//-----------------------------------------------------------------------------

//
// Define the following DBCS message.  This message currently only
// is defined in the KK winuser.h file.
//
#if !defined(WM_CONVERTREQUESTEX)
#define WM_CONVERTREQUESTEX	 0x0109
#endif

typedef struct
{
	UINT nValue;
	CHAR* pszValue;
} VALUETABLE;

#define ARRAY_LEN(Array)	(sizeof(Array) / sizeof(Array[0]))
#define TABLEENTRY(Value)	(UINT)(Value), #Value
#define TABLEEND			0, NULL
#define HANDLEENTRY(Value)	{0, Value, #Value}
#define HANDLEENTRYLAST		{0, 0, NULL}

//
// Prototypes for the Message Decode (cracker) functions.
//
typedef BOOL (*PFNDECODE)(PMSGSTREAMDATA);
typedef BOOL (*PFNDECODERET)(PMSGSTREAMDATA);
typedef void (*PFNDECODEPARM)(PMSGSTREAMDATA2, CMsgParmStream*);

//
// Macro to produce the declarations for a cracker function
// set for a specific message.
//
#define DECLAREDECODE(msg)				\
BOOL Msg##msg(PMSGSTREAMDATA pmsd);		\
BOOL Ret##msg(PMSGSTREAMDATA pmsd);		\
void Parm##msg(PMSGSTREAMDATA2 pmsd2, CMsgParmStream* pmps)

//
// Macro to form an entry in the message description table array.
//
#define MSGDESCENTRY(msg, iMsgType)	{msg, #msg, iMsgType, Msg##msg, Ret##msg, Parm##msg}

//-----------------------------------------------------------------------
// Macros to support defining the cracker functions (that are common
// to all types).
//

//
// Returns the True or False string based on the value of f.
//
#define tf(f)	((f) ? ids(IDS_TRUE) : ids(IDS_FALSE))

#define PARM(var, type, src)	type var = (type)src

//-----------------------------------------------------------------------
// Macros that are specific to the message and return value type crackers.
//

#define HW		pmsd->hwnd

#define WP		pmsd->wParam
#define LP		pmsd->lParam
#define LOWP	LOWORD(pmsd->wParam)
#define HIWP	HIWORD(pmsd->wParam)
#define LOLP	LOWORD(pmsd->lParam)
#define HILP	HIWORD(pmsd->lParam)

#define RET		pmsd->lResult
#define LORET	LOWORD(pmsd->lResult)
#define HIRET	HIWORD(pmsd->lResult)

#define ED		(pmsd + 1)

#define DECODE(msg)			BOOL Msg##msg(PMSGSTREAMDATA pmsd)

#define DECODERET(msg)		BOOL Ret##msg(PMSGSTREAMDATA pmsd)

#define DECODELIKE(msg)		return Msg##msg(pmsd)

#define DECODERETLIKE(msg)	return Ret##msg(pmsd)

#define NODECODE(msg)				\
BOOL Msg##msg(PMSGSTREAMDATA pmsd)	\
{									\
	UNREFERENCED_PARAMETER(pmsd);	\
	return FALSE;					\
}

#define NODECODERET(msg)			\
BOOL Ret##msg(PMSGSTREAMDATA pmsd)	\
{									\
	UNREFERENCED_PARAMETER(pmsd);	\
	return FALSE;					\
}

//
// The global output stream that all the message and return
// crackers write to.
//
#define MOUT				*g_pmout

//
// Macro that determines if there is any extra data.
//
#define PIFDATA()			if (pmsd->cb > sizeof(MSGSTREAMDATA))

//
// Returns the size of the extra data.
//
#define PSIZEOFED()			(pmsd->cb - sizeof(MSGSTREAMDATA))

//
// Verifies the size of the extra data against an expected structure size.
//
#define PIFSIZEOF(struct)	if ((pmsd->cb - sizeof(MSGSTREAMDATA)) == sizeof(struct))

//
// Verifies that the specified PACK_STRINGORD structure contains an
// ordinal.  If it does not, then it is a string and should be
// written out as such.
//
#define PIFISORD(pso)		if (pso->dwMarker == 0xFFFFFFFF)

//
// Macro to label a parameter (but not print the value yet).
//
#define PLABEL(parm)		MOUT << " " #parm ":";

//
// Macro to print out a labeled parameter in the crackers.
//
#define POUT(parm)			MOUT << " " #parm ":" << parm

//
// Macro to print out a labeled boolean parameter in the crackers.
//
#define POUTB(parm)			MOUT << " " #parm ":" << tf(parm)

//
// Macro to print out the value of a boolean parameter (using pointer to BOOL) in the crackers.
//
#define POUTPB(parm)		MOUT << " (" << tf(*parm) << ')'

//
// Macro to print out a labeled parameter with a cast in the crackers.
//
#define POUTC(parm, cast)	MOUT << " " #parm ":" << (cast)parm

//
// Macro to print out a string in quotes.  It does not print a label.
//
#define POUTS(pstr)			MOUT << " (\"" << pstr << "\")";

//
// Macro to print out an ordinal value.  It is labeled as "dwData"
// and placed within parenthesis.
// The pso parameter must be a pointer to a PACK_STRINGORD struct.
//
#define POUTORD(pso)		MOUT << " (dwData:" << pso->dwOrdinal << ')';

//
// Macro to print out a PRECT parameter (no label).
//
#define POUTRECT(prc)		MOUT << " (" << prc->left << ',' << prc->top << ")-(" << prc->right << ',' << prc->bottom << ')';

#define TABLEORVALUE(tbl, parm)			\
	PLABEL(parm);						\
	TableOrValue(tbl, parm);

#define TABLEORVALUEC(tbl, cast, parm)	\
	PLABEL(parm);						\
	TableOrValue(tbl, (cast)parm);

#define TABLEORDWORD(tbl, parm)			\
	PLABEL(parm);						\
	TableOrDWord(tbl, parm);

#define TABLEORINT(tbl, parm)			\
	PLABEL(parm);						\
	TableOrInt(tbl, parm);

#define TABLEFLAGS(tbl, parm)			\
	PLABEL(parm);						\
	TableFlags(tbl, parm);

//-----------------------------------------------------------------------
// Macros that are specific to the message parameter type crackers.
//

#define DECODEPARM(msg)										\
void Parm##msg(PMSGSTREAMDATA2 pmsd2, CMsgParmStream* pmps)

#define DECODEPARMLIKE(msg)									\
	Parm##msg(pmsd2, pmps)

#define NODECODEPARM(msg)									\
void Parm##msg(PMSGSTREAMDATA2 pmsd2, CMsgParmStream* pmps)	\
{															\
	DECODEPARMLIKE(Generic);								\
}

//
// Use this with the PARM macro to define a parameter that
// comes from wParam.
//
#define WP2					pmsd2->wParam

//
// Use this with the PARM macro to define a parameter that
// comes from wParam.
//
#define RET2				pmsd2->lResult

//
// Use this in the PARM macro to define that the parameter
// comes from the Extra Data field of pmsd2.
//
#define ED2					pmsd2->pbExtraData

//
// The output stream that all the parameter crackers
// write to.
//
#define M2OUT				*pmps

//
// Increases the indent level by one.  The indent level is
// used when EndLine() is called to determine how far to indent
// the line in the listbox.
//
#define INDENT()			pmps->Indent()

//
// Decreases the indent level by one.
//
#define OUTDENT()			pmps->Outdent()

//
// Macro to label a parameter (but not print the value yet).
//
#define P2LABEL(parm)		M2OUT << #parm << ": "

//
// Outputs a newline.
//
#define P2ENDLINE()			pmps->EndLine()

//
// Print out the wParam label and value.
//
#define P2WPOUT()																			\
	*pmps << "wParam: " << (DWORD)pmsd2->wParam;											\
	pmps->EndLine()

#define P2WPOUTPTR(struct)																	\
	*pmps << "wParam: " << (DWORD)pmsd2->wParam << ids(IDS_POINTER_TO) << #struct << ')';	\
	pmps->EndLine()

//
// Print out the lParam label and value.
//
#define P2LPOUT()																			\
	*pmps << "lParam: " << (DWORD)pmsd2->lParam;											\
	pmps->EndLine()

#define P2LPOUTPTR(struct)																	\
	*pmps << "lParam: " << (DWORD)pmsd2->lParam << ids(IDS_POINTER_TO) << #struct << ')';	\
	pmps->EndLine()

#define P2LPOUTPTRSTR																		\
	*pmps << "lParam: " << (DWORD)pmsd2->lParam << ids(IDS_POINTER_TO_STRING);				\
	pmps->EndLine()

#define P2IFRETURN()		if (pmsd2->fPostType == POSTTYPE_RETURN)

//
// Returns the size of the extra data.
//
#define P2SIZEOFED()		(pmsd2->cbExtraData)

#define P2IFSIZEOF(struct)	if (pmsd2->cbExtraData == sizeof(struct))

#define P2IFDATA()			if (pmsd2->cbExtraData != 0)

//
// Verifies that the specified PACK_STRINGORD structure contains an
// ordinal.  If it does not, then it is a string and should be
// written out as such.
//
#define P2IFISORD(pso)	PIFISORD(pso)

#define P2OUTPTR(ps, p, struct)																\
	*pmps << #p  << ": " << (DWORD)ps->p << ids(IDS_POINTER_TO) << #struct << ')';			\
	pmps->EndLine()

#define P2OUTPTRSTR(ps, p)																	\
	*pmps << #p  << ": " << (DWORD)ps->p << ids(IDS_POINTER_TO_STRING);						\
	pmps->EndLine()

//
// Macro to print out a labeled parameter.
//
#define P2OUT(ps, parm)																		\
	M2OUT << #parm << ": " << ps->parm;														\
	pmps->EndLine()

//
// Macro to print out a labeled boolean parameter in the crackers.
//
#define P2OUTB(ps, parm)																	\
	M2OUT << #parm << ": " << tf(ps->parm);													\
	pmps->EndLine()

//
// Macro to print out a labeled parameter with a cast in the crackers.
//
#define P2OUTC(ps, parm, cast)																\
	M2OUT << #parm << ": " << (cast)ps->parm;												\
	pmps->EndLine()

//
// Macro to print out a labeled POINT parameter.
//
#define P2OUTPT(ps, parm)																	\
	M2OUT << #parm << ": " << ps->parm.x													\
		<< ',' << ps->parm.y;																\
	pmps->EndLine()

//
// Macro to print out a labeled RECT parameter.
//
#define P2OUTRECT(ps, parm)																	\
	M2OUT << #parm << ": (" << ps->parm.left << ',' << ps->parm.top << ") - ("				\
		 << ps->parm.right << ',' << ps->parm.bottom << ')';								\
	pmps->EndLine()

//
// Macro to print out a string.
//
#define P2OUTS(pstr)																		\
	M2OUT << '\"' << pstr << '\"';															\
	pmps->EndLine()

//
// Macro to print out an ordinal value.  It is labeled as "dwData".
// The pso parameter must be a pointer to a PACK_STRINGORD struct.
//
#define P2OUTORD(pso)																		\
	M2OUT << "dwData:" << pso->dwOrdinal;													\
	pmps->EndLine()

#define P2TABLEORVALUE(tbl, ps, parm)														\
	P2LABEL(parm);																			\
	TableOrValue2(pmps, tbl, ps->parm);														\
	pmps->EndLine()

#define P2TABLEORDWORD(tbl, ps, parm)														\
	P2LABEL(parm);																			\
	TableOrDWord2(pmps, tbl, ps->parm);														\
	pmps->EndLine()

#define P2TABLEORINT(tbl, ps, parm)															\
	P2LABEL(parm);																			\
	TableOrInt2(pmps, tbl, ps->parm);														\
	pmps->EndLine()

#define P2TABLEFLAGS(tbl, ps, parm)															\
	P2LABEL(parm);																			\
	TableFlags2(pmps, tbl, ps->parm);														\
	pmps->EndLine()

//
// End of cracker macros.
//-----------------------------------------------------------------------

//
// MsgCrack.cpp
//

extern CMsgStream *g_pmout;

void TableOrValue(VALUETABLE *ptbl, UINT uValue);
void TableOrValue(VALUETABLE *ptbl, WORD wValue);
inline void TableOrValue(VALUETABLE *ptbl, INT iValue)
{
	TableOrValue(ptbl, (UINT)iValue);
}
inline void TableOrValue(VALUETABLE *ptbl, LRESULT lResult)
{
	TableOrValue(ptbl, (UINT)lResult);
}

void TableOrValueNotHot(VALUETABLE *ptbl, UINT uValue);
void TableOrDWord(VALUETABLE *ptbl, UINT uValue);
void TableOrInt(VALUETABLE *ptbl, UINT uValue);

BOOL TableOrNothing(VALUETABLE *ptbl, UINT uValue);
BOOL TableOrNothing(VALUETABLE *ptbl, WORD wValue);
inline BOOL TableOrNothing(VALUETABLE *ptbl, LRESULT lResult)
{
	return TableOrNothing(ptbl, (UINT)lResult);
}

BOOL TableOrNothingNotHot(VALUETABLE *ptbl, UINT uValue);
void TableFlags(VALUETABLE *ptbl, UINT uValue);
VOID PrintIntArray(PINT pInts, INT cInts, INT cMaxInts);
VOID PrintFont(PPACK_FONT ppf);

//
// Generic message parameter decoder function for messages that do
// not have parameters that point to structures or strings
//
DECODEPARM(Generic);				// ParmGeneric

VOID TableOrValue2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue);
VOID TableOrDWord2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue);
BOOL TableOrNothing2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue);
VOID TableOrInt2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue);
VOID TableFlags2(CMsgParmStream* pmps, VALUETABLE *ptbl, UINT uValue);
VOID PrintIntArray2(CMsgParmStream* pmps, PINT pInts, INT cInts, INT cMaxInts);

//
// AniMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(ACM_OPEN);
DECLAREDECODE(ACM_PLAY);
DECLAREDECODE(ACM_STOP);
#endif	// DISABLE_WIN95_MESSAGES

//
// BtnMsgs.cpp
//
// TODO: DECLAREDECODE(BM_CLICK);
DECLAREDECODE(BM_GETCHECK);
DECLAREDECODE(BM_GETSTATE);
DECLAREDECODE(BM_SETCHECK);
DECLAREDECODE(BM_SETSTATE);
DECLAREDECODE(BM_SETSTYLE);

//
// CBMsgs.cpp
//
extern VALUETABLE tblCBRet[];

DECLAREDECODE(CB_ADDSTRING);
DECLAREDECODE(CB_DELETESTRING);
DECLAREDECODE(CB_DIR);
DECLAREDECODE(CB_FINDSTRING);
DECLAREDECODE(CB_FINDSTRINGEXACT);
DECLAREDECODE(CB_GETCOUNT);
DECLAREDECODE(CB_GETCURSEL);
DECLAREDECODE(CB_GETDROPPEDCONTROLRECT);
DECLAREDECODE(CB_GETDROPPEDSTATE);
DECLAREDECODE(CB_GETEDITSEL);
DECLAREDECODE(CB_GETEXTENDEDUI);
DECLAREDECODE(CB_GETITEMDATA);
DECLAREDECODE(CB_GETITEMHEIGHT);
DECLAREDECODE(CB_GETLBTEXT);
DECLAREDECODE(CB_GETLBTEXTLEN);
DECLAREDECODE(CB_GETLOCALE);
DECLAREDECODE(CB_INSERTSTRING);
DECLAREDECODE(CB_LIMITTEXT);
DECLAREDECODE(CB_RESETCONTENT);
DECLAREDECODE(CB_SELECTSTRING);
DECLAREDECODE(CB_SETCURSEL);
DECLAREDECODE(CB_SETEDITSEL);
DECLAREDECODE(CB_SETEXTENDEDUI);
DECLAREDECODE(CB_SETITEMDATA);
DECLAREDECODE(CB_SETITEMHEIGHT);
DECLAREDECODE(CB_SETLOCALE);
DECLAREDECODE(CB_SHOWDROPDOWN);

//
// ClipMsgs.cpp
//
extern VALUETABLE tblCBFormat[];

DECLAREDECODE(WM_ASKCBFORMATNAME);
DECLAREDECODE(WM_CHANGECBCHAIN);
DECLAREDECODE(WM_CLEAR);
DECLAREDECODE(WM_COPY);
DECLAREDECODE(WM_CUT);
DECLAREDECODE(WM_DESTROYCLIPBOARD);
DECLAREDECODE(WM_DRAWCLIPBOARD);
DECLAREDECODE(WM_HSCROLLCLIPBOARD);
DECLAREDECODE(WM_PAINTCLIPBOARD);
DECLAREDECODE(WM_PASTE);
DECLAREDECODE(WM_RENDERALLFORMATS);
DECLAREDECODE(WM_RENDERFORMAT);
DECLAREDECODE(WM_SIZECLIPBOARD);
DECLAREDECODE(WM_UNDO);
DECLAREDECODE(WM_VSCROLLCLIPBOARD);

//
// DDEMsgs.cpp
//
DECLAREDECODE(WM_DDE_ACK);
DECLAREDECODE(WM_DDE_ADVISE);
DECLAREDECODE(WM_DDE_DATA);
DECLAREDECODE(WM_DDE_EXECUTE);
DECLAREDECODE(WM_DDE_INITIATE);
DECLAREDECODE(WM_DDE_POKE);
DECLAREDECODE(WM_DDE_REQUEST);
DECLAREDECODE(WM_DDE_TERMINATE);
DECLAREDECODE(WM_DDE_UNADVISE);

//
// DlgMsgs.cpp
//
DECLAREDECODE(DM_GETDEFID);
DECLAREDECODE(DM_SETDEFID);
// TODO: DECLAREDECODE(DM_REPOSITION);

//
// EditMsgs.cpp
//
DECLAREDECODE(EM_CANUNDO);
// TODO: DECLAREDECODE(EM_CHARFROMPOS);
DECLAREDECODE(EM_EMPTYUNDOBUFFER);
DECLAREDECODE(EM_FMTLINES);
DECLAREDECODE(EM_GETFIRSTVISIBLELINE);
DECLAREDECODE(EM_GETHANDLE);
// TODO: DECLAREDECODE(EM_GETLIMITTEXT);
DECLAREDECODE(EM_GETLINE);
DECLAREDECODE(EM_GETLINECOUNT);
// TODO: DECLAREDECODE(EM_GETMARGINS);
DECLAREDECODE(EM_GETMODIFY);
DECLAREDECODE(EM_GETPASSWORDCHAR);
DECLAREDECODE(EM_GETRECT);
DECLAREDECODE(EM_GETSEL);
DECLAREDECODE(EM_GETTHUMB);
DECLAREDECODE(EM_GETWORDBREAKPROC);
DECLAREDECODE(EM_LIMITTEXT);
DECLAREDECODE(EM_LINEFROMCHAR);
DECLAREDECODE(EM_LINEINDEX);
DECLAREDECODE(EM_LINELENGTH);
DECLAREDECODE(EM_LINESCROLL);
// TODO: DECLAREDECODE(EM_POSFROMCHAR);
DECLAREDECODE(EM_REPLACESEL);
DECLAREDECODE(EM_SCROLL);
DECLAREDECODE(EM_SCROLLCARET);
// TODO: DECLAREDECODE(EM_SETFONT);
DECLAREDECODE(EM_SETHANDLE);
// TODO: DECLAREDECODE(EM_SETLIMITTEXT);
// TODO: DECLAREDECODE(EM_SETMARGINS);
DECLAREDECODE(EM_SETMODIFY);
DECLAREDECODE(EM_SETPASSWORDCHAR);
DECLAREDECODE(EM_SETREADONLY);
DECLAREDECODE(EM_SETRECT);
DECLAREDECODE(EM_SETRECTNP);
DECLAREDECODE(EM_SETSEL);
DECLAREDECODE(EM_SETTABSTOPS);
DECLAREDECODE(EM_SETWORDBREAKPROC);
DECLAREDECODE(EM_UNDO);

//
// HdrMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(HDM_GETITEMCOUNT);
DECLAREDECODE(HDM_INSERTITEM);
DECLAREDECODE(HDM_DELETEITEM);
DECLAREDECODE(HDM_GETITEM);
DECLAREDECODE(HDM_SETITEM);
DECLAREDECODE(HDM_LAYOUT);
DECLAREDECODE(HDM_HITTEST);
#endif	// DISABLE_WIN95_MESSAGES

//
// HKMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(HKM_SETHOTKEY);
DECLAREDECODE(HKM_GETHOTKEY);
DECLAREDECODE(HKM_SETRULES);
#endif	// DISABLE_WIN95_MESSAGES

//
// IMEMsgs.cpp
//
DECLAREDECODE(WM_CONVERTREQUESTEX);
DECLAREDECODE(WM_IMEKEYDOWN);
DECLAREDECODE(WM_IMEKEYUP);
DECLAREDECODE(WM_IME_REPORT);
DECLAREDECODE(WM_IME_STARTCOMPOSITION);
DECLAREDECODE(WM_IME_ENDCOMPOSITION);
DECLAREDECODE(WM_IME_COMPOSITION);
DECLAREDECODE(WM_IME_SETCONTEXT);
DECLAREDECODE(WM_IME_NOTIFY);
DECLAREDECODE(WM_IME_CONTROL);
DECLAREDECODE(WM_IME_COMPOSITIONFULL);
DECLAREDECODE(WM_IME_SELECT);
DECLAREDECODE(WM_IME_CHAR);
DECLAREDECODE(WM_IME_KEYDOWN);
DECLAREDECODE(WM_IME_KEYUP);

//
// KbdMsgs.cpp
//
DECLAREDECODE(WM_CHAR);
DECLAREDECODE(WM_CHARTOITEM);
DECLAREDECODE(WM_DEADCHAR);
DECLAREDECODE(WM_GETHOTKEY);
DECLAREDECODE(WM_HOTKEY);
DECLAREDECODE(WM_KEYDOWN);
DECLAREDECODE(WM_KEYUP);
DECLAREDECODE(WM_SETHOTKEY);
DECLAREDECODE(WM_SYSCHAR);
DECLAREDECODE(WM_SYSDEADCHAR);
DECLAREDECODE(WM_SYSKEYDOWN);
DECLAREDECODE(WM_SYSKEYUP);
DECLAREDECODE(WM_VKEYTOITEM);

//
// LBMsgs.cpp
//
extern VALUETABLE tblFileAttrs[];
extern VALUETABLE tblLBRet[];

DECLAREDECODE(LB_ADDFILE);
DECLAREDECODE(LB_ADDSTRING);
DECLAREDECODE(LB_DELETESTRING);
DECLAREDECODE(LB_DIR);
DECLAREDECODE(LB_FINDSTRING);
DECLAREDECODE(LB_FINDSTRINGEXACT);
DECLAREDECODE(LB_GETANCHORINDEX);
DECLAREDECODE(LB_GETCARETINDEX);
DECLAREDECODE(LB_GETCOUNT);
DECLAREDECODE(LB_GETCURSEL);
DECLAREDECODE(LB_GETHORIZONTALEXTENT);
DECLAREDECODE(LB_GETITEMDATA);
DECLAREDECODE(LB_GETITEMHEIGHT);
DECLAREDECODE(LB_GETITEMRECT);
DECLAREDECODE(LB_GETLOCALE);
DECLAREDECODE(LB_GETSEL);
DECLAREDECODE(LB_GETSELCOUNT);
DECLAREDECODE(LB_GETSELITEMS);
DECLAREDECODE(LB_GETTEXT);
DECLAREDECODE(LB_GETTEXTLEN);
DECLAREDECODE(LB_GETTOPINDEX);
DECLAREDECODE(LB_INSERTSTRING);
DECLAREDECODE(LB_RESETCONTENT);
DECLAREDECODE(LB_SELECTSTRING);
DECLAREDECODE(LB_SELITEMRANGE);
DECLAREDECODE(LB_SELITEMRANGEEX);
DECLAREDECODE(LB_SETANCHORINDEX);
DECLAREDECODE(LB_SETCARETINDEX);
DECLAREDECODE(LB_SETCOLUMNWIDTH);
DECLAREDECODE(LB_SETCOUNT);
DECLAREDECODE(LB_SETCURSEL);
DECLAREDECODE(LB_SETHORIZONTALEXTENT);
DECLAREDECODE(LB_SETITEMDATA);
DECLAREDECODE(LB_SETITEMHEIGHT);
DECLAREDECODE(LB_SETLOCALE);
DECLAREDECODE(LB_SETSEL);
DECLAREDECODE(LB_SETTABSTOPS);
DECLAREDECODE(LB_SETTOPINDEX);

//
// LVMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(LVM_GETBKCOLOR);
DECLAREDECODE(LVM_SETBKCOLOR);
DECLAREDECODE(LVM_GETIMAGELIST);
DECLAREDECODE(LVM_SETIMAGELIST);
DECLAREDECODE(LVM_GETITEMCOUNT);
DECLAREDECODE(LVM_GETITEM);
DECLAREDECODE(LVM_SETITEM);
DECLAREDECODE(LVM_INSERTITEM);
DECLAREDECODE(LVM_DELETEITEM);
DECLAREDECODE(LVM_DELETEALLITEMS);
DECLAREDECODE(LVM_GETCALLBACKMASK);
DECLAREDECODE(LVM_SETCALLBACKMASK);
DECLAREDECODE(LVM_GETNEXTITEM);
DECLAREDECODE(LVM_FINDITEM);
DECLAREDECODE(LVM_GETITEMRECT);
DECLAREDECODE(LVM_SETITEMPOSITION);
DECLAREDECODE(LVM_GETITEMPOSITION);
DECLAREDECODE(LVM_GETSTRINGWIDTH);
DECLAREDECODE(LVM_HITTEST);
DECLAREDECODE(LVM_ENSUREVISIBLE);
DECLAREDECODE(LVM_SCROLL);
DECLAREDECODE(LVM_REDRAWITEMS);
DECLAREDECODE(LVM_ARRANGE);
DECLAREDECODE(LVM_EDITLABEL);
DECLAREDECODE(LVM_GETEDITCONTROL);
DECLAREDECODE(LVM_GETCOLUMN);
DECLAREDECODE(LVM_SETCOLUMN);
DECLAREDECODE(LVM_INSERTCOLUMN);
DECLAREDECODE(LVM_DELETECOLUMN);
DECLAREDECODE(LVM_GETCOLUMNWIDTH);
DECLAREDECODE(LVM_SETCOLUMNWIDTH);
DECLAREDECODE(LVM_CREATEDRAGIMAGE);
DECLAREDECODE(LVM_GETVIEWRECT);
DECLAREDECODE(LVM_GETTEXTCOLOR);
DECLAREDECODE(LVM_SETTEXTCOLOR);
DECLAREDECODE(LVM_GETTEXTBKCOLOR);
DECLAREDECODE(LVM_SETTEXTBKCOLOR);
DECLAREDECODE(LVM_GETTOPINDEX);
DECLAREDECODE(LVM_GETCOUNTPERPAGE);
DECLAREDECODE(LVM_GETORIGIN);
DECLAREDECODE(LVM_UPDATE);
DECLAREDECODE(LVM_SETITEMSTATE);
DECLAREDECODE(LVM_GETITEMSTATE);
DECLAREDECODE(LVM_GETITEMTEXT);
DECLAREDECODE(LVM_SETITEMTEXT);
DECLAREDECODE(LVM_SETITEMCOUNT);
DECLAREDECODE(LVM_SORTITEMS);
DECLAREDECODE(LVM_SETITEMPOSITION32);
DECLAREDECODE(LVM_GETSELECTEDCOUNT);
DECLAREDECODE(LVM_GETITEMSPACING);
DECLAREDECODE(LVM_GETISEARCHSTRING);
#endif	// DISABLE_WIN95_MESSAGES

//
// MDIMsgs.cpp
//
DECLAREDECODE(WM_CHILDACTIVATE);
DECLAREDECODE(WM_MDIACTIVATE);
DECLAREDECODE(WM_MDICASCADE);
DECLAREDECODE(WM_MDICREATE);
DECLAREDECODE(WM_MDIDESTROY);
DECLAREDECODE(WM_MDIGETACTIVE);
DECLAREDECODE(WM_MDIICONARRANGE);
DECLAREDECODE(WM_MDIMAXIMIZE);
DECLAREDECODE(WM_MDINEXT);
DECLAREDECODE(WM_MDIREFRESHMENU);
DECLAREDECODE(WM_MDIRESTORE);
DECLAREDECODE(WM_MDISETMENU);
DECLAREDECODE(WM_MDITILE);

//
// MousMsgs.cpp
//
extern VALUETABLE tblHitTest[];

DECLAREDECODE(WM_LBUTTONDBLCLK);
DECLAREDECODE(WM_LBUTTONDOWN);
DECLAREDECODE(WM_LBUTTONUP);
DECLAREDECODE(WM_MBUTTONDBLCLK);
DECLAREDECODE(WM_MBUTTONDOWN);
DECLAREDECODE(WM_MBUTTONUP);
DECLAREDECODE(WM_MOUSEACTIVATE);
DECLAREDECODE(WM_MOUSEMOVE);
DECLAREDECODE(WM_PARENTNOTIFY);
DECLAREDECODE(WM_RBUTTONDBLCLK);
DECLAREDECODE(WM_RBUTTONDOWN);
DECLAREDECODE(WM_RBUTTONUP);
DECLAREDECODE(WM_SETCURSOR);

//
// NCMsgs.cpp
//
DECLAREDECODE(WM_NCACTIVATE);
DECLAREDECODE(WM_NCCALCSIZE);
DECLAREDECODE(WM_NCCREATE);
DECLAREDECODE(WM_NCDESTROY);
DECLAREDECODE(WM_NCHITTEST);
DECLAREDECODE(WM_NCLBUTTONDBLCLK);
DECLAREDECODE(WM_NCLBUTTONDOWN);
DECLAREDECODE(WM_NCLBUTTONUP);
DECLAREDECODE(WM_NCMBUTTONDBLCLK);
DECLAREDECODE(WM_NCMBUTTONDOWN);
DECLAREDECODE(WM_NCMBUTTONUP);
DECLAREDECODE(WM_NCMOUSEMOVE);
DECLAREDECODE(WM_NCPAINT);
DECLAREDECODE(WM_NCRBUTTONDBLCLK);
DECLAREDECODE(WM_NCRBUTTONDOWN);
DECLAREDECODE(WM_NCRBUTTONUP);

//
// ProgMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(PBM_SETRANGE);
DECLAREDECODE(PBM_SETPOS);
DECLAREDECODE(PBM_DELTAPOS);
DECLAREDECODE(PBM_SETSTEP);
DECLAREDECODE(PBM_STEPIT);
#endif	// DISABLE_WIN95_MESSAGES

//
// SBMsgs.cpp
//
DECLAREDECODE(SBM_ENABLE_ARROWS);
// TODO: DECLAREDECODE(SBM_GETPAGE);
DECLAREDECODE(SBM_GETPOS);
DECLAREDECODE(SBM_GETRANGE);
// TODO: DECLAREDECODE(SBM_GETSCROLLINFO);
// TODO: DECLAREDECODE(SBM_SETPAGE);
DECLAREDECODE(SBM_SETPOS);
DECLAREDECODE(SBM_SETRANGE);
DECLAREDECODE(SBM_SETRANGEREDRAW);
// TODO: DECLAREDECODE(SBM_SETSCROLLINFO);

//
//  StatMsgs.cpp
//
// TODO: DECLAREDECODE(STM_GETBITMAP);
DECLAREDECODE(STM_GETICON);
// TODO: DECLAREDECODE(STM_GETMETAPICT);
// TODO: DECLAREDECODE(STM_SETBITMAP);
DECLAREDECODE(STM_SETICON);
// TODO: DECLAREDECODE(STM_SETMETAPICT);

//
// StbMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(SB_SETTEXT);
DECLAREDECODE(SB_GETTEXT);
DECLAREDECODE(SB_GETTEXTLENGTH);
DECLAREDECODE(SB_SETPARTS);
DECLAREDECODE(SB_GETPARTS);
DECLAREDECODE(SB_GETBORDERS);
DECLAREDECODE(SB_SETMINHEIGHT);
DECLAREDECODE(SB_SIMPLE);
DECLAREDECODE(SB_GETRECT);
#endif	// DISABLE_WIN95_MESSAGES

//
// TabMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(TCM_GETBKCOLOR);
DECLAREDECODE(TCM_SETBKCOLOR);
DECLAREDECODE(TCM_GETIMAGELIST);
DECLAREDECODE(TCM_SETIMAGELIST);
DECLAREDECODE(TCM_GETITEMCOUNT);
DECLAREDECODE(TCM_GETITEM);
DECLAREDECODE(TCM_SETITEM);
DECLAREDECODE(TCM_INSERTITEM);
DECLAREDECODE(TCM_DELETEITEM);
DECLAREDECODE(TCM_DELETEALLITEMS);
DECLAREDECODE(TCM_GETITEMRECT);
DECLAREDECODE(TCM_GETCURSEL);
DECLAREDECODE(TCM_SETCURSEL);
DECLAREDECODE(TCM_HITTEST);
DECLAREDECODE(TCM_SETITEMEXTRA);
DECLAREDECODE(TCM_ADJUSTRECT);
DECLAREDECODE(TCM_SETITEMSIZE);
DECLAREDECODE(TCM_REMOVEIMAGE);
DECLAREDECODE(TCM_SETPADDING);
DECLAREDECODE(TCM_GETROWCOUNT);
DECLAREDECODE(TCM_GETTOOLTIPS);
DECLAREDECODE(TCM_SETTOOLTIPS);
DECLAREDECODE(TCM_GETCURFOCUS);
DECLAREDECODE(TCM_SETCURFOCUS);
#endif	// DISABLE_WIN95_MESSAGES

//
// TBMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(TB_ENABLEBUTTON);
DECLAREDECODE(TB_CHECKBUTTON);
DECLAREDECODE(TB_PRESSBUTTON);
DECLAREDECODE(TB_HIDEBUTTON);
DECLAREDECODE(TB_INDETERMINATE);
DECLAREDECODE(TB_ISBUTTONENABLED);
DECLAREDECODE(TB_ISBUTTONCHECKED);
DECLAREDECODE(TB_ISBUTTONPRESSED);
DECLAREDECODE(TB_ISBUTTONHIDDEN);
DECLAREDECODE(TB_ISBUTTONINDETERMINATE);
DECLAREDECODE(TB_SETSTATE);
DECLAREDECODE(TB_GETSTATE);
DECLAREDECODE(TB_ADDBITMAP);
DECLAREDECODE(TB_ADDBUTTONS);
DECLAREDECODE(TB_INSERTBUTTON);
DECLAREDECODE(TB_DELETEBUTTON);
DECLAREDECODE(TB_GETBUTTON);
DECLAREDECODE(TB_BUTTONCOUNT);
DECLAREDECODE(TB_COMMANDTOINDEX);
DECLAREDECODE(TB_SAVERESTORE);
DECLAREDECODE(TB_CUSTOMIZE);
DECLAREDECODE(TB_ADDSTRING);
DECLAREDECODE(TB_GETITEMRECT);
DECLAREDECODE(TB_BUTTONSTRUCTSIZE);
DECLAREDECODE(TB_SETBUTTONSIZE);
DECLAREDECODE(TB_SETBITMAPSIZE);
DECLAREDECODE(TB_AUTOSIZE);
DECLAREDECODE(TB_SETBUTTONTYPE);
DECLAREDECODE(TB_GETTOOLTIPS);
DECLAREDECODE(TB_SETTOOLTIPS);
DECLAREDECODE(TB_SETPARENT);
DECLAREDECODE(TB_SETROWS);
DECLAREDECODE(TB_GETROWS);
DECLAREDECODE(TB_SETCMDID);
DECLAREDECODE(TB_CHANGEBITMAP);
DECLAREDECODE(TB_GETBITMAP);
DECLAREDECODE(TB_GETBUTTONTEXT);
DECLAREDECODE(TB_GETBITMAPFLAGS);
#endif	// DISABLE_WIN95_MESSAGES

//
// TrkMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(TBM_GETPOS);
DECLAREDECODE(TBM_GETRANGEMIN);
DECLAREDECODE(TBM_GETRANGEMAX);
DECLAREDECODE(TBM_GETTIC);
DECLAREDECODE(TBM_SETTIC);
DECLAREDECODE(TBM_SETPOS);
DECLAREDECODE(TBM_SETRANGE);
DECLAREDECODE(TBM_SETRANGEMIN);
DECLAREDECODE(TBM_SETRANGEMAX);
DECLAREDECODE(TBM_CLEARTICS);
DECLAREDECODE(TBM_SETSEL);
DECLAREDECODE(TBM_SETSELSTART);
DECLAREDECODE(TBM_SETSELEND);
DECLAREDECODE(TBM_GETPTICS);
DECLAREDECODE(TBM_GETTICPOS);
DECLAREDECODE(TBM_GETNUMTICS);
DECLAREDECODE(TBM_GETSELSTART);
DECLAREDECODE(TBM_GETSELEND);
DECLAREDECODE(TBM_CLEARSEL);
DECLAREDECODE(TBM_SETTICFREQ);
DECLAREDECODE(TBM_SETPAGESIZE);
DECLAREDECODE(TBM_GETPAGESIZE);
DECLAREDECODE(TBM_SETLINESIZE);
DECLAREDECODE(TBM_GETLINESIZE);
DECLAREDECODE(TBM_GETTHUMBRECT);
DECLAREDECODE(TBM_GETCHANNELRECT);
DECLAREDECODE(TBM_SETTHUMBLENGTH);
DECLAREDECODE(TBM_GETTHUMBLENGTH);
#endif	// DISABLE_WIN95_MESSAGES

//
// TTMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(TTM_ACTIVATE);
DECLAREDECODE(TTM_SETDELAYTIME);
DECLAREDECODE(TTM_ADDTOOL);
DECLAREDECODE(TTM_DELTOOL);
DECLAREDECODE(TTM_NEWTOOLRECT);
DECLAREDECODE(TTM_RELAYEVENT);
DECLAREDECODE(TTM_GETTOOLINFO);
DECLAREDECODE(TTM_SETTOOLINFO);
DECLAREDECODE(TTM_HITTEST);
DECLAREDECODE(TTM_GETTEXT);
DECLAREDECODE(TTM_UPDATETIPTEXT);
DECLAREDECODE(TTM_GETTOOLCOUNT);
DECLAREDECODE(TTM_ENUMTOOLS);
DECLAREDECODE(TTM_GETCURRENTTOOL);
#endif	// DISABLE_WIN95_MESSAGES

//
// TVMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(TVM_INSERTITEM);
DECLAREDECODE(TVM_DELETEITEM);
DECLAREDECODE(TVM_EXPAND);
DECLAREDECODE(TVM_GETITEMRECT);
DECLAREDECODE(TVM_GETCOUNT);
DECLAREDECODE(TVM_GETINDENT);
DECLAREDECODE(TVM_SETINDENT);
DECLAREDECODE(TVM_GETIMAGELIST);
DECLAREDECODE(TVM_SETIMAGELIST);
DECLAREDECODE(TVM_GETNEXTITEM);
DECLAREDECODE(TVM_SELECTITEM);
DECLAREDECODE(TVM_GETITEM);
DECLAREDECODE(TVM_SETITEM);
DECLAREDECODE(TVM_EDITLABEL);
DECLAREDECODE(TVM_GETEDITCONTROL);
DECLAREDECODE(TVM_GETVISIBLECOUNT);
DECLAREDECODE(TVM_HITTEST);
DECLAREDECODE(TVM_CREATEDRAGIMAGE);
DECLAREDECODE(TVM_SORTCHILDREN);
DECLAREDECODE(TVM_ENSUREVISIBLE);
DECLAREDECODE(TVM_SORTCHILDRENCB);
DECLAREDECODE(TVM_ENDEDITLABELNOW);
DECLAREDECODE(TVM_GETISEARCHSTRING);
#endif	// DISABLE_WIN95_MESSAGES

//
// UpDnMsgs.cpp
//
#ifndef DISABLE_WIN95_MESSAGES
DECLAREDECODE(UDM_SETRANGE);
DECLAREDECODE(UDM_GETRANGE);
DECLAREDECODE(UDM_SETPOS);
DECLAREDECODE(UDM_GETPOS);
DECLAREDECODE(UDM_SETBUDDY);
DECLAREDECODE(UDM_GETBUDDY);
DECLAREDECODE(UDM_SETACCEL);
DECLAREDECODE(UDM_GETACCEL);
DECLAREDECODE(UDM_SETBASE);
DECLAREDECODE(UDM_GETBASE);
#endif	// DISABLE_WIN95_MESSAGES

//
// WinMsgs.cpp
//
extern VALUETABLE tblWindowStyles[];
extern VALUETABLE tblDialogStyles[];
extern VALUETABLE tblButtonStyles[];
extern VALUETABLE tblComboBoxStyles[];
extern VALUETABLE tblEditStyles[];
extern VALUETABLE tblListBoxStyles[];
extern VALUETABLE tblScrollBarStyles[];
extern VALUETABLE tblStaticStyles[];
extern VALUETABLE tblExtWindowStyles[];
extern VALUETABLE tblHeaderStyles[];
extern VALUETABLE tblToolbarStyles[];
extern VALUETABLE tblToolTipStyles[];
extern VALUETABLE tblStatusBarStyles[];
extern VALUETABLE tblTrackBarStyles[];
extern VALUETABLE tblUpDownStyles[];
extern VALUETABLE tblListViewStyles[];
extern VALUETABLE tblTreeViewStyles[];
extern VALUETABLE tblTabControlStyles[];
extern VALUETABLE tblAnimateStyles[];
extern VALUETABLE tblHScroll[];
extern VALUETABLE tblVScroll[];
extern VALUETABLE tblWindowPos[];
extern VALUETABLE tblCreateWindowPos[];
extern VALUETABLE tblTrueFalse[];

DECLAREDECODE(WM_ACTIVATE);
DECLAREDECODE(WM_ACTIVATEAPP);
DECLAREDECODE(WM_CANCELJOURNAL);
DECLAREDECODE(WM_CANCELMODE);
DECLAREDECODE(WM_CLOSE);
DECLAREDECODE(WM_COMMAND);
DECLAREDECODE(WM_COMPACTING);
DECLAREDECODE(WM_COMPAREITEM);
DECLAREDECODE(WM_COPYDATA);
// TODO: DECLAREDECODE(WM_CPL_LAUNCH);
// TODO: DECLAREDECODE(WM_CPL_LAUNCHED);
DECLAREDECODE(WM_CREATE);
DECLAREDECODE(WM_CTLCOLORBTN);
DECLAREDECODE(WM_CTLCOLORDLG);
DECLAREDECODE(WM_CTLCOLOREDIT);
DECLAREDECODE(WM_CTLCOLORLISTBOX);
DECLAREDECODE(WM_CTLCOLORMSGBOX);
DECLAREDECODE(WM_CTLCOLORSCROLLBAR);
DECLAREDECODE(WM_CTLCOLORSTATIC);
DECLAREDECODE(WM_DELETEITEM);
DECLAREDECODE(WM_DESTROY);
DECLAREDECODE(WM_DEVMODECHANGE);
DECLAREDECODE(WM_DRAWITEM);
DECLAREDECODE(WM_DROPFILES);
DECLAREDECODE(WM_ENABLE);
DECLAREDECODE(WM_ENDSESSION);
DECLAREDECODE(WM_ENTERIDLE);
DECLAREDECODE(WM_ENTERMENULOOP);
DECLAREDECODE(WM_EXITMENULOOP);
DECLAREDECODE(WM_ERASEBKGND);
DECLAREDECODE(WM_FONTCHANGE);
DECLAREDECODE(WM_GETDLGCODE);
DECLAREDECODE(WM_GETFONT);
DECLAREDECODE(WM_GETMINMAXINFO);
DECLAREDECODE(WM_GETTEXT);
DECLAREDECODE(WM_GETTEXTLENGTH);
DECLAREDECODE(WM_HSCROLL);
DECLAREDECODE(WM_ICONERASEBKGND);
DECLAREDECODE(WM_INITDIALOG);
DECLAREDECODE(WM_INITMENU);
DECLAREDECODE(WM_INITMENUPOPUP);
DECLAREDECODE(WM_KILLFOCUS);
DECLAREDECODE(WM_MEASUREITEM);
DECLAREDECODE(WM_MENUCHAR);
DECLAREDECODE(WM_MENUSELECT);
DECLAREDECODE(WM_MOVE);
DECLAREDECODE(WM_NEXTDLGCTL);
DECLAREDECODE(WM_NOTIFY);
DECLAREDECODE(WM_NULL);
DECLAREDECODE(WM_PAINT);
DECLAREDECODE(WM_PAINTICON);
DECLAREDECODE(WM_PALETTECHANGED);
DECLAREDECODE(WM_PALETTEISCHANGING);
DECLAREDECODE(WM_POWER);
DECLAREDECODE(WM_PRINT);
DECLAREDECODE(WM_PRINTCLIENT);
DECLAREDECODE(WM_QUERYDRAGICON);
DECLAREDECODE(WM_QUERYENDSESSION);
DECLAREDECODE(WM_QUERYNEWPALETTE);
DECLAREDECODE(WM_QUERYOPEN);
DECLAREDECODE(WM_QUEUESYNC);
DECLAREDECODE(WM_QUIT);
DECLAREDECODE(WM_SETFOCUS);
DECLAREDECODE(WM_SETFONT);
DECLAREDECODE(WM_SETREDRAW);
DECLAREDECODE(WM_SETTEXT);
DECLAREDECODE(WM_SETTINGCHANGE);
DECLAREDECODE(WM_SHOWWINDOW);
DECLAREDECODE(WM_SIZE);
DECLAREDECODE(WM_SPOOLERSTATUS);
DECLAREDECODE(WM_SYSCOLORCHANGE);
DECLAREDECODE(WM_SYSCOMMAND);
DECLAREDECODE(WM_TIMECHANGE);
DECLAREDECODE(WM_TIMER);
DECLAREDECODE(WM_VSCROLL);
DECLAREDECODE(WM_WINDOWPOSCHANGED);
DECLAREDECODE(WM_WINDOWPOSCHANGING);
DECLAREDECODE(WM_WININICHANGE);
DECLAREDECODE(WM_INPUTLANGCHANGEREQUEST);
DECLAREDECODE(WM_INPUTLANGCHANGE);
DECLAREDECODE(WM_TCARD);
DECLAREDECODE(WM_HELP);
DECLAREDECODE(WM_USERCHANGED);
DECLAREDECODE(WM_NOTIFYFORMAT);
DECLAREDECODE(WM_CONTEXTMENU);
DECLAREDECODE(WM_STYLECHANGING);
DECLAREDECODE(WM_STYLECHANGED);
DECLAREDECODE(WM_DISPLAYCHANGE);
DECLAREDECODE(WM_GETICON);
DECLAREDECODE(WM_SETICON);
DECLAREDECODE(WM_NEXTMENU);
DECLAREDECODE(WM_SIZING);
DECLAREDECODE(WM_CAPTURECHANGED);
DECLAREDECODE(WM_MOVING);
DECLAREDECODE(WM_POWERBROADCAST);
DECLAREDECODE(WM_DEVICECHANGE);
