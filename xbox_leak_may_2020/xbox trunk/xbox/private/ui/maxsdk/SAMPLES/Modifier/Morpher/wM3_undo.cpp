/*===========================================================================*\
 | 
 |  FILE:	wM3_undo.cpp
 |			Weighted Morpher for MAX R3
 |			RestoreObj for deleting morph channels
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 17-2-99
 | 
\*===========================================================================*/

#include "wM3.h"



/*===========================================================================*\
 | MCRestore Class
\*===========================================================================*/

// Constructor
MCRestore::MCRestore(MorphR3 *mpi, int idx) 
{ 
	mp = mpi;
	mcIndex = idx;
	undoMC = mp->chanBank[idx];
}


// Called when Undo is selected
void MCRestore::Restore(int isUndo) 
{

	// This is NOT an undo operation ... we shouldn't get any of these
	// (but if we do, I'd like to know about it <g>)
	if (!isUndo) assert(0);

	mp->chanBank[mcIndex] = undoMC;

	// Update the rollups and the viewports
	mp->Update_channelFULL();
	mp->Update_channelParams();

	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	mp->ip->RedrawViews(mp->ip->GetTime());
}



// Called when Redo is selected
// since my undo-er is only for deleting channels
// if you redo, you're deleting a channel. So i do that here.
void MCRestore::Redo() 
{
	mp->chanBank[mcIndex].ResetMe();

	// Reassign paramblock info
	ParamBlockDescID *channelParams = new ParamBlockDescID[1];

	ParamBlockDescID add;
	add.type=TYPE_FLOAT;
	add.user=NULL;
	add.animatable=TRUE;
	add.id=1;
	channelParams[0] = add;

	mp->MakeRefByID(FOREVER, 1+mcIndex, CreateParameterBlock(channelParams,1,1));	
	assert(mp->chanBank[mcIndex].cblock);

	Control *c = (Control*)CreateInstance(CTRL_FLOAT_CLASS_ID,GetDefaultController(CTRL_FLOAT_CLASS_ID)->ClassID());

	mp->chanBank[mcIndex].cblock->SetValue(0,0,0.0f);
	mp->chanBank[mcIndex].cblock->SetController(0,c);

	delete channelParams;

	// Update the rollups and the viewports
	mp->Update_channelFULL();
	mp->Update_channelParams();

	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	mp->ip->RedrawViews(mp->ip->GetTime());
}


// Called to return the size in bytes of this RestoreObj
int MCRestore::Size() {
	return sizeof(MCRestore);
}
