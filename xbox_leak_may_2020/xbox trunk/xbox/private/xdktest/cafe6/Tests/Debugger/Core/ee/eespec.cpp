///////////////////////////////////////////////////////////////////////////////
//	EECASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "eespec.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CSpecialTypesTest, CDbgTestBase, "PtFns VARIANTs GUIDs HRs Errs MMX", -1, CEESubSuite)
									   
												 
void CSpecialTypesTest::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CSpecialTypesTest::Run(void)	
{
	/******************
	 * INITIALIZATION * non-incremental - just for PointerToFunctions
	 ******************/
#pragma message (__LOC__ "Revise incremental & add other switches when VC#8157 fixed")
	if(!SetProject("ee2\\ee2"))
	{
		m_pLog->RecordInfo("ERROR: could not init non-incremental ee2 project");
		return;
	}

	// step into the main function
	dbg.StepInto();	
	dbg.SetSteppingMode(SRC);

	EXPECT_TRUE( src.AttachActiveEditor() );
	cxx.Enable();
	/******************************************
	 * PointerToFunc on non-incremental build *
	 ******************************************/
#pragma message (__LOC__ "Disabled PointerToFun, because VC98#8157 is postponed")
//	PointerToFun();

	/******************
	 * INITIALIZATION *
	 ******************/
	if(!SetProject("ee2\\ee2",PROJECT_EXE,fDefault,"INCREMENTAL"))
	{
		m_pLog->RecordInfo("ERROR: could not init incremental ee2 project");
		return;
	}

	// step into the main function
	dbg.StepInto();	
	dbg.SetSteppingMode(SRC);

	EXPECT_TRUE( src.AttachActiveEditor() );
	cxx.Enable();

	EXPECT_TRUE(cxx.GetExpressionValue("@TIB", m_TIBValue) );
		
	/*********
	 * TESTS *
	 *********/
#pragma message (__LOC__ "Disabled PointerToFun, because VC98#8157 is postponed")
//	PointerToFun();
	XSAFETY;
	GUIDs();
	XSAFETY;
	Variants();
	XSAFETY;
	MMXRegs();
	XSAFETY;
	ErrorCodes();

	StopDbgCloseProject();
}

//PointerToFun
void CSpecialTypesTest::PointerToFun(void)
{
 	LogTestHeader( "PointerToFunctions Test" );

	EXPECT_TRUE(bps.SetBreakpoint("pointers_to_functions") );
	EXPECT_TRUE(dbg.Go("pointers_to_functions") );
//pointers to functions
	EXPECT_TRUE( src.Find("aclass.pfnint = afuncint;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "aclass.pfnint = afuncint;") );

	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pfunint_table[0]", "ADDRESS afuncint(void *,unsigned long)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pfunvar", "ADDRESS afunvar1(char *, <no type>)") );

	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("afuncint", "ADDRESS afuncint(void *, unsigned long)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("afunvar1", "ADDRESS afunvar1(char *, <no type>)") );

//pointers to functions as members of class
	EXPECT_TRUE(dbg.CurrentLineIs("aclass.pfnint = afuncint;") );
	EXPECT_TRUE(dbg.StepOver() );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pclass->pfnint", "ADDRESS afuncint(void *, unsigned long)") );

	EXPECT_TRUE(dbg.CurrentLineIs("aclass.pfnvar = afuncvar;") );
	EXPECT_TRUE(dbg.StepOver() );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pclass->pfnvar", "ADDRESS afuncvar(void *, <no type>)") );

	EXPECT_TRUE(dbg.CurrentLineIs("aclass.pfnvar = (PFNVAR)afunvar1;") );
	EXPECT_TRUE(dbg.StepOver() );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pclass->pfnvar", "ADDRESS afunvar1(char *, <no type>)") );

	EXPECT_TRUE(dbg.CurrentLineIs("aclass.pfnvar = (PFNVAR)afunvar2;") );
	EXPECT_TRUE(dbg.StepOver() );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("pclass->pfnvar", "ADDRESS afunvar2(int *, int, <no type>)") );
//stepping into the class member - pointer to a function
	EXPECT_TRUE( src.Find("ret=(*aclass.pfnvar)(&ret,15,&ret);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL) );
	EXPECT_TRUE( dbg.StepInto( ) );
	EXPECT_TRUE( dbg.AtSymbol("afunvar2") );

//chaek if vtable pointers are correct
	EXPECT_TRUE( src.Find("memset(class_table,0, sizeof(class_table));") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "memset(class_table,0, sizeof(class_table));") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("(class_table[0]).__vfptr[0]", "ADDRESS AClass::virt_method1(void)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("(class_table[0]).__vfptr[1]", "ADDRESS AClass::virt_method2(long, <no type>)") );
	//NULLify all pointers by executing memset(0,...)
	EXPECT_TRUE( dbg.StepOver( ) );
	EXPECT_TRUE( cxx.ExpressionErrorIs("(class_table[0]).__vfptr[0]", "Error: cannot display value") );
	EXPECT_TRUE( cxx.ExpressionErrorIs("(class_table[0]).__vfptr[1]", "Error: cannot display value") );
	//assign the object from a class
	EXPECT_TRUE(dbg.CurrentLineIs("class_table[0] = aclass;") );
	EXPECT_TRUE( dbg.StepOver( ) );
//class assignment does override function pointers but does not override vtable
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("(aclass).__vfptr[0]", "ADDRESS AClass::virt_method1(void)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("(aclass).__vfptr[1]", "ADDRESS AClass::virt_method2(long, <no type>)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("class_table[0].pfnint", "ADDRESS afuncint(void *, unsigned long)") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("class_table[0].pfnvar", "ADDRESS afunvar2(int *, int, <no type>)") );
	EXPECT_TRUE(cxx.ExpressionErrorIs("(class_table[0]).__vfptr[0]", "Error: cannot display value") );
	EXPECT_TRUE(cxx.ExpressionErrorIs("(class_table[0]).__vfptr[1]", "Error: cannot display value") );
//function which returns a pointer to a function
	EXPECT_TRUE(bps.SetBreakpoint("fun_return_pfn") );
	EXPECT_TRUE(dbg.Go("fun_return_pfn") );
	EXPECT_TRUE(dbg.StepOut() );
//verify in autopane the string "fun_return_pfn returned"	"ADDRESS afuncint(void *, unsigned long)"
	uivar.Activate();
	ERROR_SUCCESS(uivar.SetPane(PANE_AUTO) );
	EXPR_INFO expr_info_actual[1], expr_info_expected[1];
	
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));

	
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, 
		"void (void *, unsigned long)*",
		"fun_return_pfn returned",
		"ADDRESS afuncint(void *, unsigned long)");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1) );
	EXPECT_TRUE(dbg.ShowNextStatement() ); //uivar.Activate() steals the focus
	
	bps.ClearAllBreakpoints();
}//PointerToFun


//GUIDs
void CSpecialTypesTest::GUIDs(void)
{
 	LogTestHeader( "GUIDs Test" );
	
	char badGUIDstring[40]; //{????????-????-????-????-????????????}
	memset(badGUIDstring,'?',sizeof(badGUIDstring));
	badGUIDstring[0]='{';badGUIDstring[37]='}';badGUIDstring[38]=0;
	badGUIDstring[9]=badGUIDstring[14]=badGUIDstring[19]=badGUIDstring[24]='-';
	
	EXPECT_TRUE(bps.SetBreakpoint("guids_and_refguids") );
	EXPECT_TRUE(dbg.Go("guids_and_refguids") );
//check some global guids initialization
	EXPECT_TRUE( cxx.ExpressionValueIs("Global_Guid", "{00000000-0000-0000-0000-000000000000}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("unKnownGuid", "{F0FF0FF0-FF0F-FF0F-FF00-FF00FF00FF00}") );

//check all value (including locals) at the following checkpoint	
	EXPECT_TRUE( src.Find("pGuid =&CLSID_FileProtocol;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, /*"pGuid*/"d =&CLSID_FileProtocol;") );
	MST.DoKeys("{HOME}"); //get out of the assignment

	EXPECT_TRUE( cxx.ExpressionValueIs("Guid", "{IID_IClassFactory}") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global_Guid", "{IID_IClassFactory2}") );
	EXPECT_TRUE( cxx.ExpressionValueSubstringIs("pGuid", "ADDRESS {IID_IClassFactory}") );
	EXPECT_TRUE(cxx.ExpressionErrorIs("pBadGuid", CString("0x00000005 ") + badGUIDstring) );
	EXPECT_TRUE(cxx.ExpressionErrorIs("pBadrefId", badGUIDstring) );
	EXPECT_TRUE( cxx.ExpressionValueIs("unKnown", "{F0FF0FF0-FF0F-FF0F-FF00-FF00FF00FF00}") );

//check all REFIIDs after the checkpoint was executed
	EXPECT_TRUE( src.Find("return_Unknown();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "return_Unknown();") );
static char* data[][2] = {
	"refiid0",		"{IID_IUnknown}",
	"refiid1",		"{IID_IDispatch}",
	"refiid2",		"{IID_IMoniker}",
	"refiid3",		"{IID_IEnumString}",
	"refiid4",		"{IID_IStream}",
	"refiid5",		"{IID_IStorage}",
	"refiid6",		"{IID_IPersistStorage}",
	"refiid7",		"{IID_IPersist}",
	"refiid8",		"{IID_IActiveScript}",
	"refiid9",		"{IID_IObjectSafety}",
};
	for(int i = 0; i < (sizeof(data) / (sizeof(CString) * 2)); i++)
	{
		LOG->RecordCompare(cxx.ExpressionValueIs(data[i][0], data[i][1]),
			"Expression:(%s), expected:(%s)\n", data[i][0],data[i][1]);
	}

	uivar.Activate();
	ERROR_SUCCESS(uivar.SetPane(PANE_AUTO) );
	EXPR_INFO expr_info_actual[1], expr_info_expected[1];

	EXPECT_TRUE( dbg.StepOver() );
//verify in autopane the string "const _GUID &" "return_Unknown returned"	"{IID_IUnknown}"
		
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, 
		"const _GUID &",
		"return_Unknown returned",
		"{IID_IUnknown}");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1) );

	EXPECT_TRUE( dbg.StepOver() );		
//verify in autopane the string "_GUID" "return_GUIDUnknown returned"	"{IID_IUnknown}"
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 1, ROW_ALL));
	
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, 
		"_GUID",
		"return_GUIDUnknown returned",
		"{IID_IUnknown}");
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1) );

	EXPECT_TRUE(dbg.ShowNextStatement() ); //uivar.Activate() steals the focus
	bps.ClearAllBreakpoints();
}//GUIDs


//VARIANTs
#include <wtypes.h> //for VT_BYREF define
void CSpecialTypesTest::Variants(void)
{
 	LogTestHeader( "VARIANTs Test" );
	char badVARIANTstring[6]; //{???}
	memset(badVARIANTstring,'?',sizeof(badVARIANTstring));
	badVARIANTstring[0]='{';badVARIANTstring[4]='}';badVARIANTstring[5]=0;

//check the variant1 before being initialized
	EXPECT_TRUE( src.Find("::VariantInit(&variant1);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "VariantInit(&variant1);") );
	EXPECT_TRUE( cxx.ExpressionValueIs("variant1", badVARIANTstring) );
	EXPECT_TRUE(dbg.StepOver() ); //initialize the variant

//	added by dverma 4/25/2000 to check date for Win NT
	char strDateVal[64];

	if (GetSystem() == SYSTEM_NT_4)	//	NT4.0
		sprintf(strDateVal,"{1/1/96 12:00:00 AM VT_DATE}");	
	else
		sprintf(strDateVal,"{1/1/1996 12:00:00 AM VT_DATE}");	
		
	//check the variant1 assigned different values
	static char* variant1data[][2] = { //"line of code identifying variant", "variant value" 
		"variant1.vt = VT_I4;",		"{1052 VT_I4}",
		"variant1.vt = VT_UI1;",	"{255 VT_UI1}",
		"variant1.vt = VT_I2;",		"{32000 VT_I2}",
		"variant1.vt = VT_R4;",		"{20.0000 VT_R4}",
		"variant1.vt = VT_R8;",		"{3.1400000000000e+100 VT_R8}",
		"variant1.vt = VT_BOOL;",	"{True VT_BOOL}",
		"variant1.vt = VT_ERROR;",	"{0x80004005 VT_ERROR}",
		"variant1.vt = VT_CY;",		"{922337203685477.5807 VT_CY}",
		"variant1.vt = VT_DATE;",	strDateVal
	};

	for(int i = 0; i < (sizeof(variant1data) / (sizeof(CString) * 2)); i++)
	{
		EXPECT_TRUE(dbg.CurrentLineIs(variant1data[i][0]) );
		dbg.StepOver(2);
		LOG->RecordCompare(cxx.ExpressionValueIs("variant1", variant1data[i][1]),
			"variant1 expected:(%s)\n",variant1data[i][1]);
	}
//check the variant2 assigned different stricte automation type values
	EXPECT_TRUE( src.Find("variant2.vt = VT_BSTR;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "variant2.vt = VT_BSTR;") );
	EXPECT_TRUE( dbg.StepOver() );
	
	//	modified by dverma 6/21/2000 - now variant shows string instead of badvalue
//	EXPECT_TRUE( cxx.ExpressionValueIs("variant2", badVARIANTstring) ); //VT_BSTR but pointing to bad address
	char tempstr[16];
	strcpy(tempstr,"{\"\" VT_BSTR}");	//	{"" VT_BSTR}
	EXPECT_TRUE( cxx.ExpressionValueIs("variant2", tempstr) ); //VT_BSTR

	EXPECT_TRUE( dbg.StepOverUntilLine("::SysFreeString(variant2.bstrVal);") );
	LOG->RecordCompare( cxx.ExpressionValueIs("variant2", "{\"BSTR test\" VT_BSTR}"),
		"variant2 shld be \"BSTR test\"");
	dbg.StepOver();

static char* variant2data[][2] = { //"line of code identifying variant", "variant value" 
	"variant2.vt = VT_UNKNOWN;",	"{0x000000ff VT_UNKNOWN}",
	"variant2.vt = VT_DISPATCH;",	"{0x000000ff VT_DISPATCH}",
	"variant2.vt = VT_ARRAY;",		"{0x000000ff VT_ARRAY}",
};
	for(i = 0; i < (sizeof(variant2data) / (sizeof(CString) * 2)); i++)
	{
		EXPECT_TRUE(dbg.CurrentLineIs(variant2data[i][0]) );
		dbg.StepOver(2);
		LOG->RecordCompare(cxx.ExpressionValueIs("variant2", variant2data[i][1]),
			"variant2 expected:(%s)\n",variant2data[i][1]);
	}
	dbg.StepOver(); //Init of global variant

	if (GetSystem() == SYSTEM_NT_4)	//	NT4.0
		sprintf(strDateVal,"{7/31/97 10:50:59 PM VT_DATE|VT_BYREF}");	
	else
		sprintf(strDateVal,"{7/31/1997 10:50:59 PM VT_DATE|VT_BYREF}");	

	//check the glob_variant assigned different VT_BYREF values
	static char* globvariantdata[][2] = { //"line of code identifying variant", "variant value" 
		"glob_variant.vt = VT_UI1|VT_BYREF;",		"{255 VT_UI1|VT_BYREF}",
		"glob_variant.vt = VT_I2|VT_BYREF;",		"{1000 VT_I2|VT_BYREF}",
		"glob_variant.vt = VT_I4|VT_BYREF;",		"{1000000 VT_I4|VT_BYREF}",
		"glob_variant.vt = VT_R4|VT_BYREF;",		"{20.0000 VT_R4|VT_BYREF}",
		"glob_variant.vt = VT_R8|VT_BYREF;",		"{10.000000000000 VT_R8|VT_BYREF}",
		"glob_variant.vt = VT_BOOL|VT_BYREF;",		"{False VT_BOOL|VT_BYREF}",
		"glob_variant.vt = VT_ERROR|VT_BYREF;",		"{0x80004001 VT_ERROR|VT_BYREF}",
		"glob_variant.vt = VT_CY|VT_BYREF;",		"{-922337203685477.5808 VT_CY|VT_BYREF}",
		"glob_variant.vt = VT_DATE|VT_BYREF;",		strDateVal,
		"glob_variant.vt = VT_UNKNOWN|VT_BYREF;",	badVARIANTstring, //pVal==5 (bad pointer)
		"glob_variant.vt = VT_DISPATCH|VT_BYREF;",	badVARIANTstring, //pVal==5 (bad pointer)
		"glob_variant.vt = VT_ARRAY|VT_BYREF;",		badVARIANTstring, //pVal==5 (bad pointer)
		"glob_variant.vt = VT_VARIANT|VT_BYREF;",	"{VT_VARIANT|VT_BYREF}",
	};


	for(i = 0; i < (sizeof(globvariantdata) / (sizeof(CString) * 2)); i++)
	{
		EXPECT_TRUE(dbg.CurrentLineIs(globvariantdata[i][0]) );
		dbg.StepOver(2);
		LOG->RecordCompare(cxx.ExpressionValueIs("glob_variant", globvariantdata[i][1]),
			"glob_variant expected:(%s)\n",globvariantdata[i][1]);
	}
	//the last variant references itself
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("glob_variant.pvarVal", "ADDRESS struct tagVARIANT  glob_variant") );
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("*glob_variant.pvarVal", "{VT_VARIANT|VT_BYREF}") );

	dbg.StepOver(); //glob_variant references variant2
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("glob_variant.pvarVal", "ADDRESS {0x000000ff VT_ARRAY}") );

//check reference to BSTR
	EXPECT_TRUE( src.Find("SysFreeString(pbstrVal);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "SysFreeString(pbstrVal);") );
	LOG->RecordCompare( cxx.ExpressionValueIs("glob_variant", "{\"PBST test\" VT_BSTR|VT_BYREF}"),
		"glob_variant shld be \"PBST test\"");
//check variant2 - non-statc, local variant
	EXPECT_TRUE( src.Find("void* anyref=&subvariant;variant2.byref = &subvariant;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "void* anyref=&subvariant;variant2.byref = &subvariant;") );
	EXPECT_TRUE(cxx.ExpressionValueIs("variant2", badVARIANTstring) ); //pointer is still bad (==0xff)
	dbg.StepOver(); 
	EXPECT_TRUE(cxx.ExpressionValueIs("variant2", badVARIANTstring) );//now the pointer is valid, pointing to subvariant, but "{VT_BYREF}" is wrong variant type, according to DOCs
	EXPECT_TRUE(cxx.ExpressionValueSubstringIs("(tagVARIANT*)variant2.byref", "ADDRESS {10 VT_I4}") );
//check simple values of variant2
static char strVT_UI2_VT_BYREF[50];
	strVT_UI2_VT_BYREF[0]='{';
	itoa(VT_UI2|VT_BYREF,strVT_UI2_VT_BYREF+1,10);
	strcat(strVT_UI2_VT_BYREF, " VT_UI2|VT_BYREF}"); //it will be a variant value pointing to itself 
static char* simpledata[][2] = { //"line of code identifying variant", "variant value" 
	"variant2.vt = VT_I1;",		"{86 'V' VT_I1}",
	"variant2.vt = VT_UI2;",	"{65535 VT_UI2}",
	"variant2.vt = VT_UI4;",	"{4294967295 VT_UI4}",
	"variant2.vt = VT_INT;",	"{-256 VT_INT}",
	"variant2.vt = VT_UINT;",	"{256 VT_UINT}",
	"variant2.vt = VT_DECIMAL|VT_BYREF;",	"{0.0000000018446744073709551616 VT_DECIMAL|VT_BYREF}",
	"variant2.vt = VT_I1|VT_BYREF;",		"{99 'c' VT_I1|VT_BYREF}", //display only 1 char as it's  marshalled
	"variant2.vt = VT_UI2|VT_BYREF;",		strVT_UI2_VT_BYREF, //bogus, pointing to itself,
	"variant2.vt = VT_UI4|VT_BYREF;",		badVARIANTstring, //pVal==5 (bad pointer),
	"variant2.vt = VT_INT|VT_BYREF;",		badVARIANTstring, //pVal==NULL (bad pointer),
	"variant2.vt = VT_UINT|VT_BYREF;",		badVARIANTstring, //pVal==0xff (bad pointer)
};

	for(i = 0; i < (sizeof(simpledata) / (sizeof(CString) * 2)); i++)
	{
		EXPECT_TRUE(dbg.CurrentLineIs(simpledata[i][0]) );
		dbg.StepOver(2);
		LOG->RecordCompare(cxx.ExpressionValueIs("variant2", simpledata[i][1]),
			"simple variant2 expected:(%s)\n",simpledata[i][1]);
	}

	uivar.Activate();
	ERROR_SUCCESS(uivar.SetPane(PANE_AUTO) );
	EXPR_INFO expr_info_actual[1], expr_info_expected[1];

	EXPECT_TRUE( dbg.StepOver() );
//check return_variant function returns the VARIANT
	EXPECT_SUCCESS(uivar.GetAllFields(expr_info_actual, 3 /*ROW_BOTTOM does not exist*/));
	
	FillExprInfo(expr_info_expected[0], NOT_EXPANDABLE, 
		"tagVARIANT", "return_variant returned", "{65535 VT_I4}" );
	EXPECT_TRUE(ExprInfoIs(expr_info_actual, expr_info_expected, 1) );
	EXPECT_TRUE(dbg.ShowNextStatement() ); //uivar.Activate() steals the focus

}//VARIANTs

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
}

//MMXRegs
void CSpecialTypesTest::MMXRegs(void)
{
 	LogTestHeader( "MMX Test" );
	if(!MMX_Chip())
	{
		LOG->RecordInfo("No MMX chip detected - skipping the test");
		return;
	}
}//MMXRegs

//ErrorCodes
void CSpecialTypesTest::ErrorCodes(void)
{
	char lastErrorLoc[16];

	//	this code needed due to bug 65586 - dverma 4/26/2000
	if (GetSystem() == SYSTEM_WIN_MILL)
	{
		strcpy(lastErrorLoc,"dw @tib+0x74");		//	location of last error set
	}
	else
	{
		strcpy(lastErrorLoc,"@ERR");
	}


static char* data[][2] = {
	"s_ok,hr",			"S_OK",
	"s_err,hr",			"E_NOINTERFACE",
	"s_unexp,hr",		"E_UNEXPECTED",
	"s_eomem,hr",		"E_OUTOFMEMORY",
//NOTE extra space required after a full text of message
	"5,hr",				"0x00000005 Access is denied. ",
	"8,hr",				"0x00000008 Not enough storage is available to process this command. ",
	"24L,hr",			"0x00000018 The program issued a command but the command length is incorrect. ",
//this message is truncated by the debugger	
	"0x95L,hr",			"0x00000095 An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous sub",
	lastErrorLoc,				"42",
};

 	LogTestHeader( "ErrorCodes Test" );

	src.Find("_asm int 3;");	
	dbg.StepToCursor();	

	for(int i = 0; i < (sizeof(data) / (sizeof(CString) * 2)); i++)
	{
		LOG->RecordCompare(cxx.ExpressionValueIs(data[i][0], data[i][1]),
			"Expression:(%s), expected:(%s)\n", data[i][0],data[i][1]);
	}
	CString strTIBEqual = "@TIB==" + m_TIBValue;
	LOG->RecordCompare(cxx.ExpressionValueIs(strTIBEqual, "1"),
			"TIB is equal:(%s)\n", m_TIBValue);

	//TODO fill with tests for the actual value of TIB?
}//ErrorCodes



