#include "SpartaApi.h"

int __cdecl main(int argc, TCHAR *argv[])
{

    BOOL lQuit = FALSE;
    int iCount  = 1;
	CHAR  szDestIPAddr[20], szSrcIPAddr[] = "157.55.34.163";
    UCHAR testbuf[100];       // this is a random buffer which we'll send
    memset(testbuf,0xef,104); 
    _stprintf((TCHAR*)testbuf,TEXT("This is a test buffer for send from SPARTA"));


    if(argc > 1)
    {
        strcpy(szDestIPAddr, argv[1]);
    }

    try
    {
        CInterfaceList InterfaceList;

        if(InterfaceList.GetCount() == 1)
        {
            _tprintf(TEXT("Opening Interface: %s\n"),InterfaceList.GetNameAt(0));
        }
        else
        {
            _tprintf(TEXT("For this test it is recommended to unbind all but one of these adapters:\n"));
            for(DWORD i = 0; i < InterfaceList.GetCount(); i++)
            {
                _tprintf("%s  ",InterfaceList.GetNameAt(i));
            }
            _tprintf(TEXT("\n"));
            return 1;
        }

        CAutoArp AutoArp(InterfaceList.GetNameAt(0), szSrcIPAddr);

        CInterface Interface(InterfaceList.GetNameAt(0));

        MAC_MEDIA_TYPE MediaType = Interface.GetMediaType();

		CIPPacket IPPacket(MediaType);

        IPPacket.MacHeader.SetDestAddress(*(AutoArp.ResolveIpAddress(szDestIPAddr)));
        IPPacket.MacHeader.SetSrcAddress(*Interface.GetMediaAddress());
        IPPacket.MacHeader.SetProtocolType(0x800);
		IPPacket.IPHeader.SetVersion(4);
		IPPacket.IPHeader.SetHdrLength(20);
		IPPacket.IPHeader.SetTOS(0);
		IPPacket.IPHeader.SetDatagramLength(124);
		IPPacket.IPHeader.SetFlag(1);
		IPPacket.IPHeader.SetFragOffset(50);
		IPPacket.IPHeader.SetTTL(64);
		IPPacket.IPHeader.SetProtocolType(6);

		IPPacket.IPHeader.SetDestAddr(szDestIPAddr);
		IPPacket.IPHeader.SetSrcAddr(szSrcIPAddr);
		IPPacket.IPHeader.SetAutoCalcChecksum(ON);

        IPPacket.SetUserBuffer(testbuf,104,TRUE);

        for(USHORT usID = 1000; usID < 2000; usID++)
        {
			IPPacket.IPHeader.SetID(usID);
            Interface.Send(IPPacket);
        }
    }
    catch(CSpartaException *Ex)
    {
        _tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
        delete Ex;
    }

    return 0;
}

extern "C" void PrintDebug(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}

extern "C" void LogDebugMessage(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}


