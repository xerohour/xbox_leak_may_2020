//depot/xbox-aug01-final/private/ui/dvd/driver/softwarecinemaster/ClosedCaption/ClosedCaptionDecoder.cpp#4 - edit change 19203 (text)
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "ClosedCaptionDecoder.h"
#include "library\common\vddebug.h"

#define		DISABLE_CC		0
#define		TIME_CC			1

extern "C"
{
void    WINAPI D3DDevice_EnableCC(BOOL Enable);
void    WINAPI D3DDevice_SendCC(BOOL Field, BYTE cc1, BYTE cc2);
void    WINAPI D3DDevice_GetCCStatus(BOOL *pField1, BOOL *pField2);
};

// Interval between CGMS XDS packets

#define     CGMS_XDS_PACKET_INTERVAL    1000

bool ClosedCaptionDecoder::DecodeCCCommand(bool initial, BYTE cc0, BYTE cc1)
	{

	// wmp - very temp - disable sending cc.
#if DISABLE_CC
	return true;
#else

	int field1, field2;
	int timeout;

	if ((initial == LINE21_FIELD_CLOSEDCAPTION) && (cc0 == 0x80) && (cc1 == 0x80))
		return true;

	timeout = timer->CurrentTime() + 200;

    do
	    {
        D3DDevice_GetCCStatus(&field1, &field2);
 		if ((!field1) || (!field2))
			{
			lock.Leave();
			YieldTimedFiber(2);
			lock.Enter();
			}
        }
        while ((!field1 || !field2) && running && (cc_ok = (timeout > (timer->CurrentTime()))) );

	if (!cc_ok)
		DP("CC Status timed out");

//	char buffer[100];
//	wsprintf(buffer, "cc0:%x %c   cc1:%x %c\n", cc0, cc0, cc1, cc1);
//	OutputDebugString(buffer);


    D3DDevice_SendCC(initial, cc0, cc1);

	return true;
#endif DISABLE_CC
	}

void ClosedCaptionDecoder::FiberRoutine(void)
	{
	int time;

	lock.Enter();
	while (!terminate)
		{
		if (running && first != last && cc_ok)
			{
			if (buffer[first].pos != buffer[first].size)
				{
				time = timer->CurrentTime() + 300;
				while (first != last && time > buffer[first].displayTime && running)
					{
#if TIME_CC && _DEBUG
					int t1 = timeGetTime();
#endif
					DecodeCCCommand(buffer[first].field, buffer[first].data[buffer[first].pos], buffer[first].data[buffer[first].pos + 1]);
#if TIME_CC && _DEBUG
					int t2 = timeGetTime();
					if (t2 - t1 > 100)
						DP("ERROR CC took to long %d", t2-t1);
#endif

					buffer[first].pos += 2;
					buffer[first].displayTime += 16;

					if (buffer[first].pos == buffer[first].size)
						{
						first = (first + 1) & 63;
						}
					}

				}
			else
				{
				first = (first + 1) & 63;
				}

			if (first != last)
				{
				lock.Leave();
				Wait(&event, buffer[first].displayTime, FALSE);
				lock.Enter();
				}
			}
		else
			{
			lock.Leave();
			event.Wait(CGMS_XDS_PACKET_INTERVAL);
			lock.Enter();
			}

        // We're required to periodically send a Copy Generation Management System (CGMS)
        // packet through line 21. The priority, however, is the caption data. If the channel
        // is completely available, send the packet.

        if( first == last &&
            (!iNextCGMSPacket || 
            timer->CurrentTime() > iNextCGMSPacket))
            {

            // Send packet

            lock.Leave();
            SendCGMSPacket();
            lock.Enter();

            // Schedule next packet
            
            iNextCGMSPacket = timer->CurrentTime() + CGMS_XDS_PACKET_INTERVAL;
            }
        
		}
	lock.Leave();
	}

void ClosedCaptionDecoder::SendLine21Data(int displayTime, BYTE * data, int size)
	{
	lock.Enter();
	if (streaming && size)
		{
        buffer[last].field = LINE21_FIELD_CLOSEDCAPTION;
		buffer[last].displayTime = displayTime;
		buffer[last].size = size;
		buffer[last].pos = 0;

		memcpy(buffer[last].data, data, size);

		last = (last + 1) & 63;
		event.SetEvent();
		}
	lock.Leave();
	}

void ClosedCaptionDecoder::SendLine21DataEx(bool field, int displayTime, BYTE * data, int size)
	{
	lock.Enter();
	if (streaming && size)
		{
        buffer[last].field = field;
		buffer[last].displayTime = displayTime;
		buffer[last].size = size;
		buffer[last].pos = 0;

		memcpy(buffer[last].data, data, size);

		last = (last + 1) & 63;
		event.SetEvent();
		}
	lock.Leave();
	}


ClosedCaptionDecoder::ClosedCaptionDecoder(TimingSlave * timer)
	: TimedFiber(9), TimingClient(timer), event(FALSE, TRUE)
	{
	this->timer = timer;
	terminate = false;
	streaming = true;
	running = false;
	first = last = 0;
	iMacrovisionLevel = 0;
	eCGMSMode = VCPMD_COPYING_PERMITTED;
	iNextCGMSPacket = 0;
	}

ClosedCaptionDecoder::~ClosedCaptionDecoder(void)
	{
	}

void ClosedCaptionDecoder::BeginStreaming(void)
	{
	if (!streaming)
		{
		cc_ok = true;
#if !DISABLE_CC
		D3DDevice_EnableCC(TRUE);
#endif DISABLE_CC
		terminate = false;
		streaming = true;
		running = false;
		first = last = 0;
    	iNextCGMSPacket = 0;
		StartFiber();
		}
	}

void ClosedCaptionDecoder::EndStreaming(void)
	{
	if (streaming)
		{
		terminate = true;
		event.SetEvent();
		CompleteFiber();
		streaming = false;
        
        Reset();
		}
	}

void ClosedCaptionDecoder::StartStreaming(void)
	{
	cc_ok = true;
	running = true;
	iNextCGMSPacket = 0;
	event.SetEvent();
	}

void ClosedCaptionDecoder::StopStreaming(void)
	{
	running = false;
	}

void ClosedCaptionDecoder::Reset()
    {
#if !DISABLE_CC
    	int field1, field2;
    	int timeout;

    	timeout = timer->CurrentTime() + 200;

        do
    		{
    		D3DDevice_GetCCStatus(&field1, &field2);
    			if ((!field1) || (!field2))
    			{
    			YieldTimedFiber(2);
    			}
    		}
    		while ((!field1 || !field2) && running && (cc_ok = (timeout > (timer->CurrentTime()))) );

    	if (!cc_ok)
    		DP("CC Status timed out");

    	// clear CC data
        D3DDevice_SendCC(LINE21_FIELD_CLOSEDCAPTION, 0x94, 0x2c);

    	D3DDevice_EnableCC(FALSE);
#endif DISABLE_CC
    }    

//
// ClosedCaptionDecoder::SendCGMSPacket
// Parameters: none
// Retuns: void
// Sends a Copy Generation Management System packet through line 21
//
void ClosedCaptionDecoder::SendCGMSPacket()
    {

    __declspec(align(1)) struct
        {
        BYTE bPacketStartCode;
        BYTE bPacketType;

        union
            {
            BYTE bCGMSInfo;

            struct 
                {
                unsigned char uAnalogSource : 1;
                unsigned char uMacrovisionLevel : 2;
                unsigned char uCGMSLevel : 2;
                unsigned char uBit5 : 1;
                unsigned char uBit6 : 1;
                unsigned char uBit7 : 1;
                } info;
            };

        BYTE bPlaceHolder;

        BYTE bPacketEndCode;
        BYTE bCheckSum;
        } CGMSPacket;

    CGMSPacket.bPacketStartCode = 0x01;
    CGMSPacket.bPacketType = 0x08;
    CGMSPacket.info.uAnalogSource = 0;
    CGMSPacket.info.uMacrovisionLevel = iMacrovisionLevel;

    switch(eCGMSMode)
        {

        case VCPMD_ONE_COPY_PERMITTED:

            // One generation of copies may be made

            CGMSPacket.info.uCGMSLevel = 2;
            break;

        case VCPMD_NO_COPYING_PERMITTED:

            // No copying is permitted

            CGMSPacket.info.uCGMSLevel = 3;
            break;
        

        case VCPMD_COPYING_PERMITTED:
        default:

              // Copying is permitted without restriction

              CGMSPacket.info.uCGMSLevel = 0;
              break;
        }

    // The rest of the bits by the spec

    CGMSPacket.info.uBit5 = 0;
    CGMSPacket.info.uBit6 = 1;
    CGMSPacket.info.uBit7 = 0;

    CGMSPacket.bPlaceHolder = 0;
    CGMSPacket.bPacketEndCode = 0x0F;

    // Extracted from the spec:
    // "The Checksum Data Byte represents the 7-bit binary number necessary for the sum 
    // of the Start and Type characters, all of the following informational characters 
    // plus the End and Checksum characters to equal zero (i.e. the two’s complement of 
    // the sum of the information characters plus the Start, Type and End characters)" 

    CGMSPacket.bCheckSum = ((CGMSPacket.bPacketStartCode + CGMSPacket.bPacketType + 
        +CGMSPacket.bCGMSInfo + CGMSPacket.bPacketEndCode) ^ 0x7F) + 1;

    // The packet is formed. Send it to line 21.

    SendLine21DataEx(LINE21_FIELD_XDS, timer->CurrentTime(), (BYTE*)&CGMSPacket, sizeof(CGMSPacket));

/*
    BYTE* pBytes = (BYTE*) &CGMSPacket;

    DP("Sending CGMS packet, size=%d -- %x, %x, %x, %x, %x, %x", sizeof(CGMSPacket),
        pBytes[0], pBytes[1], pBytes[2], pBytes[3], pBytes[4], pBytes[5]);
*/       

    }
    
// ClosedCaptionDecoder::SetMacrovisionLevel
// Parameters:
//   iMacrovisionLevel - New macrovision level
// Returns: void
// This function is called to notify this object that the 
// macrovision level has changed (for the CGMS packet).

void ClosedCaptionDecoder::SetMacrovisionLevel(int iMacrovisionLevel) 
    { 
    if(this->iMacrovisionLevel != iMacrovisionLevel)
        {

        // New macrovision level

	    this->iMacrovisionLevel = iMacrovisionLevel; 

        // Send the CGMS packet as soon as possible
        
	    this->iNextCGMSPacket = 0;
        }
    }

// ClosedCaptionDecoder::SetCGMSMode
// Parameters:
//   eCGMSMode - New CGMS mode
// Returns: void
// This function is called to notify this object that the 
// CGMS mode has changed (for the CGMS packet).

void ClosedCaptionDecoder::SetCGMSMode(VideoCopyMode eCGMSMode) 
    { 
    if(this->eCGMSMode != eCGMSMode)
        {

        // New CGMS mode

	    this->eCGMSMode = eCGMSMode; 

        // Send the CGMS packet as soon as possible

	    this->iNextCGMSPacket = 0;
	    }
    }


