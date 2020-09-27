//-----------------------------------------------------------------------
//
// File:	 lexcpp.h
//
// Contains: Smart indenting code for C / C++ files.  A number of defines
//			 and structures from various places in the old shell tree have
//			 been added to this file in order to get things to work.
//
// REVIEW:   This code is just a port of the old lexer code and is a 
//			 candidate for a rewrite since it has to be hacked in order
//			 to work with the new shell editor.  I've tried to isolate
//			 this code as much as possible so that a rewrite would be 
//			 easier in the event that we decide to do one.		
//
//-----------------------------------------------------------------------

#pragma once

#include "lex.h"
	
class CLexCPP: public CLex
{
public :

	static CVCStringList m_listLexExtensions;
	static CVCStringList m_listNCBExtensions;
    //default NCBExtentions if not present in registry
    static LPCTSTR m_rgszNCBExtensions[];

	CLexCPP(ReservedWord *pUserDefKW = NULL);
	virtual ~CLexCPP(void);

	// see if this lexer will by default handle this type of file
//	static BOOL FCanLexFile(LPCTSTR szFilename);
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
						BYTE *indexTable,
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
	void FindRightTBracket (LPCWSTR ptchLine,
							UINT cbLength,
							UINT cbCurrent,
							TXTB & tokenInfo);
} ;

bool FIsLocationInsideAttribBlock(long ich, long iline, long *pichBeginBlock, long *pilineBeginBlock, IVsTextLines *pTextLines);
