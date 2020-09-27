///////////////////////////////////////////////////////////////////////////////
// File: CamControl.cpp
//
// Copyright 2001 Pipeworks Software
///////////////////////////////////////////////////////////////////////////////
#include "precomp.h"
#include "xbs_math.h"
#include "xbs_app.h"
#include "CamControl.h"
///////////////////////////////////////////////////////////////////////////////
// Negative tension is rounder, Positive tension is sharper (and comes
// to more of a stop at the point).
// Negative bias favors direction from previous to current, positive
// favors direction from current to next.
CamControlNodeData CameraController::svCameraListData[] =
{
//MTS	// Focusing on widgets, passing past blob.
//MTS    {  0,   +00, +00,        +3.2f,   -17.4f,   +13.0f,       -1.9f,    +7.8f,   -15.3f },
//MTS    { 20,   +00, +00,        +2.1f,   -12.1f,    +6.15f,      -1.9f,    +7.8f,   -15.3f },
//MTS    { 40,   +00, +00,        +1.0f,    -6.8f,    -0.7f,       -1.9f,    +7.8f,   -15.3f },
//MTS    { 55,   +00, +00,        -7.2f,    -3.6f,    -7.5f,       -1.9f,    +7.8f,   -15.3f },
//MTS    { 70,   +00, +00,       -18.5f,   +14.1f,   -18.4f,       -1.9f,    +7.8f,   -15.3f },
//MTS    { 85,   +00, +00,       -16.2f,   +29.2f,   -23.1f,       -5.1f,    +8.4f,   -10.3f },
//MTS    { 95,   +00, +00,       -10.6f,   +49.3f,   -21.2f,       -5.1f,    +8.4f,   -10.3f },
//MTS
//MTS    // "drunken"
//MTS	{  0,   +00, +00,        -1.5f,    +2.8f,   -14.0f,       -0.0f,    -1.3f,    +0.0f },
//MTS    { 20,   +00, +00,        -3.8f,   +16.9f,   -22.8f,       -0.0f,    -1.3f,    +0.0f },
//MTS    { 45,   +00, +00,       +20.7f,   +27.6f,   -21.6f,       -0.0f,    -1.3f,    +0.0f },
//MTS    { 75,   +00, +00,       +18.1f,   +46.8f,   -27.0f,       -0.0f,    -1.3f,    +0.0f },
//MTS    { 95,   +00, +00,       +28.9f,   +75.1f,   -36.7f,       -0.0f,    -1.3f,    +0.0f },

	// Top, pan down, pull out.
    {  0,   +00, +00,       +11.4f,   -32.1f,   +33.0f,       +0.0f,    +0.0f,    +0.0f },
    { 20,   +00, +00,       +13.4f,   -37.7f,   +25.6f,       +0.0f,    +0.0f,    +0.0f },
    { 40,   +00, +00,       +15.6f,   -43.9f,    +8.8f,       +0.0f,    +0.0f,    +0.0f },
    { 60,   +00, +00,       +16.0f,   -45.0f,   -12.8f,       +0.0f,    +0.0f,    +0.0f },
    { 90,   +00, +00,       +18.2f,   -51.2f,   -29.6f,       +0.0f,    +0.0f,    +0.0f },

//MTS	// Low, looking up, pull out, and pan up.
//MTS    {  0,   +00, +00,       +15.2f,    -3.3f,   -15.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    { 20,   +00, +00,       +26.2f,    -5.7f,   -20.5f,       +0.0f,    +0.0f,    +0.0f },
//MTS    { 45,   +00, +00,       +40.2f,    -8.7f,   -23.6f,       +0.0f,    +0.0f,    +0.0f },
//MTS    { 65,   +00, +00,       +61.2f,   -13.3f,    -3.6f,       +0.0f,    +0.0f,    +0.0f },
//MTS    { 85,   +00, +00,       +84.4f,    -4.8f,   +11.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    { 95,   +00, +00,      +120.4f,    -8.8f,   +14.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// Looking low, closeup on the bottom, pull out and pan up.
//MTS    {  0,   +00, +00,       -20.5f,   +48.8f,   +12.0f,       +0.0f,    +0.0f,   -18.6f },
//MTS    { 35,   +00, +00,       -10.4f,   +24.6f,    -2.4f,       +0.0f,    +0.0f,   -18.6f },
//MTS    { 70,   +00, +00,       -10.4f,   +24.6f,    -2.4f,       +0.0f,    +0.0f,   -18.6f },
//MTS    { 85,   +00, +00,       -16.5f,   +39.3f,    +4.2f,       -0.3f,    +0.1f,    -5.3f },
//MTS    { 95,   +00, +00,       -26.3f,   +75.4f,    -4.6f,       -0.3f,    +0.1f,    -0.5f },
//MTS
//MTS	// Somewhat high, rotate left, recenter, pause, and pull out.
//MTS    {  00,   +0, +0,       -92.5f,   -10.1f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  15,   +0, +0,       -88.0f,   +30.4f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  30,   +0, +0,       -64.7f,   +67.0f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  45,   +0, +0,       -22.6f,   +90.3f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  60,   +0, +0,       +21.7f,   +90.5f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  72,   +0, +0,       +65.4f,   +66.2f,   +20.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  85,   +0, +0,       +97.5f,   +23.7f,   +16.1f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,      +110.8f,    -6.1f,    +0.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// start a little low, slowly move up, drift right, pull out.
//MTS    {  00,   +0, +0,       +62.3f,   -28.5f,   -20.2f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  30,   +0, +0,       +62.3f,   -28.5f,   -20.2f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  50,   +0, +0,       +55.2f,   -25.3f,   -10.1f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  75,   +0, +0,       +56.7f,   -12.9f,    +0.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,       +73.9f,   -13.0f,    +0.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// Towards top, tube to the right, move down and left.
//MTS    {  00,   +0, +0,       +50.4f,   +33.2f,   +25.3f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  30,   +0, +0,       +50.4f,   +33.2f,   +25.3f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  45,   +0, +0,       +55.7f,    +9.3f,   +15.9f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  65,   +0, +0,       +39.1f,   -35.8f,    +3.3f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  90,   +0, +0,        +7.1f,   -53.7f,    +0.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,       -16.8f,   -78.0f,    +0.0f,       +0.0f,    +0.0f,    +0.0f },

	// Start low near tube, pause, quickly go up and to the right, pause, pull out.
    {  00,   +0, +0,       -55.4f,   +19.7f,   -31.5f,       +0.0f,    +0.0f,    +0.0f },
    {  30,   +0, +0,       -55.4f,   +19.7f,   -31.5f,       +0.0f,    +0.0f,    +0.0f },
    {  45,   +0, +0,       -39.5f,    -0.6f,    -7.8f,       +0.0f,    +0.0f,    +0.0f },
    {  60,   +0, +0,        -4.3f,   -35.5f,   +16.6f,       +0.0f,    +0.0f,    +0.0f },
    {  70,   +0, +0,       +31.1f,   -32.6f,   +17.6f,       +0.0f,    +0.0f,    +0.0f },
    {  80,   +0, +0,       +57.7f,    -7.2f,    +3.3f,       +0.0f,    +0.0f,    +0.0f },
    {  95,   +0, +0,       +70.9f,    +1.8f,    +3.1f,       +0.0f,    +0.0f,    +0.0f },

	// Low, move up, go left, pull out.
//MTS    {  00,   +0, +0,       -51.7f,    -2.4f,   -32.4f,       +0.0f,    +0.0f,   +12.0f },
//MTS    {  25,   +0, +0,       -51.7f,    -2.4f,   -32.4f,       +0.0f,    +0.0f,   +12.0f },
//MTS    {  40,   +0, +0,       -54.4f,    -2.6f,   -23.3f,       +0.0f,    +0.0f,   +10.0f },
//MTS    {  50,   +0, +0,       -53.2f,    +6.7f,   -14.3f,       +0.0f,    +0.0f,    +7.0f },
//MTS    {  60,   +0, +0,       -41.8f,   +21.2f,    -9.3f,       +0.0f,    +0.0f,    +4.5f },
//MTS    {  70,   +0, +0,       -27.1f,   +34.2f,    -9.3f,       +0.0f,    +0.0f,    +2.0f },
//MTS    {  80,   +0, +0,        -6.9f,   +47.5f,    -9.3f,       +0.0f,    +0.0f,    +1.0f },
//MTS    {  88,   +0, +0,       +11.3f,   +52.3f,    -9.2f,       +0.0f,    +0.0f,    +0.5f },
//MTS    {  95,   +0, +0,       +24.2f,   +62.8f,    -9.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// Orbit right (a little up).
//MTS    {  00,   +0, +0,        -2.6f,   -85.0f,   +17.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  20,   +0, +0,       +48.1f,   -70.1f,   +17.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  40,   +0, +0,       +80.1f,   -28.6f,   +17.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  60,   +0, +0,       +81.4f,   +24.8f,   +17.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  80,   +0, +0,       +56.5f,   +63.6f,   +17.7f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,       +27.9f,   +86.5f,    +8.2f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// Orbit left.
//MTS    {  00,   +0, +0,       +47.3f,   -76.6f,   -10.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  25,   +0, +0,       +11.5f,   -89.3f,   -10.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  50,   +0, +0,       -35.7f,   -82.6f,   -10.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  75,   +0, +0,       -71.9f,   -54.2f,   -10.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,       -89.5f,    -9.6f,   -10.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS
//MTS	// Move down and to the right.
//MTS    {  00,   +0, +0,       -11.2f,   -42.9f,   +17.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  20,   +0, +0,        +2.6f,   -46.9f,    +7.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  40,   +0, +0,       +24.3f,   -40.5f,    -4.2f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  60,   +0, +0,       +38.7f,   -24.8f,    -9.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  80,   +0, +0,       +44.3f,   -12.1f,    -7.0f,       +0.0f,    +0.0f,    +0.0f },
//MTS    {  95,   +0, +0,       +52.4f,    +6.3f,    +2.0f,       +0.0f,    +0.0f,    +0.0f },

	// Rotate to left, fairly close.
    {  00,   +0, +0,       +34.7f,   +25.9f,   +12.3f,       +0.0f,    +0.0f,    +0.0f },
    {  25,   +0, +0,       +42.3f,    +9.3f,   +12.3f,       +0.0f,    +0.0f,    +0.0f },
    {  50,   +0, +0,       +42.4f,    -8.8f,   +12.3f,       +0.0f,    +0.0f,    +0.0f },
    {  75,   +0, +0,       +34.4f,   -26.3f,   +12.3f,       +0.0f,    +0.0f,    +0.0f },
    {  95,   +0, +0,       +30.7f,   -48.1f,   +14.3f,       +0.0f,    +0.0f,    +0.0f },

	// Start out low, pause, rotate up slightly and pull out.
	{  0,   +00, +00,       -50.1f,    -0.3f,   -51.5f,       +0.0f,    +0.0f,    +0.0f },
	{  25,  +00, +00,       -50.1f,    -0.3f,   -51.5f,       +0.0f,    +0.0f,    +0.0f },
	{  75,  +00, +00,       -50.1f,    -0.3f,   -51.5f,       +0.0f,    +0.0f,    +0.0f },
    {  95,  +00, +00,       -62.2f,    -0.4f,   -12.0f,       +0.0f,    +0.0f,    +0.0f },
};

#define NUM_CC_NODES  (sizeof(CameraController::svCameraListData)/sizeof(CamControlNodeData))

CamControlNode CameraController::svCameraList[NUM_CC_NODES];
///////////////////////////////////////////////////////////////////////////////
void CameraController::Init()
{
	numNodes = NUM_CC_NODES;
	numPaths = 0;
	curPathNum = -1;
	for (int i=0; i<numNodes; i++)
	{
		if (svCameraListData[i].ucTime == (unsigned char) 0) numPaths++;

		svCameraList[i].fTime = FINISH_START_TIME * ((float)svCameraListData[i].ucTime) * 0.01f;
		svCameraList[i].ptPosition = svCameraListData[i].ptPosition;
		svCameraList[i].vecLookAt = svCameraListData[i].vecLookAt;
		svCameraList[i].tension = ((float)svCameraListData[i].scTension) * 0.01f;
		svCameraList[i].bias = ((float)svCameraListData[i].scBias) * 0.01f;

//MTS		char buf[1024];
//MTS		D3DVECTOR pos = svCameraList[i].ptPosition;
//MTS		D3DVECTOR la = svCameraList[i].vecLookAt;
//MTS		sprintf(buf, "    {  %02d,   %+02d, %+02d,    %+8.1ff,%+8.1ff,%+8.1ff,   %+8.1ff,%+8.1ff,%+8.1ff },\n",
//MTS						(int) svCameraListData[i].ucTime, (int) svCameraListData[i].scTension, (int) svCameraListData[i].scBias,
//MTS						pos.x, pos.y, pos.z, la.x, la.y, la.z);
//MTS		if (svCameraListData[i].ucTime==0) OutputDebugString("\n");
//MTS		OutputDebugString(buf);
	}

	pickPath(-1);
}
///////////////////////////////////////////////////////////////////////////////
// negative indicates random
void CameraController::pickPath(int path)
{
	if (path<0)
	{
		path = ((unsigned int) (gApp.rand() & 0x7FFFFFFF));
	}
	if (path >= numPaths) path = path % numPaths;


	// The following line cycles the selected path. Without it, path selection is random.
//MTS	path = (curPathNum + 1) % numPaths;


//MTS	path = 1;

	curPathNum = path;



//MTS	char buf[1024];
//MTS	sprintf(buf, "Picked path %d\n", path);
//MTS	OutputDebugString(buf);


	int i;
	for (i=0; i<numNodes; i++)
	{
		if (svCameraList[i].fTime == 0.0f)
		{
			if (!path) break;
			path--;
		}
	}
	curStartNode = i;

	for (i=curStartNode+1; i<numNodes; i++)
	{
		if (svCameraList[i].fTime == 0.0f) break;
	}
	curVariableNodes = i - curStartNode;
	curNumNodes = curVariableNodes + NUM_FINISH_NODES;


	// Calculate the finishNodes.
	int j;
	for (j=0; j<NUM_FINISH_NODES; j++)
	{
		finishNodes[j].fTime = FINISH_START_TIME + FINISH_TRANSITION_TIME * ((float)j) / ((float)(NUM_FINISH_NODES-1));
		finishNodes[j].tension = +0.0f;
		finishNodes[j].bias = 0.0f;
	}

	fCameraLookatInterpStart = finishNodes[2].fTime;
	fOOCameraLookatInterpDelta = 1.0f / (finishNodes[5].fTime - fCameraLookatInterpStart);




	const CamControlNode* plast = &svCameraList[curStartNode + curVariableNodes - 1];
	CamControlNode* pthis = &finishNodes[0];
	
	float start_time = plast->fTime;

//MTS	const float slash_start_rad = -185.0f;
	const float slash_start_rad = -95.0f;
	const float slash_end_rad = 132.14f;

//MTS	const float cfYPositions[NUM_FINISH_NODES] = { 185.0f, 117.548f, -8.819f, -150.298f, -220.64f, -243.021f, -261.441f, -287.773f };
	const float cfYPositions[NUM_FINISH_NODES] = { +95.0f, +30.548f, -70.819f, -150.298f, -220.64f, -243.021f, -261.441f, -287.773f };
	const float cfZPositions[NUM_FINISH_NODES] = { 0.0f,   0.322f,   1.821f, 2.323f,    -11.926f,  -39.973f,  -60.774f,  -90.795f };
	const float camera_end_coord_y = cfYPositions[NUM_FINISH_NODES-1];
	const float camera_end_coord_z = cfZPositions[NUM_FINISH_NODES-1];

	const float lookat_offset = slash_end_rad * camera_end_coord_z / camera_end_coord_y;
	const float cfSlashDist = slash_end_rad - slash_start_rad;
	const float cfMinStartDist = 100.0f;






	
	// First finish node is at the entrance of the slash. Take the last nodes
	// velocity, move along it for a little bit, and then push it out to the
	// appropriate radius, about 200 units.
	// The slash's transform will also be set now.

	pthis->ptPosition = plast->ptPosition;

	D3DVECTOR vel;
	if (curVariableNodes >= 2)
	{
		Sub(getNode(curVariableNodes-1)->ptPosition, getNode(curVariableNodes-2)->ptPosition, &vel);
		Scale(&vel, 1.0f / (getNode(curVariableNodes-1)->fTime - getNode(curVariableNodes-2)->fTime));
	}
	else
	{
		Set(&vel, 0.0f, 0.0f, 0.0f);
	}

	AddScaled(&pthis->ptPosition, vel, (pthis->fTime - plast->fTime) * 0.7f);
	float vel_adj_len = Length(pthis->ptPosition);
	Scale(&pthis->ptPosition, 1.0f / vel_adj_len);	// normalizes

	D3DVECTOR slash_dir = pthis->ptPosition;
	float slash_y_offset = max(cfMinStartDist - slash_start_rad, vel_adj_len * 1.2f - slash_start_rad);
	Scale(&pthis->ptPosition, slash_y_offset + slash_start_rad);

	Set(&pthis->vecLookAt, 0.0f, 0.0f, 0.0f);



//MTS// Matches slash with barry's coordinates.
//MTSSet(&pthis->ptPosition, 0.0f, -1.0f, 0.0f);
//MTSSet(&slash_dir, 0.0f, -1.0f, 0.0f);
//MTSslash_y_offset = 0.0f;



	// Now do the slash xform.

	D3DVECTOR origin, up, x_dir, y_dir, z_dir;
	Set(&origin, 0.0f, 0.0f, 0.0f);
	Set(&up, 0.0f, 0.0f, 1.0f);

	// The shape has negative y going away from core.
	// Negative Y is normalized vector from slash_pos to origin
	y_dir = slash_dir;
	Scale(&y_dir, -1.0f);

	// X is y_dir crossed with up and normalized.
	Cross(y_dir, up, &x_dir);
	Normalize(&x_dir);

	// Z is x_dir cross y_dir.
	Cross(x_dir, y_dir, &z_dir);		// I think this is the wrong cross order...

	SetIdentity(&xfSlash);
	*((D3DVECTOR*)(&xfSlash._11)) = x_dir;
	*((D3DVECTOR*)(&xfSlash._21)) = y_dir;
	*((D3DVECTOR*)(&xfSlash._31)) = z_dir;
	*((D3DVECTOR*)(&xfSlash._41)) = y_dir;
	Scale((D3DVECTOR*)(&xfSlash._41), -slash_y_offset);
	ptSlashCenter = y_dir;
	Scale(&ptSlashCenter, - slash_end_rad - slash_y_offset);

//MTS	AddScaled(&pthis->ptPosition, z_dir, -7.0f);



	//const float cfYPositions[NUM_FINISH_NODES] = { 185.0f, 177.548f, 58.819f, -170.298f, -220.871f, -243.021f, -260.364f, -277.299f };
	float y_basis = slash_end_rad;
	for (j=1; j<NUM_FINISH_NODES; j++)
	{
		plast = pthis++;
		D3DVECTOR pt_in_slash;
		Set(&pt_in_slash, 0.0f, cfYPositions[j] + y_basis , cfZPositions[j]);
		TransformVector(pt_in_slash, xfSlash, &pthis->ptPosition);
		Add(pthis->ptPosition, ptSlashCenter, &pthis->ptPosition);
		Set(&pthis->vecLookAt, 0.0f, 0.0f, 0.0f);
	}






	// Set the final lookat point
	D3DVECTOR t;
	Set(&t, 0.0f, slash_end_rad, 25.0f);
	TransformVector(t, xfSlash, &ptFinalLookAt);
	Add(ptFinalLookAt, ptSlashCenter, &ptFinalLookAt);


	
	// Set the "m" parameters implicitly.

	for (j=0; j<curNumNodes; j++)
	{
		CamControlNode* pthis = getNode(j);
		Set(&pthis->vecVelocity, 0.0f, 0.0f, 0.0f);
		Set(&pthis->vecLookAtW, 0.0f, 0.0f, 0.0f);

		if (j)
		{
			D3DVECTOR delta;
			Sub(pthis->ptPosition, getNode(j-1)->ptPosition, &delta);
			AddScaled(&pthis->vecVelocity, delta, (1.0f - pthis->tension) * (1.0f + pthis->bias) * 0.5f);

			Sub(pthis->vecLookAt, getNode(j-1)->vecLookAt, &delta);
			AddScaled(&pthis->vecLookAtW, delta, (1.0f - pthis->tension) * (1.0f + pthis->bias) * 0.5f);
		}
		if (j<curNumNodes-1)
		{
			D3DVECTOR delta;
			Sub(getNode(j+1)->ptPosition, pthis->ptPosition, &delta);
			AddScaled(&pthis->vecVelocity, delta, (1.0f - pthis->tension) * (1.0f - pthis->bias) * 0.5f);

			Sub(getNode(j+1)->vecLookAt, pthis->vecLookAt, &delta);
			AddScaled(&pthis->vecLookAtW, delta, (1.0f - pthis->tension) * (1.0f - pthis->bias) * 0.5f);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CameraController::getPosition(float t, D3DVECTOR* p_pos, D3DVECTOR* p_look, bool* pb_render_geom, bool* pb_render_slash)
{

	if (t > FINISH_STOP_TIME)
	{
		*p_pos = finishNodes[NUM_FINISH_NODES-1].ptPosition;
		*p_look = ptFinalLookAt;
		*pb_render_slash = true;
		*pb_render_geom = false;
		return;
	}

	int i;
	for (i=1; i<curNumNodes; i++)
	{
		if (getNode(i)->fTime > t) break;
	}
	if (i==curNumNodes)
	{
		*p_pos = getNode(curNumNodes-1)->ptPosition;
		*p_look = ptFinalLookAt;
		*pb_render_slash = true;
		*pb_render_geom = false;
		return;
	}
	if (i==0)
	{
		// Should never happen, but something is causing a lock...
		Set(p_pos, 0.0f, -90.0f, 0.0f);
		Set(p_look, 0.0f, 0.0f, 0.0f);
		*pb_render_slash = true;
		*pb_render_geom = false;
		return;
	}


	const CamControlNode* pprev = getNode(i-1);
	const CamControlNode* pnext = getNode(i);

	float dtc = max(0.001f, pnext->fTime - pprev->fTime);
	float dtp = max(0.001f, (i>=2) ? pprev->fTime - getNode(i-2)->fTime : dtc);
	float dtn = max(0.001f, (i<curNumNodes-1) ? getNode(i+1)->fTime - pnext->fTime : dtc);

	float uts = min(1.0f, max(0.0f, (t - pprev->fTime) / dtc));	// un-tweaked s
	float utss = uts*uts;
	float utsss = utss*uts;
	float frac = -2.0f*utsss + 3.0f*utss;
	float s = (t - pprev->fTime) / ((1.0f-frac)*dtp + frac*dtc);


	float ss = s*s;
	float sss = ss*s;
	float cA = 2.0f * sss - 3.0f * ss + 1.0f;
	float cB = sss - 2.0f * ss + s;
	float cC = sss - ss;
	float cD = -2.0f * sss + 3.0f * ss;

	p_pos->x = cA * pprev->ptPosition.x + cB * pprev->vecVelocity.x + cC * pnext->vecVelocity.x + cD * pnext->ptPosition.x;
	p_pos->y = cA * pprev->ptPosition.y + cB * pprev->vecVelocity.y + cC * pnext->vecVelocity.y + cD * pnext->ptPosition.y;
	p_pos->z = cA * pprev->ptPosition.z + cB * pprev->vecVelocity.z + cC * pnext->vecVelocity.z + cD * pnext->ptPosition.z;

	p_look->x = cA * pprev->vecLookAt.x + cB * pprev->vecLookAtW.x + cC * pnext->vecLookAtW.x + cD * pnext->vecLookAt.x;
	p_look->y = cA * pprev->vecLookAt.y + cB * pprev->vecLookAtW.y + cC * pnext->vecLookAtW.y + cD * pnext->vecLookAt.y;
	p_look->z = cA * pprev->vecLookAt.z + cB * pprev->vecLookAtW.z + cC * pnext->vecLookAtW.z + cD * pnext->vecLookAt.z;


	float sl = max(0.0f, min(1.0f, (t - fCameraLookatInterpStart) * fOOCameraLookatInterpDelta));
	
	float _sin,_cos;
	SinCos(sl * Pi,&_sin,&_cos);
	float interp = 0.5f*(1.0f - _cos);

	Scale(p_look, 1.0f - interp);
	AddScaled(p_look, ptFinalLookAt, interp);



	*pb_render_slash = (i > curVariableNodes - 1);
	*pb_render_geom  = (i < curNumNodes - 1);
}
///////////////////////////////////////////////////////////////////////////////
void CameraController::buttonPressed()
{
#ifndef STARTUPANIMATION
#ifdef INCLUDE_PLACEMENT_DOODAD	
	char buf[1024];
	D3DVECTOR pos;
	D3DVECTOR la = gApp.getLookatPoint();
	gApp.theCamera.getCameraPos(&pos);
	sprintf(buf, "    {  0,   +00, +00,    %+8.1ff,%+8.1ff,%+8.1ff,   %+8.1ff,%+8.1ff,%+8.1ff },\n",
					pos.x, pos.y, pos.z, la.x, la.y, la.z);
	OutputDebugString(buf);
#endif
#endif // STARTUPANIMATION
}
///////////////////////////////////////////////////////////////////////////////
