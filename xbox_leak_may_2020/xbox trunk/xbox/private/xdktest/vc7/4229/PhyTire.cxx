// Tire.cpp: implementation of the CTire class.
//
//////////////////////////////////////////////////////////////////////

#include "PhyTire.h"

#include "moremath.h"
#include "SurfaceType.h"


float g_minLoadWeight = 100.0f;

const float g_muSurfaceTable[SURFACETYPE_COUNT] = 
{
	1.0f,   // road
	0.7f,   // runofff
	0.5f,  // sand
	0.9f    // grass
};


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EPS (0.0001f)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void Tire::m_Init()
{
	m_a = 0.0f;
	m_b = 0.714f;
	m_c = 1.4f;
	m_d = 1.0f;
	m_e = -0.2f;

	// Real numbers for an F1 car
	m_corneringStiffness = 40.0f; // per Rad
	m_baseLoad = 832.0f;
	m_csSensitivityToFv = 0.1f;
	m_muY = 1.6f;
	m_muX = 1.4f;
	m_muYSensitivityToFv = -0.24f;
	m_muXSensitivityToFv = -0.24f;

	m_camberStiffness = 0.42f;

	m_longStiffness = 25.0f;		// Initial gradient of Normalised long force versus Slip
	m_tireSpringConst = 150000.0f;
	m_peakTraction = 0.1f;  // where on the tire curve is the maximum traction point

}


float Tire::m_Pacejka(float alpha)
{
	return 0;

}

float Tire::m_CorneringStiffness(float fv)
{
	return (m_corneringStiffness * (float)exp((double)-m_csSensitivityToFv*(fv/m_baseLoad - 1)));
}

float Tire::m_MuY(float fv, short groundType)
{
	float ret;
	if (fv <= EPS)
	{
		return (EPS);
	}
	else
	{
		ret = m_muY * powf(fv/m_baseLoad, m_muYSensitivityToFv);
	}
	if (groundType != SURFACETYPE_ROAD)
	{
		ret *= g_muSurfaceTable[groundType];
	}
	return (ret);
}


float Tire::m_MuX(float fv, short groundType)
{
	float ret;
	if (fv <= EPS)
	{
		return (EPS);
	}
	else
	{
		ret = m_muX * powf(fv/m_baseLoad, m_muXSensitivityToFv);
	}
	if (groundType != SURFACETYPE_ROAD)
	{
		ret *= g_muSurfaceTable[groundType];
	}
	return (ret);
}


float Tire::m_NormalizedCamber(float c, float mu)
{
	return m_camberStiffness * sinf(c) / mu;
}



#define sign(x) ((x)>=0.0f? 1.0f: -1.0f)

void Tire::m_ComputeTireForces(float fz, float camber, float tanSlipAngle, float slipRatio, 
							   float *fy, float *fx, short groundType)
{
#if (!SIMLOAD)
	if (fz<MINLOAD)
		fz = MINLOAD;
	if (fz>MAXLOAD)
		fz = MAXLOAD;
#endif
#if (FIXEDLOAD)
	fz = MINLOAD;
#endif
#if (LIMITEDLOADSHIFT)

#endif
	

}

