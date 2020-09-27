
#include <windows.h>
#include <comcat.h>
#include <ocidl.h>
#include <urlmon.h>
#include <ACTIVSCP.H>
#include <OBJSAFE.H>
// test VC5.0 EE failures, so we can test 6.0 and 7.0
// and also new features

typedef void (*PFNINT)(void*, unsigned long);
typedef int (*PFNVAR)(void*, ...);

void afuncint(void*, DWORD)
{
}

int afuncvar(void*, ...)
{
	return 0;
}

int afunvar1(char*, ...)
{
	return 1;
}

int afunvar2(int*, int, ...)
{
	return 2;
}

class AClass
{
public:
	PFNINT pfnint;
	PFNVAR pfnvar;
protected:
	virtual void __cdecl    virt_method1() {};
	virtual void __fastcall virt_method2(long, ...) {};
};

PFNINT fun_return_pfn()
{
	return afuncint;
}

void pointers_to_functions()
{
	AClass aclass;
	AClass *pclass=&aclass;
	AClass class_table[1];

	PFNINT pfunint_table[1];
	PFNVAR pfunvar;

//pointers to functions
	pfunint_table[0] = afuncint; //eval("pfunint_table[0]")=="ADDRESS afuncint(void*, unsigned int)"
	pfunvar = (PFNVAR)afunvar1; //eval("pfunvar")=="ADDRESS afunvar1(char*, <no type>)"

//pointers to functions as members of class
	aclass.pfnint = afuncint; //eval("pclass->pfnint")=="ADDRESS afuncint(void*, unsigned int)"
	aclass.pfnvar = afuncvar; //eval("pclass->pfnvar")=="ADDRESS afuncvar(void*, <No type>)"
	aclass.pfnvar = (PFNVAR)afunvar1;//eval("pclass->pfnvar")=="ADDRESS afunvar1(char*, <no type>)"
	aclass.pfnvar = (PFNVAR)afunvar2;//eval("pclass->pfnvar")=="ADDRESS afunvar2(int*, int, <no type>)"

	(*aclass.pfnint)(NULL,1000);
	int ret=1;
	ret=(*aclass.pfnvar)(&ret,15,&ret);
//vtable pointers correct
	//eval("(class_table[0]).__vfptr[0]")=="ADDRESS AClass::virt_method1"
	//eval("(class_table[0]).__vfptr[1]")=="ADDRESS AClass::virt_method2"
	memset(class_table,0, sizeof(class_table)); //evaluate the NULL pointers (including virtual pointers)
	class_table[0] = aclass; //eval("class_table[0].pfnint")=="ADDRESS afuncint(void*, unsigned int)"
							//eval("class_table[0].pfnvar")=="ADDRESS afuncvar2(int*, int, <no type>)"
//TODO add the invocation of each function through all pointers and check the result
//	(*pfunc)(); //just like that

//vtable pointers
	//eval("(aclass).__vfptr[0]")=="ADDRESS AClass::virt_method1"
	//eval("(aclass).__vfptr[1]")=="ADDRESS AClass::virt_method2"
//vtable pointers corrupted by memset
	//eval("(class_table[0]).__vfptr[0]")=="Error: cannot display value"
	//eval("(class_table[0]).__vfptr[1]")=="Error: cannot display value"
	fun_return_pfn();
}


REFIID return_Unknown()
{
	REFIID unknown=IID_IUnknown;
	return unknown;
}

GUID return_GUIDUnknown()
{
	GUID guidunknown=IID_IUnknown;
	return guidunknown;
}

VARIANT return_variant()
{
	VARIANT variant;
	variant.vt=VT_I4;			//LONG
	variant.lVal = 0xffff;

	return variant;
}

const GUID unKnownGuid = {0xf0ff0ff0, 0xff0f, 0xff0f, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00}; //global, misc. guid
GUID Global_Guid;
REFIID glob_refiid0=IID_IMarshal;
REFIID glob_refiid1=IID_IMalloc;
void guids_and_refguids()
{
//non-static, local
	GUID Guid =IID_IClassFactory;
//global
	Global_Guid =IID_IClassFactory2;
//pointers to GUIDs
	GUID const *pGuid = &Guid;
	GUID *pBadGuid = NULL; //following NULL pointer
	pBadGuid = (GUID *)5;	//following AV
	REFIID pBadrefId = *pBadGuid;	//following AV
//static, local GUID
	static GUID unKnown = unKnownGuid;

	pGuid =&CLSID_FileProtocol;
	pGuid =&CLSID_FtpProtocol;
	Guid =CLSID_GopherProtocol;
	Guid =CLSID_HttpProtocol;

	CATID pCAT= CATID_Control;
	pCAT = CATID_DocObject;


	static REFIID refiid0 =IID_IUnknown;
	static REFIID refiid1 =IID_IDispatch;
	static REFIID refiid2 =IID_IMoniker;
	static REFIID refiid3 =IID_IEnumString;
	static REFIID refiid4 =IID_IStream;
	REFIID refiid5 =IID_IStorage;
	REFIID refiid6 =IID_IPersistStorage;
	REFIID refiid7 =IID_IPersist;
	REFIID refiid8 =IID_IActiveScript;
	REFIID refiid9 =IID_IObjectSafety;
//global REFIIDs impossible to init here, but they shld be checked here
//	REFIID glob_refiid0=IID_IMarshal;
//	REFIID glob_refiid1=IID_IMalloc;

	return_Unknown();
	return_GUIDUnknown();
}


VARIANT glob_variant; //declaration of global VARIANT
void variants()
{
//static local VARIANT
	static VARIANT variant1;

		variant1.vt = ~VT_BYREF|VT_UI1; //non-existent
		::VariantInit(&variant1); //VT_EMPTY
		variant1.vt = VT_I4;			//LONG
		variant1.lVal = 1052;
		
		variant1.vt = VT_UI1;		//BYTE
		variant1.bVal = 255;

		variant1.vt = VT_I2;			//SHORT
		variant1.iVal = 32000;

		variant1.vt = VT_R4;			//FLOAT
		variant1.fltVal = 20.00;

		variant1.vt = VT_R8;			//DOUBLE
		variant1.dblVal = 3.14e100;

		variant1.vt = VT_BOOL;		//VARIANT_BOOL
		variant1.boolVal = TRUE;

		variant1.vt = VT_ERROR;		//SCODE
		variant1.scode = E_FAIL; //variant.scode,hr=="E_FAIL", but variant=="{0x80004005 VT_ERROR}"

		variant1.vt = VT_CY;			//CY
		variant1.cyVal.Lo = 0xffffffff;variant1.cyVal.Hi = 0x7fffffff;	//variant=="{922337203685477.5807 VT_CY}"

		variant1.vt = VT_DATE;		//DATE //variant=="{???}"
		variant1.date = 35065.000000000; //COleDateTime(1996,1,1,0,0,0); == "{1/1/96 12:00:00 AM VT_DATE}"

//non-static local VARIANT
		VARIANT variant2;
		::VariantInit(&variant2);
		variant2.vt = VT_BSTR;		//BSTR
		WCHAR szW[100];MultiByteToWideChar(CP_ACP, 0, "BSTR test", -1, szW, 100);
		variant2.bstrVal = ::SysAllocString(szW);
		::SysFreeString(variant2.bstrVal);

		variant2.vt = VT_UNKNOWN;	//IUnknown*
		variant2.lVal = 255; //punkVal: see if bogus value works here

		variant2.vt = VT_DISPATCH;	//IDispatch*
		variant2.lVal = 255; //ppdispVal see if bogus value works here

		variant2.vt = VT_ARRAY;		//SAFEARRAY*
		variant2.lVal = 255; //pparray

//global VARIANT
		::VariantInit(&glob_variant);
		glob_variant.vt = VT_UI1|VT_BYREF;//BYTE*
		BYTE pbVal=255;glob_variant.pbVal = &pbVal; 

		glob_variant.vt = VT_I2|VT_BYREF;//SHORT*
		SHORT piVal=1000;glob_variant.piVal = &piVal;

		glob_variant.vt = VT_I4|VT_BYREF;//LONG*
		LONG plVal=1000000;glob_variant.plVal = &plVal;

		glob_variant.vt = VT_R4|VT_BYREF;//FLOAT*
		FLOAT pfltVal=20.00;glob_variant.pfltVal = &pfltVal;

		glob_variant.vt = VT_R8|VT_BYREF;//DOUBLE*
		DOUBLE pdblVal=10;glob_variant.pdblVal = &pdblVal;

		glob_variant.vt = VT_BOOL|VT_BYREF;//VARIANT_BOOL*
		VARIANT_BOOL pboolVal=FALSE;glob_variant.pboolVal = &pboolVal;

		glob_variant.vt = VT_ERROR|VT_BYREF;//SCODE*
		SCODE pscode=E_NOTIMPL; glob_variant.pscode = &pscode;

		glob_variant.vt = VT_CY|VT_BYREF;//CY*
		CY pcyVal;pcyVal.int64=0x7fffffffffffffffUL+1;glob_variant.pcyVal = &pcyVal; //variant=="{-922337203685477.5808 VT_CY|VT_BYREF}"

		glob_variant.vt = VT_DATE|VT_BYREF;//DATE*
		DATE pdate=35642.952071759/*COleDateTime(1997,07,31,22,50,59)*/;glob_variant.pdate = &pdate; //=="{7/31/97 10:50:59 PM VT_DATE|VT_BYREF}"

		glob_variant.vt = VT_UNKNOWN|VT_BYREF;//IUnknown **
		glob_variant.lVal = 5; //ppunkVal

		glob_variant.vt = VT_DISPATCH|VT_BYREF;//IDispatch **
		glob_variant.lVal = 5; //ppdispVal

		glob_variant.vt = VT_ARRAY|VT_BYREF;//SAFEARRAY **
		glob_variant.lVal = 5; //pparray

		glob_variant.vt = VT_VARIANT|VT_BYREF;//VARIANT *
		glob_variant.pvarVal = &glob_variant; //circular reference
		glob_variant.pvarVal = &variant2; //non-static,local variant referenced

		glob_variant.vt = VT_BSTR|VT_BYREF;//BSTR*
		//WCHAR szW[100];
		MultiByteToWideChar(CP_ACP, 0, "PBST test", -1, szW, 100);
		BSTR pbstrVal=::SysAllocString(szW);glob_variant.pbstrVal = &pbstrVal;
		SysFreeString(pbstrVal);

//subvariant - non-statc, local variant
		VARIANT subvariant;
		::VariantInit(&subvariant);
		subvariant.vt = VT_I4;
		subvariant.lVal = 10;
		variant2.vt = VT_BYREF;		//PVOID
		void* anyref=&subvariant;variant2.byref = &subvariant;

		variant2.vt = VT_I1;			//CHAR
		variant2.cVal = 'V';

		variant2.vt = VT_UI2;		//USHORT
		variant2.uiVal = 0xffff;

		variant2.vt = VT_UI4;		//ULONG
		variant2.ulVal = 0xffffffff;

		variant2.vt = VT_INT;		//INT
		variant2.intVal = -256;

		variant2.vt = VT_UINT;		//UINT
		variant2.uintVal = 256;

		variant2.vt = VT_DECIMAL|VT_BYREF;//DECIMAL *
		DECIMAL dec; memset(&dec, 0, sizeof(dec)); dec.Hi32=1; dec.scale=28; variant2.pdecVal = &dec;

		variant2.vt = VT_I1|VT_BYREF;//CHAR *
		variant2.pcVal = "char string test";

		variant2.vt = VT_UI2|VT_BYREF;//USHORT *
		variant2.puiVal = (USHORT*)&variant2; //bogus

		variant2.vt = VT_UI4|VT_BYREF;//ULONG *
		variant2.pulVal = (ULONG *)5; //error test

		variant2.vt = VT_INT|VT_BYREF;//INT *
		variant2.pintVal = NULL; //error test

		variant2.vt = VT_UINT|VT_BYREF;//UINT *
		variant2.puintVal = (UINT *)255; //still error

		return_variant();
}


BOOL MMX_Chip()
{
#pragma warning(disable:4035)
	__asm{
		mov eax,1;
		_emit 0x0f; //CPUID
		_emit 0xa2; //CPUID
		mov eax,1;
		test edx,0x0800000;
		jnz Yes
		mov eax,0
Yes:
	}
	return;
#pragma warning(default:4035)
} //MMX_Chip()

#pragma warning(disable:4799)
void MMXRegs()
{
	__int64 qmemory0=
		1+1*0x100000000;
	__int64 qmemory1=
		2+2*0x100000000;
	__int64 qmemory2=
		4+4*0x100000000;
	__int64 qmemory3=
		8+8*0x100000000;
	__int64 qmemory4=
		16+16*0x100000000;
	__int64 qmemory5=
		32+32*0x100000000;
	__int64 qmemory6=
		64+64*0x100000000;
	__int64 qmemory7=
		128+128*0x100000000;
if(MMX_Chip())
{
	int mm0=-10,Mm0=-20,mM0=-30;
	int mm1=-11,Mm1=-21,mM1=-31;
	int mm2=-12,Mm2=-22,mM2=-32;
	int mm3=-13,Mm3=-23,mM3=-33;
	__asm{
		movd mm0,ebx; //long(mm0)==ebx
		movd mm1,edx; //long(mm1)==edx
		movd mm2,esp; //long(mm2)==esp
		movd mm3,ebp; //long(mm3)==ebp

		movq mm0,qmemory0; //mm0==qmemory0
		movq mm1,qmemory1; //...
		movq mm2,qmemory2;
		movq mm3,qmemory3;
		movq mm4,qmemory4;
		movq mm5,qmemory5;
		movq mm6,qmemory6;
		movq mm7,qmemory7;
//the following sequence shldn't change the low words of mm0..mm7
		movd mm0,qmemory0; //mm0==long(qmemory0)
		movd mm1,qmemory1; //...
		movd mm2,qmemory2;
		movd mm3,qmemory3;
		movd mm4,qmemory4;
		movd mm5,qmemory5;
		movd mm6,qmemory6;
		movd mm7,qmemory7;

		movd eax,mm7; //eax==long(mm7)
//change mm0..mm7 manually
		movq qmemory0,mm0;
		movq qmemory1,mm1;
		movq qmemory2,mm2;
		movq qmemory3,mm3;
		movq qmemory4,mm4;
		movq qmemory5,mm5;
		movq qmemory6,mm6;
		movq qmemory7,mm7;
//check if qmemory[0..7] is what you've changed
//TODO some arithmetics (pand paad psub)
	} //__asm

} //if
} //MMXRegs()
#pragma warning(default:4799)

int main(int argc, char *argv[])
{
	pointers_to_functions();
	
	guids_and_refguids();
	
	variants();

	MMXRegs();
//TODO add sth with safearray
//TODO come up with more values (different facilities) & unknown valuses (bad values do not apply)
//hresults & errors & misc (@TIB, @MM)
	HRESULT s_ok = S_OK;

	HRESULT s_err = E_NOINTERFACE;

	HRESULT s_unexp = E_UNEXPECTED;

	HRESULT s_eomem = E_OUTOFMEMORY;

	SetLastError(42);


	_asm int 3;					// stop here
	return 0;
}

//	Eval					Expected			VC5.0 did
//	pfunc					ADDR func			ADDR
//	(aclass).__vfptr[0x0]	ADDR AClass::f1		ADDR
//	@ERR					42					N/A
//	@TIB					ADDR				N/A
//	@MM0					ADDR64				N/A
//	refiid					{IID_IUnknown}		{...}
//	pGuid					{IID_IClassFactory}	{...}
//	variant					{42 VT_I4}			{...}
//	s_ok,hr					S_OK				N/A
//	5,hr					0x00000005 Access is denied.	N/A


/* GLS script
launch ee2\debug\ee2.exe
until EntryPointEvent2 wait
go
until CodeBreakpointEvent2 wait
seval "pfunc","ADDRESS func"
seval "(aclass).__vfptr[0x0]","ADDRESS AClass::f1"
#seval "@ERR","42"
#seval "@TIB,x","ADDRESS "
#seval "@MM0","ADDRESS64 "
seval "refiid","{IID_IUnknown}"
seval "pGuid","{IID_IClassFactory}"
seval "variant","{42 VT_I4}"
seval "s_ok,hr","S_OK"
seval "5,hr","0x00000005 Access is denied. "
seval "pBadGuid","ADDRESS {????????-????-????-????-????????????}"
go
until SessionDestroyEvent2 wait
*/