///////////////////////////////////////////////////////////////////////////////
//	EECASEPP.CPP
//											 
//	Created by:			
//		dverma		
//
//	Description:								 
//		VC6 Processor Pack testcases.

#include "stdafx.h"
#include "eecasepp.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CEEPP, CDbgTestBase, "Processor Pack", -1, CEESubSuite)
			  
									 
void CEEPP::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CEEPP::Run(void)
	
{
	char bPPFlagsPresent = 0;

	//	test if any pp switches present
	if (CMDLINE->GetBooleanValue("MMX",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("k6",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("k7",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("kni",FALSE))  bPPFlagsPresent = 1;
	if (CMDLINE->GetBooleanValue("wni",FALSE))  bPPFlagsPresent = 1;

	if (!bPPFlagsPresent) 
	{
			LOG->RecordInfo("No Processor Pack Flags Specified");
			return;
	}

	/******************
	 * INITIALIZATION *
	 ******************/

	// the base name of the localized directories and files we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ExprEval PP";
	else
		m_strProjBase = "ExprEval PP";

	// the location of the unlocalized sources, under which the projects we will use will be located.
	m_strSrcDir = GetCWD() + "src\\ExprEval PP";
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	m_strExeDir = m_strSrcDir + "\\" + m_strProjBase + " EXE";

	if(!CMDLINE->GetBooleanValue("noclean", FALSE))

	{
		// clean-up the exe project from the last run.
		KillFiles(m_strExeDir + "\\Debug\\", "*.*");
		RemoveDirectory(m_strExeDir + "\\Debug");
		KillFiles(m_strExeDir + "\\", "*.*");
		RemoveDirectory(m_strExeDir);

		// set the options for the exe project we are building for this test.
		CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
		pWin32AppWizOpt->m_strLocation = m_strSrcDir;
		pWin32AppWizOpt->m_strName = m_strProjBase + " EXE";	
		
		// create the exe project.
		EXPECT_SUCCESS(prj.New(pWin32AppWizOpt));
		// create a new localized exe source file in the project dir from the unlocalized exe source file.
		CopyFile(m_strSrcDir + "\\" + "ExprEval PP.cpp", m_strExeDir + "\\" + m_strProjBase + " EXE.cpp", FALSE);
		// make it writable so it can be cleaned up later.
		SetFileAttributes(m_strExeDir + "\\" + m_strProjBase + " EXE.cpp", FILE_ATTRIBUTE_NORMAL);
		// add the source to the project.
		EXPECT_SUCCESS(prj.AddFiles(m_strProjBase + " EXE.cpp"));	
		// build the project.
		EXPECT_SUCCESS(prj.Build());
	}
	else
		{EXPECT_SUCCESS(prj.Open(m_strExeDir + "\\" + m_strProjBase + " EXE.dsp"));}

	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());
		
	test_MMX(cxx,dbg,src);
	test_3dNow(cxx,dbg,src);
	test_3dNowEnhanced(cxx,dbg,src);
	test_Katmai(cxx,dbg,src);

	EXPECT_TRUE(dbg.StopDebugging(ASSUME_NORMAL_TERMINATION));
	EXPECT_SUCCESS(prj.Close());
}


//	 method to test the value of register MM0

void CEEPP::testRegVal(COExpEval& cxx, char *value, char *message)
{
	EXPR_INFO expr_info;

	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM0", value);		
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);
}

void CEEPP::testRegVal(char *varname, char *vartype, COExpEval& cxx, char *value, char *message)
{
	EXPR_INFO expr_info;

	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, vartype, varname, value);		
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

}

void CEEPP::testRegVal(int regid, COExpEval& cxx, char *value, char *message)
{
	EXPR_INFO expr_info;

	switch (regid)
	{
		case 0:			
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM0", value);
					break;


		case 1:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM1", value);
					break;


		case 2:					
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM2", value);
					break;

		case 3:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM3", value);
					break;


		case 4:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM4", value);
					break;

		case 5:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM5", value);
					break;

		case 6:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM6", value);
					break;

		case 7:		
					cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "unsigned __int64", "MM7", value);
					break;

	}

	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);
}

void CEEPP::testRegVal(COExpEval& cxx, char *value1, char *value2, char *value3, char *value4, char *message)
{
	EXPR_INFO expr_info;


	//	check XMM00
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", "XMM00", value4);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM01
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", "XMM01", value3);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM02		
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", "XMM02", value2);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM03
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", "XMM03", value1);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

}

void CEEPP::testRegVal(int reg_id, COExpEval& cxx, char *value1, char *value2, char *value3, char *value4, char *message)
{

	CString MMXijRegs[] = {	"XMM00","XMM01","XMM02","XMM03", "XMM10","XMM11","XMM12","XMM13",
							"XMM20","XMM21","XMM22","XMM23", "XMM30","XMM31","XMM32","XMM33",
							"XMM40","XMM41","XMM42","XMM43", "XMM50","XMM51","XMM52","XMM53",
							"XMM60","XMM61","XMM62","XMM63", "XMM70","XMM71","XMM72","XMM73" };
	
	EXPR_INFO expr_info;


	//	check XMM00
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", MMXijRegs[reg_id*4], value4);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM01
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", MMXijRegs[reg_id*4+1], value3);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM02		
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", MMXijRegs[reg_id*4+2], value2);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);

	//	check XMM03
	cxx.FillExprInfo(expr_info, NOT_EXPANDABLE, "float", MMXijRegs[reg_id*4+3], value1);		
	
	if(cxx.VerifyExprInfo(expr_info))
		LOG->RecordSuccess("%s : %s\n", expr_info.name,message);
	else
		LOG->RecordFailure("%s : %s\n", expr_info.name,message);


}

void CEEPP::test_MMX(COExpEval& cxx, CODebug& dbg, COSource& src)
{
	//	MMX switch has already been checked for in Run()
	
	__int64 count = 0;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");	
	EXPECT_TRUE(src.Find("test_MMX();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());

	char MMReg[][8] = {	"MM0", "MM1", "MM2", "MM3", "MM4", 
							"MM5", "MM6", "MM7"		};

	int num_reg = 8;
	int index = 0;

	cxx.Enable(EE_ALL_INFO);

	//	set value in watch window
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));
	for (index = 0; index <num_reg; index++)
	{
		EXPECT_TRUE(cxx.SetExpressionValue(MMReg[index],index*101));
	}

	//	do one stepover
	EXPECT_TRUE(dbg.StepOver());

	//	verify value in watch window
	for (index = 0; index <num_reg; index++)
	{
		char str[8];
		sprintf(str,"%d",index*101);
		testRegVal(MMReg[index], "unsigned __int64", cxx, str,"Test MMi in Watch");
	}

	//	verify value in register window
	EXPECT_TRUE(regs.Enable());
	for (index = 0; index <num_reg; index++)
	{
		EXPECT_TRUE(regs.RegisterValueIs(MMReg[index], "" , index*101, FALSE));
	}
	EXPECT_TRUE(regs.Disable());


	//	test all registers for operability
	dbg.StepOver();
	testRegVal(0, cxx,"1000","move 64 bit");

	dbg.StepOver();
	testRegVal(1, cxx,"2000","move 64 bit");

	dbg.StepOver();
	testRegVal(2, cxx,"3000","move 64 bit");

	dbg.StepOver();
	testRegVal(3, cxx,"4000","move 64 bit");

	dbg.StepOver();
	testRegVal(4, cxx,"5000","move 64 bit");

	dbg.StepOver();
	testRegVal(5, cxx,"6000","move 64 bit");

	dbg.StepOver();
	testRegVal(6, cxx,"7000","move 64 bit");

	dbg.StepOver();
	testRegVal(7, cxx,"8000","move 64 bit");
	
	//	shift left
	dbg.StepOver();
	testRegVal(cxx,"1600","Shift Left");

	dbg.StepOver();
	testRegVal(cxx,"576","Shift Left");

	//	mov 32 bit
	dbg.StepOver();
	testRegVal(cxx,"123456789","mov 32 bit");
	
	dbg.StepOver();
	testRegVal(cxx,"0","mov 32 bit");
	
	dbg.StepOver();	//	32 bits - 1
	testRegVal(cxx,"4294967295","mov 32 bit");
	
	dbg.StepOver();	//	32 bits + 1
	testRegVal(cxx,"1","mov 32 bit");
	
	dbg.StepOver();	//	33 bits
	testRegVal(cxx,"0","mov 32 bit");


	//	move 64 bit

	dbg.StepOver();
	testRegVal(cxx,"123456789","move 64 bit");

	dbg.StepOver();
	testRegVal(cxx,"4294967296","move 64 bit");
	
	dbg.StepOver();
	testRegVal(cxx,"4294967295","move 64 bit");
	
	dbg.StepOver();	
	testRegVal(cxx,"4294967297","move 64 bit");
	
	dbg.StepOver();	
	testRegVal(cxx,"8589934592","move 64 bit");

	
	//	Pack with Signed Saturation
	dbg.StepOver();
	testRegVal(cxx,"4294967297","Pack with Signed Saturation");
	
	dbg.StepOver();
	testRegVal(cxx,"4294967297","Pack with Signed Saturation");

	//	Pack with Unsigned Saturation	
	dbg.StepOver();
	testRegVal(cxx,"4294967297","Pack with Unsigned Saturation");

	//	Packed Add
	dbg.StepOver();
	testRegVal(cxx,"101","Packed Add Byte");

	dbg.StepOver();
	testRegVal(cxx,"254","Packed Add Byte");
	
	dbg.StepOver();
	testRegVal(cxx,"0","Packed Add Byte");

	dbg.StepOver();
	testRegVal(cxx,"512","Packed Add Byte");


	dbg.StepOver();
	testRegVal(cxx,"101","Packed Add Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Add Word");

	dbg.StepOver();
	testRegVal(cxx,"131072","Packed Add Word");

	
	dbg.StepOver();
	testRegVal(cxx,"101","Packed Add Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Add Double");

	dbg.StepOver();
	testRegVal(cxx,"8589934592","Packed Add Double");


	//	Packed Add with Saturation
	dbg.StepOver();
	testRegVal(cxx,"250","Packed Add with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"34","Packed Add with Saturation");


	//	Packed Add Unsigned with Saturation 
	dbg.StepOver();
	testRegVal(cxx,"255","Packed Add Unsigned with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"65535","Packed Add Unsigned with Saturation");


	//	Bitwise Logical And 
	dbg.StepOver();
	testRegVal(cxx,"0","Logical And");

	dbg.StepOver();
	testRegVal(cxx,"15","Logical And");


	//	Bitwise Logical And Not 
	dbg.StepOver();
	testRegVal(cxx,"0","Logical And Not");

	dbg.StepOver();
	testRegVal(cxx,"14","Logical And Not");


	//	Bitwise Logical Or
	dbg.StepOver();
	testRegVal(cxx,"79","Logical Or");


	//	Compare for Equal 
	dbg.StepOver();
	testRegVal(cxx,"18446744073709486080","Byte Compare for Equal");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551360","Byte Compare for Equal");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","Byte Compare for Equal");


	dbg.StepOver();
	testRegVal(cxx,"18446744073709486080","Word Compare for Equal");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","Word Compare for Equal");
	
	dbg.StepOver();
	testRegVal(cxx,"18446744069414584320","Double Compare for Equal");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","Double Compare for Equal");


	//	Compare for Greater Than
	dbg.StepOver();
	testRegVal(cxx,"0","Byte Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"255","Byte Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"0","Byte Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"255","Byte Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"65280","Byte Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"65280","Byte Compare for Greater Than");

	
	dbg.StepOver();
	testRegVal(cxx,"65535","Word Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"4294901760","Word Compare for Greater Than");

	
	dbg.StepOver();
	testRegVal(cxx,"4294967295","Double Compare for Greater Than");

	dbg.StepOver();
	testRegVal(cxx,"18446744069414584320","Double Compare for Greater Than");


	//	Packed Multiply and Add 
	dbg.StepOver();
	testRegVal(cxx,"1300","Packed Multiply and Add");

	dbg.StepOver();
	testRegVal(cxx,"1","Packed Multiply and Add");


	//	Packed Multiply High
	dbg.StepOver();
	testRegVal(cxx,"0","Packed Multiply High");

	dbg.StepOver();
	testRegVal(cxx,"1","Packed Multiply High");


	//	Packed Multiply Low
	dbg.StepOver();
	testRegVal(cxx,"24","Multiply Low");

	dbg.StepOver();
	testRegVal(cxx,"6550","Multiply Low");


	//	Packed Shift Left Logical
	dbg.StepOver();
	testRegVal(cxx,"368","Packed Shift Left Logical Word");

	dbg.StepOver();
	testRegVal(cxx,"131074","Packed Shift Left Logical Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Left Logical Word");

	
	dbg.StepOver();
	testRegVal(cxx,"368","Packed Shift Left Logical Double");

	dbg.StepOver();
	testRegVal(cxx,"2","Packed Shift Left Logical Double");

	dbg.StepOver();	//	=> 2^33
	testRegVal(cxx,"8589934592","Packed Shift Left Logical Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Left Logical Double");

	
	dbg.StepOver();
	testRegVal(cxx,"368","Packed Shift Left Logical 64");

	//	Packed Shift Right Arithmetic
	dbg.StepOver();
	testRegVal(cxx,"50","Packed Shift Right Arithmetic Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Word");

	dbg.StepOver();
	testRegVal(cxx,"1","Packed Shift Right Arithmetic Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Word");

	
	dbg.StepOver();
	testRegVal(cxx,"50","Packed Shift Right Arithmetic Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Double");

	dbg.StepOver();
	testRegVal(cxx,"1","Packed Shift Right Arithmetic Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Arithmetic Double");

	
	//	Packed Shift Right Logical
	dbg.StepOver();
	testRegVal(cxx,"16","Packed Shift Right Logical Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Logical Word");

	dbg.StepOver();	//	=>0
	testRegVal(cxx,"0","Packed Shift Right Logical Word");

	dbg.StepOver();	//	=>0
	testRegVal(cxx,"0","Packed Shift Right Logical Word");

	dbg.StepOver();	//	=>1
	testRegVal(cxx,"1","Packed Shift Right Logical Word");

	
	dbg.StepOver();
	testRegVal(cxx,"16","Packed Shift Right Logical Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Logical Double");

	dbg.StepOver();	//	0
	testRegVal(cxx,"0","Packed Shift Right Logical Double");

	dbg.StepOver();	//	1
	testRegVal(cxx,"1","Packed Shift Right Logical Double");

	dbg.StepOver();	//	0
	testRegVal(cxx,"0","Packed Shift Right Logical Double");

	
	dbg.StepOver();
	testRegVal(cxx,"16","Packed Shift Right Logical Q");

	dbg.StepOver();
	testRegVal(cxx,"0","Packed Shift Right Logical Q");

	//	Packed Subtract
	dbg.StepOver();
	testRegVal(cxx,"140","Packed Subtract Byte");

	dbg.StepOver();
	testRegVal(cxx,"496","Packed Subtract Byte");

	dbg.StepOver();
	testRegVal(cxx,"65636","Packed Subtract Byte");

	
	dbg.StepOver();
	testRegVal(cxx,"300","Packed Subtract Word");

	dbg.StepOver();
	testRegVal(cxx,"65650","Packed Subtract Word");

	dbg.StepOver();
	testRegVal(cxx,"4294967396","Packed Subtract Word");

	dbg.StepOver();
	testRegVal(cxx,"4295032826","Packed Subtract Word");


	dbg.StepOver();
	testRegVal(cxx,"300","Packed Subtract Double");

	dbg.StepOver();
	testRegVal(cxx,"4294967396","Packed Subtract Double");

	//	Packed Subtract with Saturation
	dbg.StepOver();
	testRegVal(cxx,"140","Byte Packed Subtract with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"384","Byte Packed Subtract with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"65664","Byte Packed Subtract with Saturation");

	
	dbg.StepOver();
	testRegVal(cxx,"300","Word Packed Subtract with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"65650","Word Packed Subtract with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"4294967396","Word Packed Subtract with Saturation");


	//	Packed Subtract Unsigned with Saturation
	dbg.StepOver();	//	all "correct" values
	testRegVal(cxx,"140","Byte Packed Subtract Unsigned with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"300","Byte Packed Subtract Unsigned with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"65636","Byte Packed Subtract Unsigned with Saturation");

	
	dbg.StepOver();
	testRegVal(cxx,"300","Word Packed Subtract Unsigned with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"65650","Word Packed Subtract Unsigned with Saturation");

	dbg.StepOver();
	testRegVal(cxx,"4294967396","Word Packed Subtract Unsigned with Saturation");

	
	//	Unpack High Packed Data
	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"256","Unpack High Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"1","Unpack High Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"257","Unpack High Packed Data Word");
	

	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Double");

	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Double");

	dbg.StepOver();
	testRegVal(cxx,"65536","Unpack High Packed Data Double");

	dbg.StepOver();
	testRegVal(cxx,"1","Unpack High Packed Data Double");

	dbg.StepOver();
	testRegVal(cxx,"65537","Unpack High Packed Data Double");
	
	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Q");

	dbg.StepOver();
	testRegVal(cxx,"0","Unpack High Packed Data Q");

	dbg.StepOver();
	testRegVal(cxx,"4294967296","Unpack High Packed Data Q");

	dbg.StepOver();
	testRegVal(cxx,"1","Unpack High Packed Data Q");

	dbg.StepOver();
	testRegVal(cxx,"4294967297","Unpack High Packed Data Q");
	
	//	Unpack Low Packed Data
	dbg.StepOver();
	testRegVal(cxx,"513","Unpack Low Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"258","Unpack Low Packed Data Word");

	dbg.StepOver();
	testRegVal(cxx,"131073","Unpack Low Packed Data Double");

	dbg.StepOver();
	testRegVal(cxx,"65538","Unpack Low Packed Data Double");
	
	dbg.StepOver();
	testRegVal(cxx,"8589934593","Unpack Low Packed Data Q");

	dbg.StepOver();
	testRegVal(cxx,"4294967298","Unpack Low Packed Data Q");
	
	//	Bitwise Logical Exclusive OR
	dbg.StepOver();
	testRegVal(cxx,"240","Logical Exclusive OR");

	dbg.StepOver();
	testRegVal(cxx,"0","Logical Exclusive OR");

	cxx.Disable();
}
	
void CEEPP::test_3dNow(COExpEval& cxx, CODebug& dbg, COSource& src)
{
	if (!(CMDLINE->GetBooleanValue("k6",FALSE)||CMDLINE->GetBooleanValue("k7",FALSE))) 
		return;	//	exit if 3dnow switch is not set	

	// Restart and Go to the line at which we want to begin our tests

	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");	
	EXPECT_TRUE(src.Find("test_3dNow();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	cxx.Enable(EE_ALL_INFO);

	char MMijReg[][8] = {	"MM00", "MM01", "MM10", "MM11", "MM20", "MM21", "MM30", "MM31", 
							"MM40", "MM41",	"MM50", "MM51", "MM60", "MM61", "MM70", "MM71"	};

	int num_reg = 16;
	int count = 0;

	//	set value in watch window
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));
	for (count = 0; count <num_reg; count++)
	{
		EXPECT_TRUE(cxx.SetExpressionValue(MMijReg[count],count*50));
	}

	//	do one stepover
	EXPECT_TRUE(dbg.StepOver());

	//	verify value in watch window
	for (count = 0; count <num_reg; count++)
	{
		char str[16];
		float fVal = (float) count*50;
		sprintf(str,"%f",fVal);
		if (count==0) 
			str[8] = '\0';	//	0 is shown as 0.000000
		else
			str[7] = '\0';
		
		testRegVal(MMijReg[count], "float", cxx, str,"Test MMij in Watch");
	}

	//	verify value in register window

	EXPECT_TRUE(regs.Enable());

	for (count = 0; count < num_reg; count++)
	{
		char str[16];
		double d = count*50;		
		sprintf(str,"+%.5E",d);

		EXPECT_TRUE(regs.RegisterValueIs(MMijReg[count], str, 0, TRUE));	
	}

	EXPECT_TRUE(regs.Disable());


	//	average of unsigned int bytes
	dbg.StepOver();
	testRegVal(cxx,"1","average of unsigned int bytes");

	dbg.StepOver();
	testRegVal(cxx,"1","average of unsigned int bytes");
	
	dbg.StepOver();
	testRegVal(cxx,"1","average of unsigned int bytes");
	
	dbg.StepOver();
	testRegVal(cxx,"734423025668","average of unsigned int bytes");


	//	float to 32 bit signed int

	dbg.StepOver();
	testRegVal(cxx,"2147483647","float to 32 bit signed int");

	dbg.StepOver();
	testRegVal(cxx,"9223372034707292159","float to 32 bit signed int");
	
	dbg.StepOver();
	testRegVal(cxx,"1","float to 32 bit signed int");
	
	dbg.StepOver();
	testRegVal(cxx,"4","float to 32 bit signed int");

	//	accumulator
	dbg.StepOver();
	testRegVal(cxx,"4575657231080488962","accumulator");
	
	dbg.StepOver();
	testRegVal(cxx,"1065353218","accumulator");

	dbg.StepOver();
	testRegVal(cxx,"4575657229998358528","accumulator");

	//	add
	dbg.StepOver();
	testRegVal(cxx,"1065353218","add");
	
	dbg.StepOver();
	testRegVal(cxx,"1077936131","add");

	dbg.StepOver();
	testRegVal(cxx,"1088421892","add");

	//	compare for equal
	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for equal");
	
	dbg.StepOver();
	testRegVal(cxx,"18446744069414584320","compare for equal");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for equal");

	dbg.StepOver();
	testRegVal(cxx,"4294967295","compare for equal");

	dbg.StepOver();
	testRegVal(cxx,"0","compare for equal");

	//	compare for greater than or equal to
	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for greater than or equal to");
	
	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for greater than or equal to");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for greater than or equal to");

	dbg.StepOver();
	testRegVal(cxx,"18446744069414584320","compare for greater than or equal to");

	//	compare for greater than
	dbg.StepOver();
	testRegVal(cxx,"0","compare for greater than");
	
	dbg.StepOver();
	testRegVal(cxx,"0","compare for greater than");

	dbg.StepOver();
	testRegVal(cxx,"18446744069414584320","compare for greater than");

	dbg.StepOver();
	testRegVal(cxx,"18446744073709551615","compare for greater than");

	//	maximum
	dbg.StepOver();
	testRegVal(cxx,"0","maximum");
	
	dbg.StepOver();
	testRegVal(cxx,"4575657231063711746","maximum");

	dbg.StepOver();
	testRegVal(cxx,"4575657231076294659","maximum");

	dbg.StepOver();
	testRegVal(cxx,"4611686028095258627","maximum");

	//	minimum
	dbg.StepOver();
	testRegVal(cxx,"0","minimum");
	
	dbg.StepOver();
	testRegVal(cxx,"0","minimum");

	dbg.StepOver();
	testRegVal(cxx,"4575657231072100354","minimum");

	dbg.StepOver();
	testRegVal(cxx,"1065353218","minimum");

	//	multiply
	dbg.StepOver();
	testRegVal(cxx,"0","multiply");
	
	dbg.StepOver();
	testRegVal(cxx,"0","multiply");

	dbg.StepOver();
	testRegVal(cxx,"4611686036693581830","multiply");

	//	reciprocal
	dbg.StepOver();
	testRegVal(cxx,"9187343237679939583","reciprocal");
	
	dbg.StepOver();
	testRegVal(cxx,"4539626226423168512","reciprocal");

	dbg.StepOver();
	testRegVal(cxx,"1084227265","reciprocal");

	//	reciprocal, reciprocal intermediate step 1, reciprocal intermediate step 2
	dbg.StepOver();
	testRegVal(cxx,"4539626226423168512","reciprocal step1 step 2");
	
	dbg.StepOver();
	testRegVal(1, cxx,"1065418239","reciprocal step1 step 2");

	dbg.StepOver();
	testRegVal(cxx,"1057029630","reciprocal step1 step 2");

	dbg.StepOver();
	testRegVal(cxx,"1084308863","reciprocal step1 step 2");
	
	//	reciprocal sqrt
	dbg.StepOver();
	testRegVal(cxx,"9187343237679939583","reciprocal sqrt");
	
	dbg.StepOver();
	testRegVal(cxx,"4539627325934796544","reciprocal sqrt");

	dbg.StepOver();
	testRegVal(cxx,"4515608494420437504","reciprocal sqrt");

	//	reciprocal sqrt, reciprocal sqrt intermediate step 1	
	dbg.StepOver();
	testRegVal(cxx,"4539627325934796544","reciprocal sqrt, step 1");
	
	dbg.StepOver();
	testRegVal(1, cxx,"1065369343","reciprocal sqrt, step 1");

	dbg.StepOver();
	testRegVal(cxx,"4515608494420437504","reciprocal sqrt, step 1");

	dbg.StepOver();
	testRegVal(1, cxx,"1065365418","reciprocal sqrt, step 1");	

	//	subtract
	dbg.StepOver();
	testRegVal(cxx,"0","subtract");
	
	dbg.StepOver();
	testRegVal(cxx,"0","subtract");

	dbg.StepOver();
	testRegVal(cxx,"1077936131","subtract");

	dbg.StepOver();
	testRegVal(cxx,"4575657231072100354","subtract");

	//	reverse subtraction
	dbg.StepOver();
	testRegVal(cxx,"0","reverse subtraction");
	
	dbg.StepOver();
	testRegVal(cxx,"0","reverse subtraction");

	dbg.StepOver();
	testRegVal(cxx,"1077936131","reverse subtraction");

	dbg.StepOver();
	testRegVal(cxx,"4575657231072100354","reverse subtraction");

	//	32 bit signed int to float
	dbg.StepOver();
	testRegVal(cxx,"1065353216","32 bit signed int to float");
	
	dbg.StepOver();
	testRegVal(cxx,"4629700418010611712","32 bit signed int to float");

	dbg.StepOver();
	testRegVal(cxx,"4611686019492741120","32 bit signed int to float");

	//	multiply high
	dbg.StepOver();
	testRegVal(cxx,"0","multiply high");
	
	dbg.StepOver();
	testRegVal(cxx,"0","multiply high");

	dbg.StepOver();
	testRegVal(cxx,"8590000129","multiply high");

	cxx.Disable();
}
     
void CEEPP::test_3dNowEnhanced(COExpEval& cxx, CODebug& dbg, COSource& src)
{
	if (!CMDLINE->GetBooleanValue("k7",FALSE)) return;		//	exit if k7 switch is not set	

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");	
	EXPECT_TRUE(src.Find("test_3dNowEnhanced();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	cxx.Enable(EE_ALL_INFO);

	//	packed float to int word with sign extend
	dbg.StepOver();
	testRegVal(cxx,"0","packed float to int word with sign extend");

	dbg.StepOver();
	testRegVal(cxx,"30064771076","packed float to int word with sign extend");

	dbg.StepOver();
	testRegVal(cxx,"42949672960","packed float to int word with sign extend");

	dbg.StepOver();
	testRegVal(cxx,"18446744069414584319","packed float to int word with sign extend");

	dbg.StepOver();
	testRegVal(cxx,"4294934528","packed float to int word with sign extend");

	//	packed float negative accumulate
	dbg.StepOver();
	testRegVal(cxx,"0","packed float negative accumulate");

	dbg.StepOver();
	testRegVal(cxx,"4629700416936869888","packed float negative accumulate");

	dbg.StepOver();
	testRegVal(cxx,"13799029259328552960","packed float negative accumulate");

	//	packed float mixed +ve -ve accumulate
	dbg.StepOver();
	testRegVal(cxx,"0","packed float mixed +ve -ve accumulate");

	dbg.StepOver();
	testRegVal(cxx,"1077936128","packed float mixed +ve -ve accumulate");

	dbg.StepOver();
	testRegVal(cxx,"4688247221760557056","packed float mixed +ve -ve accumulate");
   
	//	packed int word to float
	//	somewhat erroneous values
	dbg.StepOver();
	testRegVal(cxx,"1065353216","packed int word to float");

	dbg.StepOver();
	testRegVal(cxx,"13835058058495000576","packed int word to float");

	dbg.StepOver();
	testRegVal(cxx,"4674736414292705280","packed int word to float");

	dbg.StepOver();
	testRegVal(cxx,"4692750811720056832","packed int word to float");

	dbg.StepOver();
	testRegVal(cxx,"3338665984","packed int word to float");

	//	packed swap double word
	dbg.StepOver();
	testRegVal(cxx,"4294967296","packed swap double word");

	dbg.StepOver();
	testRegVal(cxx,"25769803777","packed swap double word");

	dbg.StepOver();
	testRegVal(cxx,"6","packed swap double word");

	//	streaming store using byte mask
	dbg.StepOver();
	dbg.StepOver();
	dbg.StepOver();

	//	streaming store
	dbg.StepOver();
	testRegVal("count", "__int64", cxx,"10","streaming store");

	dbg.StepOver();
	testRegVal("count", "__int64", cxx,"34359738373","streaming store");

			
	//	packed avg of unsigned byte	
	dbg.StepOver();
	testRegVal(cxx,"1","packed avg of unsigned byte");

	dbg.StepOver();
	testRegVal(cxx,"1","packed avg of unsigned byte");

	dbg.StepOver();
	testRegVal(cxx,"1","packed avg of unsigned byte");

	dbg.StepOver();
	testRegVal(cxx,"734423025668","packed avg of unsigned byte");
	
	//	packed avg of unsigned word	
	dbg.StepOver();
	testRegVal(cxx,"1","packed avg of unsigned word");

	dbg.StepOver();
	testRegVal(cxx,"12293138128684057218","packed avg of unsigned word");

	dbg.StepOver();
	testRegVal(cxx,"55000","packed avg of unsigned word");

	//	extract word into int register
	dbg.StepOver();
	testRegVal("eax", "unsigned long", cxx,"0","extract word into int register");

	dbg.StepOver();
	testRegVal("eax", "unsigned long", cxx,"1","extract word into int register");

	//	insert word from int register
	dbg.StepOver();
	testRegVal(cxx,"131076","insert word from int register");

	//	packed max signed word
	dbg.StepOver();
	testRegVal(cxx,"576742223002009601","packed max signed word");

	dbg.StepOver();
	testRegVal(cxx,"1431633926","packed max signed word");
	
	//	packed max unsigned byte
	dbg.StepOver();
	testRegVal(cxx,"1834219928319","packed max unsigned byte");
	
	//	packed min signed word
	dbg.StepOver();
	testRegVal(cxx,"533958022247022591","packed min signed word");

	dbg.StepOver();
	testRegVal(cxx,"1431633921","packed min signed word");
	
	//	packed min unsigned byte	
	dbg.StepOver();
	testRegVal(cxx,"730396487935","packed min unsigned byte");
	
	//	move mask to integer register
	dbg.StepOver();
	testRegVal("eax", "unsigned long", cxx,"217","move mask to integer register");
	
	//	packed multiply high unsigned word
	dbg.StepOver();
	testRegVal(cxx,"281474976841729","packed multiply high unsigned word");

	//	prefetch non-temporal access
	dbg.StepOver();
	
	//	prefetch to all cache levels
	dbg.StepOver();
	
	//	prefetch to all cache levels except 0
	dbg.StepOver();
	
	//	prefetch to all cache levels except 0 & 1
	dbg.StepOver();

	//	packed sum of absolute byte differences
	dbg.StepOver();
	testRegVal(cxx,"35","packed sum of absolute byte differences");

	dbg.StepOver();
	testRegVal(cxx,"35","packed sum of absolute byte differences");
	
	//	packed shuffle word
	dbg.StepOver();
	testRegVal(cxx,"281479271743489","packed shuffle word");

	dbg.StepOver();
	testRegVal(cxx,"578437695752307201","packed shuffle word");

	//	sfence
	dbg.StepOver();

	cxx.Disable();
}


void CEEPP::test_Katmai(COExpEval& cxx, CODebug& dbg, COSource& src)
{
	if (!(CMDLINE->GetBooleanValue("kni",FALSE)||CMDLINE->GetBooleanValue("wni",FALSE))) 
		return;	//	exit if P3 switch is not set	

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	MST.DoKeys("{ESC}");	
	EXPECT_TRUE(src.Find("test_Katmai();")); 
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.StepInto());	
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.StepOver());

	cxx.Enable(EE_ALL_INFO);

	char XMMijReg[][8] = {	"XMM00", "XMM01", "XMM02", "XMM03", 
							"XMM10", "XMM11", "XMM12", "XMM13", 
							"XMM20", "XMM21", "XMM22", "XMM23", 
							"XMM30", "XMM31", "XMM32", "XMM33", 
							"XMM40", "XMM41", "XMM42", "XMM43", 
							"XMM50", "XMM51", "XMM52", "XMM53", 
							"XMM60", "XMM61", "XMM62", "XMM63", 
							"XMM70", "XMM71", "XMM72", "XMM73"	};

	int num_reg = 32;
	int count = 0;

	//	set value in watch window
	for (count = 0; count <num_reg; count++)
	{
		cxx.SetExpressionValue(XMMijReg[count],count*50);
	}

	//	do one stepover
	EXPECT_TRUE(dbg.StepOver());

	//	verify value in watch window
	for (count = 0; count <num_reg; count++)
	{
		cxx.ExpressionValueIs(XMMijReg[count],count*50);
	}

	//	verify value in register window

	EXPECT_TRUE(regs.Enable());

	for (count = 0; count < num_reg; count++)
	{
		char str[16];
		double d = count*50;		
		sprintf(str,"+%.5E",d);

		EXPECT_TRUE(regs.RegisterValueIs(XMMijReg[count], str, 0, TRUE));	
	}

	EXPECT_TRUE(regs.Disable());


	//	verifying each register for expression evaluation
	dbg.StepOver();
	testRegVal(0, cxx,"1.00000","2.00000","3.00000","4.00000","verification xmm0");

	dbg.StepOver();
	testRegVal(1, cxx,"2.00000","3.00000","4.00000","5.00000","verification xmm1");

	dbg.StepOver();
	testRegVal(2, cxx,"3.00000","4.00000","5.00000","6.00000","verification xmm2");

	dbg.StepOver();
	testRegVal(3, cxx,"4.00000","5.00000","6.00000","7.00000","verification xmm3");

	dbg.StepOver();
	testRegVal(4, cxx,"5.00000","6.00000","7.00000","8.00000","verification xmm4");

	dbg.StepOver();
	testRegVal(5, cxx,"6.00000","7.00000","8.00000","9.00000","verification xmm5");

	dbg.StepOver();
	testRegVal(6, cxx,"7.00000","8.00000","9.00000","10.0000","verification xmm6");

	dbg.StepOver();
	testRegVal(7, cxx,"-1.00000","-2.00000","-3.00000", "6.00000","verification xmm7");


	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"3.00000","5.00000","7.00000","9.00000","addps");

	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","9.00000","addss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","4.00000","0.000000","1.00000","andnps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"2.00000","2.00000","3.00000","3.50000","andnps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","0.000000","0.000000","0.000000","andps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","andps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"-1.#QNAN","-1.#QNAN","-1.#QNAN","0.000000","cmpeqps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"-1.#QNAN","-1.#QNAN","-1.#QNAN","-1.#QNAN","cmpeqps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpeqss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpeqss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","-1.#QNAN","0.000000","0.000000","cmpltps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"-1.#QNAN","-1.#QNAN","0.000000","0.000000","cmpleps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","-1.#QNAN","-1.#QNAN","0.000000","cmpneqps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpneqss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpneqss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","-1.#QNAN","0.000000","-1.#QNAN","cmpnltps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpnltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpnltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpnltss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","0.000000","0.000000","-1.#QNAN","cmpnleps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","-1.#QNAN","cmpnless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpnless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.000000","cmpnless");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","comiss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","comiss");
	
	dbg.StepOver();
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","cvtpi2ps");

	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"8589934597","cvtps2pi");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","17.0000","cvtsi2ss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal("eax","unsigned long", cxx, "4", "cvtss2si");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"30064771077","cvttps2pi");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal("eax","unsigned long", cxx, "7", "cvttss2si");

	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.500000","1.00000","3.00000","4.00000","divps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","5.00000","divss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","2.00000","9.00000","5.00000","maxps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","2.00000","7.00000","6.00000","maxss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","2.00000","7.00000","4.00000","maxss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"4.00000","2.00000","7.00000","3.00000","minps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","6.00000","minss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","minss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","6.00000","7.00000","8.00000","movaps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","5.00000","6.00000","movhlps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"6.00000","7.00000","3.00000","4.00000","movhps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","6.00000","7.00000","movlps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","6.00000","3.00000","4.00000","movlhps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal("eax","unsigned long", cxx, "5", "movmskps");

	dbg.StepOver();
	dbg.StepOver();
	testRegVal("Var128.m128_f32[0]", "float", cxx, "-1.00000" ,"movntps");
	testRegVal("Var128.m128_f32[1]", "float", cxx, "9.00000" ,"movntps");
	testRegVal("Var128.m128_f32[2]", "float", cxx, "8.00000" ,"movntps");
	testRegVal("Var128.m128_f32[3]", "float", cxx, "7.00000" ,"movntps");

	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","0.000000","0.000000","9.00000","movss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","6.00000","7.00000","8.00000","movups");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"2.00000","6.00000","0.000000","10.0000","mulps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","8.00000","mulss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","3.00000","6.00000","5.00000","orps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.499878","0.333252","0.249939","0.199951","rcpps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.499878","rcpss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.999756","0.577271","0.499878","0.333252","rsqrtps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","0.333252","rsqrtss");
	
	dbg.StepOver();

	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"7.00000","5.00000","4.00000","2.00000","shufps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","1.73205","2.00000","3.00000","sqrtps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","3.00000","sqrtss");

	dbg.StepOver();
	testRegVal("i32Val","long", cxx, "8096", "stmxcsr");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"-1.00000","5.00000","1.00000","3.00000","subps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","8.00000","5.00000","-2.00000","subss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","4.00000","ucomiss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"1.00000","2.00000","3.00000","5.00000","ucomiss");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"5.00000","1.00000","6.00000","2.00000","unpckhps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"7.00000","3.00000","8.00000","4.00000","unpcklps");
	
	dbg.StepOver();
	dbg.StepOver();
	testRegVal(cxx,"0.000000","0.000000","1.76324e-038","2.93874e-039","xorps");

	cxx.Disable();

}

