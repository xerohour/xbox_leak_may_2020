/////////////////////////////////////////////////////////////////////////////
// apitext.h
//
// email	date		change
// enriquep	10/28/94	created
//
// copyright 1994 Microsoft


// RTF filename
CString strListFile = "apitext.lst";
CString strTXTFile = "apidata.txt";

// comment block constants

const CString cstrBeginClassHelp = "BEGIN_CLASS_HELP";
const CString cstrEndClassHelp = "END_CLASS_HELP";
const CString cstrClassName = "ClassName:";
const CString cstrBaseClass = "BaseClass:";
const CString cstrCategory = "Category:";

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
void ProcessCommentBlocks(CString strFilename, CStdioFile &file);
CString EliminateLeadingChars(CString str, CString strSet);
CString EliminateTrailingChars(CString str, CString strSet);
void ReadParams(CString strLine);
void WriteParams(CStdioFile &fTXT);
CString VerifyFuncBlockIsUpToDate(CString strLine, CString strFuncBlock);
CString VerifyReturnBlockIsUpToDate(CString strLine, CString strReturnBlock);
void VerifyParamBlockIsUpToDate( CString strLine );

/////////////////////////////////////////////////////////////////////////////
// CParam

class CParam
{
public:
    CParam();
    ~CParam();
// Attributes
public:
	CString m_strName;
    CString m_strType;
    CString m_strValue;

// Operations
};

typedef CTypedPtrList<CPtrList, CParam*> CParamList;

CParam::CParam()
{
}

CParam::~CParam()
{
}

