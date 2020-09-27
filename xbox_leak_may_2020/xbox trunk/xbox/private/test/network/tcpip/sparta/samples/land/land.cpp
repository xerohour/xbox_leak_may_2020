#include "SpartaApi.h"

int __cdecl main(int argc, TCHAR *argv[])
{

    BOOL lQuit = FALSE;
    int iCount  = 1;
	CHAR  szDestIPAddr[20], szSrcIPAddr[] = "157.55.34.163";
	USHORT usDestPort = 0;
    UCHAR testbuf[100];       // this is a random buffer which we'll send
    memset(testbuf,0xef,104); 
    _stprintf((TCHAR*)testbuf,TEXT("This is a test buffer for send from SPARTA"));


    if(argc > 2)
    {
        strcpy(szDestIPAddr, argv[1]);
		if(atoi(argv[2]) < 0)
			goto USAGE;

        usDestPort = (USHORT) atoi(argv[2]);
    }
	else
		goto USAGE;

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

		CTCPPacket TCPPacket(MediaType);

		CMacAddress* pDestMacAddr = AutoArp.ResolveIpAddress(szDestIPAddr);

        TCPPacket.MacHeader.SetDestAddress(*pDestMacAddr);
        TCPPacket.MacHeader.SetSrcAddress(*pDestMacAddr);
        TCPPacket.MacHeader.SetProtocolType(0x800);
		TCPPacket.IPHeader.SetVersion(4);
		TCPPacket.IPHeader.SetHdrLength(20);
		TCPPacket.IPHeader.SetTOS(0);
		TCPPacket.IPHeader.SetDatagramLength(44);
		TCPPacket.IPHeader.SetID(1000);
		TCPPacket.IPHeader.SetFlag(0);
		TCPPacket.IPHeader.SetFragOffset(0);
		TCPPacket.IPHeader.SetTTL(64);
		TCPPacket.IPHeader.SetProtocolType(6);
		TCPPacket.IPHeader.SetDestAddr(szDestIPAddr);
		TCPPacket.IPHeader.SetSrcAddr("127.0.0.1");
		TCPPacket.IPHeader.SetAutoCalcChecksum(ON);

		TCPPacket.TCPHeader.SetSrcPort(usDestPort);
		TCPPacket.TCPHeader.SetDestPort(usDestPort);

		TCPPacket.TCPHeader.SetSeqNumber(1000);
		TCPPacket.TCPHeader.SetAckNumber(0);
		TCPPacket.TCPHeader.SetReserved(0);
		TCPPacket.TCPHeader.SetFlag(2);
		TCPPacket.TCPHeader.SetWindowSize(17520);
		TCPPacket.TCPHeader.SetUrgentPointer(0);

		TCPPacket.TCPHeader.Options[0].AddMSS(1460);

		TCPPacket.TCPHeader.SetAutoCalcChecksum(ON);
		TCPPacket.TCPHeader.SetAutoCalcLength(ON);

        Interface.Send(TCPPacket);
    }
    catch(CSpartaException *Ex)
    {
        _tprintf(TEXT("EXCEPTION: %s\n"),Ex->GetMessage());
        delete Ex;
    }

    return 0;

USAGE:
	
	_tprintf(TEXT("Usage: land <IPAddr of listening host> <Port of listening host>\n"));

	return -1;
}

extern "C" void PrintDebug(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}

extern "C" void LogDebugMessage(TCHAR *strMessage)
{
   _tprintf(strMessage);		// change to redirect your output
}


