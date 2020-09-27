#include "DefUse.h"
#include "ActiveFile.h"

struct BBox
{
	D3DXVECTOR3 center;
	D3DXVECTOR3 size;
};

////////////////////////////////////////////////////////////////////////////

class CIntArray
{
public:
	CIntArray();
	~CIntArray();

	void SetSize(int nSize);

	int m_nAlloc;
	int m_nSize;
	int* m_value;
};

////////////////////////////////////////////////////////////////////////////

class CNumArray
{
public:
	CNumArray();
	~CNumArray();

	void SetSize(int nSize);

	int m_nAlloc;
	int m_nSize;
	float* m_value;
};

////////////////////////////////////////////////////////////////////////////

class CVec2Array
{
public:
	CVec2Array();
	~CVec2Array();

	void SetSize(int nSize);

	int m_nAlloc;
	int m_nSize;
	D3DXVECTOR2* m_value;
};

////////////////////////////////////////////////////////////////////////////

class CVec3Array
{
public:
	CVec3Array();
	~CVec3Array();

	void SetSize(int nSize);

	int m_nAlloc;
	int m_nSize;
	D3DXVECTOR3* m_value;
};

////////////////////////////////////////////////////////////////////////////

class CVec4Array
{
public:
	CVec4Array();
	~CVec4Array();

	void SetSize(int nSize);

	int m_nAlloc;
	int m_nSize;
	D3DXVECTOR4* m_value;
};

////////////////////////////////////////////////////////////////////////////

class CObject;
class CNode;
class CNodeClass;
class CNumObject;
class CStrObject;
class CSurfx;
class CFunction;
class CRunner;

#define objUndefined		0
#define objNull				1
#define objNumber			2
#define objString			3
#define objVariable			4
#define objNode				5
#define objClass			6
#define objNodeArray		7
#define objFunctionRef		8
#define objMember			9
#define objMemberVar		10
#define objInstance			11
#define objUse				12
#define objFunction			13
#define objMemberFunction	14
#define objArray			15
#define objVec3				16

enum PROP_TYPE
{
	pt_null,
	pt_integer,
	pt_boolean,
	pt_number,
	pt_children,
	pt_vec3,
	pt_vec4,
	pt_quaternion,
	pt_color,
	pt_node,
	pt_string,
	pt_nodearray,
	pt_intarray,
	pt_numarray,
	pt_vec2array,
	pt_vec3array,
	pt_vec4array
};


// PRoperty Descriptor
//
struct PRD
{
	const void* pbOffset;
	PROP_TYPE nType;
	const TCHAR* szName;
};


////////////////////////////////////////////////////////////////////////////
// Member Functions...

typedef CObject* (__cdecl CObject::*NODE_PFN)(void);

#define sig_default			0
#define sig_vv				1
#define sig_iv				2
#define sig_ii				3
#define sig_si				4
#define sig_ss				5
#define sig_isi				6
#define sig_sii				7
#define sig_sv				8
#define sig_nv				9
#define sig_nn				10
#define sig_nnn				11
#define sig_vs				12
#define sig_vss				13
#define sig_vi				14
#define sig_ov				15
#define sig_os				16
#define sig_oo				17
#define sig_vii				18
#define sig_vn				19
#define sig_vnn				20
#define sig_vo				21
#define sig_vnnn			22
#define sig_vnnnn			23
#define sig_iii				24
#define sig_is				25
#define sig_viis			26
#define sig_ni				27
#define sig_vis				28

union FSIG
{
	NODE_PFN pfn;

	CObject* (CObject::*pfn_default)(CObject** rgparam, int nParam);
	void (CObject::*pfn_vv)(void);
	void (CObject::*pfn_vi)(int);
	void (CObject::*pfn_vii)(int, int);
	void (CObject::*pfn_vis)(int, const TCHAR*);
	void (CObject::*pfn_viis)(int, int, const TCHAR*);
	int (CObject::*pfn_iv)(void);
	int (CObject::*pfn_ii)(int);
	int (CObject::*pfn_is)(const TCHAR*);
	int (CObject::*pfn_iii)(int, int);
	CStrObject* (CObject::*pfn_sv)(void);
	CStrObject* (CObject::*pfn_si)(int);
	CStrObject* (CObject::*pfn_sii)(int, int);
	CStrObject* (CObject::*pfn_ss)(const TCHAR*);
	void (CObject::*pfn_vs)(const TCHAR*);
	void (CObject::*pfn_vss)(const TCHAR*, const TCHAR*);
	float (CObject::*pfn_nv)(void);
	float (CObject::*pfn_nn)(float);
	float (CObject::*pfn_nnn)(float, float);
	void (CObject::*pfn_vn)(float);
	void (CObject::*pfn_vnn)(float, float);
	void (CObject::*pfn_vnnn)(float, float, float);
	void (CObject::*pfn_vnnnn)(float, float, float, float);
	CObject* (CObject::*pfn_ov)(void);
	CObject* (CObject::*pfn_os)(const TCHAR*);
	CObject* (CObject::*pfn_oo)(CObject* pObject);
	void (CObject::*pfn_vo)(CObject* pObject);
	float (CObject::*pfn_ni)(int);
};

// FuNction Descriptors
//
struct FND
{
	union FSIG pfn;
	WORD sig;
	const TCHAR* szName;
};

#define START_NODE_FUN(className, baseClassName) \
	FND className::c_rgfnd [] = {

#define END_NODE_FUN() \
	{ NULL, 0, NULL } };

#define NODE_FUN(name) \
	{ (NODE_PFN)(CObject* (CObject::*)(CObject**, int))&name, sig_default, _T(#name) },

#define NODE_FUN_VV(name) \
	{ (NODE_PFN)(void (CObject::*)(void))&name, sig_vv, _T(#name) },

#define NODE_FUN_VI(name) \
	{ (NODE_PFN)(void (CObject::*)(int))&name, sig_vi, _T(#name) },

#define NODE_FUN_VII(name) \
	{ (NODE_PFN)(void (CObject::*)(int, int))&name, sig_vii, _T(#name) },

#define NODE_FUN_VIIS(name) \
	{ (NODE_PFN)(void (CObject::*)(int, int, const TCHAR*))&name, sig_viis, _T(#name) },

#define NODE_FUN_VIS(name) \
	{ (NODE_PFN)(void (CObject::*)(int, const TCHAR*))&name, sig_vis, _T(#name) },

#define NODE_FUN_IV(name) \
	{ (NODE_PFN)(int (CObject::*)(void))&name, sig_iv, _T(#name) },

#define NODE_FUN_II(name) \
	{ (NODE_PFN)(int (CObject::*)(int))&name, sig_ii, _T(#name) },

#define NODE_FUN_III(name) \
	{ (NODE_PFN)(int (CObject::*)(int, int))&name, sig_iii, _T(#name) },

#define NODE_FUN_SI(name) \
	{ (NODE_PFN)(CStrObject* (CObject::*)(int))&name, sig_si, _T(#name) },

#define NODE_FUN_SII(name) \
	{ (NODE_PFN)(CStrObject* (CObject::*)(int, int))&name, sig_sii, _T(#name) },

#define NODE_FUN_SS(name) \
	{ (NODE_PFN)(CStrObject* (CObject::*)(const TCHAR*))&name, sig_ss, _T(#name) },

#define NODE_FUN_SV(name) \
	{ (NODE_PFN)(CStrObject* (CObject::*)(void))&name, sig_sv, _T(#name) },

#define NODE_FUN_VS(name) \
	{ (NODE_PFN)(void (CObject::*)(const TCHAR*))&name, sig_vs, _T(#name) },

#define NODE_FUN_VSS(name) \
	{ (NODE_PFN)(void (CObject::*)(const TCHAR*, const TCHAR*))&name, sig_vss, _T(#name) },

#define NODE_FUN_NV(name) \
	{ (NODE_PFN)(float (CObject::*)(void))&name, sig_nv, _T(#name) },

#define NODE_FUN_NI(name) \
	{ (NODE_PFN)(float (CObject::*)(int))&name, sig_ni, _T(#name) },

#define NODE_FUN_NN(name) \
	{ (NODE_PFN)(float (CObject::*)(float))&name, sig_nn, _T(#name) },

#define NODE_FUN_NNN(name) \
	{ (NODE_PFN)(float (CObject::*)(float, float))&name, sig_nnn, _T(#name) },

#define NODE_FUN_VN(name) \
	{ (NODE_PFN)(void (CObject::*)(float))&name, sig_vn, _T(#name) },

#define NODE_FUN_VNN(name) \
	{ (NODE_PFN)(void (CObject::*)(float, float))&name, sig_vnn, _T(#name) },

#define NODE_FUN_VNNN(name) \
	{ (NODE_PFN)(void (CObject::*)(float, float, float))&name, sig_vnnn, _T(#name) },

#define NODE_FUN_VNNNN(name) \
	{ (NODE_PFN)(void (CObject::*)(float, float, float, float))&name, sig_vnnnn, _T(#name) },

#define NODE_FUN_OV(name) \
	{ (NODE_PFN)(CObject* (CObject::*)(void))&name, sig_ov, _T(#name) },

#define NODE_FUN_OS(name) \
	{ (NODE_PFN)(CObject* (CObject::*)(const TCHAR*))&name, sig_os, _T(#name) },

#define NODE_FUN_OO(name) \
	{ (NODE_PFN)(CObject* (CObject::*)(CObject*))&name, sig_oo, _T(#name) },

#define NODE_FUN_VO(name) \
	{ (NODE_PFN)(void (CObject::*)(CObject*))&name, sig_vo, _T(#name) },

#define NODE_FUN_IS(name) \
	{ (NODE_PFN)(int (CObject::*)(const TCHAR*))&name, sig_is, _T(#name) },

////////////////////////////////////////////////////////////////////////////

class CNodeArray
{
public:
	CNodeArray();
	~CNodeArray();

	void AddNode(CNode* pNode);
	void RemoveNode(CNode* pNode);

	void RemoveAll();
	void ReleaseAll();

	int GetLength()
	{
		return m_nLength;
	}

	CNode* GetNode(int iNode)
	{
		if (iNode < 0 || iNode >= m_nLength)
			return NULL;

		return m_rgpNode[iNode];
	}

	void SetNode(int iNode, CNode* pNode)
	{
		if (iNode < 0 || iNode >= m_nLength)
		{
			ASSERT(FALSE);
			return;
		}

		m_rgpNode[iNode] = pNode;
	}

	void Allocate(int nAlloc);
	void SetLength(int nNodes);

	void Sort(int (__cdecl *pfnCompare)(const void*, const void*))
	{
		qsort(m_rgpNode, m_nLength, sizeof (CNode*), pfnCompare);
	}

protected:
	int m_nLength;
	int m_nAlloc;
	CNode** m_rgpNode;
};

class CObject
{
public:
	CObject();
	virtual ~CObject();

	static PRD m_rgprd [];
	static CNodeClass classCObject;
	static CObject* CreateNode();

	virtual const PRD* GetPropMap() const
	{ return &m_rgprd[0]; }
	const PRD* FindProp(const TCHAR* szName, int cchName);

	void SetProperty(const PRD* pprd, const void* pvValue, int cbValue);
	virtual bool OnSetProperty(const PRD* pprd, const void* pvValue);

	virtual void AddRef();
	virtual void Release();

	int m_nRefCount;

	int m_obj;
	CObject* m_pParent;

	int FindFunction(const TCHAR* pchFunction, int cchFunction);
	virtual FND* GetFunctionMap() const;


	virtual CNodeClass* GetNodeClass() const;
	bool IsKindOf(CNodeClass* pClass) const;

	virtual CNumObject* ToNum();
	virtual CStrObject* ToStr();

	virtual CObject* Deref();
	virtual void Assign(CObject* pObject);

	virtual CObject* Call(int nFunction, CObject** rgparam, int nParam);
	virtual CObject* Dot(CObject* pObj);

	CNameSpace* m_members;
	void AddMember(const TCHAR* pchName, int cchName, CObject* pObject);
	void SetMember(const TCHAR* pchName, int cchName, CObject* pObject);
	virtual CObject* GetMember(const TCHAR* pchName, int cchName);

	CFunction* FindMemberFunction(const TCHAR* pchName, int cchName);

#ifdef _DEBUG
	virtual void Dump() const;
#endif
};

class CNodeClass : public CObject
{
public:
	CNodeClass(const TCHAR* szClassName, int nObjectSize, CObject* (*pfnCreateNode)(), CNodeClass* pBaseClass, const PRD* rgprd);
	~CNodeClass();

	const TCHAR* m_szClassName;
	int m_nObjectSize;
	CObject* (*m_pfnCreateNode)();
	CNodeClass* m_pBaseClass;
	const PRD* m_rgprd;

	CObject* CreateNode();

	const PRD* FindProp(const TCHAR* pchPropName, int cchPropName);
	const PRD* GetPropMap() const { return m_rgprd; }

	static CNodeClass* FindByName(const TCHAR* szNodeClass, int cchNodeClass);
	static CNodeClass* c_pFirstClass;
	CNodeClass* m_pNextClass;
};

// This is an entry in CObject's m_members table...
class CFunction : public CObject
{
public:
	CFunction();
	~CFunction();

#ifdef _DEBUG
	TCHAR* m_szName;
#endif

	int m_cbop;
	BYTE m_rgop[ANYSIZE_ARRAY];

#pragma push_macro("new")
#undef new
    void * operator new(size_t sz, int cbop)
    {
        return ::operator new(sz + cbop);
    }
#pragma pop_macro("new")
};

class CClass : public CNodeClass
{
public:
	CClass();
	~CClass();

	bool Load(const TCHAR* szURL);
	bool LoadAbsURL(const TCHAR* szURL);
	bool ParseFile(const TCHAR* szFileName, const TCHAR* szFile);
	const TCHAR* ParseClassBody(const TCHAR* pch);

	CNode* CreateNode();

	TCHAR* m_szURL;

	CActiveFile m_file;

	CNodeArray m_instanceFunctions;

	CFunction* m_pConstructor;

	int m_nVarCount;

	int GetVariableCount() { return m_nVarCount; }
};


#define NODE_CLASS(className) (&className::class##className)



#define DECLARE_NODE_FUNCTIONS() \
public: \
	static FND c_rgfnd []; \
	virtual FND* GetFunctionMap() const \
	{ return c_rgfnd; };


#define DECLARE_NODE_PROPS() \
	public: \
		static PRD m_rgprd []; \
		virtual const PRD* GetPropMap() const \
		{ return &m_rgprd[0]; }

#define DECLARE_NODE(className, baseClassName) \
public: \
	static CNodeClass class##className; \
	virtual CNodeClass* GetNodeClass() const; \
	static CObject* CreateNode();

#define IMPLEMENT_NODE(friendlyName, className, baseClassName) \
	CObject* className::CreateNode() \
		{ return new className; } \
	CNodeClass className::class##className( \
			_T(##friendlyName), sizeof(class className), className::CreateNode, \
			NODE_CLASS(baseClassName), className::m_rgprd ); \
	CNodeClass* className::GetNodeClass() const \
		{ return NODE_CLASS(className); }



#define START_NODE_PROPS(className, baseClassName) \
	PRD className::m_rgprd [] = { \
		{ &baseClassName::m_rgprd[0], pt_null, _T(#className) },

#define END_NODE_PROPS() \
	{ NULL, pt_null, NULL } };

#define NODE_PROP(theType, theClass, name) \
	{ &((theClass*)0)->m_##name, theType, _T(#name) },


class CNode : public CObject
{
public:
	CNode();
	virtual ~CNode();

	DECLARE_NODE_PROPS()

	CObject* Dot(CObject* pObj);

	CStrObject* ToStr();

	static CObject* CreateNode();
	static CNodeClass classCNode;
	virtual CNodeClass* GetNodeClass() const;

	virtual void OnLoad();

	virtual void Advance(float nSeconds);
	virtual void Render();
	virtual void GetBBox(BBox* pBBox);
	virtual float GetRadius();
#ifdef _LIGHTS
	virtual void SetLight(int& nLight, D3DCOLORVALUE& ambient);
#endif
	virtual const DWORD* GetPalette();
	virtual void RenderDynamicTexture(CSurfx* pSurfx);
	virtual float GetGroundHeight(float x, float z);

	virtual LPDIRECT3DTEXTURE8 GetTextureSurface();

	CRunner* m_pBehavior;
	bool m_visible;

#ifdef _DEBUG
	virtual void Dump() const;
#endif
};

////////////////////////////////////////////////////////////////////////////

class CGroup : public CNode
{
	DECLARE_NODE(CGroup, CNode)
public:
	CGroup();
	~CGroup();

	CNodeArray m_children;

	D3DXVECTOR3 m_bboxCenter;
	D3DXVECTOR3 m_bboxSize;

	bool m_bboxSpecified;
	bool m_bboxDirty;

	void GetBBox(BBox* pBBox);
	float GetRadius();
#ifdef _LIGHTS
	void SetLight(int& nLight, D3DCOLORVALUE& ambient);
#endif
	LPDIRECT3DTEXTURE8 GetTextureSurface();

	void Render();
	void Advance(float nSeconds);
	void RenderDynamicTexture(CSurfx* pSurfx);

	DECLARE_NODE_PROPS()
};

////////////////////////////////////////////////////////////////////////////

class CInstance : public CGroup
{
public:
	CInstance(CClass* pClass);
	~CInstance();

	void Render();

	BYTE* Construct();
	CNodeClass* GetNodeClass() const;

	CObject* Dot(CObject* pObj);

	CClass* m_pClass;
	CNodeArray m_vars;
};

////////////////////////////////////////////////////////////////////////////

class CTimeDepNode : public CNode
{
	DECLARE_NODE(CTimeDepNode, CNode)
public:
	CTimeDepNode();

	bool m_loop;
	float m_startTime;
	float m_stopTime;
	bool m_isActive;

	void Advance(float nSeconds);

	virtual void OnIsActiveChanged();
	virtual void OnCycleEnded();

	float m_lastStartTime;

	DECLARE_NODE_PROPS()
};

////////////////////////////////////////////////////////////////////////////


class CProperty : public CObject
{
public:
	CProperty(CObject* pNode, const PRD* pprd);
	~CProperty();

	CObject* m_pNode;
	const PRD* m_pprd;

	CObject* Deref();
	void Assign(CObject* pObject);
};

////////////////////////////////////////////////////////////////////////////


struct TAGID;


CObject* NewNode(const TCHAR* pchNodeClass, int cchNodeClass);
