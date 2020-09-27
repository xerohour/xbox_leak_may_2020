// FILE:			library\hardware\panel\specific\vestel3button.h
// AUTHOR:		M Spinnenweber
// COPYRIGHT:	(c) 2000 RAVISENT TECHNOLOGIES.  All Rights Reserved.
// CREATED:		APR.18.2000
//
// PURPOSE:		Vestel 3 Button Panel
//
// HISTORY:
//


#ifndef VESTEL3BUTTONPANEL_H
#define VESTEL3BUTTONPANEL_H


#include "..\generic\panels.h"
#include "library\common\VDDEBUG.h"
#include "library\hardware\uproc\specific\st55xx\cpu5505.h"
#include "embedded\library\generic\cemsg.h"


#define VESTEL3BUTTON_PANEL_BUFFER_SIZE			                                 64		  // size of the stack
#define VESTEL3BUTTON_PANEL_POLLING_INTERVAL                                     100000  // 20000us = 100ms

#define VESTEL3BUTTON_PANEL_DEBOUNCE_TIME                                        2       // Time in polling intervals


class Vestel3ButtonPanel : public PollingPanel
	{
	private:
      GPIOBitInputPort*    switch1;
      GPIOBitInputPort*    switch2;
      GPIOBitInputPort*    switch3;

      BOOL                 initialized;

      DWORD                switch1DebounceTimeRemaining;
      DWORD                switch2DebounceTimeRemaining;
      DWORD                switch3DebounceTimeRemaining;

	protected:

           BOOL     PollCEMessage         ( DWORD &ceMsg );

	public:
                    Vestel3ButtonPanel ( int taskPriority, GPIOBitInputPort* pswitch1,GPIOBitInputPort* pswitch2,GPIOBitInputPort* pswitch3 );
                   ~Vestel3ButtonPanel ( void );
      POPList       GetInitPOPList     ( int __far & size );
      void          ExecuteOperation   (POP pop);


      VirtualUnit*  CreateVirtual      ( void );
      void          SetSystemPower     ( BOOL on );
   

	};


class VirtualVestel3ButtonPanel : public VirtualPanel
	{
	public:
		VirtualVestel3ButtonPanel(Vestel3ButtonPanel* panel) : VirtualPanel(panel) {;}
	};


#endif

