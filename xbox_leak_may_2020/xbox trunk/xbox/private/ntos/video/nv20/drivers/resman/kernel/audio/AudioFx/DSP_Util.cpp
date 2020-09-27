// ****************************************************************
//
//	DSP_Util:		DSP Execution Support routines
//
//	Written by Gints Klimanis
//
// ****************************************************************
#include "DSP_Util.h"

// ****************************************************************
// DefaultCommandNode:		
// **************************************************************** 
	void
DefaultCommandNode(DSPCOMMANDNODE *d)
{
d->commandID     =  kDSPCommandID_IgnoreMe;		
d->stateID       = (kDSPStateID_ModuleListStopped | kDSPStateID_WaitingForCommand);			
d->moduleIndex = 0;	

d->moduleCount = 0;	

d->dspModuleBlock      = 0;
d->dspModuleStateBlock = 0;
d->dspModuleCodeBlock  = 0;

d->dspModuleBlockSize      = 0;
d->dspModuleStateBlockSize = 0;
d->dspModuleCodeBlockSize  = 0;
}	// ---- end DefaultCommandNode() ---- 

// ****************************************************************
// DefaultDSPModule:		
// **************************************************************** 
	void
DefaultDSPModule(DSPMODULE *d)
{
d->state   = 0;
d->compute = 0;
d->typeID  = 2; //kAudioFxModule_Null;

}	// ---- end DefaultDSPModule() ---- 

