//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\stack.h"

#include "tncontrl.h"
#include "main.h"

#include "comm.h"
#include "periodicq.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNPeriodicSend::CTNPeriodicSend()"
//==================================================================================
// CTNPeriodicSend constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNPeriodicSend object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNPeriodicSend::CTNPeriodicSend(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNPeriodicSend));

	this->m_pCtrlComm = NULL;
	this->m_dwInterval = 0;
	this->m_dwFirstSent = 0;
	this->m_dwLastSent = 0;
	this->m_dwTimeout = 0;
} // CTNPeriodicSend::CTNPeriodicSend
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNPeriodicSend::~CTNPeriodicSend()"
//==================================================================================
// CTNPeriodicSend destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNPeriodicSend object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNPeriodicSend::~CTNPeriodicSend(void)
{
	HRESULT		hr;


	//DPL(0, "this = %x", 1, this);

	if (this->m_pCtrlComm != NULL)
	{
		this->m_pCtrlComm->m_dwRefCount--;
		if (this->m_pCtrlComm->m_dwRefCount == 0)
		{
			DPL(7, "Releasing and deleting control comm object %x.",
				1, this->m_pCtrlComm);

			hr = this->m_pCtrlComm->Release();
			if (hr != S_OK)
			{
				DPL(0, "Releasing control communication method failed!  %e", 1, hr);
			} // end if (releasing control method failed)

			delete (this->m_pCtrlComm);
		} // end if (last reference)
		else
		{
			DPL(7, "Not releasing and deleting control comm object %x, its refcount is %u.",
				2, this->m_pCtrlComm, this->m_pCtrlComm->m_dwRefCount);
		} // end else (not last reference)

		this->m_pCtrlComm = NULL;
	} // end if (an address was allocated)
} // CTNPeriodicSend::~CTNPeriodicSend
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

