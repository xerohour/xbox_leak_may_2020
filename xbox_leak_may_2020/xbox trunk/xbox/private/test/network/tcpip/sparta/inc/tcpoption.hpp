/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: TCPOptions.hpp                                                              *
* Description: This is the definition of the TCP Option class                           *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       balasha      6/15/2000    created                               *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_TCPOPTION_H__
#define __SPARTA_TCPOPTION_H__

#include "TCP.h"
#include "Packets.h"


class CTCPOption
{

friend class CTCPHeader;

protected:

   BOOL    IsActive;
   UCHAR   Kind;
   USHORT  Length;
   UCHAR * m_TCPOption;
   
  public:
   
   CTCPOption();
   CTCPOption(const UCHAR Kind);
   ~CTCPOption();

   SPARTA_STATUS  AddEOO();
   SPARTA_STATUS  AddNoOp();
   SPARTA_STATUS  AddMSS(USHORT MSSValue);
   SPARTA_STATUS  AddWs(UCHAR WsFactor);
   SPARTA_STATUS  AddTs(ULONG TSValue, ULONG TSEcho);
   SPARTA_STATUS  AddSackP();
   SPARTA_STATUS  AddSack();
   SPARTA_STATUS  AddRaw(UCHAR LocalKind, UCHAR LocalLength, UCHAR * Buffer);
   
   SPARTA_STATUS  SetLength(UCHAR LocalLength);
   SPARTA_STATUS  SetKind(UCHAR LocalKind);
   
   SPARTA_STATUS  SetSackValue(USHORT Position, ULONG Value);
   SPARTA_STATUS  SetTSValue(ULONG Value);
   SPARTA_STATUS  SetTSEcho(ULONG Value);
   SPARTA_STATUS  SetMSSValue(USHORT Value);
   SPARTA_STATUS  SetWSFactor(UCHAR Value);
   
   
   SPARTA_STATUS  Remove();
   BOOLEAN        Print();
   
   ULONG  GetSackValue(USHORT Position);
   
   ULONG  GetTSValue();
   ULONG  GetTSEcho();
   USHORT GetMSSValue();
   UCHAR  GetWSFactor();
   
   
   
   
   
   UCHAR  GetKind();
   UCHAR  GetLength();
};

#endif //__SPARTA_TCPPOPTION_H__

