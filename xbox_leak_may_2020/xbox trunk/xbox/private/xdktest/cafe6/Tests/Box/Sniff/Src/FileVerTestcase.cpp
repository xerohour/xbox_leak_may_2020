///////////////////////////////////////////////////////////////////////////////
//	FileVerTestcase.CPP
//											 
//	Created by :			
//		Anushas
//			
//	Description :								 
//		Implements CFileVerTestcase

#include "stdafx.h"
#include "FileVerTestcase.h"					 	 

typedef	std::list<std::string> lstring;
			   
#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

IMPLEMENT_TEST(CFileVerTestcase, CTest, "VC6 - File Verification", -1, CVerCheckSuite)
												 
void CFileVerTestcase::Run(void)
{
	//checking for pre file
	m_pLog->RecordInfo("Comparing Control file and the PRE file .........");
	
	std::string pre_file = GetDrive("PRE");
	std::string post_file = GetDrive("POST");
	int flag = fc.CompareFiles(".\\Control.txt",pre_file,"PRE");
	m_pLog->RecordInfo(fc.getOSinfo().c_str());
	if (flag == 1)
	{
		m_pLog->RecordInfo("OS info doesnot exist in the Control file .........");
		m_pLog->RecordFailure("OS info doesnot exist in the :%s","Control file.....");
	}
	else if(flag == 2)
	{
		m_pLog->RecordInfo("File doesnot exist.........");
		m_pLog->RecordFailure(":%s","File does not exist .....");
	}
	else
		WriteLog();//write the results in the log file

	//checking for post file
	m_pLog->RecordInfo("\nComparing Control file and the POST file .........");
	flag = fc.CompareFiles(".\\Control.txt",post_file,"POST");
	m_pLog->RecordInfo(fc.getOSinfo().c_str());
	if (flag == 1)
	{
		m_pLog->RecordInfo("OS info doesnot exist in the Control file .........");
		m_pLog->RecordFailure("OS info doesnot exist in the :%s","Control file.....");
	}
	else if(flag == 2)
	{
		m_pLog->RecordInfo("File doesnot exist.........");
		m_pLog->RecordFailure(":%s","File does not exist .....");
	}
	else
		WriteLog();//write the results in the log file
/*	DeleteFile(pre_file.c_str());
	DeleteFile(post_file.c_str());*/
}

/*///////////////////////////////////////////////////////////
This function extracts the controllist,pre/post list and the
successful file search list and records them as either successes
or as failures 
Input : N/A
Output: N/A
///////////////////////////////////////////////////////////*/

void CFileVerTestcase::WriteLog()
{
	lstring Ctrllist,Plist,succlist;
	lstring::iterator iter;
	Ctrllist = fc.getCtrllist();
	Plist = fc.getPlist();
	succlist = fc.getsucclist();

	//Record Failures
	char buff[20];
	_itoa((Ctrllist.size()+Plist.size()),buff,10);
	std::string temp = "\n\n\nTotal number of failures  : " ;
	temp.append(buff);
	m_pLog->RecordInfo(temp.c_str());
	if(Ctrllist.size() > 0)//if list has data
		for(iter = Ctrllist.begin();iter!=Ctrllist.end();iter++)
			m_pLog->RecordFailure("Additional Files : %s", (*iter).c_str());

	if(Plist.size() > 0)//if list has data
		for(iter = Plist.begin();iter!=Plist.end();iter++)
			m_pLog->RecordFailure("Missing files : %s", (*iter).c_str());

	//record successes
	_itoa(succlist.size(),buff,10);
	temp = "\n\n\nTotal number of successes : " ;
	temp.append(buff);
	
	m_pLog->RecordInfo(temp.c_str());
	if (succlist.size()>0)// if success list has data
		for(iter = succlist.begin();iter!=succlist.end();iter++)
			m_pLog->RecordSuccess("%s", (*iter).c_str());
}//end of function


std::string CFileVerTestcase::GetDrive(std::string pfile)
{
	char* sysdir ;
	sysdir = (char*)malloc(sizeof(char)*20);
	std::string filename;
	GetSystemDirectory(sysdir,256);
	filename = sysdir;
	sysdir[filename.length()]='\0';
	filename = filename.substr(0,3);
	filename.append(pfile);
	filename.append(".txt");
	return filename;	
}