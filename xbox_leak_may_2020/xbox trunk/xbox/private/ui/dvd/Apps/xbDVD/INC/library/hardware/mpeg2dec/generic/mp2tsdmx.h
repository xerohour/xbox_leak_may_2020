// FILE:			library\hardware\mpeg2dec\generic\mp2tsdmx.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		19.12.96
//
// PURPOSE:		MPEG-2 Transport Stream Demultiplexer
//
// HISTORY:


#ifndef MP2TSDMX_H
#define MP2TSDMX_H

#include "..\..\..\common\prelude.h"
#include "..\..\..\general\lists.h"
#include "..\..\..\general\asncstrm.h"
#include "mp2eldec.h"


///////////////////////////////////////////////////////////////////////////////
// General Section Receiver
///////////////////////////////////////////////////////////////////////////////

class SectionReceiver
	{
	protected:
		DWORD	shiftReg;
	public:
		SectionReceiver() {;}
		virtual void Reset()  = 0;
		virtual DWORD SendData(HPTR data, DWORD size) = 0;
		virtual void CRCAddByte(BYTE b);
	};

///////////////////////////////////////////////////////////////////////////////
// Program Association Table Receiver
///////////////////////////////////////////////////////////////////////////////

class ProgramAssociation : public Node
	{
	public:
		WORD	number;		// Program Number
		WORD	pid;			// PID of Program Map Table
		
		ProgramAssociation(WORD number, WORD pid) {this->number = number; this->pid = pid;}
	};

class PAList : public List
	{
	public:
		PAList() {;}
		~PAList();
		
		void DeleteAll();

		void InsertPA(ProgramAssociation * pa);
		WORD FindPA(WORD number);					// Delivers PID for a certain program Number. Values > 0x1fff signal error.
	};

class PATReceiver : public SectionReceiver
	{
	protected:
		int	state;	                      
		DWORD muxPos;
		BOOL	resync;
		
		WORD	sectionLength;

		WORD	streamID;
		int	patVersion, lastVersion;
		BOOL	currentNext;
		BYTE	sectionNumber;
		BYTE	lastSectionNumber;
		WORD	entries;
		WORD	curProgNum;
		WORD	curPID;

		PAList * tempPAList;
		PAList * paList;

		void	InitTempProgramAssoc();
		void	AddTempProgramAssoc(WORD curProgramNum, WORD curPID);
		void	ValidateTempProgramAssoc();
		void	UseTempProgramAssoc();
		void	DiscardTempProgramAssoc();

	public:
		PATReceiver();
		~PATReceiver();
	   DWORD SendData(HPTR data, DWORD size);
		void	Reset(void);
		
		int	NumPrograms();		// Delivers number of programs
		PAList * GetPAList();
	};


///////////////////////////////////////////////////////////////////////////////
// Program Map Table Receiver
///////////////////////////////////////////////////////////////////////////////

class PMTReceiver : public SectionReceiver
	{
	protected:
		int	state;	                      
		DWORD muxPos;
		BOOL	resync;
		
		WORD	sectionLength;
   public:
		PMTReceiver();
	   DWORD SendData(HPTR data, DWORD size);
		void	Reset(void);
	};

///////////////////////////////////////////////////////////////////////////////
// Transport Stream Demux (Main Part)
///////////////////////////////////////////////////////////////////////////////

class MPEG2TransportStreamDemux
	{        
	protected:                             
		PATReceiver			*	patHandler;
		SectionReceiver	*	mapHandler;
		
		int	state;	                      
		
		WORD	cnt;
		DWORD	length; 
		BYTE 	high, low;    
		DWORD muxPos;

		BOOL	transportError;
		BOOL	payloadUnitStart;
		WORD	pid;
		BYTE	scramblingCtrl;
		BYTE	continuityCounter;
		BYTE	adaptionFieldCtrl;
		DWORD	packetRemain;
		DWORD	fieldRemain;
		DWORD adaptionFieldSize;

		BYTE	pointer;

		WORD	programMapPID;
	public:
		MPEG2TransportStreamDemux(PATReceiver * patHandler, SectionReceiver * mapHandler);
	                
		void Reset(void);
		
		DWORD SendData(HPTR data, DWORD size);
		void CompleteData(void);              
	};





#endif
