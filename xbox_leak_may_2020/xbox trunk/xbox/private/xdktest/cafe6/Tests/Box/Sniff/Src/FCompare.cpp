/////////////////////////////////////////////////////////////
//	FCompare.cpp
//
//	Created by :			
//		Anushas


#include "FCompare.h"
#include "winuser.h"
using namespace std;

/*///////////////////////////////////////////////////////////
This function parses the control file and if a block matches 
the system's OS info then extracts that block and stores
the block in a list.
Input : Control file
Output: Flag to if the block is found or not
///////////////////////////////////////////////////////////*/

int CFileCompare::StoreCtrlList(string Ctrlfile)
{
	int flag=1;//flag for if the block is found or not
	ifstream in;
	in.open(Ctrlfile.c_str());
	string line,temp;
	if(in.good() == 0)
		return 2;
	while(in.good() && flag)
	{
		getline(in,line);//get the 1st line
		if (line.compare(m_OSinfo)== 0) //Check if the block is found
		{
			flag =0;//if found set to 0
			while(in.good())		//for the original file
			{
				line ="";
				in >> temp;// exclude the path name
				if(temp.length()>0)// check if current record is empty 
				{//if not empty
					if(temp.find_first_of("[")== 0) // Check if the end of the block is reached
						break;						// If reached break.
										
					for(int i=0;i<4;i++)
					{
						in >> temp;//filename
						line.append(temp);
						line.append(" ");//add delimiter
					}
					m_Ctrllist.push_back(line);//insert the file details in the list
					in>>temp;//exclude the time
					in>>temp;//exclude the CRC comment
				}
				else
					getline(in,line);//next line
			}
		}
	}
	in.close();
	return flag;
}//end of function

/*///////////////////////////////////////////////////////////
This function parses the pre/post file and stores
the block in a list.
Input : pre/Post file
Output: Flag to if the file is found or not
///////////////////////////////////////////////////////////*/
int CFileCompare::StorePList(string pfile)
{
	ifstream in;
	string line,temp;
	in.open(pfile.c_str());
	if(in.good() == 0)
		return 2;
	getline(in,line);// The 1st line is not necessary as it contains the labels
	while(in.good())
	{
		line="";
		in >> temp;//path name we don't need
		if (temp.length() == 0)// check if current record is empty 
			break;				//If so , break	
		for(int i=0;i<4;i++)
		{
			in >> temp;//filename
			line.append(temp);
			line.append(" ");//add delimiter
		}
		m_Plist.push_back(line);//insert the file details in the list
		in>>temp;//exclude the time
		in>>temp;//exclude the CRC comment
	}
	in.close();
	return 1;
}//end of function

/*///////////////////////////////////////////////////////////
This function finds the current system's OS and returns
it
Input : N/A
Output: OS info
///////////////////////////////////////////////////////////*/
string CFileCompare::getplatform()
{
	string plname;
	OSVERSIONINFOEX exOSInfo;
	exOSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *) &exOSInfo);//get the extended OSversion info
	switch (m_OSINFO.dwPlatformId)
	{

	case VER_PLATFORM_WIN32_NT:
		if ( m_OSINFO.dwMajorVersion <= 4 )
			plname = "Microsoft Windows NT";
		else if ( m_OSINFO.dwMajorVersion == 5 )
		{
			/*if(exOSInfo.wProductType == VER_NT_WORKSTATION)
				plname = "Microsoft Windows 2000 Profesional";
			else if ((exOSInfo.wProductType == VER_NT_SERVER) && (exOSInfo.wSuiteMask == VER_SUITE_DATACENTER))
				plname = "Microsoft Windows 2000 DataCenter Server";
			else if ((exOSInfo.wProductType == VER_NT_SERVER) && (exOSInfo.wSuiteMask == VER_SUITE_ENTERPRISE))
				plname = "Microsoft Windows 2000 Advanced Server";
			else*/
				plname = "Microsoft Windows 2000";// Server";
		}
		else
			plname = "Unknown Windows Class (NT)";
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		if ( (m_OSINFO.dwMajorVersion == 4) && (m_OSINFO.dwMinorVersion > 0) && (m_OSINFO.dwMinorVersion < 90) )
			plname = "Microsoft Windows 98";
		else if ( (m_OSINFO.dwMajorVersion == 4) && (m_OSINFO.dwMinorVersion == 0))
			plname = "Microsoft Windows 95";
		else if ( (m_OSINFO.dwMajorVersion == 4) && (m_OSINFO.dwMinorVersion == 90))
			plname = "Microsoft Windows Millennium";
		else
			plname = "Unknown Windows Class (95/98/ME)";
		break;

	case VER_PLATFORM_WIN32s:
		plname ="Microsoft Win32s ";

	}
	return plname;
}//end of function

/*///////////////////////////////////////////////////////////
This function finds the current system's language and 
returns it
Input : N/A
Output: language
///////////////////////////////////////////////////////////*/

string CFileCompare::getLang()
{
	string lng;
	if( GetSystemDefaultLangID() == 0x0409)
		lng ="English";
	else if (GetSystemDefaultLangID() == 0x0411)
		lng = "Japanese";
	else
		lng = "Unknown";
	return lng;
}//end of function

/*///////////////////////////////////////////////////////////
This function finds the current system's OS info and stores 
it in a private variable
Input : "PRE" or "POST" string
Output: N/A
///////////////////////////////////////////////////////////*/

void CFileCompare::getOSinfo(string msg)
{
	m_OSINFO.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&m_OSINFO);//get the OSversion info
	string plform="[";
	char buff[20];
	plform.append(getplatform());//get OS platform
	_itoa(m_OSINFO.dwMajorVersion,buff,10);//get major version number
	plform.append(" ");
	plform.append(buff);
	_itoa(m_OSINFO.dwMinorVersion,buff,10);//get minor version number
	plform.append(".");
	plform.append(buff);
	_itoa(m_OSINFO.dwBuildNumber,buff,10);//get build number
	plform.append(" Build ");
	plform.append(buff);
	plform.append(" ");
	plform.append(m_OSINFO.szCSDVersion);//get service pack
	plform.append(" ");
	plform.append(getLang());//get language
	m_OSinfo = plform+" "+ msg +"]";
}//end of function

/*///////////////////////////////////////////////////////////
This function gets the control file and the pre/post file
info parses those files and stores them in a list
and compares the 2 lists and finds out teh common files,stores
them in a list and deletes them from the original lists
returns it
Input : Ctrl file name,Pre/Post file name,"PRE" or "POST" msg
Output: 0/1 based on the search was successful or not
///////////////////////////////////////////////////////////*/

int CFileCompare::CompareFiles(string orfile,string pfile,string msg)
{
	list<string>::iterator iter1,iter2,titer1,titer2;
	int flag =0,ctrlflag;
	getOSinfo(msg);//gets the OS info
	ctrlflag = StoreCtrlList(orfile);//store sthe ctrl file info onto a list
	if(ctrlflag == 1)
		return 1;//unsuccessful
	
	if (StorePList(pfile) == 2 || ctrlflag == 2)//store sthe pre/post file info onto a list
		return 2;//file not found

	for(iter1 = m_Ctrllist.begin();iter1!=m_Ctrllist.end();)
	{
		flag =0;
		for(iter2= m_Plist.begin();iter2!=m_Plist.end();)
		{
			titer1 = iter1;
			titer2 = iter2;
			if((*iter1).compare(*iter2) == 0)
			{
				m_succlist.push_back(*iter1);
				iter1++;
				iter2++;
				m_Ctrllist.erase(titer1);//delete the entry
				m_Plist.erase(titer2);//delete the entry
				flag = 1;
				break;
			}
			else
				iter2++;
		}
		if(flag == 0)
			iter1++;
	}
	return 0;//if successful
}//end of function
