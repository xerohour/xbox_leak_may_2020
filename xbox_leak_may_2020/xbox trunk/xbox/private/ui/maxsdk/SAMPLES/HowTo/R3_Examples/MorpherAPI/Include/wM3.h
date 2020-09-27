/*===========================================================================*\
 | 
 |  FILE:	wM3.h
 |			Weighted Morpher for MAX R3
 |			Main header file
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 22-5-98
 |			Prep'd for MorpherAPI 4-4-99
 | 
\*===========================================================================*/


#ifndef __MORPHR3__H
#define __MORPHR3__H

#define MorphExport __declspec( dllexport )


#include <Max.h>
#include <istdplug.h>
#include <meshadj.h>
#include <modstack.h>
#include <imtl.h>
#include <texutil.h>
#include <stdmat.h>
#include <macrorec.h>


static Class_ID M3MatClassID(0x4b9937e0, 0x3a1c3da4);
#define MR3_CLASS_ID		Class_ID(0x17bb6854, 0xa5cba2a3)
#define MR3_NUM_CHANNELS	100

#define MR3_MORPHERVERSION	010


// Save codes for the morphChannel class
#define MR3_POINTCOUNT		0x0100
#define MR3_SELARRAY		0x0110
#define MR3_NAME			0x0120
#define MR3_PARAMS			0x0130
#define MR3_POINTDATA_MP	0x0140
#define MR3_POINTDATA_MW	0x0150
#define MR3_POINTDATA_MD	0x0160
#define MR3_POINTDATA_MO	0x0170

// Save codes for the MorphR3 class
#define MR3_MARKERNAME		0x0180
#define MR3_MARKERINDEX		0x0185
#define MR3_MC_CHUNK		0x0190
#define MR3_MC_SUBCHUNK		0x0200
#define MR3_FLAGS			0x0210



// paramblock index table
#define PB_OV_USELIMITS		0
#define PB_OV_SPINMIN		1
#define PB_OV_SPINMAX		2
#define PB_OV_USESEL		3
#define PB_AD_VALUEINC		4
#define PB_CL_AUTOLOAD		5


// Channel operation flags
#define OP_MOVE				0
#define OP_SWAP				1


// two handy macros to set cursors for busy or normal operation
#define UI_MAKEBUSY			SetCursor(LoadCursor(NULL, IDC_WAIT));
#define UI_MAKEFREE			SetCursor(LoadCursor(NULL, IDC_ARROW));
			

// Morph Material ui defines
#define NSUBMTL 10

// Updater flags
#define UD_NORM				0
#define UD_LINK				1


class MorphR3;
class M3Mat;
class M3MatDlg;
class MCRestore;


/*===========================================================================*\
 | Morph channel manager class
 | Storage for all the morph data needed
\*===========================================================================*/

class morphChannel
{
public:

	// Construct/Destruct
	~morphChannel();
	morphChannel();

	MorphR3		*mp;

	// Number of points and morphable points
	int			nPts;
	int			nmPts;

	// Actual morphable points
	Point3*		mPoints;
	Point3*		mDeltas;
	double*		mWeights;
	// Point indicies that are different from the host
	// mesh. This is a simple but efficient way to speed up the
	// morphing process.
	int*		mOptdata;
	// BitArray to check against for point selection
	BitArray	mSel;
	// INode that we use to update and reload from, if possible
	INode*		mConnection;
	// Name for the morph channel
	TSTR		mName;

	// Various, non-animatable stuff
	// mActive is TRUE if the channel has data in it in some form
	// mModded is TRUE if the channel has been changed in SOME form.
	//		ie, had its name changed or similar
	// mUseLimit is TRUE if the channel limits are turned on
	// mSpinmin and mSpinmax are the two clamp values for channel limits
	// mUseSel is TRUE if the channel respects vertex/point selection
	BOOL		mActive, mModded, mUseLimit, mUseSel;
	float		mSpinmin,mSpinmax;

	// TRUE if the channel has been marked as bad. It will not be 
	// considered when building the morph results.
	BOOL		mInvalid;

	// Channel enabled/disabled
	BOOL		mActiveOverride;

	// paramblock for the morph channels values
	IParamBlock* cblock;
	

	// Delete and reset channel
	// Note that the paramblock needs to be manually built and wired in
	MorphExport void ResetMe();


	MorphExport void AllocBuffers( int sizeA, int sizeB );

	// Do some rough calculations about how much space this channel
	// takes up
	// This isn't meant to be fast or terribly accurate!
	MorphExport float getMemSize();


	// The rebuildChannel call will recalculate the optimization data
	// and refill the mSel selection array. This will be called each time a
	// targeted node is changed, or any of the 'Update Target' buttons is
	// pressed on the UI
	MorphExport void rebuildChannel();

	// Initialize a channel using a scene node
	MorphExport void buildFromNode( INode *node , BOOL resetTime=TRUE , TimeValue t=0 );

	// Transfer data to another channel
	MorphExport morphChannel& operator=(morphChannel& from);

	// Load/Save channel to stream
	MorphExport IOResult Save(ISave* isave);
	MorphExport IOResult Load(ILoad* iload);
};


// Internally-used local object morph cache
class morphCache
{

public:

	BOOL CacheValid;

	Point3*		oPoints;
	double*		oWeights;
	BitArray	sel;

	int		Count;

	morphCache ();
	~morphCache () { NukeCache(); }

	MorphExport void MakeCache(Object *obj);
	MorphExport void NukeCache();

	MorphExport BOOL AreWeCached();
};



/*===========================================================================*\
 | Modifer class definition
\*===========================================================================*/
class MorphR3 : public Modifier, TimeChangeCallback {
	public:

		// Access to the interface
		static IObjParam *ip;
		
		// Pointer to the morph channels
		morphChannel		*chanBank;
		
		// Currently selected channel (0-9)
		int					chanSel;
		
		// Currently viewable channel banks (0-99)
		int					chanNum;

		// Spinners from main page
		ISpinnerControl		*chanSpins[10];

		// Spinners from global settings page
		ISpinnerControl		*glSpinmin,*glSpinmax;

		// Spinners from the channel params dlg
		ISpinnerControl		*cSpinmin,*cSpinmax;

		// Global parameter block
		IParamBlock			*pblock;

		// The window handles for the 4 rollout pages
		HWND hwGlobalParams, hwChannelList,	hwChannelParams, hwAdvanced, hwLegend;

		// For the namer dialog
		ICustEdit			*newname;

		// Morph Cache
		morphCache MC_Local;

		BOOL tccI;
		char trimD[50];

		// 'Save as Current' support
		BOOL recordModifications;
		int recordTarget;


		// Marker support
		Tab<int>			markerIndex;
		NameTab				markerName;
		int					markerSel;


		// Channel operation flag for dialog use
		int					cOp;
		int					srcIdx;


		// Handles the scroll bar on the channel list UI
		MorphExport void VScroll(int code, short int cpos );
		// Clamps channel number to valid range
		MorphExport void Clamp_chanNum();

		
		MorphExport void ChannelOp(int src, int targ, int flags);


		MorphExport void Update_channelParams();


		// evaluate the value increments setting
		MorphExport float GetIncrements();
		// SetScale on the channel list spinners
		MorphExport void Update_SpinnerIncrements();

		// Functions to update the channel list dialog box:
		MorphExport void Update_colorIndicators();
		MorphExport void Update_channelNames();
		MorphExport void Update_channelValues();
		MorphExport void Update_channelLimits();
		MorphExport void Update_channelInfo();
		MorphExport void Update_channelMarkers();
		// Seperated cause this function is pretty expensive
		// Lots done, complete update - calls all functions above
		MorphExport void Update_channelFULL();
		
		// Used to trim fp values to a number of decimal points
		MorphExport float TrimDown(float value, int decimalpts);

		BOOL inRender;
};



/*===========================================================================*\
 |
 | Morph Material definitions
 |
\*===========================================================================*/

class M3MatDlg : public ParamDlg {
	public:		
		HWND hwmedit;

		IMtlParams *ip;

		M3Mat *theMtl;

		HWND hPanel; 

		ICustButton *iBut[NSUBMTL];
		ICustButton *bBut;
		ICustButton *pickBut;

		MtlDADMgr dadMgr;

		BOOL valid;

		MorphExport void VScroll(int code, short int cpos );
		MorphExport void Clamp_listSel();

		MorphExport void UpdateMorphInfo(int upFlag);
};

class M3Mat : public Mtl {	
	public:
		M3MatDlg *dlg;

		// 100 materials for 100 morph channels, plus 1 base material
		Mtl *mTex[101];
		BOOL mapOn[100];

		BOOL inRender;

		// Morph mod pointer
		MorphR3	*morphp;
		TSTR obName;

		// Temp node pointer used in the mtl pickmode
		INode *Wnode;

		IParamBlock *pblock;

		Interval ivalid;
		int listSel;
	};



#endif