/**********************************************************************
 *<
	FILE: reactor.h

	DESCRIPTION: Header file for Reactor Controller

	CREATED BY: Adam Felt

	HISTORY:

 *>	Copyright (c) 1998-1999 Adam Felt, All Rights Reserved.
 **********************************************************************/

#ifndef __REACTOR__H
#define __REACTOR__H


#include "Max.h"
#include "resource.h"
#include "KbdShortcut.h"
#include "Maxscrpt.h"
#include "definsfn.h"

extern ClassDesc* GetFloatReactorDesc();
extern ClassDesc* GetPositionReactorDesc();
extern ClassDesc* GetPoint3ReactorDesc();
extern ClassDesc* GetRotationReactorDesc();
extern ClassDesc* GetScaleReactorDesc();

extern HINSTANCE hInstance;

TCHAR *GetString(int id);

//-----------------------------------------------



//--------------------------------------------------------------------------

// Keyboard Shortcuts stuff
const ShortcutTableId kReactorShortcuts = 0x6bd55e20;

#define NumElements(array) (sizeof(array) / sizeof(array[0]))

ShortcutTable* GetShortcuts();

template <class T>
class ReactorShortcutCB : public ShortcutCallback
{
	public:
		T*		reactor;
				ReactorShortcutCB(T *reactor) { this->reactor = reactor; }
		BOOL	KeyboardShortcut(int id); 
};


template <class T>
BOOL ReactorShortcutCB<T>::KeyboardShortcut(int id)
{
	switch (id)
	{
	case ID_MIN_INFLUENCE:
		reactor->setMinInfluence();
		break;
	case ID_MAX_INFLUENCE:
		reactor->setMaxInfluence();
		break;
	case ID_CREATE_REACTION:
		reactor->CreateReaction();
		break;
	case ID_DELETE_REACTION:
		reactor->DeleteReaction();
		break;
	case ID_SET_VALUE:
		reactor->setReactionValue();
		break;
	case ID_EDIT_STATE:
		reactor->editing = (reactor->editing == FALSE ? TRUE : FALSE);  //toggle the editing state
		reactor->dlg->iEditBut->SetCheck(reactor->editing);
		reactor->setEditing(reactor->editing);
		break;
	}
	return TRUE;
}

#endif // __REACTOR__H
