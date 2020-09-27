/*===========================================================================*\
  How To Create a System Plug-In

  FILE: RingArry.cpp
 
  DESCRIPTION: Ring Array System Plug-in sample code.

  HISTORY: created November 11 1994

  Copyright (c) 1994, All Rights Reserved.
\*===========================================================================*/
#include "max.h"
#include "resource.h"
#include "props.h"
#include "dummy.h"
#include "Simpobj.h"

// This is the DLL instance handle passed in when the plug-in is 
// loaded at startup.
HINSTANCE hInstance;

// This function returns a pointer to a string in the string table of
// the resource library.  Note that this function maintains the buffer
// and that only one string is loaded at a time.  Therefore if you intend
// to use this string, you must copy to another buffer since it will 
// be overwritten on the next GetString() call.
TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

// Unique Class ID.  It is specified as two 32-bit quantities.
// These must be unique.  Use the Class_ID generator program to create
// these values.  This program is available from the main menu of the
// help file.
#define RINGARRAY_CLASS_ID Class_ID(0x7f934bfa, 0x44a104c6)
#define SLAVE_CONTROL_CLASS_ID Class_ID(0x3e2c2cce, 0x2fd431f)

// Parameter block indices
#define PB_RAD	0
#define PB_CYC	1
#define PB_AMP	2
#define PB_PHS	3

//----------------------------------------------------------------------

class RingMaster: public ReferenceTarget {
	public:
		// Object parameters
		IParamBlock *pblock;
		// Table of nodes in the ring array.
		Tab<INode*> nodeTab;
		// The number of nodes in the ring array
		int numNodes;
		RingMaster();

		// This method returns the ith slave node.
		INode* GetSlaveNode(int i) { return nodeTab[i]; }
		// This method stores the ith slave node.
		void SetSlaveNode(int i, INode * node);

		// The following methods set and get values from the parameter block
		void SetNum(TimeValue t, int n); 
		void SetRad(TimeValue t, float r); 
		void SetCyc(TimeValue t, float r); 
		void SetAmp(TimeValue t, float r); 
		void SetPhs(TimeValue t, float r); 
		int GetNum(TimeValue t, Interval& valid = Interval(0,0) ); 	
		float GetRad(TimeValue t, Interval& valid = Interval(0,0) ); 	
		float GetCyc(TimeValue t, Interval& valid = Interval(0,0) ); 	
		float GetAmp(TimeValue t, Interval& valid = Interval(0,0) ); 	
		float GetPhs(TimeValue t, Interval& valid = Interval(0,0) ); 	

		// The slave controllers call this method to retireve their
		// value.  This allows the master object to control the entire
		// system.
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method, int id);
		// This makes sure that the spinner controls track when animating 
		// and in Motion Panel.
		void UpdateUI(TimeValue t);

		// Class variables
		static HWND hMasterParams;
		static IObjParam *iObjParams;
		static int dlgNum;
		static float dlgRadius;
		static float dlgAmplitude;
		static float dlgCycles;
		static float dlgPhase;
		static ISpinnerControl *numSpin;
		static ISpinnerControl *radSpin;
		static ISpinnerControl *ampSpin;
		static ISpinnerControl *cycSpin;
		static ISpinnerControl *phsSpin;

		// From Animatable

		// --- Inherited virtual methods from Animatable ---
		int NumSubs()  { return 1; }
		Animatable* SubAnim(int i) { return pblock; }
		TSTR SubAnimName(int i) { return GetString(IDS_DS_RINGARRAYPAR);}		
		Class_ID ClassID() { return RINGARRAY_CLASS_ID; }  
		SClass_ID SuperClassID() { return SYSTEM_CLASS_ID; }  
		void GetClassName(TSTR& s) { s = GetString(IDS_DB_RING_ARRAY_CLASS); }
		void DeleteThis() { delete this; }		
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next );
		void GetSystemNodes(INodeTab &nodes);

		// --- Inherited virtual methods from from ReferenceMaker/Target ---
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		int NumRefs() { return 1;	};
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
	};

// This method returns a new instance of the slave controller.
Control* GetNewSlaveControl(RingMaster *master, int i);

//------------------------------------------------------

// Initialize the class variables...
HWND RingMaster::hMasterParams = NULL;
IObjParam *RingMaster::iObjParams;

int RingMaster::dlgNum = 	4;
float RingMaster::dlgRadius = 	100.0f;
float RingMaster::dlgAmplitude = 20.0f;
float RingMaster::dlgCycles = 	3.0f;
float RingMaster::dlgPhase = 	1.0f;

ISpinnerControl *RingMaster::numSpin;
ISpinnerControl *RingMaster::radSpin;
ISpinnerControl *RingMaster::ampSpin;
ISpinnerControl *RingMaster::cycSpin;
ISpinnerControl *RingMaster::phsSpin;


RingMaster::RingMaster() {
	// The parameter block descriptor defines the type of value represented
	// by the parameter (int, float, Color...) and if it is animated or not.

	// This class requires these values to be initialized:
	// { - ParameterType, 
	//   - Not Used, must be set to NULL, 
	//   - Flag which indicates if the parameter is animatable,
	//  }
	ParamBlockDesc desc[] = {
		{ TYPE_FLOAT, NULL, TRUE },
		{ TYPE_FLOAT, NULL, TRUE },
		{ TYPE_FLOAT, NULL, TRUE },
		{ TYPE_FLOAT, NULL, TRUE }
		};
	// Create a parameter block and make a reference to it.
	MakeRefByID( FOREVER, 0, CreateParameterBlock( desc, 4 ) );	
	// Set the initial values at time 0.
	SetRad( TimeValue(0), dlgRadius );
	SetCyc( TimeValue(0), dlgCycles );
	SetAmp( TimeValue(0), dlgAmplitude );
	SetPhs( TimeValue(0), dlgPhase );
	numNodes = dlgNum;
	}

// This method is called to return a copy of the ring master.
RefTargetHandle RingMaster::Clone(RemapDir& remap) {
	int i;
    RingMaster* newm = new RingMaster();	
	newm->ReplaceReference(0,pblock->Clone(remap));
	newm->numNodes = numNodes;
	newm->nodeTab.SetCount(numNodes);
	for (i=0; i<numNodes; i++) newm->nodeTab[i] = NULL;			
	for (i=0; i<numNodes; i++) {
		remap.PatchPointer((RefTargetHandle*)&newm->nodeTab[i],(RefTargetHandle)nodeTab[i]);
		}
	return(newm);
	}

// This method is called to update the UI parameters to reflect the
// correct values at the time passed.  Note that FALSE is passed as
// the notify parameter.  This ensure that notification message are
// not sent when the values are updated.
void RingMaster::UpdateUI(TimeValue t)
	{
	if ( hMasterParams ) {
		radSpin->SetValue( GetRad(t), FALSE );
		ampSpin->SetValue( GetAmp(t), FALSE );
		cycSpin->SetValue( GetCyc(t), FALSE );
		phsSpin->SetValue( GetPhs(t), FALSE );
		numSpin->SetValue( GetNum(t), FALSE );
		}
	}

// This method is called to set the ith slave node for the ring master.
// If additional space is required in the node table it is allocated.
void RingMaster::SetSlaveNode(int i, INode * node) {
	if (i>=nodeTab.Count()) {
		int nold = nodeTab.Count();
		nodeTab.SetCount(i+1);
		if (i+1>numNodes) 
			numNodes = i+1;
		for (int j = nold; j<i+1; j++) nodeTab[j] = NULL;
		}		
	nodeTab[i] = node;
	}

// The master controller of a system plug-in should implement this 
// method to give MAX a list of nodes that are part of the system.   
// The master controller should fill in the given table with the 
// INode pointers of the nodes that are part of the system. This 
// will ensure that operations like cloning and deleting affect 
// the whole system.  MAX will use GetInterface() in the 
// tmController of each selected node to retrieve the master 
// controller and then call GetSystemNodes() on the master 
// controller to get the list of nodes.
void RingMaster::GetSystemNodes(INodeTab &nodes)
	{
	for (int i=0; i<nodeTab.Count(); i++) {
		nodes.Append(1,&nodeTab[i]);
		}
	}

// This methods returns the ith reference - there is only one, the 
// parameter block.
RefTargetHandle RingMaster::GetReference(int i)  { 
	if (i==0) return pblock;
	return NULL;
	}

// This methods sets the ith reference - there is only one, the 
// parameter block.
void RingMaster::SetReference(int i, RefTargetHandle rtarg) {
	if (i==0)
		pblock = (IParamBlock *)rtarg; 
	}		

#define TWO_PI 6.283185307f

// ======= This method is the crux of the system plug-in ==========
// It takes an input (parent) matrix and modifies it according to the id
// passed in.  This id represents the position in the ring array system
// of the node that is having it's position set by this method.
// 
// The slave controllers that set the position and orientation of the 
// ring array nodes call this method in their implementation of the
// method Control::GetValue().
void RingMaster::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method, int id) {
	float radius,amplitude,cycles, phase;

	// Retrieve the values of the UI parameters at the time passed in.
	radius = GetRad(t,valid);
	amplitude = GetAmp(t,valid);
	cycles = GetCyc(t,valid);
	phase = GetPhs(t,valid);
	Matrix3 tmat, *mat = (Matrix3*)val;
	tmat.IdentityMatrix();

	// Calculate the Z angle rotation for the nodes based on the number
	// of nodes in the ring array and the id of the node passed in.
	float ang = float(id)*TWO_PI/(float)numNodes;

	// Calculate the translation of the node based on the id and the UI
	// parameters.
	tmat.Translate(Point3(radius,0.0f,amplitude*(float)cos(cycles*ang + TWO_PI*phase)));
	tmat.RotateZ(ang);
	
	// There are two ways of updating a value; relative or absolute.
	// If the value is relative the calculated position is applied to
	// the input value using matrix multiplication.  If the method
	// is absolute then the value is simply set to the calculated value.
	(*mat) = (method==CTRL_RELATIVE) ? tmat*(*mat) : tmat;

	// Make sure spinners track when animating and in Motion Panel
	UpdateUI(t);
	}

// This is the method that recieves change notification messages
RefResult RingMaster::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
     PartID& partID, RefMessage message ) 
    {
	switch (message) {
		case REFMSG_GET_PARAM_DIM: { 
			// The ParamBlock needs info to display in the track view.
			// This message is sent to retrieve the dimension of the 
			// parameter block value.  The dimension represnets the 
			// type and order of magnitude of the parameter.
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {
				case PB_RAD:	gpd->dim = stdWorldDim; break;	  
				case PB_CYC:	gpd->dim = stdWorldDim; break;	  
				case PB_AMP:	gpd->dim = stdWorldDim; break;	  
				case PB_PHS:	gpd->dim = stdWorldDim; break;	  
					break;									
				}
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			// The ParamBlock needs info to display in the track view
			// This message is sent to retrieve the name of the parameter
			// whose index is passed.
			GetParamName *gpn = (GetParamName*)partID;
			switch (gpn->index) {
				case PB_RAD: gpn->name = GetString(IDS_DS_RADIUS);	break;
				case PB_CYC: gpn->name = GetString(IDS_DS_CYCLES);	break;
				case PB_AMP: gpn->name = GetString(IDS_DS_AMPLITUDE);	break;
				case PB_PHS: gpn->name = GetString(IDS_DS_PHASE);	break;
				}
			return REF_STOP; 
			}
		case REFMSG_TARGET_DELETED:{
			// This message is sent if one of the nodes in the ring array
			// is deleted.
			for (int i=0; i<nodeTab.Count(); i++) {
				if (hTarget==nodeTab[i]) {
					nodeTab[i] = NULL;				
					break;
					}					
				}
			}
			return REF_STOP;
		}
	return(REF_SUCCEED);
	}

//--------------------------------------------------
// This is the restore object used when the user executes an undo or redo
class RingNumRestore: public RestoreObj {
	RingMaster *rng;
	int num;
	int redNum;
	public:
		RingNumRestore(RingMaster *ringm, int n) { rng = ringm; num = n; }
		~RingNumRestore() {}
		void Restore(int isUndo);
		void Redo();
		TSTR Description() { return TSTR("RingNumRestore"); }
	};

// This method restores the MAX data base to the state it was when theHold.Put()
// was called.
void RingNumRestore::Restore(int isUndo) {
	redNum = rng->nodeTab.Count();
	rng->nodeTab.SetCount(num);
	if (num>redNum) {
		for (int i=redNum; i<num; i++)
			rng->nodeTab[i] = NULL;
		}
	rng->numNodes = num;
	if (rng->hMasterParams) 
		RingMaster::numSpin->SetValue(rng->numNodes, FALSE );
	rng->NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

// This method is called when the user selects the Redo command.  The developer 
// should restore the database to the state prior to the last Undo command.
void RingNumRestore::Redo() {	
	rng->nodeTab.SetCount(redNum);
	if (redNum>rng->numNodes) {
		for (int i=num; i<redNum; i++)
			rng->nodeTab[i] = NULL;
		}
	rng->numNodes = redNum;
	if (rng->hMasterParams) 
	    RingMaster::numSpin->SetValue(rng->numNodes, FALSE );
	rng->NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

//--------------------------------------------------
// This method is called to set the number of nodes in the ring array.
// It handles the allocation or deallocation of nodes in the node table.
void RingMaster::SetNum(TimeValue t, int n) { 
	if (n==numNodes) return;
	if (n<1) return;
	int oldNumNodes = numNodes;
	if (nodeTab.Count()>0) {
		if (n<numNodes) {
			// remove nodes;
			for (int i=numNodes-1; i>=n; i--) {
				INode* node = nodeTab[i];
				if (node) {
					if (node->Selected()) {
						// Dont want to delete selected nodes
						n = i+1;
						numSpin->SetValue( n, TRUE );
						break;
						}
					node->Delete(t,TRUE);
					nodeTab[i] = NULL;
					}
				numNodes = i;
				}
			nodeTab.SetCount(n);
			}
		else {
			nodeTab.SetCount(n);
			for (int i=numNodes; i<n; i++)
				nodeTab[i] = NULL;
			// add nodes;
			if (nodeTab[0]==NULL) {
				n++;
				numSpin->SetValue( n, TRUE );
				}
			else for (i=numNodes; i<n; i++) {
				Object * obj = nodeTab[0]->GetObjectRef();
				assert(obj);
				INode *newNode = iObjParams->CreateObjectNode(obj);
				Control* slave = GetNewSlaveControl(this,i);
				newNode->SetTMController(slave);
				newNode->FlagForeground(t,FALSE);
				INode *par = nodeTab[0]->GetParentNode();
				assert(par);
			    par->AttachChild(newNode);
				SetSlaveNode(i,newNode);
				}
			}
		}
	if (theHold.Holding())
		theHold.Put(new RingNumRestore(this, oldNumNodes));
	numNodes = n;
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

// This method retireves the number of nodes in the ring array.
int RingMaster::GetNum(TimeValue t, Interval& valid ) { 	
	return numNodes;
	}


//--------------------------------------------------
// The following methods set and get the radius value in the parameter block.
void RingMaster::SetRad(TimeValue t, float r) { 
	pblock->SetValue( PB_RAD, t, r );
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

float RingMaster::GetRad(TimeValue t, Interval& valid ) { 	
	float f;
	pblock->GetValue( PB_RAD, t, f, valid );
	return f;
	}

//--------------------------------------------------
// The following methods set and get the cycles value in the parameter block.
void RingMaster::SetCyc(TimeValue t, float r) { 
	pblock->SetValue( PB_CYC, t, r );
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

float RingMaster::GetCyc(TimeValue t, Interval& valid ) { 	
	float f;
	pblock->GetValue( PB_CYC, t, f, valid );
	return f;
	}

//--------------------------------------------------
// The following methods set and get the amplitude value in the 
// parameter block.
void RingMaster::SetAmp(TimeValue t, float r) { 
	pblock->SetValue( PB_AMP, t, r );
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

float RingMaster::GetAmp(TimeValue t, Interval& valid ) { 	
	float f;
	pblock->GetValue( PB_AMP, t, f, valid );
	return f;
	}

//--------------------------------------------------
// The following methods set and get the phase value in the 
// parameter block.
void RingMaster::SetPhs(TimeValue t, float r) { 
	pblock->SetValue( PB_PHS, t, r );
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

float RingMaster::GetPhs(TimeValue t, Interval& valid ) { 	
	float f;
	pblock->GetValue( PB_PHS, t, f, valid );
	return f;
	}

//--------------------------------------------------
// This is the dialog proc used to manage the user interaction with the
// custom controls in the user interface.  For more details on the 
// stucture and messages of a dialog proc see the Advanced Topics 
// section on Custom Controls.
BOOL CALLBACK MasterParamDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	RingMaster *mc = (RingMaster *)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !mc && message != WM_INITDIALOG ) return FALSE;
	
	assert(mc->iObjParams);
	switch ( message ) {
		case WM_INITDIALOG:
			mc = (RingMaster *)lParam;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)mc );
			SetDlgFont( hDlg, mc->iObjParams->GetAppHFont() );
			
			mc->radSpin  = GetISpinner(GetDlgItem(hDlg,IDC_RADSPINNER));
			mc->cycSpin  = GetISpinner(GetDlgItem(hDlg,IDC_CYCSPINNER));
			mc->ampSpin  = GetISpinner(GetDlgItem(hDlg,IDC_AMPSPINNER));
			mc->phsSpin  = GetISpinner(GetDlgItem(hDlg,IDC_PHSSPINNER));
			mc->numSpin  = GetISpinner(GetDlgItem(hDlg,IDC_NUMSPINNER));

			mc->radSpin->SetLimits( 0.0f, 500.0f, FALSE );
			mc->cycSpin->SetLimits( 0.0f, 10.0f, FALSE );
			mc->ampSpin->SetLimits( 0.0f, 500.0f, FALSE );
			mc->phsSpin->SetLimits( -1000.0f, 1000.0f, FALSE );
			mc->numSpin->SetLimits( 1, 200, FALSE );

			mc->radSpin->SetScale(float(0.1) );
			mc->ampSpin->SetScale(float(0.1) );
			mc->phsSpin->SetScale(float(0.1) );
			mc->numSpin->SetScale(float(0.1) );

			mc->radSpin->SetValue( mc->GetRad(mc->iObjParams->GetTime()), FALSE );
			mc->cycSpin->SetValue( mc->GetCyc(mc->iObjParams->GetTime()), FALSE );
			mc->ampSpin->SetValue( mc->GetAmp(mc->iObjParams->GetTime()), FALSE );
			mc->phsSpin->SetValue( mc->GetPhs(mc->iObjParams->GetTime()), FALSE );
			mc->numSpin->SetValue( mc->GetNum(mc->iObjParams->GetTime()), FALSE );

			mc->radSpin->LinkToEdit( GetDlgItem(hDlg,IDC_RADIUS), EDITTYPE_POS_UNIVERSE );			
			mc->cycSpin->LinkToEdit( GetDlgItem(hDlg,IDC_CYCLES), EDITTYPE_FLOAT );			
			mc->ampSpin->LinkToEdit( GetDlgItem(hDlg,IDC_AMPLITUDE), EDITTYPE_FLOAT );			
			mc->phsSpin->LinkToEdit( GetDlgItem(hDlg,IDC_PHASE), EDITTYPE_FLOAT );			
			mc->numSpin->LinkToEdit( GetDlgItem(hDlg,IDC_NUMNODES), EDITTYPE_INT );			
			
			return FALSE;	// DB 2/27

		case WM_DESTROY:
			ReleaseISpinner( mc->radSpin );
			ReleaseISpinner( mc->cycSpin );
			ReleaseISpinner( mc->ampSpin );
			ReleaseISpinner( mc->phsSpin );
			ReleaseISpinner( mc->numSpin );
			mc->radSpin = NULL;
			mc->cycSpin = NULL;
			mc->ampSpin = NULL;
			mc->phsSpin = NULL;
			mc->numSpin = NULL;
			return FALSE;

		case CC_SPINNER_CHANGE:
			if (!theHold.Holding()) theHold.Begin();
			switch ( LOWORD(wParam) ) {
				case IDC_RADSPINNER: mc->SetRad(mc->iObjParams->GetTime(),  mc->radSpin->GetFVal() );  break;
				case IDC_CYCSPINNER: mc->SetCyc(mc->iObjParams->GetTime(),  mc->cycSpin->GetFVal() );  break;
				case IDC_AMPSPINNER: mc->SetAmp(mc->iObjParams->GetTime(),  mc->ampSpin->GetFVal() );  break;
				case IDC_PHSSPINNER: mc->SetPhs(mc->iObjParams->GetTime(),  mc->phsSpin->GetFVal() );  break;
				case IDC_NUMSPINNER: mc->SetNum(mc->iObjParams->GetTime(),  mc->numSpin->GetIVal() );  break;
				}
			assert(mc->iObjParams);
			mc->iObjParams->RedrawViews(mc->iObjParams->GetTime(), REDRAW_INTERACTIVE, mc);
			return TRUE;

		case CC_SPINNER_BUTTONDOWN:
			theHold.Begin();
			return TRUE;

		case CC_SPINNER_BUTTONUP:
			if (HIWORD(wParam) || message==WM_CUSTEDIT_ENTER) theHold.Accept(GetString(IDS_DS_PARAMCHG));
			else theHold.Cancel();
			mc->iObjParams->RedrawViews(mc->iObjParams->GetTime(), REDRAW_END, mc);
			return TRUE;

		case WM_MOUSEACTIVATE:
			mc->iObjParams->RealizeParamPanel();
			return FALSE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			mc->iObjParams->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;

		case WM_COMMAND:			
			return FALSE;

		default:
			return FALSE;
		}
	}

// This method is called when the ring masters parameters may be edited
// in the motion branch of the command panel.  
void RingMaster::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	iObjParams = ip;
	
	if ( !hMasterParams ) {
		hMasterParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_SAMPLEPARAM),
				MasterParamDialogProc,
				GetString(IDS_RB_PARAMETERS), 
				(LPARAM)this );		
		ip->RegisterDlgWnd(hMasterParams);
		
	} else {
		SetWindowLong( hMasterParams, GWL_USERDATA, (LONG)this );		

		// Init the dialog to our values.
		radSpin->SetValue(GetRad(ip->GetTime()),FALSE);
		cycSpin->SetValue(GetCyc(ip->GetTime()),FALSE);
		ampSpin->SetValue(GetAmp(ip->GetTime()),FALSE);
		phsSpin->SetValue(GetPhs(ip->GetTime()),FALSE);
		numSpin->SetValue(GetNum(ip->GetTime()),FALSE);
		}
	}
		
// This method is called when the user is finished editing the ring masters
// parameters in the command panel.		
void RingMaster::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	if (hMasterParams==NULL) 
		return;
	dlgRadius   = radSpin->GetFVal();
	dlgAmplitude   = ampSpin->GetFVal();
	dlgCycles   = cycSpin->GetFVal();
	dlgPhase   = phsSpin->GetFVal();
	dlgNum   = numSpin->GetIVal();
	
	if ( flags&END_EDIT_REMOVEUI ) {		
		ip->UnRegisterDlgWnd(hMasterParams);
		ip->DeleteRollupPage(hMasterParams);
		hMasterParams = NULL;
		}
	else {		
		SetWindowLong( hMasterParams, GWL_USERDATA, 0 );
		}
	
	iObjParams = NULL;
	}

#define NUMNODES_CHUNK 0x100
#define NODE_ID_CHUNK 0x110

// This method is called to allow the plug-in to save any data it needs
// to the MAX file.  Here we store the number of nodes and the node ids.
// See the Advanced Topics on Loading and Saving for more details.
IOResult RingMaster::Save(ISave *isave) {
	ULONG nb;
	isave->BeginChunk(NUMNODES_CHUNK);
	isave->Write(&numNodes,sizeof(numNodes), &nb);
	isave->EndChunk();
	if (numNodes>0) {
		isave->BeginChunk(NODE_ID_CHUNK);
		for (int i=0; i<numNodes; i++) {
			ULONG id = isave->GetRefID(nodeTab[i]);
			isave->Write(&id,sizeof(ULONG), &nb);
			}
		isave->EndChunk();
		}
	return IO_OK;
	}

// This method is called to allow the plug-in to load it's data from the 
// MAX file. 
IOResult RingMaster::Load(ILoad *iload) {
	ULONG nb;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case NUMNODES_CHUNK: {
				res = iload->Read(&numNodes,sizeof(numNodes), &nb);
				nodeTab.SetCount(numNodes);
				for (int i=0; i<numNodes; i++) nodeTab[i] = NULL;
				}
				break;
			case NODE_ID_CHUNK:
				for (int i=0; i<numNodes; i++) {
					ULONG id;
					iload->Read(&id,sizeof(ULONG), &nb);
					if (id!=0xffffffff)
						iload->RecordBackpatch(id,(void**)&nodeTab[i]);
					}
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

//-------------------------------------------------------------
class SlaveControl : public Control {
	public:		
		// This controller maintains a reference to the ring master.  The
		// reference pointer is maintained here.
		RingMaster *master;
		// This is the id of the slave controller.  This is a number from
		// 0 to numNodes-1 that represents which dummy object in the ring
		// array the controller is assigned to.
		ULONG id;

		// Constructors, destructor, assignment operator.
		SlaveControl(BOOL loading=FALSE) { master = NULL; id = 0; }
		SlaveControl(const SlaveControl& ctrl);
		SlaveControl(const RingMaster* m, int i);
		void SetID( ULONG i) { id = i;}
		virtual ~SlaveControl() {}	
		SlaveControl& operator=(const SlaveControl& ctrl);

		// --- Inherited virtual methods from Control ---
		void Copy(Control *from) {}
		void CommitValue(TimeValue t) {}
		void RestoreValue(TimeValue t) {}
		virtual BOOL IsLeaf() {return FALSE;}
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE);
		void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);
		BOOL IsReplaceable() {return FALSE;}
		BOOL CanCopyAnim() {return FALSE;}

		// From Animatable
		void* GetInterface(ULONG id);
		int NumSubs()  { return master->NumSubs(); }
		Animatable* SubAnim(int i) { return master->SubAnim(i); }
		TSTR SubAnimName(int i) { return master->SubAnimName(i); }
		Class_ID ClassID() { return SLAVE_CONTROL_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_MATRIX3_CLASS_ID; }  
		void GetClassName(TSTR& s) { s = GetString(IDS_DB_SLAVECONTROL_CLASS); }
		void DeleteThis() { delete this; }		
		// These controllers don't create keyframes - their position 
		// is set by the ring master at all times.  Thus this controller
		// returns 0 for IsKeyable().
		int IsKeyable(){ return 0;}
		// When the system needs to allow the user to edit the controllers
		// parameters in the motion branch this method is called.  This 
		// implementation simply calls the same method of the RingMaster
		// object.  This is done because the UI controls managed by
		// the ring master control the entire system.
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev) { assert(master); master->BeginEditParams(ip,flags,prev); } 
		// This is called to end the parameter editing.  Again, the 
		// ring master method is called.
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next) { assert(master); master->EndEditParams(ip,flags,next); } 
		// --- Inherited virtual methods from ReferenceMaker and Target ---
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		// The slave controller has a single reference to the ring master.
		int NumRefs() { return 1; };	
		RefTargetHandle GetReference(int i)  { assert(i==0); return master; }
		void SetReference(int i, RefTargetHandle rtarg) { assert(i==0); master = (RingMaster *)rtarg; }		
		// This method is called when the something the slave controller 
		// depends on changes.  For example if the RingMaster that the 
		// controller references is deleted this method is called.  Because
		// the slave controller do not have any data cached they don't need
		// to invalidate anything in their implementation of this method.
		// Consequently the implementation simply returns REF_SUCCEED.
		// They only make a referrence to the RingMaster so the _system_ 
		// knows that the slaves are dependent on the RingMaster.
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage) {return REF_SUCCEED;}
		// These methods load and save the controller data from/to the MAX file
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};
 
// This method returns a new instance of the slave controller.
Control* GetNewSlaveControl(RingMaster *master, int i) {
	return new SlaveControl(master,i);
	}

SlaveControl::SlaveControl(const SlaveControl& ctrl) {
	master = ctrl.master;
	id = ctrl.id;
	}

// This constructor creates a reference from the slave controller to 
// the ring master object.
SlaveControl::SlaveControl(const RingMaster* m, int i) {
	id = i;
    MakeRefByID( FOREVER, 0, (ReferenceTarget *)m);
	}

// This method is called to create a copy of the slave controller.
RefTargetHandle SlaveControl::Clone(RemapDir& remap) {
	SlaveControl *sl = new SlaveControl;
	sl->id = id;
	sl->ReplaceReference(0, remap.CloneRef(master));
	return sl;
	}

SlaveControl& SlaveControl::operator=(const SlaveControl& ctrl) {
	master = ctrl.master;
	id = ctrl.id;
	return (*this);
	}

// ========= This method is used to retrieve the value of the 
// controller at the specified time. =========

// This is a important aspect of the system plug-in - this method 
// calls the master object to get the value.
void SlaveControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method) {
	assert(master);
	master->GetValue(t,val,valid,method,id);	
	}

void SlaveControl::SetValue(TimeValue t, void *val, int commit, GetSetMethod method) { }

void* SlaveControl::GetInterface(ULONG id) {
	if (id==I_MASTER) 
		return (void *)master;
	else 
		return NULL;
	}

// IO
#define SLAVE_ID_CHUNK 0x200
IOResult SlaveControl::Save(ISave *isave) {
	ULONG nb;
	isave->BeginChunk(SLAVE_ID_CHUNK);
	isave->Write(&id,sizeof(id), &nb);
	isave->EndChunk();
	return IO_OK;
	}

IOResult SlaveControl::Load(ILoad *iload) {
	ULONG nb;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SLAVE_ID_CHUNK:
				res = iload->Read(&id,sizeof(id), &nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

//----------------------------------------------------------------------


class RingMasterCreationManager : public MouseCallBack, ReferenceMaker {
	public:
		CreateMouseCallBack *createCB;	
		INode *node0;
		RingMaster *theMaster;
		IObjCreate *createInterface;
		ClassDesc *cDesc;
		Matrix3 mat;  // the nodes TM relative to the CP
		IPoint2 pt0;
		Point3 center;
		BOOL attachedToNode;
		int lastPutCount;

		void CreateNewMaster();
			
		int ignoreSelectionChange;

		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		// StdNotifyRefChanged calls this, which can change the partID to new value 
		// If it doesnt depend on the particular message& partID, it should return
		// REF_DONTCARE
	    RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message);

		void Begin( IObjCreate *ioc, ClassDesc *desc );
		void End();
		
		RingMasterCreationManager()
			{
			ignoreSelectionChange = FALSE;
			}
		int proc( HWND hwnd, int msg, int point, int flag, IPoint2 m );
	};

#define CID_BONECREATE	CID_USER + 1

class RingMasterCreateMode : public CommandMode {
		RingMasterCreationManager proc;
	public:
		void Begin( IObjCreate *ioc, ClassDesc *desc ) { proc.Begin( ioc, desc ); }
		void End() { proc.End(); }
		int Class() { return CREATE_COMMAND; }
		int ID() { return CID_BONECREATE; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints = 100000; return &proc; }
		ChangeForegroundCallback *ChangeFGProc() { return CHANGE_FG_SELECTED; }
		BOOL ChangeFG( CommandMode *oldMode ) { return (oldMode->ChangeFGProc() != CHANGE_FG_SELECTED); }
		void EnterMode() {}
		void ExitMode() {}
		BOOL IsSticky() { return FALSE; }
	};

static RingMasterCreateMode theRingMasterCreateMode;

//RingMasterCreationManager::RingMasterCreationManager( IObjCreate *ioc, ClassDesc *desc )
void RingMasterCreationManager::Begin( IObjCreate *ioc, ClassDesc *desc )
	{
	createInterface = ioc;
	cDesc           = desc;
	createCB        = NULL;
	node0			= NULL;
	theMaster 		= NULL;
	attachedToNode = FALSE;
	CreateNewMaster();
	}

void RingMasterCreationManager::SetReference(int i, RefTargetHandle rtarg) { 
	switch(i) {
		case 0: node0 = (INode *)rtarg; break;
		default: assert(0); 
		}
	}

RefTargetHandle RingMasterCreationManager::GetReference(int i) { 
	switch(i) {
		case 0: return (RefTargetHandle)node0;
		default: assert(0); 
		}
	return NULL;
	}

//RingMasterCreationManager::~RingMasterCreationManager
void RingMasterCreationManager::End()
	{
	if (theMaster) {
		theMaster->EndEditParams( (IObjParam*)createInterface, 
	                    	          TRUE/*destroy*/, NULL );
		if ( !attachedToNode ) {
			delete theMaster;
			theMaster = NULL;
			// DS 8/21/97: If something has been put on the undo stack since this object was created, we have to flush the undo stack.
			if (theHold.GetGlobalPutCount()!=lastPutCount) {
				GetSystemSetting(SYSSET_CLEAR_UNDO);
				}
		} else if ( node0 ) {
			 // Get rid of the references.
			DeleteAllRefsFromMe();
			}
		}	
	}

RefResult RingMasterCreationManager::NotifyRefChanged(
	Interval changeInt, 
	RefTargetHandle hTarget, 
	PartID& partID,  
	RefMessage message) 
	{
	switch (message) {
		case REFMSG_TARGET_SELECTIONCHANGE:
		 	if ( ignoreSelectionChange ) {
				break;
				}
		 	if (theMaster) {
				// this will set node0 ==NULL;
				DeleteAllRefsFromMe();
				goto endEdit;
				}
			// fall through

		case REFMSG_TARGET_DELETED:
			if (theMaster) {
				endEdit:
				theMaster->EndEditParams( (IObjParam*)createInterface, FALSE/*destroy*/,NULL );
				theMaster = NULL;
				node0 = NULL;
				CreateNewMaster();	
				attachedToNode = FALSE;
				}
			break;		
		}
	return REF_SUCCEED;
	}

void RingMasterCreationManager::CreateNewMaster()
	{
	theMaster = new RingMaster();
	
	// Start the edit params process
	theMaster->BeginEditParams( (IObjParam*)createInterface, BEGIN_EDIT_CREATE,NULL );
	lastPutCount = theHold.GetGlobalPutCount();
	}

#define DUMSZ 20.0f
#define BOXSZ 20.0f

int RingMasterCreationManager::proc( 
				HWND hwnd,
				int msg,
				int point,
				int flag,
				IPoint2 m )
	{	
	int res;
	INode *newNode,*dummyNode;	
	float r;
	ViewExp *vpx = createInterface->GetViewport(hwnd); 
	assert( vpx );


	switch ( msg ) {
		case MOUSE_POINT:
				{
				if (point==0) {
					pt0 = m;	

					assert(theMaster);

					mat.IdentityMatrix();
					if ( createInterface->SetActiveViewport(hwnd) ) {
						return FALSE;
						}
					if (createInterface->IsCPEdgeOnInView()) { 
						return FALSE;
						}
					if ( attachedToNode ) {
				   		// send this one on its way
				   		theMaster->EndEditParams( (IObjParam*)createInterface,0,NULL );
						
						// Get rid of the references.
						DeleteAllRefsFromMe();

						// new object
						CreateNewMaster();   // creates theMaster
						}


				   	theHold.Begin();	 // begin hold for undo
					mat.IdentityMatrix();
					center = vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
					mat.SetTrans(center);

					// Create a dummy object & node
					DummyObject *dumObj = (DummyObject *)createInterface->
						CreateInstance(HELPER_CLASS_ID,Class_ID(DUMMY_CLASS_ID,0)); 			
					assert(dumObj);					
					dummyNode = createInterface->CreateObjectNode(dumObj);
					dumObj->SetBox(Box3(Point3(-DUMSZ,-DUMSZ,-DUMSZ),Point3(DUMSZ,DUMSZ,DUMSZ)));

					// make a box object
					GenBoxObject *ob = (GenBoxObject *)createInterface->
						CreateInstance(GEOMOBJECT_CLASS_ID,Class_ID(BOXOBJ_CLASS_ID,0));
					ob->SetParams(BOXSZ,BOXSZ,BOXSZ,1,1,1,FALSE); 

					// Make a bunch of nodes, hook the box object to and a
					// slave controller of the master control to each
					for (int i=0; i<theMaster->numNodes; i++) {
						newNode = createInterface->CreateObjectNode(ob);
						SlaveControl* slave = new SlaveControl(theMaster,i);
						newNode->SetTMController(slave);
						dummyNode->AttachChild(newNode);
						theMaster->SetSlaveNode(i,newNode);
						}

					// select the dummy node.
					attachedToNode = TRUE;

					// Reference the node so we'll get notifications.
				    MakeRefByID( FOREVER, 0, theMaster->GetSlaveNode(0) );
					theMaster->SetRad(TimeValue(0),0.0f);
					mat.SetTrans(vpx->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
					createInterface->SetNodeTMRelConstPlane(dummyNode, mat);
					res = TRUE;
					}
				else {
					// select a node so if go into modify branch, see params 
					ignoreSelectionChange = TRUE;
				   	createInterface->SelectNode( theMaster->GetSlaveNode(0) );
					ignoreSelectionChange = FALSE;
					theHold.Accept(IDS_DS_CREATE);
					res = FALSE;
					}
 				createInterface->RedrawViews(createInterface->GetTime(),REDRAW_NORMAL,theMaster);  
				}
				break;
		case MOUSE_MOVE:
			if (node0) {
				r = (float)fabs(vpx->SnapLength(vpx->GetCPDisp(center,Point3(0,1,0),pt0,m)));
				theMaster->SetRad(0,r);
				theMaster->radSpin->SetValue(r, FALSE );
				createInterface->RedrawViews(createInterface->GetTime(),REDRAW_NORMAL,theMaster);
				}
			res = TRUE;
			break;

		case MOUSE_ABORT:
			assert(theMaster);
			theMaster->EndEditParams( (IObjParam*)createInterface, 0,NULL );
			theHold.Cancel();  // undo the changes
			DeleteAllRefsFromMe();
			// DS 8/21/97: If something has been put on the undo stack since this object was created, we have to flush the undo stack.
			if (theHold.GetGlobalPutCount()!=lastPutCount) {
				GetSystemSetting(SYSSET_CLEAR_UNDO);
				}
			CreateNewMaster();	
			createInterface->RedrawViews(createInterface->GetTime(),REDRAW_END,theMaster); 
			attachedToNode = FALSE;
			res = FALSE;						
			break;
		}
	
	createInterface->ReleaseViewport(vpx); 
	return res;
	}

class RingMasterClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new RingMaster(); }
	const TCHAR *	ClassName() { return GetString(IDS_DB_RING_ARRAY_CLASS); }
	int 			BeginCreate(Interface *i);
	int 			EndCreate(Interface *i);
	SClass_ID		SuperClassID() { return SYSTEM_CLASS_ID; }
	Class_ID		ClassID() { return RINGARRAY_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	};
static RingMasterClassDesc mcDesc;

ClassDesc* GetRingMasterDesc() { return &mcDesc; }


int RingMasterClassDesc::BeginCreate(Interface *i)
	{
	IObjCreate *iob = i->GetIObjCreate();
	
	theRingMasterCreateMode.Begin( iob, this );
	iob->PushCommandMode( &theRingMasterCreateMode );
	
	return TRUE;
	}

int RingMasterClassDesc::EndCreate(Interface *i)
	{
	theRingMasterCreateMode.End();
	i->RemoveMode( &theRingMasterCreateMode );
	return TRUE;
	}

class SlaveControlClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) { return new SlaveControl(); }
	const TCHAR *	ClassName() { return GetString(IDS_DB_SLAVE_CONTROL); }
	SClass_ID		SuperClassID() { return CTRL_MATRIX3_CLASS_ID; }
	Class_ID		ClassID() { return SLAVE_CONTROL_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	};

static SlaveControlClassDesc slvDesc;

ClassDesc* GetSlaveControlDesc() { return &slvDesc; }

// The following five functions are used by every plug-in DLL.
/*===========================================================================*\
 | The DLL and Library Functions
\*===========================================================================*/
// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.
int controlsInit = FALSE;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

	if (! controlsInit) {
		controlsInit = TRUE;
		
		// Initialize MAX's custom controls
		InitCustomControls(hInstance);
		
		// Initialize Win95 controls
		InitCommonControls();
	}
	
	return(TRUE);
	}

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int LibNumberClasses() {return 2;}

// This function return the ith class descriptor
__declspec( dllexport ) ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case 0: return GetSlaveControlDesc();		
		case 1: return GetRingMasterDesc();		
		default: return 0;
		}
	}

// This function returns a string that describes the DLL.  This string appears in 
// the File / Summary Info / Plug-In Info dialog box.
__declspec( dllexport ) const TCHAR *LibDescription() { 
	return GetString(IDS_LIB_DESC);
	}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }

