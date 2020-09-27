
// mainloop.cpp
#include <iostream.h>
#include <conio.h>
#include <fstream.h>


#include <stdlib.h>
#include <stdio.h>
#include <time.h>


//#include <afxdb.h>
#include <windows.h>

#include "thread.h"
#include "cppodbc.h"

#undef _MSC_VER
#ifdef _MSC_VER
#include <crtdbg.h>
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#endif

//for display synchronization
CRITICAL_SECTION csCriticalSection; 

// ERROR/MSG Strings

enum  { MAXPATH=512 };
enum  { MAXSTRINGS = 15};
char * strings[MAXSTRINGS] = { "Debug 0",                   //0
"If this text prints there was an error!!!\n"};           //1

// Debug String

enum  { MAXSTRLEN = 1024,MAXSTRLEN_P1 = 1025};
char * pcDebugString[MAXSTRLEN_P1];

// Program ID/HELP

static char *pcName    = "Û loop Û\n";
static char *pcVersion = "0.00\n";
static char *pcPurpose = "The puropse of this program is execute multiple "
"stored procedures\n each in a separate thread.\n";

static char *cmdlinehelp = "\n"
"Usage:   Main     [/options]\n\n"
"Options:      Description\n"
" ? H h        : Help Option will Display this Help Screen.\n"
" l n          : n=Number of times to run the sp's listed in the file(loops). default=1\n"
" s n          : n=Seconds of delay between loops. default=0\n"
" f string     : File which contains DSN's and SP's to run.\n"
"eg. c:\\main /f sp.txt\n\n"
"The format ofthe file is:\n"
"line1 number of sp's to execute:n\n"
"line2 the connection string specifing the data source\n"
"line3 stored procedure to execute\n"
"remaining lines will mimic lines 2 & 3 for additional sp's to execute\n"
"eg. File for two sp's.\n"
"2\nDSN=ChrisKoz4;DATABASE=PUBS;UID=SA;PWD=\nbyRoyalty 20\n"
"DSN=johnba2;DATABASE=PUBS;UID=SA;PWD=\nvartest2\n\n";


void vOutputHeader(void)
{
	cout << pcName;
	cout << "Version "<< pcVersion << endl;
	cout << pcPurpose;
	cout << "MS     bwp" << endl;
	
	cout << "Compiled using:"<< endl;
#ifdef __BORLANDC__
	cout << "Borland   Ver "<<__BORLANDC__<<endl;
#endif
#ifdef _MSC_VER
	cout << "MICROSOFT   Ver "<<_MSC_VER<<endl;
#endif
	
	cout << "Compiled on " << __DATE__<< " at "<< __TIME__<< endl;
}

void help(char *help_string)
{
	cout<<help_string<<endl;
	cout<<"Help::"<<endl;
	cout<<cmdlinehelp << endl;
	vOutputHeader();
	exit(1);
}

int cvtarg(char * pc)     // This fn provides parsing of the cmdline args
{                         // supporting the usage of /x or -x (2 char)
	int i = 0;              // arguments
	int firstbyte,secondbyte;
	
#ifdef __BORLANDC__
    firstbyte = 1;
    secondbyte = 0;
#else                     // MSC or MOTO
    firstbyte = 0;
    secondbyte = 1;
#endif
	
	if( strlen(pc) == 2)
	{
		i = (int) pc[firstbyte];
		i = i << 8;
		i += (int) pc[secondbyte];
	}
	else i = (int) pc[0];
	
	return(i);
}

typedef struct
{
	TCHAR * strConnectString;
	TCHAR * strSP;
	CThread * pct;
	int i;
}structODBC, *pstructODBC;


DWORD dwSQLThread(pstructODBC podbc)
{
	//   cout << "Thread::Running" << GetCurrentThreadId() << " " <<podbc->strConnectString << endl;
	Sleep(GetCurrentThreadId() % 10000);
	//  cout <<podbc->i << " Thread:: " << GetCurrentThreadId() << " " <<podbc->strConnectString << " :sp= "<<podbc->strSP <<" " << (GetCurrentThreadId() % 10000) << endl;
	
	if(podbc != NULL)
	{
		CODBCExec a;
//serialize the SQLConnect calls, because AutMgr hangs
//on multiple connections comming to fast
		BOOL bRes;
		try 
		{ 
			EnterCriticalSection(&csCriticalSection); 
			bRes=a.fConnect(podbc->strConnectString);
			LeaveCriticalSection(&csCriticalSection); 
		}catch(...)
		{
			LeaveCriticalSection(&csCriticalSection);
			throw;
		}
		if(bRes==TRUE)
		{
			if(a.fExecSql(podbc->strSP))
			{
				//      cout <<"Thread:: " << GetCurrentThreadId() << " " <<podbc->strConnectString << " :sp= "<<podbc->strSP <<" Executed." << endl;
				a.vDumpResults();
			    EnterCriticalSection(&csCriticalSection); 
				cout <<"Thread:: " << GetCurrentThreadId() << " " <<podbc->strConnectString << " :sp= "<<podbc->strSP <<" Executed." << endl;
				LeaveCriticalSection(&csCriticalSection); 

			}
			else
			{
				EnterCriticalSection(&csCriticalSection); 
				cout <<"Thread:: " << GetCurrentThreadId() << " " <<podbc->strConnectString << " :sp= "<<podbc->strSP <<" Failed to execute."<< endl;
				LeaveCriticalSection(&csCriticalSection); 
			}
			a.vDisconnect();
		}
		else
		{
			EnterCriticalSection(&csCriticalSection); 
			cout << "Thread::Failed to connect to " << GetCurrentThreadId() << " " <<podbc->strConnectString << endl;
			LeaveCriticalSection(&csCriticalSection); 
		}
		delete [] podbc->strConnectString;
		delete [] podbc->strSP;
		delete podbc->pct;
		delete podbc;
	}  // end if(podbc!=NULL)
	return 0;
}


int  main(int argc, char *argv[])
{ 
	
	
/*	
#ifdef _MSC_VER
	int nDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	nDbgFlag |= _CRTDBG_LEAK_CHECK_DF;    // turn on leak checking
	//  nDbgFlag &= !_CRTDBG_LEAK_CHECK_DF;  // turn off leak checking
	nDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(nDbgFlag);
	nDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	if( nDbgFlag & _CRTDBG_LEAK_CHECK_DF)
		cout << "DEBUG checking for memory leaks." << endl;
	else
		cout << "DEBUG checking for memory leaks NOT SET UP CORRECTLY!" << endl;
#endif
*/	
	
	ifstream ifs; // file streams to hold message and recipient file names
	CThread * pcthread = NULL;       // threads
	pstructODBC  pODBCstruct = NULL; // dsn/sp's
	pstructODBC  pODBC = NULL; // dsn/sp's
	int n = 0;                       // number of sp's to run from file
	int nLoops = 1 ;
	int nSec = 0;
	BOOL bAllocOK = TRUE;
	
	while((argc--) > 1 )
	{
		**argv++;
		//  cout << "ARG[" << argc+1 << "]= " << *argv << endl;
		//  cout << " hex *((short*) *argv) = " << hex << *((short*) *argv) <<"\n";
		switch(cvtarg(*argv))
		{
		case '-h': case 'h': case '/h':
		case '-H': case 'H': case '/H':
		case '-?': case '?': case '/?': help("");
			break;
		case '-l': case 'l': case '/l':
			**argv++; if((argc--) <= 1) help("Number of loops must be specified.\n");
			nLoops = atoi(*argv);
			break;
		case '-s': case 's': case '/s':
			**argv++; if((argc--) <= 1) help("Time delay between loops must be specified.\n");
			nSec = atoi(*argv);
			break;
		case '-f': case 'f': case '/f':
			**argv++; if((argc--) <= 1) help("No DSN/SP File specified.\n");
			ifs.open(*argv, ios::nocreate);
			if(!ifs.good())
				help("Could NOT open DSN/SP file.\n");
			break;
		default: cout <<(*argv) << "\n"; help("Invalid argument.\n");
			break;
		}  // end switch
	}  // end while
	
	cout << nSec << " second delay between loops" << endl;
	// read in the data for the dsn/sp's
	
	if(!ifs.good())
		help("Could NOT open DSN/SP file.\n");
	
	InitializeCriticalSection(&csCriticalSection);

	if(ifs.is_open())
	{
		TCHAR char256[256 + 1];
		
		if(ifs.getline(char256,256,'\n'))    // get # sp's
		{
			n = atoi(char256);
			EnterCriticalSection(&csCriticalSection); 
			cout << n << " Threads will be run."<< endl;
			LeaveCriticalSection(&csCriticalSection); 

		}
		
		if( n> 0)
		{
			pODBCstruct = new structODBC[n];
		}
		
		if(pODBCstruct != NULL)
		{
			int i;
			
			for(i = 0;i<n;i++)
			{
				pODBCstruct[i].strConnectString = NULL;
				pODBCstruct[i].strSP = NULL;
				pODBCstruct[i].pct = NULL;
			}
			
			for(i=0;i<n;i++)
			{
				if(ifs.getline(char256,256,'\n')) // get DSN
				{
					pODBCstruct[i].strConnectString = new TCHAR[strlen(char256) +1];
					if(pODBCstruct[i].strConnectString != NULL)
						strcpy(pODBCstruct[i].strConnectString,char256);
					else bAllocOK = FALSE;
				}
				
				if(ifs.getline(char256,256,'\n')) // get SP
				{
					pODBCstruct[i].strSP = new TCHAR[strlen(char256) +1];
					if(pODBCstruct[i].strSP != NULL)
						strcpy(pODBCstruct[i].strSP,char256);
					else bAllocOK = FALSE;
					
				}
			}
			
			ifs.close();
			
			if(bAllocOK)
			{
				for(int k=0;k< nLoops; k++)
				{	
					EnterCriticalSection(&csCriticalSection); 
					cout << "loop " << (k+1) << "/"<< nLoops << endl;
					LeaveCriticalSection(&csCriticalSection); 
					// allocate threads 
					
					for(int i=0;i<n;i++)
					{ 
						pODBC = new structODBC;
						if(pODBC != NULL)
						{
							pODBC->strConnectString = NULL;
							pODBC->strSP = NULL;
							pODBC->pct = NULL;
							pODBC->i = k;
							
							pODBC->strConnectString = new TCHAR[strlen(pODBCstruct[i].strConnectString) +1];
							if(pODBC->strConnectString != NULL)
							{
								strcpy(pODBC->strConnectString,pODBCstruct[i].strConnectString);
								
								pODBC->strSP = new TCHAR[strlen(pODBCstruct[i].strSP) +1];
								if(pODBC->strSP  != NULL)
									strcpy(pODBC->strSP ,pODBCstruct[i].strSP);
								else bAllocOK = FALSE;
							}
							else bAllocOK = FALSE;
							
							
							if(bAllocOK)
							{
								if(pODBC->pct = new CThread)
								{ 	  		   // create threads
									pcthread = pODBC->pct;
									
									if (pcthread->bCreate((PTHREAD_START_ROUTINE)dwSQLThread,(LPVOID)  pODBC))
									{
										if(pcthread->bStart())
										{
											//           cout << "Main::Started Thread "<< i <<" "<< pcthread->GetTID() <<endl;
										}
										else
										{
											EnterCriticalSection(&csCriticalSection); 
											cout << "Main::Could Not Start Thread " << i <<" "<< pcthread->dwGetTID() << endl;
											LeaveCriticalSection(&csCriticalSection); 
										}
										//            cout << "Main::Created Thread " << i <<" "<< pcthread->dwGetTID() << endl;
									} 
									else 
									{
										EnterCriticalSection(&csCriticalSection); 
										cout << "Main::Did Not Create Thread " << i << endl;
										LeaveCriticalSection(&csCriticalSection); 
									}
								} // end if new CThread
							} // end if (bAllocOK)
						} // end if (pODBC != NULL)
					}  // end for(int i=0;i<n;i++)
					Sleep(nSec*1000);
				} // end  for(int k=0;k< nLoops; k++)
				
			} // end if(bAllocOK)
		}
		else help ("Memory allocation error");
	}
	else  help("Must specify a file containing the sp's to execute.");

	cout << "Main::Press any key to exit." << endl;
	WaitForKeystroke();
	
	if( pODBCstruct != NULL)
	{ 
		for(int i = 0;i<n;i++)
		{
			if(pODBCstruct[i].strConnectString != NULL)
				delete [] (pODBCstruct[i].strConnectString);
			if(pODBCstruct[i].strSP != NULL)
				delete [] (pODBCstruct[i].strSP);
		}
		delete []pODBCstruct;
	}

	DeleteCriticalSection(&csCriticalSection);

	return 0;
} // main
