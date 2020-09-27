///////////////////////////////////////////////////////////////////////////////
// File: CamControl.h
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#ifndef CAMCONTROL_H
#define CAMCONTROL_H

#include "xbs_math.h"



/*
Uses Hermite interpolation, from page 54 of "Real-Time Rendering" (Moeller, Haines)


The finish nodes are:
	0: before the beginning of the slash, anything before this doesn't need to render the slash
	1: after exiting slash
	2: translated down, but still looking at center (but it is eclipsed by slash geometry) after this, only render slash
	3: partly rotated to final position, looking at slash center now
	4: final position
	5: final position, with high time value (the endcap)

*/


struct CamControlNodeData
{
public:
	unsigned char	ucTime;					// Time when the camera arrives here (0 indicates start of new path, 100 is start of finalization)
	signed char		scTension, scBias;		// Tension and bias of point, from -100 to +100
	D3DVECTOR		ptPosition;				// Position of node
	D3DVECTOR		vecLookAt;				// Position the camera looks at
};




struct CamControlNode
{
public:
	float		fTime;			// Time when the camera arrives here (0 indicates start of new path)
	D3DVECTOR	ptPosition;		// Position of node
	D3DVECTOR	vecVelocity;	// Velocity at this point
	D3DVECTOR	vecLookAt;		// Position the camera looks at
	D3DVECTOR	vecLookAtW;		// Velocity of what the camera looks at
	float		tension, bias;
};


class CameraController
{
protected:
	static CamControlNodeData	svCameraListData[];
	static CamControlNode		svCameraList[];

	enum { NUM_FINISH_NODES = 8 };
	CamControlNode		finishNodes[NUM_FINISH_NODES];
	D3DMATRIX	xfSlash;
	D3DVECTOR	ptSlashCenter;
	D3DVECTOR	ptFinalLookAt;

	int			numNodes;
	int			numPaths;


	int			curPathNum;
	int			curStartNode;
	int			curNumNodes;
	int			curVariableNodes;
	
	float		fCameraLookatInterpStart, fOOCameraLookatInterpDelta;


	CamControlNode*			getNode(int i)			{ return (i<curVariableNodes) ? &svCameraList[i+curStartNode] : &finishNodes[i-curVariableNodes]; }
	const CamControlNode*	getNode(int i)	const	{ return (i<curVariableNodes) ? &svCameraList[i+curStartNode] : &finishNodes[i-curVariableNodes]; }

public:
	void Init();
	void UnInit() {}

	void		buttonPressed();


	void		pickPath(int path = -1);	// negative indicates random


	void		getPosition(float t, D3DVECTOR* p_pos, D3DVECTOR* p_look, bool* pb_render_geom, bool* pb_render_slash);

	const D3DMATRIX&	getSlashTransform()	const	{ return xfSlash; }
};



#endif // CAMCONTROL_H
