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

//
// FILE:      library\common\virtunit.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   30.03.95
//
// PURPOSE:   Virtual and physical units
//
// A physical unit represents a shared physical hardware or software
// resource.  Several virtual units try to use the same resource at a time,
// but only one gets access.
//
// When a virtual unit requests access, but the physical unit is currently
// busy by an other virtual unit, the previous unit gets preempted.  The
// virtual units are responsible for storing the current state of the
// physical unit, so it can be reactivated, when the virtual unit gets in
// charge again.
//
// A virtual unit may prevent preemption by two means, first by locking
// the physical unit or second by returning FALSE to inquisitive calls.
//
// A virtual unit may be in any of (3/2) states:
//
//    Generalstate: passive  P the virtual unit does not compete
//                  active   A the virtual unit is currently preempted
//                  current  C the virtual unit owns the physical unit
//
//    Lockstate:    unlocked U the physical unit is not locked by this
//                             virtual unit
//                  locked   L the physical unit is currently locked
//                             by this virtual unit
//
//
// State transition:
// none or (s) : success, (f) : failed
//
// Initial Transition    Final   Actions performed
// State                 state
// ---------------------------------------------------------------------------
// PU      Lock(s)       PL      current->PreparePreempt() -> OK
//         Lock(f)       PU      current->PreparePreempt() -> FAIL
//         Activate(s)   CU      current->MayPreempt() -> OK
//                               current->IsPreempted()
//                               IsActivated()
//                               IsScheduled()
//                               Preempt(current)
//         Activate(f)   AU      current->MayPreempt() -> FAIL
//                               IsActivated()
//
// PL      Lock          PL
//         Unlock        PU      current->UnpreparePreempt()
//         Activate      CL      current->IsPreempted()
//                               IsActivated()
//                               IsScheduled()
//                               Preempt(current)
//
// AU      Lock(s)       AL      current->PreparePreempt() -> OK
//         Lock(f)       AU      current->PreparePreempt() -> FAIL
//         Activate(s)   CU      current->MayPreempt() -> OK
//                               current->IsPreempted()
//                               IsScheduled()
//                               Preempt(current)
//         Activate(f)   AU      current->MayPreempt() -> FAIL
//         Passivate     PU      IsPassivated()
//
// AL      Lock          AL
//         Unlock        AU      current->UnpreparePreempt()
//         Activate      CL      current->IsPreempted()
//                               IsScheduled()
//                               Preempt(current)
//         Passivate     PL      IsPassivated()
//
// CU      Lock(s)       CL
//         Lock(f)       CU
//         Passivate     PU      IsPreempted()
//                               IsPassivated()
//                               new->IsScheduled()
//                               new->Preempt(this)
//
// CL      Unlock        CU
//         Passivate     PL      IsPreempted()
//                               IsPassivated()
//                               new->IsScheduled()
//                               new->Preempt(this)
//
//
// Activation/Passivation sequence for preemptable use:
//
//   Activate()
//   ....
//   Passivate()
//
// Activation/Passivation sequence for nonpreemptable use:
//
//   Lock()
//   Activate()
//   ....
//   Unlock()
//   Passivate()
//
// To nonpreemptably lock a sequence of units, you need a two phase protocol:
//
//   if (!IS_ERROR(unit1->Lock()))
//     if (!IS_ERROR(unit2->Lock()))
//        if (!IS_ERROR(unit3->Lock()))
//          {
//          unit1->Activate(); // must succeed
//          unit2->Activate();
//          unit3->Activate();
//          ...
//          unit1->Unlock();
//          unit2->Unlock();
//          unit3->Unlock();
//          unit1->Passivate();
//          unit2->Passivate();
//          unit3->Passivate();
//          }
//        else
//          {
//          unit2->Unlock();
//          unit1->Unlock();
//          }
//      else
//        unit1->Unlock;
//
//
// HISTORY:
//
//

#ifndef VIRTUNIT_H
#define VIRTUNIT_H

#include "prelude.h"
#include "gnerrors.h"
#include "Tags.h"
#ifndef ONLY_EXTERNAL_VISIBLE
#include "library/general/Lists.h"
#endif

#define GNR_CAN_NOT_PASSIVATE_IDLE_UNIT			MKERR(ERROR,	UNITS,	OBJECT,	0x01)

#define GNR_OBJECT_NOT_CURRENT						MKERR(ERROR,	UNITS,	OBJECT,	0x02)

#define GNR_OBJECT_ALREADY_JOINED					MKERR(ERROR,	UNITS,	OBJECT,	0x03)

#define GNR_UNITS_BUSY									MKERR(ERROR,	UNITS,	BUSY,		0x04)

#define GNR_INVALID_UNITS								MKERR(ERROR,	UNITS,	PARAMS,	0x05)

#ifndef ONLY_EXTERNAL_VISIBLE

class VirtualUnit;

class PhysicalUnit : public TagClass
	{
	friend class VirtualUnit;
	private:
		VirtualUnit *	current;
		VirtualUnit	*	lockedBy;
		VirtualUnit *	old, * rep;

		enum {preemptNone, preemptInitial, preemptStopped, preemptChanged, preemptComplete} preemptState;

		List				preempted;
		int				lockCnt;

		Error InternalActivate(VirtualUnit * unit);
		BOOL InternalMayPreempt(VirtualUnit * unit);

		Error InternalPreemptBegin(VirtualUnit * unit);

		Error InternalPreemptStopPrevious(void);
		Error InternalPreemptChange(void);
		Error InternalPreemptStartNew(void);

		Error InternalPreemptComplete(void);

		void InternalPreemptFail(void);

		Error InternalPassivate(VirtualUnit * unit);
	public:
		PhysicalUnit(void);
		virtual ~PhysicalUnit(void);

		//
		// A successful lock implies no problem with the
		// current unit during a later activation.
		//
		virtual Error Lock(VirtualUnit * unit);
		virtual Error Unlock(VirtualUnit * unit);

		virtual Error Activate(VirtualUnit * unit);
		virtual Error Passivate(VirtualUnit * unit);

		virtual VirtualUnit * CreateVirtual(void) = 0;

		virtual Error Initialize(void) {GNRAISE_OK;}
		virtual Error ReInitialize(void) {GNRAISE_OK;}

		VirtualUnit * GetCurrent(void) {return current;}
	};

class VirtualUnit : public TagClass, Node
	{
	friend class PhysicalUnit;
	public:
		enum State {passive, active, current};
	private:
		PhysicalUnit *	physical;
		State			 	state;

		BOOL				preemptVisited;

		VirtualUnit	 * predLink, * succLink;

		int				useCount;
	protected:
		//
		// the one for parameter only devices
		//
		virtual Error Preempt(VirtualUnit * previous);

      virtual Error PreemptStopPrevious(VirtualUnit * previous) {GNRAISE_OK;}
      virtual Error PreemptChange(VirtualUnit * previous) {return Preempt(previous);}
      virtual Error PreemptStartNew(VirtualUnit * previous) {GNRAISE_OK;}

		//
		// Positive response to PreparePreempt implies
		// positive response on may preempt at any time
		// until Preempt or UnpreparePreempt.
		//
		virtual BOOL PreparePreempt(void) {return TRUE;}
		virtual void UnpreparePreempt(void) {;}

      virtual BOOL MayPreempt(void) {return TRUE;}
		virtual Error IsPreempted(void) {GNRAISE_OK;}
		virtual Error IsScheduled(void) {GNRAISE_OK;}
		virtual Error IsActivated(void) {GNRAISE_OK;}
		virtual Error IsPassivated(void) {GNRAISE_OK;}

	public:
		VirtualUnit(PhysicalUnit *	physical);
		virtual ~VirtualUnit(void);

		PhysicalUnit *	GetPhysical (void) {return physical;}

		Error Activate(void);
		Error Passivate(void);

		Error Lock(void);
		Error Unlock(void);

		Error Join(VirtualUnit * unit);
		Error Split(VirtualUnit * unit);

		State GetState(void) {return state;}

		void Obtain() {useCount++;}
		void Release() {useCount--;if (!useCount) {Passivate(); delete this;}}
	};


#define PREEMPT_START(type)	\
	{ type *	p = (type *)previous;	\

#define PREEMPT_END	}

#define PREEMPT_UPDATE(param, update)	\
	if (!p || p->param != param) update;

#endif // External visible

#endif
