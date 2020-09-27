class CInstance;
class CFunction;

// This is an entry in the namespace...
class CMember : public CObject
{
public:
	CMember();
	~CMember();

	void Assign(CObject* pObject);
	CObject* Deref();

	int m_nMember;
//	CObject* m_pValue;
};

// This is a run-time reference to a member variable...
class CMemberVarObject : public CObject
{
public:
	CMemberVarObject(CInstance* pInstance, int m_nMember);
	~CMemberVarObject();

	CInstance* m_pObject;
	int m_nMember;

	CObject* Deref();
	void Assign(CObject* pObject);
};


// This is a run-time reference to a member function...
class CMemberFunctionObject : public CObject
{
public:
	CMemberFunctionObject(CInstance* pInstance, CFunction* pFunction);
	~CMemberFunctionObject();

	CInstance* m_pObject;
	CFunction* m_pFunction;

	CObject* Deref();
};


CClass* LookupClass(const TCHAR* pchName, int cchName);
//CObject* LookupVariable(const TCHAR* pchName, int cchName);

class CVarObject : public CObject
{
public:
	CVarObject(const TCHAR* pch, int cch);
	~CVarObject();

	int m_nLength;
	int m_nAlloc;
	TCHAR* m_sz;

//	CObject* m_pObject;

	CObject* Deref();
	void Assign(CObject* pObject);

private:
    // Need this to prevent the compiler from using default copy ctor
    CVarObject(const CVarObject&);
    CVarObject& operator=(const CVarObject& rhs);
};

class CNumObject : public CObject
{
public:
	CNumObject();
	CNumObject(float n);
	CNumObject(const TCHAR* szNum);

	CStrObject* ToStr();

	CNumObject* ToNum()
	{
		AddRef();
		return this;
	}

	float m_nValue;
};


class CStrObject : public CObject
{
public:
	CStrObject();
	CStrObject(const TCHAR* sz);
	CStrObject(const TCHAR* pch, int cch);
	~CStrObject();

	inline const TCHAR* GetSz()
	{
		if (m_sz == NULL)
			return _T("");
		return m_sz;
	}

	inline GetLength()
	{
		return m_nLength;
	}

	CStrObject* ToStr();
	CNumObject* ToNum();

	TCHAR* SetLength(int nLength);
	void Append(const TCHAR* szAppend);


	// Methods
	//
	int length(); // REVIEW: This should be a property, not a method!
	CStrObject* charAt(int index);
	int charCodeAt(int index);
	CStrObject* concat(const TCHAR* sz);
	CObject* indexOf(CObject** rgparam, int nParam);
	CObject* lastIndexOf(CObject** rgparam, int nParam);
	CStrObject* slice(int start, int end);
	CObject* substr(CObject** rgparam, int nParam);
	CObject* substring(CObject** rgparam, int nParam);
	CStrObject* toLowerCase();
	CStrObject* toUpperCase();

protected:
	int m_nLength;
	int m_nAlloc;
	TCHAR* m_sz;

	DECLARE_NODE_FUNCTIONS()

private:
    // Need this to prevent the compiler from using default copy ctor
    CStrObject(const CStrObject&);
    CStrObject& operator=(const CStrObject& rhs);
};

class CNodeArrayObject : public CObject
{
public:
	CNodeArrayObject(CNodeArray* pNodeArray);

	CObject* Call(int nFunction, CObject** rgparam, int nParam);
	CObject* Dot(CObject* pObject/*const TCHAR* pchFunc, int cchFunc*/);

	CNodeArray* m_pNodeArray;
};

class CFunctionObject : public CObject
{
public:
	CFunctionObject();
	~CFunctionObject();

	int m_nFunction;
	CObject* m_pObject;

	CObject* Call(CObject** rgparam, int nParam);
};


#define opNull		0
#define opNew		1
#define opVar		2
#define opJump		3
#define opCond		4
#define opCall		5
#define opRet		6
#define opStr		7
#define opNum		8
#define opAdd		9
#define opSub		10
#define opMul		11
#define opDiv		12
#define opDot		13
#define opAssign	14
#define opLocal		15
#define opEQ		16
#define opNE		17
#define opLT		18
#define opGT		19
#define opLE		20
#define opGE		21
#define opMod		22
#define opSHL		23
#define opSHR		24
#define opSHRU		25
#define opAnd		26
#define opOr		27
#define opXor		28
#define opLAnd		29
#define opLOr		30
#define opQuest		31
#define opAddAssign	32
#define opSubAssign	33
#define opMulAssign	34
#define opDivAssign	35
#define opModAssign	36
#define opAndAssign	37
#define opOrAssign	38
#define opXorAssign	39
#define opSHLAssign	40
#define opSHRAssign	41
#define opArray		42
#define opThis		43
#define opFrame		44
#define opStatement	45
#define opNeg		46
#define opDrop		47
#define opEndFrame	48
#define opSleep		49


#define opNewNode		100
#define opNewNodeProp	101
#define opDefNode		102
#define opUseNode		103
#define opEndNode		104
#define opInitProp		105
#define opInitArray		106
#define opEndArray		107
#define opFunction		108

struct RUNCONTEXT
{
	RUNCONTEXT* m_pNextContext;

	BYTE* m_ops;
	UINT m_nop;
	UINT m_spFrame;
	CObject* m_pSelf;
	CObject* m_pThis;
	UINT m_sp;
};

class CRunner
{
public:
	CRunner(CObject* pThis);
	~CRunner();

	bool SetFunc(const TCHAR* szFunc);

	void Push(float nValue);
	void Push(int nValue);
	void Push(const TCHAR* szValue, int nLen = -1);

	void Push(CObject* pObject);
	CObject* Pop();

	bool HasFunc() const { return m_ops != NULL; }
	void SetFunc(CFunction* pFunction);
	void ResetFunc();

	CObject* Run();
	bool Step(CObject** ppRetObj = NULL);
	void Error(const TCHAR* szFmt, ...);

	BYTE* LookupFunction(const TCHAR* pchName, int cchName, CObject*& pOwner);
	CObject* LookupVariable(const TCHAR* pchName, int cchName);

	bool IsSleeping()
	{
		if (m_wakeup != 0.0f)
		{
			if (m_wakeup > XAppGetNow())
				return true;

			m_wakeup = 0.0f;
		}

		return false;
	}

#ifdef _DEBUG
	void DumpStack();
#endif

protected:
	XTIME m_wakeup;
	UINT m_nop;
	BYTE* m_ops;
	UINT m_sp;
	CObject* m_stack [100];
	UINT m_spFrame;
	UINT m_spBase;
	CObject* m_pSelf;
	CObject* m_pThis;

	bool m_bError;
	int m_nLine;

	bool BinaryOperator(BYTE op);

	void PushContext(UINT nPopStack);
	void PopContext();
	BOOL ExecuteBuiltIn(const TCHAR* pchName, int cchName, int nParam, CObject** rgParam, CObject*& pRetObj);

	inline int FetchInt()
	{
#if defined(_WIN32_WCE)
		int n;
		CopyMemory(&n, &m_ops[m_nop], sizeof (int));
#else
		int n = *((int*)&m_ops[m_nop]);
#endif
		m_nop += sizeof (int);
		return n;
	}

	inline UINT FetchUInt()
	{
#if defined(_WIN32_WCE)
		UINT n;
		CopyMemory(&n, &m_ops[m_nop], sizeof (UINT));
#else
		UINT n = *((UINT*)&m_ops[m_nop]);
#endif
		m_nop += sizeof (UINT);
		return n;
	}

	inline float FetchFloat()
	{
#if defined(_WIN32_WCE)
		float n;
		CopyMemory(&n, &m_ops[m_nop], sizeof (float));
#else
		float n = *((float*)&m_ops[m_nop]);
#endif
		m_nop += sizeof (float);
		return n;
	}

	inline const TCHAR* FetchString(int& cch)
	{
		cch = FetchInt();
		const TCHAR* pch = (const TCHAR*)&m_ops[m_nop];
		m_nop += cch * sizeof (TCHAR);
		return pch;
	}

	RUNCONTEXT* m_pNextContext;

#ifdef _DEBUG
//	_CrtMemState m_memState;
#endif
};

extern CRunner* g_pRunner;
extern CObject* g_pThis; // REVIEW:: Kill this hack!

extern bool ExecuteScript(CObject* pObject, const TCHAR* szFunc);




#undef min
#undef max

class CMathClass : public CNodeClass
{
public:
	CMathClass();

	void AddRef();
	void Release();

	// These are all "non-members" (static functions)
	//
	float abs(float number);
	float acos(float number);
	float asin(float number);
	float atan(float number);
	float atan2(float y, float x);
	float ceil(float number);
	float cos(float number);
	float exp(float number);
	float floor(float number);
	float log(float number);
	float max(float number1, float number2);
	float min(float number1, float number2);
	float pow(float base, float exponent);
	float random();
	float round(float number);
	float sin(float number);
	float sqrt(float number);
	float tan(float number);

	float m_E;
	float m_LN2;
	float m_LN10;
	float m_LOG2E;
	float m_LOG10E;
	float m_PI;
	float m_SQRT1_2;
	float m_SQRT2;

	DECLARE_NODE_FUNCTIONS()
};

