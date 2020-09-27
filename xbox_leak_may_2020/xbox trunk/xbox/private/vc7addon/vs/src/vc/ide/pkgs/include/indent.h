//--------------------------------------------------------------
//
//  File:       indent.h
//
//  Contents:   Indentation code called by language manager
//
//  History:    
//---------------------------------------------------------------
#pragma once

#include "lexcpp.h"
#include <tokcpp.h>

HRESULT InitSmartIndent(long initialBufferSize);
void UninitSmartIndent(void);

HRESULT DoSmartIndent(long lLine, long lCol, IVsTextLines *pVsTextLines, IVsTextView *pVsTextView,
    LANGPREFERENCES *plangPref, VCPREFERENCES * pVCPrefs, long *pDifflLine=NULL, long *pDifflEndLine=NULL);
BOOL DoMatchBraces(long *lLine, long *lColumn, IVsTextLines *pVsTextLines);
BOOL FFindTokenMatchWithinStatement(TOKCPP tok, long *plLine, long *plCol, IVsTextLines *pVsTextLines);

//////////////////////////////////////////////////
// CLineStatus
//  used to hold the tokenized line information
//  it knows its line number and can reposition itself if IsFixed()==FALSE
//  if IsFixed()==TRUE, it does not know its line number, ex. when holding a buffer
/////////////////////////////////////////////////
class CLineStatus
{
public:
    enum eSLineKind
    {
        eMovable,
        eFixed
    };
protected:
    //when IsFixed(), it's -1, which means this does not know its line number
    long    m_currentLine; //if it knows the line, then it can relex/move to a different line
public:
    RGTXTB  m_TokenInfo;
    LINEDATA m_LineData;
public:
    CLineStatus ();
    CLineStatus (IVsTextLines *pVsTextLines, UINT phyLine, eSLineKind bFixed = eMovable, BOOL bIsODL = FALSE);
    ~CLineStatus();
    inline BOOL IsFixed() {return m_currentLine==-1;}
    inline void SetFixed(){m_currentLine=-1;}
    BOOL    InitTokens(UINT phyLine, eSLineKind bFixed = eMovable, BOOL bIsODL = FALSE);
    void    ReleaseTokens();
    BOOL    InitTokensAtPosition(long iPosition, long *piLine, CharIndex *piColumn, eSLineKind bFixed);
    _TS     GetTabSpace(int ctchTab);
    BOOL    GetFirstToken(TOKCPP * pToken, BOOL bSkipComment, UINT * piPos = NULL);
    BOOL    GetLastToken (TOKCPP * pToken, BOOL bSkipComment, UINT * piPos = NULL);
    BOOL    CompareTokenStream(CLineStatus &lsCompare, BOOL bSkipComment = TRUE);
//when IsFixed()==FALSE, those function can move it to previous/next line
//when IsFixed()==TURE, those functions return FALSE on its First/Last token
    INT     GetNextToken(TOKCPP * pToken, BOOL bSkipComment, /*in,out*/UINT * piPos);
    INT     GetPrevToken (TOKCPP * pToken, BOOL bSkipComment,/*in,out*/UINT * piPos);
    inline UINT GetNumToken(){ return m_TokenInfo.size();}
protected:
    IVsTextLines *m_spVsTextLines;
    //functions that must access m_spVsTextLines
    friend class CSmartIndentCPP;
    friend class CVsCppLangInfo;
public:
//getter functions
    inline long     GetCurrentLine()     {return m_currentLine;}
};


//limit of BraceMatcher.DoMatchBraces() in some editor situations
//like brace highlighting & current block searching for Find Scope
const ULONG g_cLineLimit = 1000;
//HRESULTs that can be returned by CBraceMatcher::GetBlockName() and included in dwClient field of new region
//S_OK is returned for function block
#define S_BM_NAMESPACE 8 //namespace block
#define S_BM_CLASS 4 //class/struct/interface/union/enum block
#define S_BM_STMNT 2 //statement block (other than function or type declaration)
#define S_BM_FUNCT 1 //function definition

#define S_BM_COMMENT 16//comment block
#define S_BM_EXTERN  32//extern "C" {} block
#define S_BM_NONAME  64//unnamed block (syntax error before opening "{" or simply grouping instruction)
#define S_BM_JUNK   128//the block of globals in between codeelements' definitions
//flags for CBraceMatcher::GetOutlinesInBlock()
#define dwBlockCollapsed       0 //it's an opposite to dwBlockExpanded
#define dwBlockExpanded        1
#define dwBlockOneOnly         2 //only the current block, ends at the closing brace


// CBraceMatcher - class to help finding a match for [ and { braces, < and ( parens, the ends of strings
//   comments, and conditional compile directives (#if/#ifdef/#ifndef/#elif/#else/#endif)
// Can either find a match for the token at current position (initialized in the constructor)
//   or the caller can supply a token (DoMatchBracesForThisToken).
class CBraceMatcher : public CLineStatus
{
public:
    enum eBMatchDirection
    {
        eBackward = -1,
        eForward = 1
    };
    CBraceMatcher   ();
    HRESULT         Init(LPCWSTR szStream, ULONG lPosCur = 0, LXS lxs = 0);
    HRESULT         Init(long lLineCur, long lColCur, IVsTextLines *pVsTextLines, eSLineKind bFixed = eMovable);
    
    //when tok = tokUNKNOWN : use token at position given by *plLine, *plCol
    BOOL            DoMatchBraces(TOKCPP tok = tokUNKNOWN, INT nIgnoreTokens = 0, ULONG lineLimit = 0xFFFFFFFF);

    BOOL            FFindMatchingForStringOrComment(long *plLine, long *plCol, ULONG lineLimit = 0xFFFFFFFF);
    BOOL            FFindEndOfMacro(long *plLine, long *plCol);

    BOOL            FSkip_ToPPBoundary(eBMatchDirection direction, ULONG *plineLimit=NULL);
    BOOL            FFindMatching_PPKeyw(eBMatchDirection direction, ULONG lineLimit=0xFFFFFFFF);
public:
    //direction<0 - backwards; direction>0 - forwards; direction==0 - does not move
    //returns the TOKCPP at the new location or -1 if error
    TOKCPP          MoveToNextToken(int direction = eForward, BOOL bSkipComment = TRUE, ULONG *plineLimit = NULL);
    BOOL            FindToken(TOKCPP tok, int direction = eForward, BOOL bSkipComment = TRUE, ULONG *plineLimit = NULL);
    BOOL            FindToken(TOKCPP * rgtok, int direction = eForward, BOOL bSkipComment = TRUE, ULONG *plineLimit = NULL);
    BOOL            FindIdentifier(LPCWSTR bstrText, int direction = eForward, BOOL bSkipComment = TRUE, ULONG *plineLimit = NULL);
    //block parsing operations
    HRESULT         GetBlockName(BSTR *pbstrFunName = NULL, bool bRestoreState = true);
    void            GetOutlinesInBlock(DWORD dwFlags, CSimpleArray<NewHiddenRegion> &OpenRegions, CSimpleArray<NewHiddenRegion> &MatchedRegions,
                    ULONG *nLineLimit = NULL);
    void            OpenJunkOutline(HIDDEN_REGION_STATE dwState, CSimpleArray<NewHiddenRegion> &OpenRegions);
private:
    void            OpenOutline(DWORD dwFlags, HRESULT hrType, long firstLine, CharIndex firstCol,
                    CSimpleArray<NewHiddenRegion> &OpenRegions);
    HRESULT         GetJunkToFirstDefinition(CSimpleArray<NewHiddenRegion> &OpenRegions, CSimpleArray<NewHiddenRegion> &Regions,
                    DWORD flags, ULONG *nLineLimit = NULL);
    TOKCPP          AddCommentOutline(HIDDEN_REGION_STATE dwState, CSimpleArray<NewHiddenRegion> &OpenRegions, CSimpleArray<NewHiddenRegion> &MatchedRegions,
                    ULONG *pnLineLimit = NULL);
//state of this object
protected:
    INT                         m_iPos;
//  CLexCPP                     m_lex; //could be hidden inside CLineStatus, but it's a stack object there
public:
    inline UINT     GetCurrentPos()      
    {
        VSASSERT(m_TokenInfo.size()>(UINT)m_iPos, "BraceMatcher in unknown state. Assign bug to ChrisKoz");
        return m_iPos;
    }
    inline TOKCPP   GetCurrentToken()    {return static_cast<TOKCPP>(m_TokenInfo[GetCurrentPos()].tok);}
    INT             CompareCurrentTokenText(LPCWSTR bstrCompare);
    HRESULT         GetCurrentToken(BSTR *pbstrToken);
    INT             CompareCurrentTokenTextFirstNChars(BSTR bstrCompare, int nChar);
    inline UINT     GetCurrentBeginCol() {return m_TokenInfo[GetCurrentPos()].ibTokMin;}
    inline UINT     GetCurrentEndCol()   {return m_TokenInfo[GetCurrentPos()].ibTokMac;}

    inline void     SetCurrentPos(int iPos)
    {
        VSASSERT(m_TokenInfo.size()>(UINT)iPos, "Attempt to set BraceMatcher to wrong state");
        m_iPos=iPos; //how useful is it?
    }
    inline void     ResetPos(INT iLine, INT iPos)
    {
        if(iLine!=GetCurrentLine())
        {
            ReleaseTokens();
            InitTokens(iLine);
        }
        SetCurrentPos(iPos);
    }
};
