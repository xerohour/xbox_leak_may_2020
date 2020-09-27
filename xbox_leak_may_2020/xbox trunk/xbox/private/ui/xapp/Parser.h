class CNodeClass;
class CClass;
class CFunction;

// REVIEW: These should all be in CLexer...
void StartParse(const TCHAR* pch, const TCHAR* szFileName, int nLine = 1);
void EndParse();

enum COMMENT_TYPE
{
	none = 0,
	pound = 1,
	slashslash = 2,
	slashstar = 4,
	any = 0xffffffff
};

const TCHAR* SkipWhite(const TCHAR* pch, COMMENT_TYPE ct = any);
const TCHAR* Token(const TCHAR* pch, const TCHAR*& pchToken, int& cchToken, bool bAllowPaths = false, COMMENT_TYPE ct = any);
extern void SyntaxError(const TCHAR* szMsg, ...);
const TCHAR* ParseBoolean(const TCHAR* pch, bool& b);
const TCHAR* ParseInteger(const TCHAR* pch, int& i);
const TCHAR* ParseNumber(const TCHAR* pch, float& n);
const TCHAR* ParseString(const TCHAR* pch, TCHAR*& sz);
const TCHAR* ParseVec3(const TCHAR* pch, float v[3]);
const TCHAR* ParseVec4(const TCHAR* pch, float v[4]);
const TCHAR* ParseIntArray(const TCHAR* pch, BYTE*& pbArray, int& cbArray);
const TCHAR* ParseVecArray(const TCHAR* pch, BYTE*& pbArray, int& cbArray, int nVecSize);
int ExpandCString(TCHAR* szString, int cchMaxString, const TCHAR* pchToken, int cchToken);


class CParser
{
public:
	CParser(const TCHAR* szFilePath, const TCHAR* pchFile, int cchFile = -1);
	~CParser();

	void SkipWhite();
	bool Token(const TCHAR*& pchToken, int& cchToken, bool bAllowPaths = false);
	void SyntaxError(const TCHAR* szMsg, ...);
	bool AtEnd() const;

	const TCHAR* m_pchFile;
	int m_cchFile;

	const TCHAR* m_szFilePath;

	const TCHAR* m_pch;
	int m_nLine;
};

inline bool CParser::AtEnd() const
{
	return (m_pch >= m_pchFile + m_cchFile);
}



class CCompiler
{
public:
	CCompiler();
	virtual ~CCompiler();

	inline void Write(const void* pv, int cb)
	{
		if (m_nop + cb >= m_opsSize)
			GrowTo(m_nop + cb);
		CopyMemory(&m_ops[m_nop], pv, cb);
		m_nop += cb;
	}

	inline void Write(BYTE b) { Write(&b, 1); }
	inline void WriteInteger(int n) { Write(&n, sizeof (int)); }
	inline void WriteNumber(float n) { Write(&n, sizeof (float)); }
	inline void WriteString(const TCHAR* pch, int cch) { WriteInteger(cch); Write(pch, cch * sizeof (TCHAR)); }

	inline UINT GetAddress() { return m_nop; }

	inline void Fixup(UINT nAddress, UINT nValue)
	{
		ASSERT(nAddress < m_nop);
		CopyMemory(&m_ops[nAddress], &nValue, sizeof (UINT));
	}

	CFunction* CreateFunction();

	void SyntaxError(const TCHAR* szMsg, ...);
	inline void SetError() { m_bError = true; }
	inline bool HadError() const { return m_bError; }

	void GrowTo(int nNewSize);

protected:
	UINT m_nop;
	BYTE* m_ops;
	UINT m_opsSize;
	bool m_bError;
};


struct ST
{
	short cchName;
	const TCHAR* pchName;
};

struct DOPER
{
	TCHAR m_ch1;
	TCHAR m_ch2;
	TCHAR m_ch3;
	TCHAR m_pri;
	BYTE m_op;
};

struct BREAK
{
	BREAK* m_pNext;
	int m_nop;
};

class CFunctionCompiler : public CCompiler
{
public:
	CFunctionCompiler();

	ST m_rgstLocal [100]; // REVIEW: Ack!
	int m_nFrameSize;

	const TCHAR* ParseOperator(const TCHAR* pch, const DOPER*& pdoper);
	const TCHAR* ParseExpression(const TCHAR* pch);
	const TCHAR* ParseStatement(const TCHAR* pch);
	const TCHAR* ParseTerm(const TCHAR* pch);
	const TCHAR* ParseBlock(const TCHAR* pch);
	const TCHAR* ParseLocalVar(const TCHAR* pch);
	const TCHAR* ParseArray(const TCHAR* pch);
	const TCHAR* ParseCall(const TCHAR* pch);
	const TCHAR* ParseExp(const TCHAR* pch, int nPrio);
	const TCHAR* ParseIF(const TCHAR* pch);
	const TCHAR* ParseWHILE(const TCHAR* pch);
	const TCHAR* ParseFOR(const TCHAR* pch);
	const TCHAR* ParseDO(const TCHAR* pch);
	const TCHAR* ParseBREAK(const TCHAR* pch);
	const TCHAR* ParseCONTINUE(const TCHAR* pch);
	const TCHAR* ParseRETURN(const TCHAR* pch);
	const TCHAR* ParseSLEEP(const TCHAR* pch);

	void FixupBreaks();
	BREAK* m_pBreak;
	int m_nBreakables;
	int m_nopTopOfLoop;
	bool m_bBehavior;
};

class CClassCompiler : public CCompiler
{
public:
	CClassCompiler(CClass* pClass);

	const TCHAR* Compile(const TCHAR* pch);

protected:
	const TCHAR* ParseNode(const TCHAR* pch);
	const TCHAR* ParseProps(const TCHAR* pch, CNodeClass* pNodeClass);
	const TCHAR* ParseChildren(const TCHAR* pch);
	const TCHAR* ParseMemberVar(const TCHAR* pch);
	const TCHAR* ParseClass(const TCHAR* pch);
	const TCHAR* ParseFunction(const TCHAR* pch, CFunction*& pFunction, bool bBehavior = false);

	CClass* m_pClass;
};
