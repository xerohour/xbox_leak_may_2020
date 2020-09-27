#ifndef __SENDINPUT__

#include <afxwin.h>			// MFC core and standard components
#include <winperf.h>		// skipped in windows.h due to WIN32_LEAN_AND_MEAN
#include <afxext.h> 		// MFC extensions
#include <afxtempl.h>

#endif
//
// Note that the dwFlags field uses the same flags as keybd_event and 
// mouse_event, depending on what type of input this is.
//
typedef struct tagMOUSEINPUT {
    LONG    dx;
    LONG    dy;
    DWORD   mouseData;
    DWORD   dwFlags;
    DWORD   time;
    DWORD   dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, FAR* LPMOUSEINPUT;

typedef struct tagKEYBDINPUT {
    WORD    wVk;
    WORD    wScan;
    DWORD   dwFlags;
    DWORD   time;
    DWORD   dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, FAR* LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD   uMsg;
    WORD    wParamL;
    WORD    wParamH;
	DWORD	dwExtraInfo;
} HARDWAREINPUT, *PHARDWAREINPUT, FAR* LPHARDWAREINPUT;

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2


typedef struct tagINPUT {
    DWORD   type;

    union
    {
        MOUSEINPUT      mi;
        KEYBDINPUT      ki;
        HARDWAREINPUT   hi;
    };
} INPUT, *PINPUT, FAR* LPINPUT;

UINT
WINAPI
SendInput(
    UINT    cInputs,     // number of input in the array
    LPINPUT pInputs,     // array of inputs
    int     cbSize);     // sizeof(INPUT)

extern "C" typedef
UINT
(WINAPI *PSENDINPUT)(
    UINT    cInputs,     // number of input in the array
    LPINPUT pInputs,     // array of inputs
    int     cbSize);     // sizeof(INPUT)


typedef struct tagmyINPUT {
	    WORD    wVk;
		bool	bUp;
} myINPUT, *PmyINPUT, FAR* LPmyINPUT;

myINPUT   *rgInput;
int inputlength,inputalocated; //actual & allocated length of input stream
unsigned short altkeystate[3]; //stack of VK_CONTROL, VK_SHIFT and VK_MENU keys pressed
short altkeynum; //how many alt-keys deperessed
#define KEYDOWN 0

#ifdef _DEBUG
char szDebugOutput[1024];
#endif
//RETURN: FLASE - out of memory
bool emitscancode(unsigned short scancode, int keyup)
{
	myINPUT *pnext=rgInput+inputlength;
	if(inputlength!=0 && 
		(scancode==VK_CONTROL
		|| scancode==VK_MENU
		|| scancode==VK_SHIFT)
	)
	{//optimize out the collated altkeys (ex: VK_SHIFT-UP and VK_SHIFT-DOWN)
		myINPUT *pprev=pnext-1;
		if(pprev->/*ki.*/wVk==(short)scancode)
		{
			inputlength--; //optimized out!!!
#ifdef _DEBUG
			szDebugOutput[strlen(szDebugOutput)-4]=0; //erase the debug output
#endif
			return TRUE;
		}
	}
	if(inputalocated<=inputlength)
	{ //reallocate memory in 16-character increments
		if((rgInput=(myINPUT*)realloc(rgInput, (inputalocated+=32)*sizeof(myINPUT)))==NULL)
			return FALSE;
		pnext=rgInput+inputlength;
	}
//    pnext->type = INPUT_KEYBOARD;
    pnext->/*ki.dwFlags = keyup*/bUp = (keyup!=0);
//	pnext->ki.dwExtraInfo =  0; // don't use it: MAKELONG(0x9F, 0x14FE);
	pnext->/*ki.*/wVk = scancode;
//	pnext->ki.wScan =  0; // don't use it: 0x1D;
	inputlength++;
#ifdef _DEBUG
	char test[20];
	sprintf(test, "%X%sƒ", scancode, (keyup?"u":" "));
	strcat(szDebugOutput, test);
#endif
	return TRUE;
}

BOOL vk_altkeydepressed(unsigned short scan_code)
{
	int i;
	for(i=0;i<altkeynum;i++)
		if(altkeystate[i]==scan_code)
			return TRUE;
	return FALSE;
}

void emitchar(unsigned short scan_code, int repeats=1);
void emitchar(char inchar, int repeats=1);

void emitchar(unsigned short scan_code, int repeats)
{
	BOOL bUpcase=HIBYTE(scan_code);
	scan_code=LOBYTE(scan_code); //adjust scancode to ignore highbyte

	if(repeats<=0)
		return;

	if(bUpcase && !altkeynum) //altkeynum depressed supersedes the upcase
		emitscancode(VK_SHIFT, KEYDOWN);
	for(int i=0;i<repeats;i++)
	{
		emitscancode(scan_code, KEYDOWN);				
		emitscancode(scan_code, KEYEVENTF_KEYUP);
	}
	if(bUpcase && !altkeynum) //altkeynum depressed supersedes the upcase
		emitscancode(VK_SHIFT, KEYEVENTF_KEYUP);
}

void emitchar(char inchar, int repeats)
{
	unsigned short scan_code=VkKeyScan(inchar);
	emitchar(scan_code, repeats);
}


//spec_expr - function scans the special characters in {}
//PARAM - szKeys: input stream of chars
//PARAM - vk_altkey: state of the keaboard before {}
						//		-1	if '{' scanned
					   //VK_CONTROL	if '^{' scanned
					   //VK_MENU	if '%{' scanned
					   //VK_SHIFT	if '+{' scanned
//RETURN: the input stream position after having scanned the special char
const char *spec_expr(LPCSTR szKeys, unsigned short vk_altkey)
{ //scan the special character
//	_ASSERTE(*szKeys=='{');
	char *pclose_bra=strchr(++szKeys,'}');
	if(pclose_bra==NULL)
	{
		OutputDebugString("Error in expression, no close brace");
	}
	else
	{ //scan the special characters
		if(pclose_bra==szKeys)
		{ //the very next character (most likely "{}}" or "{} n}" sequence)
			pclose_bra++;
			if(*pclose_bra=='}' || (*pclose_bra==' ' && isdigit(pclose_bra[1])))
			{
				pclose_bra=strchr(pclose_bra,'}');
				if(pclose_bra==NULL)
				{ //"{}" or erroneous sentence; skip "{}", don't scan the rest
					szKeys++;
				}
			}
			else
			{ //"{}" sentence; skip "{}", don't scan the rest
				szKeys++;
				pclose_bra=NULL;
			}
		}
		if(pclose_bra!=NULL)
		{ //special characters from szKeys ending before pclose_bra
			unsigned short scan_code;
			if(toupper(*szKeys)=='F' && isdigit(szKeys[1]))
			{
				int fun_num=atoi(szKeys+1);
				scan_code=VK_F1+fun_num-1;
			}
			else if(_strnicmp(szKeys,"NUMPAD",strlen("NUMPAD"))==0)
			{
				if(isdigit(szKeys[strlen("NUMPAD")]))
				{
					int number=atoi(szKeys+strlen("NUMPAD"));
					scan_code=VK_NUMPAD0+number;
				}
				else if(szKeys[strlen("NUMPAD")]=='/')
					scan_code=VK_DIVIDE;
				else if(szKeys[strlen("NUMPAD")]=='*')
					scan_code=VK_MULTIPLY;
				else if(szKeys[strlen("NUMPAD")]=='-')
					scan_code=VK_SUBTRACT;
				else if(szKeys[strlen("NUMPAD")]=='+')
					scan_code=VK_ADD;
				else if(szKeys[strlen("NUMPAD")]=='.')
					scan_code=VK_DECIMAL; //?????
				else 
				{
					OutputDebugString("Error: nonexisting NUMPAD code");
					scan_code=0;
				}
			}
			else if(_strnicmp(szKeys,"LEFT",strlen("LEFT"))==0)
				scan_code=VK_LEFT;
			else if(_strnicmp(szKeys,"RIGHT",strlen("RIGHT"))==0)
				scan_code=VK_RIGHT;
			else if(_strnicmp(szKeys,"UP",strlen("UP"))==0)
				scan_code=VK_UP;
			else if(_strnicmp(szKeys,"DOWN",strlen("DOWN"))==0)
				scan_code=VK_DOWN;
			else if(_strnicmp(szKeys,"HOME",strlen("HOME"))==0)
				scan_code=VK_HOME;
			else if(_strnicmp(szKeys,"END",strlen("END"))==0)
				scan_code=VK_END;

			else if(_strnicmp(szKeys,"PGDN",strlen("PGDN"))==0)
				scan_code=VK_NEXT; //???????
			else if(_strnicmp(szKeys,"PGUP",strlen("PGUP"))==0)
				scan_code=VK_PRIOR;//???????
			else if(_strnicmp(szKeys,"TAB",strlen("TAB"))==0)
				scan_code=VK_TAB;
			else if(_strnicmp(szKeys,"ESC",strlen("ESC"))==0 || 
					_strnicmp(szKeys,"ESCAPE",strlen("ESCAPE"))==0 )
				scan_code=VK_ESCAPE;
			else if(_strnicmp(szKeys,"ENTER",strlen("ENTER"))==0)
				scan_code=VK_RETURN;
			else if(_strnicmp(szKeys,"DEL",strlen("DEL"))==0 || 
					_strnicmp(szKeys,"DELETE",strlen("DELETE"))==0 )
				scan_code=VK_DELETE;
			else if(_strnicmp(szKeys,"INSERT",strlen("INSERT"))==0)
				scan_code=VK_INSERT;
			else if(_strnicmp(szKeys,"BS",strlen("BS"))==0 || 
					_strnicmp(szKeys,"BACKSPACE",strlen("BACKSPACE"))==0 )
				scan_code=VK_BACK;
			else if(_strnicmp(szKeys,"CLEAR",strlen("CLEAR"))==0)
				scan_code=VK_CLEAR;
			else if(_strnicmp(szKeys,"HELP",strlen("HELP"))==0)
				scan_code=VK_HELP;
			else if(_strnicmp(szKeys,"BREAK",strlen("BREAK"))==0)
				scan_code=VK_PAUSE;
			else if(_strnicmp(szKeys,"PRTSC",strlen("PRTSC"))==0)
				scan_code=VK_PRINT;
			else //CAPSLOCK, NUMLOCK, SCROLLLOCK not supported
			{
				scan_code=VkKeyScan(*szKeys);
			}
			char *pspace;
			int repeat=1;
			if((pspace=strchr(szKeys,' '))!=NULL &&
				pspace<pclose_bra)
			{
				if(isdigit(pspace[1]))
					repeat=atoi(pspace+1);
			}
			emitchar(scan_code, repeat);
			szKeys=pclose_bra+1;
		}
	}
	if((short)vk_altkey>0)
	{
		emitscancode(vk_altkey, KEYEVENTF_KEYUP);
		if(altkeynum)
			altkeynum--;
	}
	return szKeys;
} // spec_expr

#define	VK_QUEUESYNC  ((SHORT) -1)	// our user-defined virtual key code for queuesync
HWND g_hwndTarget;int current_char=0;
LRESULT CALLBACK JournalPlaybackProc(int code,WPARAM wParam,LPARAM lParam);
BOOL GetNextChar(SHORT* pVirtKey,/*SHORT *pVirtScan,*/BOOL* pKeyDown)
{
	
	if(current_char>=inputlength)
	{
		current_char=-1; //set to -1 meaning VK_QUEUESYNC was sent
		*pVirtKey = VK_QUEUESYNC;
		*pKeyDown = TRUE;
		return true;
	}
	if(current_char<0)
	{
		current_char=0;
		return false; //don't set anything ele after VK_QUEUESYNC
	}
	*pVirtKey = rgInput[current_char].wVk;
//	*pVirtScan = rgInput[current_char].pVScan;
	*pKeyDown = rgInput[current_char].bUp == 0;
	current_char++;
	return true;
}

void DoKeyshwnd(HWND handle,LPCSTR szKeys, BOOL bLiteral =FALSE);
void DoKeyshwnd(HWND handle,LPCSTR szKeys, BOOL bLiteral /*=FALSE*/)
{
	inputalocated=4*strlen(szKeys); //should be enough for most cases
    if((rgInput=(myINPUT*)malloc(inputalocated*sizeof(myINPUT)))==NULL)
		return;
#ifdef _DEBUG
	szDebugOutput[0]='"';
	strcat(strcpy(szDebugOutput+1, szKeys),"\"ƒ");
	if(bLiteral)strcat(szDebugOutput, "-Literal-ƒ");
#endif
	inputlength=0; //length of the input table filled

	altkeynum=0; //how many alt-keys deperessed
	while(*szKeys!=0)
	{
		if(bLiteral)
			emitchar(*szKeys++);
		else
		switch(*szKeys)
		{
			unsigned short vk_altkey;
			case')':
				if(altkeynum)
				{
					emitscancode(altkeystate[--altkeynum], KEYEVENTF_KEYUP);
				}
				else
					emitchar(')');
				szKeys++;
				break;
			case '{':
				szKeys=spec_expr(szKeys, -1); //special expression without keystate
				break;
			case '~':
				emitchar((unsigned short)VK_RETURN);
				szKeys++;
				break;
			case '^':
			case '%':
			case '+':
//				unsigned short vk_altkey;
				if(*szKeys=='^')vk_altkey=VK_CONTROL;
				if(*szKeys=='%')vk_altkey=VK_MENU;
				if(*szKeys=='+')vk_altkey=VK_SHIFT;
				if(!vk_altkeydepressed(vk_altkey))
				{
					szKeys++;
					emitscancode(vk_altkey, KEYDOWN);
					altkeystate[altkeynum++]=vk_altkey;
					if(*szKeys=='(' || *szKeys=='{')
					{ //scan the subexpression (altkeystate remain pushed)
						if(*szKeys=='{')
							szKeys=spec_expr(szKeys, vk_altkey);
						else
							szKeys++;
					}
					else if(*szKeys)
					{ //emit key and pop altkeystate
						emitchar(*szKeys++);
						emitscancode(vk_altkey, KEYEVENTF_KEYUP);
						altkeynum--;
					}
				}
				else
				{
					emitchar(*szKeys);
					szKeys++;
				}
				break;
			default: //any character
				emitchar(*szKeys++);
				break;

		} //switch(*szKeys)
	} //while
	if(altkeynum>0)
	{
		OutputDebugString("Error in expression, altkeys not depressed");
		while(altkeynum>0)
			emitscancode(altkeystate[--altkeynum], KEYEVENTF_KEYUP);
	}
//altkeystate should be empty
//send input with allocated & filled rgInput
	if(handle && SetForegroundWindow(handle)==0)
		OutputDebugString("Failure setting the foreground window");
#ifdef _DEBUG
	char number[15];
	sprintf(number, "len:%d\n", inputlength);
	strcat(szDebugOutput, number);
	OutputDebugString(szDebugOutput);
#endif
	HMODULE hUser32=::GetModuleHandle("USER32");
	PSENDINPUT pSendInput = (PSENDINPUT)::GetProcAddress(hUser32, "SendInput");
	if(pSendInput==NULL)
		OutputDebugString("Failure getting SendInput from USER32");
	else
	{
	//	(*pSendInput)(inputlength, rgInput,sizeof(INPUT));
		/*
		for(int nevent=0; nevent<inputlength; nevent++)
		{
			keybd_event((unsigned char)rgInput[nevent].wVk, 0, 
				rgInput[nevent].bUp?KEYEVENTF_KEYUP:0, 0);
		}
		*/
		g_hwndTarget=handle; //TODO: no fucken way to avoid it with WH_JOURNALPLAYBACK, must protect it with a mutex
		current_char=0; //TODO: as above

		int ret;
		HHOOK hJournalPlaybackHook = SetWindowsHookEx(WH_JOURNALPLAYBACK,
		(HOOKPROC)JournalPlaybackProc,GetModuleHandle(NULL), 0);
		ret = (hJournalPlaybackHook!=NULL);
		while(ret>0)
		{
			MSG msg;
			ret = GetMessage(&msg,NULL,0,0);

			if (msg.message == WM_CANCELJOURNAL)
			{
				PostQuitMessage(0);
			}
			else
			{
				TranslateMessage(&msg);
				DefWindowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			}
		}
		if(hJournalPlaybackHook!=NULL)
			UnhookWindowsHookEx(hJournalPlaybackHook);
	}

	free(rgInput);

} //DoKeyshwnd

LRESULT CALLBACK JournalPlaybackProc(int code,WPARAM wParam,LPARAM lParam)
{
	static SHORT	nVirtKey = 0;
	static BOOL		bKeyDown = FALSE;
	PEVENTMSG		pEvent;
	static bool		bDelay = false;

	if (bDelay == false)			// first time
	{
		BYTE			baKeyStates[256];
		if(g_hwndTarget) //todo: maybe avoid setting when it's already set but force setting (even when called with NULL) if it lost focus for whatever reason
			SetForegroundWindow(g_hwndTarget);
		

		GetKeyboardState(baKeyStates);
		baKeyStates[VK_CONTROL] &= 0x7F;
		baKeyStates[VK_MENU] &= 0x7F;
		baKeyStates[VK_SHIFT] &= 0x7F;
		baKeyStates[VK_CAPITAL] &= 0x7E;
		baKeyStates[VK_NUMLOCK] &= 0x7E;
		SetKeyboardState(baKeyStates);
		bDelay = true;
	}

	switch (code)
	{
	case HC_SKIP:
		pEvent = (PEVENTMSG)lParam;
		if (GetNextChar(&nVirtKey,&bKeyDown) == FALSE)
		{
			PostQuitMessage(0);
		}
		break;

	case HC_GETNEXT:
//		if(g_hwndTarget) //todo: maybe avoid setting when it's already set but force setting (even when called with NULL) if it lost focus for whatever reason
//			SetForegroundWindow(g_hwndTarget);
		pEvent = (PEVENTMSG)lParam;
		pEvent->time = GetTickCount();
		if(nVirtKey == VK_QUEUESYNC)
		{
			pEvent->message = WM_QUEUESYNC;
			pEvent->paramL = 0; //(UINT)g_hwndMessagWindow;
			pEvent->paramH = 0;
		}
		else
		{
			pEvent->message = bKeyDown ? WM_KEYDOWN : WM_KEYUP;
			pEvent->paramL = nVirtKey;
			pEvent->paramH = MapVirtualKey(nVirtKey,1);
		}
		break;

	}
	return 0;
}


//this is the test code
#ifndef __SENDINPUT__


BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
	int		nLen = GetWindowTextLength(hwnd) + 1;
	char*	pszWindowText = new char[nLen];
	if (GetWindowText(hwnd,pszWindowText,nLen))
	{
		if (strstr(pszWindowText,(char*)lParam) != NULL)
		{
			g_hwndTarget = hwnd;
			delete [] pszWindowText;
			return FALSE;
		}
	}
	delete [] pszWindowText;
	return TRUE;
}



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{

	EnumWindows((WNDENUMPROC)EnumWindowsProc, (DWORD)"Notepad");
	if (g_hwndTarget != NULL)
	{
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget,"HA");
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget,"%HA");
		Sleep(1000);
		DoKeyshwnd(GetActiveWindow(),"{ENTER}");
		Sleep(1000);
		DoKeyshwnd(NULL, "The test line contains %d{$$$}",TRUE);
		Sleep(1000);
		DoKeyshwnd(NULL,"{ENTER}");
	}

	g_hwndTarget=0;
	EnumWindows((WNDENUMPROC)EnumWindowsProc,(DWORD)"Microsoft Visual C++ - [Text2]");
	if (g_hwndTarget != NULL)
	{
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget,"{ESC}");
		Sleep(1000);
		DoKeyshwnd(NULL,"%Wl");
		Sleep(1000);
		DoKeyshwnd(NULL,"%Fn");
	}

	g_hwndTarget=0;
	EnumWindows((WNDENUMPROC)EnumWindowsProc,(DWORD)"Microsoft Visual C++ [break]");
	if (g_hwndTarget != NULL)
	{
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F10}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F10}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F10}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "{F5}", FALSE);
		Sleep(1000);
		DoKeyshwnd(g_hwndTarget, "%DD", FALSE);
	}

	return TRUE;
}
#endif // __SENDINPUT__
