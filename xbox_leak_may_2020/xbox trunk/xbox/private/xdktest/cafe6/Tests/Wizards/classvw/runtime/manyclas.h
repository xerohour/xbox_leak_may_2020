// For each class defined in this file check the following in the class view:
// Base class
// member functions
//		return values
//		const'ness
//		default params
//		virtual'ness
//		static'ness
#include <afx.h>

// Empty class.
class empty
{
};

// Publicly derived class
class pubclass : public base_class
{
};

// Protectedly derived class
class protclass : protected base_class
{
};

// Privately derived class
class privclass : private base_class
{
};

// Class to test scoping.
class easyclass 
{
	int priv_x;
	priv_func();

protected:
	int prot_x;
	prot_func();

public:
	int pub_x;
	pub_func();

private:
	int priv2_x;
	int priv2_func();
};

// Class to test overrides of easyclass
class newclass : public easyclass
{
	int priv_x;
	priv_func();

protected:
	int prot_x;
	prot_func();

public:
	int pub_x;
	pub_func();

private:
	int priv2_x;
	int priv2_func();
};

// Class that declares virtual functions.
class vbase
{
	virtual priv_vfunc();

protected:
	virtual prot_vfunc();

public:
    virtual ~vbase();
	virtual pub_vfunc();


};

// Class that overrides virtuals in vbase.
class newvclass : public vbase
{
	priv_vfunc();

protected:
	prot_vfunc();

public:
	pub_vfunc();
};

// Virtual class.
class virtclass : virtual public base_class
{
};

// Multiply inherited class.
class mclass: public base1, virtual private base2
{
};

// Easy member function test.
class class1
{
public:
    class1() {};
    ~class1() {};

	virtual int vfunc();
};

// Test default paramter in member function.
class class2
{
	int def_func(int i = 0);
};

// More complicated default parameter.  No variable name.
class class3
{
	int def_func(CObject* = 0);
};

// Nested class.
class class4
{
	int outer_x;

	class nested_class
	{
		int nexted_x;
	public:
		int nested_func();
	};

	int outer_func();
};

// Nested class with a var of same name as outer class.
class class5
{
	int x;
	class nested_class
	{
		int x;
	};
};

// Inline function.
class class6
{
	int m_x;

	int inline_func(int i) { m_x = i; }
};

// Abstract function, with default paramter.
class class7
{
public:
	virtual int abstract_func(int x = 0) = 0;
};

// Operator overrides.
class class8
{
public:
	class8& operator=(int) {};
	class8& operator+(int) {};
	class8& operator-(int) {};
	class8& operator>>(istream&) {};
	class8& operator<<(istream&) {};
	int operator==(class8&) {};
	char operator[](int index)const {}
	int operator()() {}

	operator char*() const {}
	operator const char&() const {}
	operator LPCTSTR() const {}
	operator const LPSTR() {}
	operator int() {};
};

// Friend class declaration.
class class9
{
	friend class class8;
};

// Operator override as friend function.
class class10
{
	friend class10& operator<<(int y);
};

// Pointer parameter and pointer return.
class class11
{
	class11 * func(class11 *);
};

// Consts everywhere.
class class13
{
	const class13* const func(int x) const;
};

// Const'd abstract function with default paramter.
class class14
{
	virtual const class13* const func(int = 0) const = 0;
};

// Static functions and pointers to functions as arguments.
class class15
{
	static sfunc();
	static sfunc2(int);
	int func(int (*pf)(const char *));
	virtual int func(char*, int (*pf)(int x)) = 0;
};

// Overloaded functions.
class class16
{
	int func(int);
	int func(BOOL);         
	int func(char);
	int func(unsigned);
	int func(unsigned char);
};

// Declare instances of classes after defining class.
class class17
{
public:
	int func() {}
} instance1, instance2;

// Array member.
class class18
{
	int* x[10][20][30];
};

// Test function that accepts a void paramter with an implementation
// that does not spell out the "void".  See the cpp file.  This is 
// a navigation only test.
class class19
{
	void func(void);
};

// Test something valid that comes between class keyword and class name.
class __declspec(dllimport) class20
{
};

// Test a pointer to function members.
class class21
{
	int (*m_pfunc)(int x, int y);
	int (*m_apfunc[10][20])(int x);
};

// Test a class with bit fields.
class class22
{
	WORD bOn:1;
	WORD bSys:1;
	WORD bSrc:1;
	WORD bRev:1;
	WORD bUpd:1;
	WORD index:5;
};

// Test const/non-const overload.
class class23
{
	int func() const;
	int func();
};

// Test a template declaration.
template<class T>
inline T& MAX(T& a, T& b)
{ return a > b? a : b; }


// Another template test.
template<class T, int size>
class Buffer1
{
private:
	T buffer[size];
};

template<class T>
class Buffer2 : public Buffer<T, 10>
{
private:
	T GetItem() {};
};

// Instaniated template test.
class class24
{
	Buffer<char, 5> m_CharBuf;
	Buffer<class23*, 100> m_Class23Buf;	// Buffer of class23 pointers.
	Buffer<int(*)(int), 5> m_pFuncBuf;	// Buffer of function pointers to functions that take and int and return an int.
	Buffer<(1>2),2> m_boolBuf;			// Buffer of booleans.
	CArray<Buffer, Buffer> m_BufArray;		// MFC defined template.
	Buffer<CArray<Buffer, Buffer>, 100> m_ArrayBuf;		// Buffer of CArray<Buffer>s
	Buffer<CArray<Buffer, Buffer>*, 100> m_ArrayPtrBuf;	// Buffer of CArray<Buffer> pointers.

	int m_x;
};

typedef struct mystruct mystruct;
class myclass;
typedef union myunion myunion;

// Test extraneous struct/class/union specifiers.
class class25
{
	foo(mystruct* p) {};
	foo(myclass& c) {};
	foo(myunion* * pp) {};

	struct mystruct m_struct;
	class myclass m_class;
	union myunion m_union;
};

// Check a class that is missing a trailing ';' 
class invalid
{
	int func();
}

// Declare a global function that unmangled naming.
extern "C" int C_Func(int);

// Declare a block of global functions that use unmangled naming.
extern "C" 
{
	int C_Func1(int);
	int C_Func2();
}

// Declare a global function that has no implementation yet.  Should not appear.
int prototype_only(int, char);

// Declare a Globals class to intentionally attempt to break the class view/parser.
class Globals
{
	int m_x;
	int foo() {};
};

// Try some wierd characters.  These break Symantec.
class Wierd$Name
{
	int m_$x;
};

// Try a class defined within a name space.	 Should not appear.
namespace NewSpace 
{
	class NamespacedClass
	{
		int foo();
		int m_x;
	};
}

// Try a class with forward declarations.
class class26
{
	class forward_class;
};

// Define a class to try out odd member definitions
class class27
{
	int func(char, int, long);
};

//;DO ALL BY REPLACING class WITH struct.  They should all work.
//;
//;ERRONEOUS CLASS DECLARATIONS
//;
class errorclass1
{
	int x
	func()
};

class errorclass2
{
}

// Try a class that has an internal unnamed union.
#define X LONG
#define Y LONG
#define DX LONG
#define DY LONG

struct PT { X x; Y y; };

struct RC
{
	union 
	{
		struct 
		{
			X xLeft;
			Y yTop;
			X xRight;
			Y yBot;
		};
		struct
		{
			PT ptTopLeft;
			PT ptBotRight;
		};
	};
		PT rgpt[2];
		RECT rect;

		DX width(void)	const {return xRight-xLeft;}
		DY height(void) const {return yBot-yTop;}
};

// How bout some good ol' OLE.
class COLEClass : public CCmdTarget
{
	DECLARE_DYNCREATE(COLEClass)
	COLEClass() {};

// Attributes
public:
	static UINT c_nTypeID;
	static LPCOMPONENTGALLERY c_lpGallery;
	static HRESULT AddComponent(CRuntimeClass* pClass) {}

// Implementation
protected:
	virtual ~COLEClass() {};

	DECLARE_OLECREATE_EX(CCawUtilType)

	// Interface for Component Gallery to use
	BEGIN_INTERFACE_PART(Type, IComponentType)
		INIT_INTERFACE_PART(CCawUtilType, Type)
		STDMETHOD(OnRegister)(THIS_ UINT nTypeID, LPCOMPONENTGALLERY lpcg);
		STDMETHOD(OnInitialRegister)(THIS);
		STDMETHOD(BltTypeGlyph)(THIS_ HDC hDC, int x, int y);
	END_INTERFACE_PART(Type)
	DECLARE_INTERFACE_MAP()

	DECLARE_MESSAGE_MAP()
};


