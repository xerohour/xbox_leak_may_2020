/*===========================================================================*\
 | 
 |  FILE:	Utility.h
 |			Skeleton project and code for a Utility
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 9-2-99
 | 
\*===========================================================================*/

#ifndef __RFXUTIL__H
#define __RFXUTIL__H

#include "max.h"
#include "iparamm2.h"
#include "utilapi.h"

#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SKUTIL_CLASSID		Class_ID(0x6eb31258, 0x362b6298)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonUtilDesc();



/*===========================================================================*\
 |	SkeletonUtility class defn
\*===========================================================================*/

class SkeletonUtility : public UtilityObj {
	public:

		IUtil *iu;
		Interface *ip;

		// Windows handle of our UI
		HWND hPanel;
		
		//Constructor/Destructor
		SkeletonUtility();
		~SkeletonUtility();

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);
		void DeleteThis() {}

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
};

static SkeletonUtility theSkeletonUtility;

#endif