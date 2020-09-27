#include "manyclas.h"

easyclass::priv_func()
{
	return priv_x = 1;
}

easyclass::prot_func()
{
	return prot_x = 2;
}

easyclass::pub_func()
{
	return pub_x = 3;
}

easyclass::priv2_func()
{
	return priv2_x = 4;
}

newclass::priv_func()
{
	return priv_x = 1;
}

newclass::prot_func()
{
	return prot_x = 2;
}

newclass::pub_func()
{
	return pub_x = 3;
}

newclass::priv2_func()
{
	return priv2_x = 4;
}

vbase::priv_vfunc()
{
	return 0;
}

vbase::prot_vfunc()
{
	return 1;
}

vbase::pub_vfunc()
{
	return 2;
}

newvclass::priv_vfunc()
{
	vbase::priv_vfunc();
}

newvclass::prot_vfunc()
{
	vbase::prot_vfunc();
}

newvclass::pub_vfunc()
{
	vbase::pub_vfunc();
}

class1::vfunc()
{
	return 0;
}

class2::def_func(int y)
{
	return y;
}

class3::def_func(CObject* pObj)
{
	return 0;
}

class4::outer_func()
{
	return outer_x;
}

class4::nested_class::nested_func()
{
	return nested_x;
}

class7::abstract_func(int x)	// Def of abstract function! 
{
	return 0;
}

class8& class8::operator+(int x)
{
	return this;
}

class11* class11::func(class11* pObj)
{
	return pObj;
}

const class13* const class13::func(int x) const
{
	return this;
}

const class13* const class14::func(int y) const	// Definition of abstract func!
{
	return (class13*)this;
}

int class15::sfunc()
{
	return 0;
}

int class15::sfunc2(int x)
{
	return x;
}

int class15::func(int (*pf)(const char *))
{
	return class15::sfunc;
}

int class15::func(char*, int (*pf)(int x))
{
	return class15::sfunc2;
}

int class16::func(int)
{
	return 0;
}

int class16::func(BOOL)
{
	return 0;
}

int class16::func(char)
{
	return 0;
}

int class16::func(unsigned)
{
	return 0;
}

int class16::func(unsigned char)
{
	return 0;
}

void class19::func()
{
	return;
}

int class23::func()
{
	return 0;
}

int class23::func() const
{
	return 0;
}

int class27::func(
				  char,
				  int,
				  long
				  )
{
	return 0;
}

int errorclass1::func()
{
	return 0;
}

int invalid::func()
{
	return 0;
}

int C_Func(int)
{
	return 0;
}

int C_Func1(int)
{
}

int C_Func2()
{
}

COLEClass::c_nTypeID = 77777;
COLEClass::c_lpGallery = (LPCOMPONENTGALLERY)0x999999;
STDMETHODIMP COLEClass::XType::OnRegister(UINT nTypeID, LPCOMPONENTGALLERY lpcg)
{
	return NOERROR;
}

STDMETHODIMP COLEClass::XType::OnInitialRegister()
{
	return NOERROR;
}

STDMETHODIMP COLEClass::XType::BltTypeGlyph(HDC hDC, int x, int y)
{
	return NOERROR;
}

STDMETHODIMP_(ULONG) COLEClass::XType::AddRef()
{
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) COLEClass::XType::Release()
{
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP COLEClass::XType::QueryInterface(
	REFIID iid, LPVOID far * ppvObj)
{
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}
