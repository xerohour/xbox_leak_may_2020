/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Module.cpp

Abstract:

	Module composed of multiple cells

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <math.h>
#include "dspbuilder.h"
#include "Module.h"
#include "PatchCord.h"
#include "Graph.h"
#include "Jack.h"
#include "ParameterDialog.h"
#include "Knob.h"
#include "ChildView.h"
#include "EffectNameDialog.h"
#include "WinDspImageBuilder.h"

//------------------------------------------------------------------------------
//	DSound defines (taken from dsound.h)
//------------------------------------------------------------------------------
#define I3DL2LISTENER_MINROOM                     -10000
#define I3DL2LISTENER_MAXROOM                     0
#define I3DL2LISTENER_DEFAULTROOM                 -10000
#define I3DL2LISTENER_MINROOMHF                   -10000
#define I3DL2LISTENER_MAXROOMHF                   0
#define I3DL2LISTENER_DEFAULTROOMHF               0
#define I3DL2LISTENER_MINROOMROLLOFFFACTOR        0.0f
#define I3DL2LISTENER_MAXROOMROLLOFFFACTOR        10.0f
#define I3DL2LISTENER_DEFAULTROOMROLLOFFFACTOR    0.0f
#define I3DL2LISTENER_MINDECAYTIME                0.1f
#define I3DL2LISTENER_MAXDECAYTIME                20.0f
#define I3DL2LISTENER_DEFAULTDECAYTIME            1.0f
#define I3DL2LISTENER_MINDECAYHFRATIO             0.1f
#define I3DL2LISTENER_MAXDECAYHFRATIO             2.0f
#define I3DL2LISTENER_DEFAULTDECAYHFRATIO         0.5f
#define I3DL2LISTENER_MINREFLECTIONS              -10000
#define I3DL2LISTENER_MAXREFLECTIONS              1000
#define I3DL2LISTENER_DEFAULTREFLECTIONS          -10000
#define I3DL2LISTENER_MINREFLECTIONSDELAY         0.0f
#define I3DL2LISTENER_MAXREFLECTIONSDELAY         0.3f
#define I3DL2LISTENER_DEFAULTREFLECTIONSDELAY     0.02f
#define I3DL2LISTENER_MINREVERB                   -10000
#define I3DL2LISTENER_MAXREVERB                   2000
#define I3DL2LISTENER_DEFAULTREVERB               -10000
#define I3DL2LISTENER_MINREVERBDELAY              0.0f
#define I3DL2LISTENER_MAXREVERBDELAY              0.1f
#define I3DL2LISTENER_DEFAULTREVERBDELAY          0.04f
#define I3DL2LISTENER_MINDIFFUSION                0.0f
#define I3DL2LISTENER_MAXDIFFUSION                100.0f
#define I3DL2LISTENER_DEFAULTDIFFUSION            100.0f
#define I3DL2LISTENER_MINDENSITY                  0.0f
#define I3DL2LISTENER_MAXDENSITY                  100.0f
#define I3DL2LISTENER_DEFAULTDENSITY              100.0f
#define I3DL2LISTENER_MINHFREFERENCE              20.0f
#define I3DL2LISTENER_MAXHFREFERENCE              20000.0f
#define I3DL2LISTENER_DEFAULTHFREFERENCE          5000.0f

static const CModule::I3DL2Listener cListenerDefault			= { -1000,   -100, 0.0f, 1.49f, 0.83f,  -2602, 0.007f,    200, 0.011f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerGeneric			= { -1000,   -100, 0.0f, 1.49f, 0.83f,  -2602, 0.007f,    200, 0.011f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerPaddedCell			= { -1000,  -6000, 0.0f, 0.17f, 0.10f,  -1204, 0.001f,    207, 0.002f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerRoom				= { -1000,   -454, 0.0f, 0.40f, 0.83f,  -1646, 0.002f,     53, 0.003f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerBathroom			= { -1000,  -1200, 0.0f, 1.49f, 0.54f,   -370, 0.007f,   1030, 0.011f, 100.0f,  60.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerLivingRoom			= { -1000,  -6000, 0.0f, 0.50f, 0.10f,  -1376, 0.003f,  -1104, 0.004f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerStoneRoom			= { -1000,   -300, 0.0f, 2.31f, 0.64f,   -711, 0.012f,     83, 0.017f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerAuditorium			= { -1000,   -476, 0.0f, 4.32f, 0.59f,   -789, 0.020f,   -289, 0.030f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerConcertHall		= { -1000,   -500, 0.0f, 3.92f, 0.70f,  -1230, 0.020f,     -2, 0.029f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerCave				= { -1000,      0, 0.0f, 2.91f, 1.30f,   -602, 0.015f,   -302, 0.022f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerArena				= { -1000,   -698, 0.0f, 7.24f, 0.33f,  -1166, 0.020f,     16, 0.030f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerHangar				= { -1000,  -1000, 0.0f,10.05f, 0.23f,   -602, 0.020f,    198, 0.030f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerCarpetedHallway	= { -1000,  -4000, 0.0f, 0.30f, 0.10f,  -1831, 0.002f,  -1630, 0.030f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerHallway			= { -1000,   -300, 0.0f, 1.49f, 0.59f,  -1219, 0.007f,    441, 0.011f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerStoneCorridor		= { -1000,   -237, 0.0f, 2.70f, 0.79f,  -1214, 0.013f,    395, 0.020f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerAlley				= { -1000,   -270, 0.0f, 1.49f, 0.86f,  -1204, 0.007f,     -4, 0.011f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerForest				= { -1000,  -3300, 0.0f, 1.49f, 0.54f,  -2560, 0.162f,   -613, 0.088f,  79.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerCity				= { -1000,   -800, 0.0f, 1.49f, 0.67f,  -2273, 0.007f,  -2217, 0.011f,  50.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerMountains			= { -1000,  -2500, 0.0f, 1.49f, 0.21f,  -2780, 0.300f,  -2014, 0.100f,  27.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerQuarry				= { -1000,  -1000, 0.0f, 1.49f, 0.83f, -10000, 0.061f,    500, 0.025f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerPlain				= { -1000,  -2000, 0.0f, 1.49f, 0.50f,  -2466, 0.179f,  -2514, 0.100f,  21.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerParkingLot			= { -1000,      0, 0.0f, 1.65f, 1.50f,  -1363, 0.008f,  -1153, 0.012f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerSewerPipe			= { -1000,  -1000, 0.0f, 2.81f, 0.14f,    429, 0.014f,    648, 0.021f,  80.0f,  60.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerUnderWater			= { -1000,  -4000, 0.0f, 1.49f, 0.10f,   -449, 0.007f,   1700, 0.011f, 100.0f, 100.0f, 5000.0f };
static const CModule::I3DL2Listener cListenerNoReverb			= { -10000, -10000, 0.0f, 1.00f, 1.00f, -10000, 0.000f, -10000, 0.000f,   0.0f,   0.0f, 5000.0f };

//------------------------------------------------------------------------------
//	Stuff
//------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
static const double cPi			= 3.14159265358979323846;
static const double cTwoPi		= 2.0 * cPi;
//static const double cFs			= 48000.0;
//static const double	cScale24	= 1.0;

//------------------------------------------------------------------------------
//	Static Member Variables:
//------------------------------------------------------------------------------
CFont* CModule::m_stateFont = NULL;
int CModule::m_stateFontRef	= 0;

//------------------------------------------------------------------------------
//	CModule::CModule
//------------------------------------------------------------------------------
CModule::CModule(
				 IN CGraph*				pGraph,
				 IN CModule::Type		type,
				 IN CModule::ColorFlag	colorFlag,
				 IN LPCTSTR				pName,
				 IN int					dspCycles,
				 IN int					yMemSize,
				 IN int					scratchLength,
				 IN LPCTSTR				dspCode,
				 IN LPCTSTR				stateFilename,
				 IN const CStringArray*	pInputs,
				 IN const CStringArray*	pOutputs
				 ) : CUnique()
/*++

Routine Description:

	Constructor

Arguments:

	IN pGraph -			Graph this module will belong to
	IN id -				ID of module
	IN type -			Type of module
	IN pName -			Name of module
	IN dspCycles -		DSP Cycles
	IN yMemSize -		Y mem size
	IN scratchLength -	Scratch Length
	IN dspCode -		DSP filename (NULL for none)
	IN stateFilename -	State filename (NULL for none)
	IN pInputs -		List of input names (NULL terminated)
	IN pOutputs -		List of ouput names (NULL terminated)

Return Value:

	None

--*/
{
	int		i;
	int		numInputs	= pInputs ? pInputs->GetSize() : 0;
	int		numOutputs	= pOutputs ? pOutputs->GetSize() : 0;
	CJack*	pJack;

	Initialize();

	m_colorFlag		= colorFlag;
	m_type			= type;
	m_pGraph		= pGraph;
	m_dspCode		= dspCode;
	m_stateFilename	= stateFilename;
	m_dspCycles		= dspCycles;
	m_yMemSize		= yMemSize;
	m_scratchLength	= scratchLength;
	m_name			= pName;
	m_effectName	= pName;
	SetDisplayName();

	if(m_type == CModule::EFFECT)
	{
		m_stats.Format(_T("DSP: %d  YMem: %d  Scr: %d"), 
			m_dspCycles, m_yMemSize, m_scratchLength);
	}

	// Build the module based on type
	m_rect.left = 0;
	m_rect.top	= 0;
	switch(m_type)
	{
	case CModule::INPUT_MIXBIN:
		m_rect.right	= 9 * SCALE;
		m_rect.bottom	= 3 * SCALE;
		break;

	case CModule::OUTPUT_MIXBIN:
		m_rect.right	= 9 * SCALE;
		m_rect.bottom	= 3 * SCALE;
		break;

	case CModule::EFFECT:
		m_rect.right	= 16 * SCALE;
		m_rect.bottom	= ((numInputs > numOutputs) ? 
						   (numInputs > 1 ? numInputs + 2 : 3) :
						   (numOutputs > 1 ? numOutputs + 2 : 3)) * SCALE;
		break;
	}
	CreatePenAndBrush();

	// Inputs
	for(i=0; i<numInputs; ++i)
	{
		pJack = new CJack(this);
		pJack->SetName((*pInputs)[i]);
		pJack->SetInput(TRUE);
		m_inputs.Add(pJack);
	}

	// Outputs
	for(i=0; i<numOutputs; ++i)
	{
		pJack = new CJack(this);
		pJack->SetName((*pOutputs)[i]);
		pJack->SetInput(FALSE);
		m_outputs.Add(pJack);
	}

	// Set the location
	SetRect(m_rect);

	// Special Case: IIR2?
	if(!m_effectName.CompareNoCase(_T("iir2")))
		m_bIIR2 = TRUE;
	else if(!m_effectName.CompareNoCase(_T("i3dl2 reverb")) ||
			!m_effectName.CompareNoCase(_T("i3dl2 24k reverb")))
	{
		m_bI3DL2 = TRUE;
		SetI3DL2Parameters();
	}
}

//------------------------------------------------------------------------------
//	CModule::CModule
//------------------------------------------------------------------------------
CModule::CModule(
				 IN const CModule&	module
				 ) : CUnique()
/*++

Routine Description:

	Copy constructor. This method will not copy patches.

Arguments:

	IN module -	Module to copy

Return Value:

	None

--*/
{
	int		i;
	CJack*	pJack;

	Initialize();

	m_type					= module.m_type;
	m_colorFlag				= module.m_colorFlag;
	m_state					= module.m_state;
	m_pGraph				= module.m_pGraph;
	m_stats					= module.m_stats;
	m_dspCode				= module.m_dspCode;
	m_stateFilename			= module.m_stateFilename;
	m_dspCycles				= module.m_dspCycles;
	m_yMemSize				= module.m_yMemSize;
	m_scratchLength			= module.m_scratchLength;
	m_activeConfig			= module.m_activeConfig;
	m_mixbin				= module.m_mixbin;
	m_fIIR2Frequency		= module.m_fIIR2Frequency;
	m_fIIR2Q				= module.m_fIIR2Q;
	m_I3DL2Listener			= module.m_I3DL2Listener;
	m_I3DL2ListenerDefault	= module.m_I3DL2ListenerDefault;
	m_name					= module.m_name;
	m_effectName			= module.m_effectName;
	SetDisplayName();

	// Copy the configs
	if(module.m_configs.GetSize() != 0)
	{
		m_configs.SetSize(module.m_configs.GetSize());
		for(i=0; i<module.m_configs.GetSize(); ++i)
			m_configs[i] = module.m_configs[i];
	}

	CreatePenAndBrush();

	// Inputs
	for(i=0; i<module.m_inputs.GetSize(); ++i)
	{
		pJack	= new CJack(this);
		*pJack	= *module.m_inputs[i];
		m_inputs.Add(pJack);
	}

	// Outputs
	for(i=0; i<module.m_outputs.GetSize(); ++i)
	{
		pJack	= new CJack(this);
		*pJack	= *module.m_outputs[i];
		m_outputs.Add(pJack);
	}

	// Parameters
	m_parameters.Copy(module.m_parameters);

	SetRect(module.m_rect);

	// Special Case: IIR2?
	if(!m_effectName.CompareNoCase(_T("iir2")))
		m_bIIR2 = TRUE;

	else if(!m_effectName.CompareNoCase(_T("i3dl2 reverb")) ||
			!m_effectName.CompareNoCase(_T("i3dl2 24k reverb")))
	{
		m_bI3DL2 = TRUE;
		SetI3DL2Parameters();
	}
}

//------------------------------------------------------------------------------
//	CModule::CModule
//------------------------------------------------------------------------------
CModule::CModule(void) : CUnique()
/*++

Routine Description:

	Constructor. Does almost nothing

Arguments:

	None

Return Value:

	None

--*/
{
	Initialize();
}

//------------------------------------------------------------------------------
//	CModule::~CModule
//------------------------------------------------------------------------------
CModule::~CModule(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Destroy the dialog
	if(m_pParameterDialog)
		m_pParameterDialog->DestroyWindow();

	if(!m_bBadData)
		Disconnect();

	// Delete the font?
	--m_stateFontRef;
	if(m_stateFontRef == 0)
	{
		delete m_stateFont;
		m_stateFont = NULL;
	}

	// Delete all the patches & connections
	for(i=0; i<m_outputs.GetSize(); ++i)
		delete m_outputs[i];
	m_outputs.RemoveAll();

	for(i=0; i<m_inputs.GetSize(); ++i)
		delete m_inputs[i];
	m_inputs.RemoveAll();

	m_configs.RemoveAll();

	m_parameters.RemoveAll();
}

//------------------------------------------------------------------------------
//	CModule::SetLocation
//------------------------------------------------------------------------------
void
CModule::SetRect(
				 IN const CRect&	rect
				 )
/*++

Routine Description:

	Sets the location of the module

Arguments:

	IN rect -	Rect

Return Value:

	None

--*/
{
	int	y;
	int	i;

	m_rect = rect;

	// Calculate the name rectangle
	m_nameRect.top		= m_rect.top;
	m_nameRect.left		= m_rect.left;
	m_nameRect.bottom	= m_nameRect.top + SCALE;
	m_nameRect.right	= m_rect.right;

	// Calculate the config rectangle
	m_configRect.top	= m_rect.top + SCALE;
	m_configRect.left	= m_rect.left;
	m_configRect.bottom	= m_configRect.top + SCALE;
	m_configRect.right	= m_rect.right;

	// Calculate the stats rectangle
	m_statsRect.top		= m_rect.bottom - SCALE;
	m_statsRect.left	= m_rect.left;
	m_statsRect.bottom	= m_rect.bottom;
	m_statsRect.right	= m_rect.right;

	// Calculate the rectangles for the connection labels
	for(y=m_rect.top+SCALE, i=0; i<m_inputs.GetSize(); ++i, y+=SCALE)
	{
		m_inputs[i]->SetRect(CRect(m_rect.left + (SCALE / 2) + 2, y,
								   m_rect.right - (SCALE / 2) - 3, y + SCALE));
	}
	for(y=m_rect.top+SCALE, i=0; i<m_outputs.GetSize(); ++i, y+=SCALE)
	{
		m_outputs[i]->SetRect(CRect(m_rect.left + (SCALE / 2) + 2, y,
									m_rect.right - (SCALE / 2) - 3, y + SCALE));
	}
}

//------------------------------------------------------------------------------
//	CModule::Draw
//------------------------------------------------------------------------------
void
CModule::Draw(
			  IN CDC*	pDC
			  )
/*++

Routine Description:

	Draws the modules.
	Draws the outline, the filled interior and the name(s) of the module

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	int				i;
	CPen*			oldPen;
	CBrush*			oldBrush;
	static CPoint	point(10, 10);

	// Setup to draw
	oldPen = pDC->SelectObject(&m_pen);
	if(m_state == CModule::OK)
		oldBrush = (CBrush*)pDC->SelectObject(&m_brush);
	else if((m_state == CModule::MOVING) || m_bHighlight)
		oldBrush = (CBrush*)pDC->SelectObject(&m_movingBrush);
	else
		oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	// Draw
	pDC->RoundRect(m_rect, point);

	// Draw the state
	if(m_state == CModule::INVALID)
	{
		int		offset = SCALE / 2;
		CPen	pen(PS_SOLID, 7, RGB(255, 0, 0));
		CPen*	statePen = pDC->SelectObject(&pen);

		pDC->MoveTo(m_rect.left + offset, m_rect.top + offset);
		pDC->LineTo(m_rect.right - offset, m_rect.bottom - offset);
		pDC->MoveTo(m_rect.left + offset, m_rect.bottom - offset);
		pDC->LineTo(m_rect.right - offset, m_rect.top + offset);
		
		pDC->SelectObject(statePen);
	}

	// Draw the name
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	if(m_state != CModule::MOVING)
		pDC->DrawText(m_displayName, m_nameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else
		pDC->DrawText(m_movingName, m_nameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Draw the configuration
	if(m_activeConfig != -1)
		pDC->DrawText(m_configs[m_activeConfig].name, m_configRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Draw the connections
	for(i=0; i<m_inputs.GetSize(); ++i)
		m_inputs[i]->Draw(pDC);
	for(i=0; i<m_outputs.GetSize(); ++i)
		m_outputs[i]->Draw(pDC);

	// Draw the statistics
	pDC->DrawText(m_stats, m_statsRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Restore the graphics state
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

//------------------------------------------------------------------------------
//	CModule::AreInputsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::AreInputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if any of the inputs are patched

Arguments:

	None

ReturnValue:

	TRUE if any of the inputs are patched

--*/
{
	int	i;

	for(i=0; i<m_inputs.GetSize(); ++i)
	{
		if(m_inputs[i]->IsPatched())
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::AreOutputsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::AreOutputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if any of the outputs are patched

Arguments:

	None

ReturnValue:

	TRUE if any of the outputs are patched

--*/
{
	int	i;

	for(i=0; i<m_outputs.GetSize(); ++i)
	{
		if(m_outputs[i]->IsPatched())
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::IsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::IsPatched(void) const
/*++

Routine Description:

	Returns TRUE if any of the inputs or outputs of the module are patched

Arguments:

	None

ReturnValue:

	TRUE if the module is patched

--*/
{
	return AreInputsPatched() || AreOutputsPatched();
}

//------------------------------------------------------------------------------
//	CModule::Disconnect
//------------------------------------------------------------------------------
void 
CModule::Disconnect(void)
/*++

Routine Description:

	Disconnect all connections

Arguments:

	None

ReturnValue:

	None

--*/
{
	DisconnectInputs();
	DisconnectOutputs();
}

//------------------------------------------------------------------------------
//	CModule::DisconnectInputs
//------------------------------------------------------------------------------
void 
CModule::DisconnectInputs(void)
/*++

Routine Description:

	Disconnects all inputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	int i;

	for(i=0; i<m_inputs.GetSize(); ++i)
	{
		if(m_inputs[i]->IsPatched())
			m_pGraph->DeletePatchCord(m_inputs[i]->GetPatchCord());
	}
}

//------------------------------------------------------------------------------
//	CModule::DisconnectOutputs
//------------------------------------------------------------------------------
void 
CModule::DisconnectOutputs(void)
/*++

Routine Description:

	Disconnects all outputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	int i;

	if(m_bBadData)
		return;

	for(i=0; i<m_outputs.GetSize(); ++i)
	{
		if(m_outputs[i]->IsPatched())
			m_pGraph->DeletePatchCord(m_outputs[i]->GetPatchCord());
	}
}

//------------------------------------------------------------------------------
//	CModule::IsConnected
//------------------------------------------------------------------------------
BOOL 
CModule::IsConnected(void) const
/*++

Routine Description:

	Returns TRUE if the module is connected to a valid input or output

Arguments:

	None

Return Value:

	TRUE if the module is connected to a valid input or output, FALSE otherwise

--*/
{
	int		i;

	// TODO: Check state of patches to see they are valid, not disconnected or 
	// invalid
	for(i=0; i<m_inputs.GetSize(); ++i)
	{
		if(m_inputs[i]->IsPatched())
			return TRUE;
	}
	for(i=0; i<m_outputs.GetSize(); ++i)
	{
		if(m_outputs[i]->IsPatched())
			return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::AddConfiguration
//------------------------------------------------------------------------------
void 
CModule::AddConfiguration(
						  IN LPCTSTR	pName,
						  IN LPCTSTR	pStateFilename
						  )
/*++

Routine Description:

	Adds a configuration to the effect

Arguments:

	IN pName -			Name of configuration
	IN pStateFilename -	State Filename

Return Value:

	None

--*/
{
	Config	config;

	config.name				= pName;
	config.stateFilename	= pStateFilename;
	m_configs.Add(config);

	if(m_activeConfig == -1)
		m_activeConfig = 0;
}

//------------------------------------------------------------------------------
//	CModule::GetStateFilename
//------------------------------------------------------------------------------
LPCTSTR 
CModule::GetStateFilename(void) const 
/*++

Routine Description:

	Returns the state filename

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_activeConfig == -1)
		return (LPCTSTR)m_stateFilename; 
	return m_configs[m_activeConfig].stateFilename;
}

//------------------------------------------------------------------------------
//	CModule::Move
//------------------------------------------------------------------------------
void 
CModule::Move(
			  IN const CPoint&	point
			  )
/*++

Routine Description:

	Moves the module to a new location

Arguments:

	IN point -	New upper left corner

Return Value:

	None

--*/
{
	SetRect(CRect(point.x, point.y, m_rect.Width() + point.x, 
				  m_rect.Height() + point.y));
}

//------------------------------------------------------------------------------
//	CModule::GetJack
//------------------------------------------------------------------------------
CJack*
CModule::GetJack(
				 IN const CPoint&	point,
				 IN BOOL			bSmallJacks
				 ) const
/*++

Routine Description:

	Returns a jack for the point

Arguments:

	IN point -			Point to query
	IN bSmallJacks -	Assume the jacks are small

Return Value:

	The jack or NULL

--*/
{
	int	index;
	
	if(::IsPointInRect(point.x, point.y, m_rect))
	{
		// Use the small jacks for picking?
		if(bSmallJacks)
		{
			if(point.x <= (m_rect.left + SCALE))
			{
				// One input?
				if(m_inputs.GetSize() == 1)
					return m_inputs[0];

				// Many inputs?
				else
				{
					index = (point.y - m_rect.top) / SCALE - 1;
					if((index >= 0) && (index < m_inputs.GetSize()))
						return m_inputs[index];
				}
			}

			// Output?
			else if(point.x >= (m_rect.right - SCALE))
			{
				// One output?
				if(m_outputs.GetSize() == 1)
					return m_outputs[0];

				// Many outputs?
				else
				{
					index = (point.y - m_rect.top) / SCALE - 1;
					if((index >= 0) && (index < m_outputs.GetSize()))
						return m_outputs[index];
				}
			}
		}

		// Otherwise, use the entire rect as a pick target
		else
		{
			// Any inputs?
			if(m_inputs.GetSize())
			{
				// Inputs and outputs
				if(m_outputs.GetSize())
				{
					// Input?
					if(point.x <= m_rect.CenterPoint().x)
					{
						// One input?
						if(m_inputs.GetSize() == 1)
							return m_inputs[0];

						// Many inputs?
						else
						{
							index = (point.y - m_rect.top) / SCALE - 1;
							if((index >= 0) && (index < m_inputs.GetSize()))
								return m_inputs[index];
						}
					}

					// Output?
					else if(point.x >= m_rect.CenterPoint().x)
					{
						// One output?
						if(m_outputs.GetSize() == 1)
							return m_outputs[0];

						// Many outputs?
						else
						{
							index = (point.y - m_rect.top) / SCALE - 1;
							if((index >= 0) && (index < m_outputs.GetSize()))
								return m_outputs[index];
						}
					}
				}

				// Only inputs
				else
				{
					// One input?
					if(m_inputs.GetSize() == 1)
						return m_inputs[0];

					// Many inputs
					else
					{
						index = (point.y - m_rect.top) / SCALE - 1;
						if((index >= 0) && (index < m_inputs.GetSize()))
							return m_inputs[index];
					}
				}
			}

			// Only outputs
			else if(m_outputs.GetSize())
			{
				// One outputs
				if(m_outputs.GetSize() == 1)
					return m_outputs[0];

				// Many outputs
				else
				{
					index = (point.y - m_rect.top) / SCALE - 1;
					if((index >= 0) && (index < m_outputs.GetSize()))
						return m_outputs[index];
				}
			}
		}
	}
	return NULL;
}

#if 0
//------------------------------------------------------------------------------
//	CModule::GetJack
//------------------------------------------------------------------------------
CJack*
CModule::GetJack(
				 IN const CPatchCord*	pPatchCord
				 ) const
/*++

Routine Description:

	Returns the jack that contains the patch cord

Arguments:

	IN pPatchCord -	Patch cord to look up

Return Value:

	The jack or NULL

--*/
{
	int	i;

	for(i=0; i<m_inputs.GetSize(); ++i)
	{
		if(m_inputs[i]->GetOwnPatchCord() == pPatchCord)
			return m_inputs[i];
	}
	for(i=0; i<m_outputs.GetSize(); ++i)
	{
		if(m_outputs[i]->GetOwnPatchCord() == pPatchCord)
			return m_outputs[i];
	}

	// This should never be hit
	return NULL;
}
#endif

//------------------------------------------------------------------------------
//	CModule::SetState
//------------------------------------------------------------------------------
void 
CModule::SetState(
				  IN State state
				  )
/*++
Routine Description:

	Sets the state of the module

Arguments:

	IN state -	State

Return Value:

	None

--*/
{
	if(m_state != state)
	{
		m_state = state;
		m_pGraph->Draw(m_rect);
	}
}

//------------------------------------------------------------------------------
//	CModule::Write
//------------------------------------------------------------------------------
void 
CModule::Write(
			   IN CFile& file
			   )
/*++

Routine Description:

	Writes the object to a file

Arguments:

	IN file -	File to write to

Return Value:

	None

--*/
{
	WORD	word;
	DWORD	dword;
	CString	string;
	int		i;

	try
	{
		// name
		file.Write((LPCTSTR)m_name, ((unsigned)m_name.GetLength() + 1) * sizeof(TCHAR));

		// effect name
		file.Write((LPCTSTR)m_effectName, ((unsigned)m_effectName.GetLength() + 1) * sizeof(TCHAR));

		// Write the id
		CUnique::Write(file);

		// ini name
		file.Write((LPCTSTR)m_iniName, ((unsigned)m_iniName.GetLength() + 1) * sizeof(TCHAR));

		// Write the index
		file.Write(&m_index, sizeof(m_index));

		// Type
		file.Write(&m_type, sizeof(m_type));

		// State
		file.Write(&m_state, sizeof(m_state));

		// Rect
		file.Write((LPRECT)m_rect, sizeof(RECT));

		// nameRect
		file.Write((LPRECT)m_nameRect, sizeof(RECT));

		// config rect
		file.Write((LPRECT)m_configRect, sizeof(RECT));

		// stats
		file.Write((LPCTSTR)m_stats, ((unsigned)m_stats.GetLength() + 1) * sizeof(TCHAR));

		// stats rect
		file.Write((LPRECT)m_statsRect, sizeof(RECT));

		// dspcode
		file.Write((LPCTSTR)m_dspCode, ((unsigned)m_dspCode.GetLength() + 1) * sizeof(TCHAR));

		// state filename
		file.Write((LPCTSTR)m_stateFilename, ((unsigned)m_stateFilename.GetLength() + 1) * sizeof(TCHAR));

		// dsp cycles
		file.Write(&m_dspCycles, sizeof(m_dspCycles));

		// ymemsize
		file.Write(&m_yMemSize, sizeof(m_yMemSize));

		// Scratch size
		file.Write(&m_scratchLength, sizeof(m_scratchLength));

		// activeconfig
		file.Write(&m_activeConfig, sizeof(m_activeConfig));

		// colorflag
		file.Write(&m_colorFlag, sizeof(m_colorFlag));

		// mixbin
		file.Write(&m_mixbin, sizeof(m_mixbin));

		// iir2 frequency
		file.Write(&m_fIIR2Frequency, sizeof(m_fIIR2Frequency));

		// iir2 Q
		file.Write(&m_fIIR2Q, sizeof(m_fIIR2Q));

		// iir2 Gain DB
		file.Write(&m_fIIR2GainDB, sizeof(m_fIIR2GainDB));

		// iir2 frequency default
		file.Write(&m_fIIR2Frequency, sizeof(m_fIIR2FrequencyDefault));

		// iir2 Q default
		file.Write(&m_fIIR2QDefault, sizeof(m_fIIR2QDefault));

		// iir2 gain db default
		file.Write(&m_fIIR2GainDBDefault, sizeof(m_fIIR2GainDBDefault));

		// inputs
		word = (WORD)m_inputs.GetSize();
		file.Write(&word, sizeof(word));
		for(i=0; i<m_inputs.GetSize(); ++i)
			m_inputs[i]->Write(file);

		// outputs
		word = (WORD)m_outputs.GetSize();
		file.Write(&word, sizeof(word));
		for(i=0; i<m_outputs.GetSize(); ++i)
			m_outputs[i]->Write(file);

		// configs
		word = (WORD)m_configs.GetSize();
		file.Write(&word, sizeof(word));
		for(i=0; i<m_configs.GetSize(); ++i)
		{
			file.Write((LPCTSTR)m_configs[i].name, ((unsigned)m_configs[i].name.GetLength() + 1) * sizeof(TCHAR));
			file.Write((LPCTSTR)m_configs[i].stateFilename, ((unsigned)m_configs[i].stateFilename.GetLength() + 1) * sizeof(TCHAR));
		}

		// Write the current parameter settings
		word = (WORD)m_parameters.GetSize();
		file.Write(&word, sizeof(word));
		for(i=0; i<m_parameters.GetSize(); ++i)
		{
			string = m_parameters[i].GetName();
			file.Write((LPCTSTR)string, ((unsigned)string.GetLength() + 1) * sizeof(TCHAR));
			dword = m_parameters[i].GetOffset();
			file.Write(&dword, sizeof(dword));
			dword = m_parameters[i].GetValue();
			file.Write(&dword, sizeof(dword));
		}
	}
	catch(...)
	{
		throw;
	}
}

//-----------------------------------------------------------------o-------------
//	CModule::FixPointers
//------------------------------------------------------------------------------
void 
CModule::FixPointers(
					 IN const CGraph* pGraph
					 )
/*++

Routine Description:

	Fixes the internal pointers from a read

Arguments:

	IN pGraph -	Graph to use to correct pointers

Return Value:

	None

--*/
{
	int	i;

	m_pGraph = (CGraph*)pGraph;

	for(i=0; i<m_inputs.GetSize(); ++i)
		m_inputs[i]->FixPointers(pGraph);
	for(i=0; i<m_outputs.GetSize(); ++i)
		m_outputs[i]->FixPointers(pGraph);

	m_bBadData = FALSE;
}

//------------------------------------------------------------------------------
//	CModule::Read
//------------------------------------------------------------------------------
BOOL
CModule::Read(
			  IN CFile& file,
			  IN BYTE	version
			  )
/*++

Routine Description:

	Constructor. Reads from a file

Arguments:

	IN file -		File to read from
	IN version -	File version

Return Value:

	None

--*/
{
	int				i;
	WORD			word;
	DWORD			dwOffset;
	DWORD			dwValue;
	CString			string;
	CJack*			pJack;
	CModule::Config	config;
	CParameter		param;
	BOOL			ret	= TRUE;
	
	m_bBadData = TRUE;

	// name
	if(!CGraph::ReadString(file, m_name))
		return FALSE;

	// effect name
	if(version >= 4)
	{
		if(!CGraph::ReadString(file, m_effectName))
			return FALSE;
	}
	else
		m_effectName = m_name;

	// Special Case: IIR2?
	if(!m_effectName.CompareNoCase(_T("iir2")))
		m_bIIR2 = TRUE;

	// Special Case: I3DL2
	else if(!m_effectName.CompareNoCase(_T("i3dl2 reverb")) ||
			!m_effectName.CompareNoCase(_T("i3dl2 24k reverb")))
	{
		m_bI3DL2 = TRUE;
	}

	// Read the id
	if(!CUnique::Read(file, version))
		return FALSE;
	
	// Version 3 or newer?
	if(version >= 3)
	{
		// ini name
		if(!CGraph::ReadString(file, m_iniName))
			return FALSE;

		// index
		if(file.Read(&m_index, sizeof(m_index)) != sizeof(m_index))
			return FALSE;
	}

	// Type
	if(file.Read(&m_type, sizeof(m_type)) != sizeof(m_type))
		return FALSE;

	// State
	if(file.Read(&m_state, sizeof(m_state)) != sizeof(m_state))
		return FALSE;

	// Rect
	if(file.Read((LPRECT)m_rect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// nameRect
	if(file.Read((LPRECT)m_nameRect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// config rect
	if(file.Read((LPRECT)m_configRect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// stats
	if(!CGraph::ReadString(file, m_stats))
		return FALSE;

	// stats rect
	if(file.Read((LPRECT)m_statsRect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// dspcode
	if(!CGraph::ReadString(file, m_dspCode))
		return FALSE;

	// state filename
	if(!CGraph::ReadString(file, m_stateFilename))
		return FALSE;

	// dsp cycles
	if(file.Read(&m_dspCycles, sizeof(m_dspCycles)) != sizeof(m_dspCycles))
		return FALSE;

	// ymemsize
	if(file.Read(&m_yMemSize, sizeof(m_yMemSize)) != sizeof(m_yMemSize))
		return FALSE;

	// Scratch size
	if(file.Read(&m_scratchLength, sizeof(m_scratchLength)) != sizeof(m_scratchLength))
		return FALSE;

	// activeconfig
	if(file.Read(&m_activeConfig, sizeof(m_activeConfig)) != sizeof(m_activeConfig))
		return FALSE;

	// colorflag
	if(file.Read(&m_colorFlag, sizeof(m_colorFlag)) != sizeof(m_colorFlag))
		return FALSE;

	// mixbin
	if(file.Read(&m_mixbin, sizeof(m_mixbin)) != sizeof(m_mixbin))
		return FALSE;

	// Version 3 or newer?
	if(version >= 3)
	{
		// iir2 frequency
		if(file.Read(&m_fIIR2Frequency, sizeof(m_fIIR2Frequency)) != sizeof(m_fIIR2Frequency))
			return FALSE;

		// iir2 Q
		if(file.Read(&m_fIIR2Q, sizeof(m_fIIR2Q)) != sizeof(m_fIIR2Q))
			return FALSE;

		// iir2 Gain DB
		if(file.Read(&m_fIIR2GainDB, sizeof(m_fIIR2GainDB)) != sizeof(m_fIIR2GainDB))
			return FALSE;

		// iir2 frequency default
		if(file.Read(&m_fIIR2FrequencyDefault, sizeof(m_fIIR2FrequencyDefault)) != sizeof(m_fIIR2FrequencyDefault))
			return FALSE;

		// iir2 Q default
		if(file.Read(&m_fIIR2QDefault, sizeof(m_fIIR2QDefault)) != sizeof(m_fIIR2QDefault))
			return FALSE;

		// iir2 gain db default
		if(file.Read(&m_fIIR2GainDBDefault, sizeof(m_fIIR2GainDBDefault)) != sizeof(m_fIIR2GainDBDefault))
			return FALSE;
	}

	// inputs
	if(file.Read(&word, sizeof(word)) != sizeof(word))
		return FALSE;
	for(i=0; ret && i<word; ++i)
	{
		pJack	= new CJack(this);
		ret		= pJack->Read(file, version);
		if(ret)
			m_inputs.Add(pJack);
		else
			delete pJack;
	}

	// outputs
	if(ret)
	{
		ret = (file.Read(&word, sizeof(word)) == sizeof(word));
		for(i=0; ret && i<word; ++i)
		{
			pJack	= new CJack(this);
			ret		= pJack->Read(file, version);
			if(ret)
				m_outputs.Add(pJack);
			else
				delete pJack;
		}
	}

	// configs
	if(ret)
	{
		ret = (file.Read(&word, sizeof(word)) == sizeof(word));
		for(i=0; ret && i<word; ++i)
		{
			ret = CGraph::ReadString(file, config.name);
			if(ret)
			{
				ret = CGraph::ReadString(file, config.stateFilename);
				if(ret)
					AddConfiguration(config.name, config.stateFilename);
			}
		}
	}

	// Read the current parameter settings
	if(ret)
	{
		ret = (file.Read(&word, sizeof(word)) == sizeof(word));
		for(i=0; ret && i<word; ++i)
		{
			ret = CGraph::ReadString(file, string);
			if(ret)
			{
				ret = (file.Read(&dwOffset, sizeof(dwOffset)) == sizeof(dwOffset));
				if(ret)
				{
					ret = (file.Read(&dwValue, sizeof(dwValue)) == sizeof(dwValue));
					if(ret)
					{
						param.SetName(string);
						param.SetOffset(dwOffset);
						param.SetValue(dwValue);
						m_parameters.Add(param);
					}
				}
			}
		}
	}

	// Success?
	if(ret)
	{
		SetDisplayName();
		CreatePenAndBrush();

		// Is this an IIR2?
		if(m_bIIR2)
		{
			ReorderIIR2Params();
			ConvertToIIR2();
		}

		// Or an i3dl2 reverb?
		else if(m_bI3DL2)
			SetI3DL2Parameters();

		return TRUE;
	}

	// Error cleanup
	for(i=0; i<m_inputs.GetSize(); ++i)
		delete m_inputs[i];
	m_inputs.RemoveAll();
	for(i=0; i<m_outputs.GetSize(); ++i)
		delete m_outputs[i];
	m_outputs.RemoveAll();
	m_configs.RemoveAll();
	m_parameters.RemoveAll();

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::SetParameters
//------------------------------------------------------------------------------
BOOL
CModule::SetParameters(
					   IN CWinDspImageBuilder*	pImageBuilder, 
					   IN const char*			pFilename
					   )
/*++

Routine Description:

	Sets the effect parameters by calling a CWinDspImageBuilder method

Arguments:

	IN pImageBuilder -	CWinDspImageBuilder to call
	IN pFilename -		Filename

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	FX_STATE_PARAMETERS*			pStateParms;
	int								i;
	CParameter						parm;
	CArray<CParameter, CParameter&>	tempParms;
	int								p;
	CString							string;

	if(!pImageBuilder)
		return FALSE;

	// Copy existing parms to update loaded values
	if(m_parameters.GetSize())
	{
		tempParms.Copy(m_parameters);
		m_parameters.RemoveAll();
	}

	// Read the parameters
	if(pImageBuilder->ParseParameterDescriptions(pFilename, &pStateParms) != ERROR_SUCCESS)
	{
		if(pStateParms)
			delete pStateParms;
		return FALSE;
	}

	for(i=0; i<(int)pStateParms->dwNumParams; ++i)
	{
		parm = pStateParms->aParamDescriptions[i];
		m_parameters.Add(parm);
	}

	// Look through the saved parameters and update the loaded ones
	for(i=0; i<tempParms.GetSize(); ++i)
	{
		string = tempParms[i].GetName();
		for(p=0; p<m_parameters.GetSize(); ++p)
		{
			// Does the name and offset match?
			if((string.Compare(m_parameters[p].GetName()) == 0) &&
			   (tempParms[i].GetOffset() == m_parameters[p].GetOffset()))
			{
				m_parameters[p].SetValue(tempParms[i].GetValue());
				break;
			}
		}
	}

	delete pStateParms;

	if(m_bIIR2)
		ReorderIIR2Params();

	return TRUE;
}

//------------------------------------------------------------------------------
//	CModule::ShowProperties
//------------------------------------------------------------------------------
void
CModule::ShowProperties(void)
/*++

Routine Description:

	Displays a properties dialog that allow the user to edit the parameters

Arguments:

	None

Return Value:

	None

--*/
{
	int					i;
	CKnob*				pKnob;
	double				fVal;
	double				fDefault;
	double				fMin;
	double				fMax;
	CKnob::DataType		type;
	CString				format;
	CRect				rect;
	LONG				x;
	LONG				y;

	// If the dialog already exists, just bring it to the top
	if(m_pParameterDialog)
	{
		m_pParameterDialog->BringWindowToTop();
		return;
	}

	// Create the dialog
	m_pParameterDialog = new CParameterDialog(this, m_displayName);
	m_pParameterDialog->Create(IDD_PARAMETER_DIALOG);

	// Special Cases: IIR2
	if(m_bIIR2)
	{
		pKnob = new CKnob(this, 0, _T("Frequency"), TRUE, CKnob::TYPE_FLOAT, 0.0, 20000.0, 
						  m_fIIR2Frequency, m_fIIR2FrequencyDefault, 
						  _T("%.0lf"), _T("Hz"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 1, _T("Q"), TRUE, CKnob::TYPE_FLOAT, 0.01, 30.0, m_fIIR2Q, 
						  m_fIIR2QDefault, _T("%.2lf"), _T(""));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 2, _T("Gain"), TRUE, CKnob::TYPE_FLOAT, -30.0, 30.0, 
						  m_fIIR2GainDB, m_fIIR2GainDBDefault, _T("%.1lf"),
						  _T("DB"));
		m_pParameterDialog->AddKnob(pKnob);
	}

	else if(m_bI3DL2)
	{
		pKnob = new CKnob(this, 0, _T("Room"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINROOM, 
						  (double)I3DL2LISTENER_MAXROOM, 
						  (double)m_I3DL2Listener.lRoom, 
						  (double)m_I3DL2ListenerDefault.lRoom,
						  _T("%ld"), _T("mB"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 1, _T("Room HF"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINROOMHF,
						  (double)I3DL2LISTENER_MAXROOMHF, 
						  (double)m_I3DL2Listener.lRoomHF, 
						  (double)m_I3DL2ListenerDefault.lRoomHF, 
						  _T("%ld"), _T("mB"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 2, _T("Room Rolloff"), TRUE, CKnob::TYPE_FLOAT, 
						  (double)I3DL2LISTENER_MINROOMROLLOFFFACTOR, 
						  (double)I3DL2LISTENER_MAXROOMROLLOFFFACTOR, 
						  (double)m_I3DL2Listener.flRoomRolloffFactor, 
						  (double)m_I3DL2ListenerDefault.flRoomRolloffFactor, 
						  _T("%.2lf"), _T(""));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 3, _T("Decay Time"), TRUE, CKnob::TYPE_FLOAT, 
						  (double)I3DL2LISTENER_MINDECAYTIME, 
						  (double)I3DL2LISTENER_MAXDECAYTIME, 
						  (double)m_I3DL2Listener.flDecayTime, 
						  (double)m_I3DL2ListenerDefault.flDecayTime, 
						  _T("%.3lf"), _T("Seconds"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 4, _T("Decay HF Ratio"), TRUE, CKnob::TYPE_FLOAT, 
						  (double)I3DL2LISTENER_MINDECAYHFRATIO, 
						  (double)I3DL2LISTENER_MAXDECAYHFRATIO, 
						  (double)m_I3DL2Listener.flDecayHFRatio, 
						  (double)m_I3DL2ListenerDefault.flDecayHFRatio, 
						  _T("%.2lf"), _T(""));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 5, _T("Reflections"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINREFLECTIONS,
						  (double)I3DL2LISTENER_MAXREFLECTIONS, 
						  (double)m_I3DL2Listener.lReflections, 
						  (double)m_I3DL2ListenerDefault.lReflections, 
						  _T("%ld"), _T(""));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 6, _T("Reflections Delay"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINREFLECTIONSDELAY * 1000.0,// Convert to ms
						  (double)I3DL2LISTENER_MAXREFLECTIONSDELAY * 1000.0,// Convert to ms
						  (double)m_I3DL2Listener.flReflectionsDelay * 1000.0,// Convert to ms
						  (double)m_I3DL2ListenerDefault.flReflectionsDelay * 1000.0,// Convert to ms
						  _T("%ld"), _T("ms"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 7, _T("Reverb"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINREVERB,
						  (double)I3DL2LISTENER_MAXREVERB, 
						  (double)m_I3DL2Listener.lReverb, 
						  (double)m_I3DL2ListenerDefault.lReverb, 
						  _T("%ld"), _T(""));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 8, _T("Reverb Delay"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINREVERBDELAY * 1000.0,// Convert to ms
						  (double)I3DL2LISTENER_MAXREVERBDELAY * 1000.0,// Convert to ms 
						  (double)m_I3DL2Listener.flReverbDelay * 1000.0,// Convert to ms 
						  (double)m_I3DL2ListenerDefault.flReverbDelay * 1000.0,// Convert to ms 
						  _T("%ld"), _T("ms"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 9, _T("Diffusion"), TRUE, CKnob::TYPE_FLOAT, 
						  (double)I3DL2LISTENER_MINDIFFUSION,
						  (double)I3DL2LISTENER_MAXDIFFUSION, 
						  (double)m_I3DL2Listener.flDiffusion, 
						  (double)m_I3DL2ListenerDefault.flDiffusion, 
						  _T("%.1lf"), _T("%"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 10, _T("Density"), TRUE, CKnob::TYPE_FLOAT, 
						  (double)I3DL2LISTENER_MINDENSITY,
						  (double)I3DL2LISTENER_MAXDENSITY, 
						  (double)m_I3DL2Listener.flDensity, 
						  (double)m_I3DL2ListenerDefault.flDensity, 
						  _T("%.1lf"), _T("%"));
		m_pParameterDialog->AddKnob(pKnob);
		pKnob = new CKnob(this, 11, _T("HF Reference"), TRUE, CKnob::TYPE_LONG, 
						  (double)I3DL2LISTENER_MINHFREFERENCE,
						  (double)I3DL2LISTENER_MAXHFREFERENCE, 
						  (double)m_I3DL2Listener.flHFReference, 
						  (double)m_I3DL2ListenerDefault.flHFReference, 
						  _T("%ld"), _T("Hz"));
		m_pParameterDialog->AddKnob(pKnob);
	}

	// If there are no parameters, there will be no dialog
	else if(m_parameters.GetSize() != 0)
	{
		// Create the knobs for all the parameters
		for(i=0; i<m_parameters.GetSize(); ++i)
		{
			// Float data?
			if(m_parameters[i].GetType() == DSFX_PARAM_TYPE_FLOAT)
			{
				type		= CKnob::TYPE_DOUBLE;
				fMin		= ToDouble(m_parameters[i].GetMin());
				fMax		= ToDouble(m_parameters[i].GetMax());
				fVal		= ToDouble(m_parameters[i].GetValue());
				fDefault	= ToDouble(m_parameters[i].GetDefault());
				if(fabs(fMax - fMin) < 1.0)
					format = _T("%.2le");
				else
					format = _T("%.2lf");
			}

			// Integer data
			else
			{
				type		= CKnob::TYPE_UNSIGNED_LONG;
				format		= _T("%lu");
				fMin		= m_parameters[i].GetMin();
				fMax		= m_parameters[i].GetMax();
				fVal		= (double)m_parameters[i].GetValue();
				fDefault	= (double)m_parameters[i].GetDefault();
			}

			pKnob = new CKnob(this, i, m_parameters[i].GetName(), 
							  m_parameters[i].GetAtomic(), type, fMin, fMax, fVal, 
							  fDefault, format, m_parameters[i].GetUnits());
			m_pParameterDialog->AddKnob(pKnob);
		}
	}

	else 
		return;

	// Update the dialog
	m_pParameterDialog->UpdateKnobs();
	m_pParameterDialog->SetRealtime(m_bRealtime);
	m_bRealtime = m_pParameterDialog->GetRealtime();

	// Place the dialog
	rect = m_rect;
	m_pGraph->GetParent()->ClientToScreen(&rect);
	x = (rect.right + rect.left) / 2;
	y = (rect.bottom + rect.top) / 2;
	m_pParameterDialog->GetWindowRect(&rect);
	m_pParameterDialog->MoveWindow(x, y, rect.Width(), rect.Height());

	// Display the dialog
	m_pParameterDialog->ShowWindow(SW_SHOW);
}

//------------------------------------------------------------------------------
//	CModule::SetEffectName
//------------------------------------------------------------------------------
void 
CModule::SetEffectName(void)
/*++

Routine Description:

	Pops up a dialog that allows the user to select a new name for the effect

Arguments:

	None

Return Value:

	None

--*/
{
	CEffectNameDialog	dialog(m_effectName, m_name);

	if(dialog.DoModal() == IDOK)
	{
		m_name = dialog.GetName();
		SetDisplayName();
		if(m_pGraph)
			m_pGraph->Draw(m_rect);
		
		// If the properties dialog is visible change the name there as well
		if(m_pParameterDialog)
			m_pParameterDialog->SetWindowText(m_name);
	}
}

//------------------------------------------------------------------------------
//	CModule::ChangeParameter
//------------------------------------------------------------------------------
void 
CModule::ChangeParameter(
						 IN const CKnob*	pKnob, 
						 IN BOOL			bTransmit
						 )
/*++

Routine Description:

	Changes the value of a parameter

Arguments:

	IN index -	Index of parameter to change

Return Value:

	None

--*/
{
	DWORD	dwValue;

	if(pKnob == NULL)
	{
		if(bTransmit)
			m_pGraph->TransmitModuleParameters(this);
		return;
	}

	// Special Case: IIR2
	if(m_bIIR2)
	{
		switch(pKnob->GetIndex())
		{
		case 0:	// Frequency
			m_fIIR2Frequency = pKnob->GetPos();
			break;
		case 1:	// Q
			m_fIIR2Q = pKnob->GetPos();
			break;
		case 2:	// Gain DB
			m_fIIR2GainDB = pKnob->GetPos();
			break;
		default:
			break;
		}

		ConvertToIIR2();
		m_pGraph->ChangesMade();

		if(bTransmit)
			m_pGraph->TransmitModuleParameters(this);
	}

	// Special Case: I3DL2
	else if(m_bI3DL2)
	{
		switch(pKnob->GetIndex())
		{
		case 0:
			m_I3DL2Listener.lRoom = (LONG)pKnob->GetPos();
			break;
		case 1:
			m_I3DL2Listener.lRoomHF = (LONG)pKnob->GetPos();
			break;
		case 2:
			m_I3DL2Listener.flRoomRolloffFactor = (FLOAT)pKnob->GetPos();
			break;
		case 3:
			m_I3DL2Listener.flDecayTime = (FLOAT)pKnob->GetPos();
			break;
		case 4:
			m_I3DL2Listener.flDecayHFRatio = (FLOAT)pKnob->GetPos();
			break;
		case 5:
			m_I3DL2Listener.lReflections = (LONG)pKnob->GetPos();
			break;
		case 6:
			m_I3DL2Listener.flReflectionsDelay = (FLOAT)(pKnob->GetPos() / 1000.0); // Convert to seconds
			break;
		case 7:
			m_I3DL2Listener.lReverb = (LONG)pKnob->GetPos();
			break;
		case 8:
			m_I3DL2Listener.flReverbDelay = (FLOAT)(pKnob->GetPos() / 1000.0); // Convert to seconds
			break;
		case 9:
			m_I3DL2Listener.flDiffusion = (FLOAT)pKnob->GetPos();
			break;
		case 10:
			m_I3DL2Listener.flDensity = (FLOAT)pKnob->GetPos();
			break;
		case 11:
			m_I3DL2Listener.flHFReference = (FLOAT)pKnob->GetPos();
			break;
		default:
			break;
		}

		// Since these changes cannot be saved, don't mark changes made
		//m_pGraph->ChangesMade();

		if(bTransmit)
			m_pGraph->TransmitModuleParameters(this);

	}

	else
	{
		if((pKnob->GetIndex() >= 0) && (pKnob->GetIndex() < m_parameters.GetSize()))
		{
			if(m_parameters[pKnob->GetIndex()].GetType() == DSFX_PARAM_TYPE_FLOAT)
				dwValue = ToDWORD(pKnob->GetPos());
			else
				dwValue = (DWORD)pKnob->GetPos();

			m_parameters[pKnob->GetIndex()].SetValue(dwValue);
			m_pGraph->ChangesMade();

			if(bTransmit)
				m_pGraph->TransmitModuleParameters(this, pKnob->GetIndex());
		}
	}
}

//------------------------------------------------------------------------------
//	CModule::WriteStateFile
//------------------------------------------------------------------------------
BOOL
CModule::WriteStateFile(
						IN LPCTSTR	pFilePath
						)
/*++

Routine Description:

	Generates a state file for the current parameters

Arguments:

	IN pFilePath -	Path to file
	
Return Value:

	TRUE on success, FALSE on failure

--*/
{
	CStdioFile	inFile;
	CStdioFile	outFile;
	CString		string;
	CString		line;
	BOOL		bModified;
	int			i;
	int			index;
	CString		key;

	// Open the real state ini file
	string.Format(_T("%s\\%s"), m_pGraph->GetIniFilePath(), m_stateFilename);
	if(!inFile.Open(string, CFile::modeRead | CFile::typeText))
		return FALSE;

	// Open the output file
	if(!outFile.Open(pFilePath, CFile::modeWrite | CFile::modeCreate | CFile::typeText))
	{
		inFile.Close();
		return FALSE;
	}

	// Are there any modifiable parameters, or modified parameters
	for(bModified=FALSE, i=0; !bModified && i<m_parameters.GetSize(); ++i)
	{
		if(m_parameters[i].GetValue() != m_parameters[i].GetDefault())
			bModified = TRUE;
	}

	// If the parameters have been modified, write a modified file
	if(bModified)
	{
		// Read the file
		while(inFile.ReadString(line))
		{
			line.TrimLeft();
			line.TrimRight();
			string = line.Left(8);
			if(!string.CompareNoCase(_T("fx_param")))
			{
				string = line.Right(line.GetLength()-8);
				index = atoi(string);

				if(index < m_parameters.GetSize())
				{
					string = line.Left(line.Find('='));
					string.TrimRight();

					// Find the key
					key.Format(_T("FX_PARAM%d"), index);

					if(string == key)
					{
						string.Format(_T("FX_PARAM%d=%lu"), index, 
									  (DWORD)m_parameters[index].GetValue());
						outFile.WriteString(string);
					}
					else
						outFile.WriteString(line);	
				}
				else
					outFile.WriteString(line);
			}
			else
				outFile.WriteString(line);	
			outFile.WriteString(_T("\n"));	
		}
	}

	// If no parameters are modifed, just copy the file
	else
	{
		while(inFile.ReadString(line))
		{
			outFile.WriteString(line);
			outFile.WriteString(_T("\n"));	
		}
	}

	inFile.Close();
	outFile.Close();
	return TRUE;
}

//------------------------------------------------------------------------------
//	CModule::SetActiveConfig
//------------------------------------------------------------------------------
void 
CModule::SetActiveConfig(
						 IN int	config
						 )
/*++

Routine Description:

	Sets the active configuration

Arguments:

	IN config -	Must be < m_configs.GetSize() and > 0

Return Value:

	None

--*/
{
	m_activeConfig = config;
	
	if(m_bI3DL2)
		SetI3DL2Parameters();
}

//------------------------------------------------------------------------------
//	CModule::AreParametersModified
//------------------------------------------------------------------------------
BOOL 
CModule::AreParametersModified(void) const
/*++

Routine Description:

	Returns TRUE if any of the modules parameters have been modified

Arguments:

	None

Return Value:

	TRUE if any parameter is not a default parameter, FALSE otherwise

--*/
{
	int	i;

	for(i=0; i<m_parameters.GetSize(); ++i)
	{
		if(m_parameters[i].GetValue() != m_parameters[i].GetDefault())
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::ParameterDialogDestroyed
//------------------------------------------------------------------------------
void 
CModule::ParameterDialogDestroyed(void)
/*++

Routine Description:

	Called whenthe parameters dialog is destroyed

Arguments:

	None

Return Value:

	None

--*/
{
	m_pParameterDialog = NULL;
}

//------------------------------------------------------------------------------
//	CModule::CreatePenAndBrush
//------------------------------------------------------------------------------
void 
CModule::CreatePenAndBrush(void)
/*++

Routine Description:

	Creates a pen and brush based on the type of module

Arguments:

	None

Return Value:

	None

--*/
{
	m_movingBrush.CreateSolidBrush(RGB(100, 200, 100));

	switch(m_type)
	{
	case CModule::INPUT_MIXBIN:
		m_pen.CreatePen(PS_SOLID, 1, RGB(128, 64, 0));

		if(m_colorFlag == CModule::COLOR_SPEAKER)
			m_brush.CreateSolidBrush(RGB(255, 110, 90));
		else if(m_colorFlag == CModule::COLOR_XTALK)
			m_brush.CreateSolidBrush(RGB(255, 130, 90));
		else if(m_colorFlag == CModule::COLOR_I3DL2)
			m_brush.CreateSolidBrush(RGB(255, 150, 90));
		else 
			m_brush.CreateSolidBrush(RGB(255, 180, 90));
		break;

	case CModule::OUTPUT_MIXBIN:
		m_pen.CreatePen(PS_SOLID, 1, RGB(128, 90, 0));

		if(m_colorFlag == CModule::COLOR_SPEAKER)
			m_brush.CreateSolidBrush(RGB(255, 130, 120));
		else if(m_colorFlag == CModule::COLOR_XTALK)
			m_brush.CreateSolidBrush(RGB(255, 150, 120));
		else if(m_colorFlag == CModule::COLOR_I3DL2)
			m_brush.CreateSolidBrush(RGB(255, 170, 120));
		else 
			m_brush.CreateSolidBrush(RGB(255, 200, 120));
		break;

	case CModule::EFFECT:
		m_pen.CreatePen(PS_SOLID, 1, RGB(0, 64, 128));
		m_brush.CreateSolidBrush(RGB(120, 200, 255));
		break;
	}
}

//------------------------------------------------------------------------------
//	CModule::ConvertToIIR2
//------------------------------------------------------------------------------
void 
CModule::ConvertToIIR2(void)
/*++

Routine Description:

	Converts from frequency and Q to the 5 iir2 parameters

Arguments:

	None

Return Value:

	None

--*/
{
	double	x0;
	double	x1;
	double	x2;
	double	z0;
	double	z2;
    double	z0m1;
	double	gm1;
	double	q		= m_fIIR2Q;
	double	f		= m_fIIR2Frequency / 48000.0;
    double	g		= pow(10.0, m_fIIR2GainDB * (1.0 / 40.0));
    double	wc		= cTwoPi * f;
    double	alpha	= (sinh(1.0 / (q + q)) * sin(wc));

    x0		= 1.0 + (alpha * g);
    x1		= -2.0 * cos(wc);
    x2		= 1.0 - (alpha * g);
    gm1		= alpha / g;
    z0		= 1.0 + gm1;
    z2		= 1.0 - gm1;
    z0m1	= 0.5 / z0;

	m_parameters[2].SetValue(ToDWORD(x0 * z0m1));		// b0
	m_parameters[3].SetValue(ToDWORD(x1 * z0m1));		// b1
	m_parameters[4].SetValue(ToDWORD(x2 * z0m1));		// b2
	m_parameters[0].SetValue(ToDWORD(-(x1 * z0m1)));	// a1 = -b1
	m_parameters[1].SetValue(ToDWORD(-z2 * z0m1));		// a2
}

#if 0
ComputeIIR2Hz_Parametric(float *h, float frequency, float q, float gainDB)
// h			  ptr to five coefficients: b0,b1,b2,a1,a2
// frequency        fc/fs normalized frequency
// q                fc/dF range [0 .. N], where dF = bandwidth
// gainDB           gain (Decibels) range [-N .. N]
{
    float x[3], z[3];
    float G     = (float) pow(10.0, gainDB*(1.0/40.0));
    float wc    = kTwoPif*frequency;
    float cosWc = (float) cos(wc);
    float alpha = (float)(sinh(1.0/(q+q))*sin(wc));
    float z0m1, Gm1;

    x0 =  1.0f + alpha*G;
    x1 = -2.0f * cosWc;
    x2 =  1.0f - alpha*G;
    
    Gm1 = alpha/G;
    z0 =  1.0f + Gm1;
//z[1] =  -2.0 * cosWc; // a1 = b1
    z2 =  1.0f - Gm1;
    
    z0m1 = 0.5f / z0;	// was 1.0/a[0],  0.5 to scale into frac2.22 format

    B0 = (x0 * z0m1);
    B1 = (x1 * z0m1);
    B2 = (x2 * z0m1);
    
    A1 = (-B1     ); // a1 = b1
    A2 = (-z2 * z0m1);
#endif

#if 0
//------------------------------------------------------------------------------
//	CModule::ConvertFromIIR2
//------------------------------------------------------------------------------
void 
CModule::ConvertFromIIR2(
						 OUT double&	freq, 
						 OUT double&	q,
						 OUT double&	gain,
						 IN double		a1,
						 IN double		a2,
						 IN double		,
						 IN double		, 
						 IN double		
						 )
/*++

Routine Description:

	Converts to frequency and Q from the 5 iir2 parameters

Arguments:

	OUT freq -	Frequency 
	OUT q -		Q
	OUT gain -	Gain (DB)
	IN a1 -		A1
	IN a2 -		A2
	IN b0 -		B0	(Ignored)
	IN b1 -		B1	(Ignored)
	IN b2 -		B2	(Ignored)

Return Value:

	None

--*/
{
	double	wc;
	double	d;
	double	t;
	double	beta;
	double	gamma;

	gamma	= a1 / cScale24;
	beta	= -a2 / cScale24;
	
	t		= (1.0 - (2.0 * beta)) / ((2.0 * beta) + 1);
	wc		= acos(gamma / (0.5 + beta));
	d		= (2.0 * t) / sin(wc);

	// Result
	freq	= wc / cTwoPi * cFs;
	q		= wc / (2.0 * atan((d * sin(wc)) / 2.0));
}
#endif

#if 0

f = cTwoPi * frequency;
q = Q + Q;
g = gainDB * 0.025;

b0 = (1.0 + ((sinh(1.0 / q) * sin(f)) * pow(10.0, g)) ) * (0.5 / (1.0 + ((sinh(1.0 / q) * sin(f)) / pow(10.0, g))));

b1 = (-2.0 * cos(f)) * (0.5 / (1.0 + ((sinh(1.0 / q) * sin(f)) / pow(10.0, g))));

b2 = (1.0 - ((sinh(1.0 / q) * sin(f)) * pow(10.0, g))) * (0.5 / (1.0 + ((sinh(1.0 / q) * sin(f)) / pow(10.0, g))));

a1 = -b1;

a2 = -(1.0 - ((sinh(1.0 / q) * sin(f)) / pow(10.0, g))) * (0.5 / (1.0 + ((sinh(1.0 / q) * sin(f)) / pow(10.0, g))));






sinh(x) = (pow(e, x) - pow(e, -x)) / 2.0

log(pow(e, x)) = x







    double	g		= pow(10.0, gain * (1.0 / 40.0));
    double	wc		= cTwoPi * freq;
    double	cosWc	= cos(wc);
    double	alpha	= (sinh(1.0 / (q + q)) * sin(wc));

    x0 = (1.0 + (alpha * g));
    x1 = (-2.0 * cosWc);
    x2 = (1.0 - (alpha * g));
    
    gm1	= (alpha / g);
    y0	= (1.0 + gm1);
    y2	= (1.0 - gm1);
    
    y0m1	= (0.5 / y0);
    b0		= (1.0 + (alpha * g)) * (0.5 / (1.0 + (alpha / g)));
    b1		= x1 * y0m1;
    b2		= x2 * y0m1;
    a1		= -b1;
    a2		= -y2 * y0m1;
}
#endif

//------------------------------------------------------------------------------
//	CModule::ReorderIIR2Params
//------------------------------------------------------------------------------
void
CModule::ReorderIIR2Params(void)
/*++

Routine Description:

	Reorders the IIR2 params to be in the following order: a1 a2 b0 b1 b2 and
	generates frequency and Q from the values

Arguments:

	None

Return Value:

	None

--*/
{
	CParameter	param;
	CString		string;
	int			i;
	int			ii;
	TCHAR*		list[5] = {_T("iir2 a1"), _T("iir2 a2"), _T("iir2 b0"), _T("iir2 b1"), _T("iir2 b2")};

	for(i=0; i<4; ++i)
	{
		string = m_parameters[i].GetName();
		if(string.CompareNoCase(list[i]) != 0)
		{
			param = m_parameters[i];
			for(ii=i+1; ii<5; ++ii)
			{
				string = m_parameters[ii].GetName();
				if(string.CompareNoCase(list[i]) == 0)
				{
					m_parameters[i] = m_parameters[ii];
					m_parameters[ii] = param;
					break;
				}
			}
		}
	}

//	ConvertFromIIR2(m_fIIR2Frequency, m_fIIR2Q, m_fIIR2GainDB,
//					ToDouble(m_parameters[0].GetValue()), 
//					ToDouble(m_parameters[1].GetValue()), 
//					ToDouble(m_parameters[2].GetValue()),
//					ToDouble(m_parameters[3].GetValue()), 
//					ToDouble(m_parameters[4].GetValue()));
}

//------------------------------------------------------------------------------
//	CModule::ToDouble
//------------------------------------------------------------------------------
double 
CModule::ToDouble(
				  IN DWORD dwVal
				  )
/*++

Routine Description:

	Converts a 24bit DWORD float to a double

Arguments:

	IN val -	Value to convert

Return Value:

	None

--*/
{
	if(dwVal >= 0x800000)
		return -((0xFFFFFF - dwVal) / (double)0x800000);

	return ((double)dwVal / (double)0x800000);
}

//------------------------------------------------------------------------------
//	CModule::ToDWORD
//------------------------------------------------------------------------------
DWORD 
CModule::ToDWORD(
				 IN double fVal
				 )
/*++

Routine Description:

	Converts a double to a 24bit DWORD

Arguments:

	IN val -	Value to convert

Return Value:

	None

--*/
{
	if(fVal >= 1.0)
		return 0x7FFFFF;

	else if (fVal <= -1.0)
		return 0x800000;

	if(fVal > 0.0)
		return 0xffffff & (DWORD)((double)0x800000 * fVal);

	return (0xFFFFFF - (DWORD)(-fVal * (double)0x800000));
}

//------------------------------------------------------------------------------
//	CModule::SetI3DL2Parameters
//------------------------------------------------------------------------------
void
CModule::SetI3DL2Parameters(void)
/*++

Routine Description:

	Sets the i3dl2 parameters based on the active configuration

Arguments:

	None

Return Value:

	None

--*/
{
	if(!m_bI3DL2)
		return;

	if(m_activeConfig >= 0)
	{
		if(!m_configs[m_activeConfig].name.CompareNoCase(_T("default")))
			m_I3DL2Listener = cListenerDefault;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("generic")))
			m_I3DL2Listener = cListenerGeneric;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("padded cell")))
			m_I3DL2Listener = cListenerPaddedCell;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("room")))
			m_I3DL2Listener = cListenerRoom;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("bathroom")))
			m_I3DL2Listener = cListenerBathroom;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("living room")))
			m_I3DL2Listener = cListenerLivingRoom;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("stoneroom")))
			m_I3DL2Listener = cListenerStoneRoom;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("auditorium")))
			m_I3DL2Listener = cListenerAuditorium;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("concert hall")))
			m_I3DL2Listener = cListenerConcertHall;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("cave")))
			m_I3DL2Listener = cListenerCave;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("arena")))
			m_I3DL2Listener = cListenerArena;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("hangar")))
			m_I3DL2Listener = cListenerHangar;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("carpeted hallway")))
			m_I3DL2Listener = cListenerCarpetedHallway;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("hallway")))
			m_I3DL2Listener = cListenerHallway;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("stone corridor")))
			m_I3DL2Listener = cListenerStoneCorridor;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("alley")))
			m_I3DL2Listener = cListenerAlley;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("forest")))
			m_I3DL2Listener = cListenerForest;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("city")))
			m_I3DL2Listener = cListenerCity;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("mountains")))
			m_I3DL2Listener = cListenerMountains;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("quarry")))
			m_I3DL2Listener = cListenerQuarry;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("plain")))
			m_I3DL2Listener = cListenerPlain;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("parkinglot")))
			m_I3DL2Listener = cListenerParkingLot;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("sewer pipe")))
			m_I3DL2Listener = cListenerSewerPipe;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("underwater")))
			m_I3DL2Listener = cListenerUnderWater;
		else if(!m_configs[m_activeConfig].name.CompareNoCase(_T("no reverb")))
			m_I3DL2Listener = cListenerNoReverb;
		else
		{
			m_I3DL2Listener.lRoom				= I3DL2LISTENER_DEFAULTROOM;
			m_I3DL2Listener.lRoomHF				= I3DL2LISTENER_DEFAULTROOMHF;
			m_I3DL2Listener.flRoomRolloffFactor	= I3DL2LISTENER_DEFAULTROOMROLLOFFFACTOR;
			m_I3DL2Listener.flDecayTime			= I3DL2LISTENER_DEFAULTDECAYTIME;
			m_I3DL2Listener.flDecayHFRatio		= I3DL2LISTENER_DEFAULTDECAYHFRATIO;
			m_I3DL2Listener.lReflections		= I3DL2LISTENER_DEFAULTREFLECTIONS;
			m_I3DL2Listener.flReflectionsDelay	= I3DL2LISTENER_DEFAULTREFLECTIONSDELAY;
			m_I3DL2Listener.lReverb				= I3DL2LISTENER_DEFAULTREVERB;
			m_I3DL2Listener.flReverbDelay		= I3DL2LISTENER_DEFAULTREVERBDELAY;
			m_I3DL2Listener.flDiffusion			= I3DL2LISTENER_DEFAULTDIFFUSION;
			m_I3DL2Listener.flDensity			= I3DL2LISTENER_DEFAULTDENSITY;
			m_I3DL2Listener.flHFReference		= I3DL2LISTENER_DEFAULTHFREFERENCE;
		}
	}
	else
	{
		m_I3DL2Listener.lRoom				= I3DL2LISTENER_DEFAULTROOM;
		m_I3DL2Listener.lRoomHF				= I3DL2LISTENER_DEFAULTROOMHF;
		m_I3DL2Listener.flRoomRolloffFactor	= I3DL2LISTENER_DEFAULTROOMROLLOFFFACTOR;
		m_I3DL2Listener.flDecayTime			= I3DL2LISTENER_DEFAULTDECAYTIME;
		m_I3DL2Listener.flDecayHFRatio		= I3DL2LISTENER_DEFAULTDECAYHFRATIO;
		m_I3DL2Listener.lReflections		= I3DL2LISTENER_DEFAULTREFLECTIONS;
		m_I3DL2Listener.flReflectionsDelay	= I3DL2LISTENER_DEFAULTREFLECTIONSDELAY;
		m_I3DL2Listener.lReverb				= I3DL2LISTENER_DEFAULTREVERB;
		m_I3DL2Listener.flReverbDelay		= I3DL2LISTENER_DEFAULTREVERBDELAY;
		m_I3DL2Listener.flDiffusion			= I3DL2LISTENER_DEFAULTDIFFUSION;
		m_I3DL2Listener.flDensity			= I3DL2LISTENER_DEFAULTDENSITY;
		m_I3DL2Listener.flHFReference		= I3DL2LISTENER_DEFAULTHFREFERENCE;
	}
	m_I3DL2ListenerDefault = m_I3DL2Listener;
}

//------------------------------------------------------------------------------
//	CModule::Initialize
//------------------------------------------------------------------------------
void 
CModule::Initialize(void)
/*++

Routine Description:

	Initializes the basic state of the module. This is meant to be called from
	a constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_state					= CModule::OK;
	m_activeConfig			= -1;
	m_mixbin				= -1;
	m_bBadData				= FALSE;
	m_fIIR2Frequency		= 4500.0;
	m_fIIR2Q				= 1.0;
	m_fIIR2GainDB			= 0.0;
	m_fIIR2FrequencyDefault	= 4500.0;
	m_fIIR2QDefault			= 1.0;
	m_fIIR2GainDBDefault	= 0.0;
	m_bIIR2					= FALSE;
	m_bI3DL2				= FALSE;
	m_pGraph				= NULL;
	m_pParameterDialog		= NULL;
	m_bRealtime				= FALSE;
	m_bHighlight			= FALSE;

	// Setup the state font
	if(!m_stateFont)
	{
		m_stateFont = new CFont;
		m_stateFont->CreatePointFont(160, "Arial");
	}
	++m_stateFontRef;
}

//------------------------------------------------------------------------------
//	CModule::SetDisplayName
//------------------------------------------------------------------------------
void 
CModule::SetDisplayName(void)
/*++

Routine Description:

	Sets the name that is displayed

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_mixbin >= 0)
		m_displayName = m_name;
	else if(m_name != m_effectName)
		m_displayName = m_name;
	else
		m_displayName.Format(_T("%s (%X)"), (LPCTSTR)m_name, GetId()); 

	m_movingName = m_name;
}

