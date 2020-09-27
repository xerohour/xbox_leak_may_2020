///////////////////////////////////////////////////////////////////////////////
//	FCompare.h
//
//	Created by :			
//		Anushas
//				
//	Description :
//		Declaration of the CFileCompare class
//
#ifndef _FCOMPARE_H
#define _FCOMPARE_H

#pragma warning (disable :4786)

#include <string>
#include <iostream>

#include "windows.h"
#include <fstream>
#include <list>

///////////////////////////////////////////////////////////////////////////////
//				File Compare class

class CFileCompare
{
private:
	OSVERSIONINFO m_OSINFO;
	std::string m_OSinfo;
	std::list<std::string> m_Ctrllist,m_Plist,m_succlist;
	int StoreCtrlList(std::string);//stores the control file info onto a list
	int StorePList(std::string);//stores the Pre/Post  file info onto a list
	void getOSinfo(std::string);//returns the OS info
	std::string getplatform();//returns the platform currently working on
	std::string getLang();//returns system language
	
	//operations
public:
	int CompareFiles(std::string ,std::string,std::string);//compares the ctrl and pre/post file
	std::list<std::string> getCtrllist(){return m_Ctrllist;}//returns the ctrl file list
	std::list<std::string> getPlist(){return m_Plist;}//returns the Pre/post file list
	std::list<std::string> getsucclist(){return m_succlist;}//returns the successful file search list
	std::string getOSinfo(){ return m_OSinfo;}//returns the OSinfo string
};
#endif