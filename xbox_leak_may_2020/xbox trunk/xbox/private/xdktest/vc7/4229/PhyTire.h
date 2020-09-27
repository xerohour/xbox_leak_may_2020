// Tire.h: interface for the CTire class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TIRE_H
#define __TIRE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern float g_minLoadWeight;

#define SIMTIRE 0			// 1 = Pajejka tire  0 = WDC cubic tire
#define SIMLOAD 1			// 1 = Actual load changes 0 = Limited load shifts - overrides producer number
#define FIXEDLOAD 0			// 1 = No load shift (always set to minload)
#define MINLOAD (4200.0f*(g_minLoadWeight/100.0f))
#define MAXLOAD 16000.0f
#define LIMITEDLOADSHIFT 0	// 


struct Tire  
{
public:
	float m_baseLoad;
	float m_corneringStiffness;
	float m_csSensitivityToFv;
	float m_muY;
	float m_muX;
	float m_muYSensitivityToFv;
	float m_muXSensitivityToFv;
	float m_camberStiffness;
	float m_longStiffness;
	float m_a, m_b, m_c, m_d, m_e;
	float m_tireSpringConst;
	float m_peakTraction;
	int m_model;

	void m_Init();
	float m_NormalizedCamber(float c, float mu);
	void m_ComputeTireForces(float fz, float camber, float tanSlipAngle, float slipRatio, 
							 float *fy, float*fx, short groundType);
	float m_MuY(float fv, short groundType);
	float m_MuX(float fv, short groundType);
	float m_CorneringStiffness(float fv);
	float m_Pacejka(float x);

private:



};

#endif // !defined(AFX_TIRE_H__6B1E5952_9D85_42CC_A5FA_EC21883F6003__INCLUDED_)
