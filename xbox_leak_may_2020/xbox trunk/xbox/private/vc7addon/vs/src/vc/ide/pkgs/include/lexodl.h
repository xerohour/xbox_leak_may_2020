#ifndef __LEXODL__
#define __LEXODL__

#include <cstringt.h>
#include "lex.h"
#include "vccolls.h"

#if 0 //obsolete, should be removed, IMO
//////////////////////////////////////////////////////needed by m_usertokenArray
const unsigned ctchUserTokenPhrase = 100;
struct USERTOKENS
{
	INT			token;		// preassigned in the user range
	TCHAR		szToken[ctchUserTokenPhrase+1];	// token class name exposed to user
	COLORREF	RGBText;
	COLORREF	RGBBackground;
	//AUTO_COLOR	autocolorFore;
	//AUTO_COLOR	autocolorBack;
};

typedef USERTOKENS *		PUSERTOKENS;
typedef const USERTOKENS *	PCUSERTOKENS;
/////////////////////////////////////////////////////end of m_usertokenArray
#endif

class CLexODL: public CLex
{
public :

	static CVCStringList m_listLexExtensions;
	static CVCStringList m_listNCBExtensions;
    //default NCBExtentions if not present in registry
    static LPCTSTR m_rgszNCBExtensions[];

	CLexODL(ReservedWord *pUserDefKW = NULL);
	virtual ~CLexODL(void) { }

//	static SMART_STYLES	m_smartStyles;
//	static USERTOKENS   m_usertokenArray[]; //replaced by m_pUserDefKW

	// give a pointer to the language specific metrics.
	// Lexer can expect it to be used at any time, so leave
	// it allocated and unchanged. It can ( and probably
	// should) be in r/o memory.
//	virtual void QueryLangMetrics (P_IO(LANGMETRICS));
	
//	virtual UINT GetDefaultFileExtension(LPCTSTR szSubLanguage);

	// see if this lexer will by default handle this type of file
	static BOOL FCanLexFile(LPCTSTR szFilename);
	static BOOL FIsNCBFile(LPCTSTR szFilename);

	// lex the line, giving us back ton'o'info,
	// returns count of bytes that were tokenized in
	// ptchLine.
	virtual UINT CbLexLine (
					LPCWSTR		ptchLine,
					UINT		cbLine,
					PLXS		plxs,	  // in: BOL state, out: eol state
					P_OUT (RGTXTB) rgtxtb,
					P_IO(DWORD)	dwUserKwArraySize
					);

	// slim version of lex line, i.e.: without
	// rgtxtb since only interested in plxs
	virtual UINT CbLexLine (
					LPCWSTR		ptchLine,
					UINT		cbLine,
					PLXS		plxs,	  // in: BOL state, out: eol state
					P_IO(DWORD)	dwUserKwArraySize
					);
	//		lex a line & fill the color table
	//		it returns the lex state at the end of the line (plxs)
	virtual UINT CbLexLine (LPCWSTR		ptchLine,
						 UINT		cbLine,
						 PLXS		plxs,
						 ULONG		pAttributes[],
						 P_IO(DWORD)	dwUserKwArraySize
						 );

	// give out the smart indent info
//	virtual void QuerySmartStyles ( P_IO(SMART_STYLES) );

	// get the initial LXS for this file, so one lexer can handle multiple
	//	languages or variations of the same language.
	virtual LXS LxsInitial ( LPCTSTR szFilename );
	
	// returns TRUE if LXS is in the special state (in C++, in AFX_WIZARD state for instance)
//	virtual BOOL IsInSpecialState (LXS lxs) { return (lxs & inWizard)? TRUE : FALSE; };

	// reset the state if it is in the special state
//	virtual void UnsetSpecialState (P_IO(LXS) lxs) { lxs &= ~inWizard; };

private:
	UINT GetToken (LPCWSTR ptchLine, 
					UINT  cbLength,
					UINT cbCurrent,
					DWORD * plxs,
					TXTB & tokenInfo);

	UINT FindEndComment (LPCWSTR ptchLine,
					UINT cbLength,
					UINT cbCurrent,
					DWORD * plxs,
					TXTB & tokenInfo);

	BOOL IsContinuationLine(LPCWSTR ptchLine,
					UINT cbLength,
					UINT cbCurrent);

	UINT FindEndString (LPCWSTR ptchLine,
					UINT cbLength,
					UINT cbCurrent,
					DWORD * plxs,
					TXTB & tokenInfo);

	UINT FindNextToken (LPCWSTR ptchLine,
					UINT cbLength,
					UINT cbCurrent,
					DWORD * plxs,
					TXTB & tokenInfo);

	BOOL IsCharacter (LPCWSTR ptchLine,
						UINT cbLength,
						UINT cbCurrent,
						TXTB & tokenInfo); 
	HINT GetHint (LPCWSTR ptchLine,
				  UINT cbLength,
				  UINT cbCurrent,
				  DWORD * plxs,
				  TXTB & tokenInfo);
	__inline BOOL IsSingleOp (HINT hint);
	__inline BOOL IsMulOp (HINT hint);
	void FindMulOp (LPCWSTR ptchLine,
					UINT cbLength,
					UINT cbCurrent,
					DWORD * plxs,
					TXTB & tokenInfo,
					HINT hint);
	UINT SkipWhiteSpace (LPCWSTR ptchLine, 
						 UINT cbLength,
						 UINT cbCurrent);
	BOOL IsWhiteSpace (WCHAR wch)
	{
		return iswspace(wch);
	};
	BOOL IsValidHintKey(WCHAR wchKey)
	{
		return wchKey < 128u;
	};
	int GetTokenLength (LPCWSTR ptchLine,
						UINT cbLength,
						UINT cbCurrent);
	BOOL IsKeyword (LPCWSTR ptchLine,
					UINT cbCurrent,
					int iTokenLength,
					DWORD * plxs,
					TXTB & tokenInfo,
					HINT hint);
	BOOL IsUserDefinedKW (LPCWSTR ptchLine,
	                      UINT cbCurrent,
	                      int iTokenLength,
	                      TXTB & tokenInfo);		
	BOOL LookupIndexedKeyWord (ReservedWord rwTable[],
						short *indexTable,
						int iTableSize,
						LPCWSTR ptchLine, 
						int iTokenLength, 
						TXTB & tokenInfo);
	BOOL IsIdentifier (int iTokenLength,
					   TXTB & tokenInfo);
	BOOL IsPPKeyWord (LPCWSTR ptchLine,
					  UINT cbLength,
					  UINT cbCurrent,
						DWORD * plxs,
					  TXTB & tokenInfo);
	BOOL LookupLinearKeyWord (ReservedWord rwTable[],
						int iTableSize,
						LPCWSTR ptchLine,
						int length, 
						TXTB & tokenInfo);
	BOOL IsUnknownID (LPCWSTR ptchLine,
					  UINT cbLength,
					  UINT cbCurrent,
					  TXTB & tokenInfo);
	BOOL IsNumber (LPCWSTR ptchLine,
				   UINT cbLength,
				   UINT cbCurrent,
				   TXTB & tokenInfo);
	BOOL IsUuidString (LPCWSTR ptchLine, 
						UINT cbLength, 
						UINT cbCurrent, 
						TXTB & tokenInfo);
	void FindRightTBracket (LPCWSTR ptchLine,
							UINT cbLength,
							UINT cbCurrent,
							TXTB & tokenInfo);
} ;


#endif
