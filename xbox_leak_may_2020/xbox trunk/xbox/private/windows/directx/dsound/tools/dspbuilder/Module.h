/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Module.h

Abstract:

	Effects module

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version
	23-Nob-2001	robheit
		Changed to no longer use cells

--*/
#if !defined(AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_)
#define AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxtempl.h>
#include "Unique.h"
//#include "WinDspImageBuilder.h"
#include "Parameter.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CGraph;
class CPatchCord;
class CJack;
class CParameterDialog;
class CKnob;
class CWinDspImageBuilder;

//------------------------------------------------------------------------------
//	CModule
//------------------------------------------------------------------------------
class CModule : public CUnique
{
public:

	enum State
	{
		OK,
		INVALID,
		MOVING
	};

	enum Type 
	{
		INPUT_MIXBIN,
		OUTPUT_MIXBIN,
		EFFECT
	};

	enum ColorFlag
	{
		COLOR_SPEAKER,
		COLOR_XTALK,
		COLOR_I3DL2,
		COLOR_FXSEND,
		COLOR_EFFECT
	};

	struct Config 
	{
		CString	name;
		CString	stateFilename;

		Config& operator = (const Config& c) 
		{
			name			= c.name;
			stateFilename	= c.stateFilename;
			return *this;
		}
	};

	struct I3DL2Listener
	{
		LONG	lRoom;
		LONG	lRoomHF;
		FLOAT	flRoomRolloffFactor;
		FLOAT	flDecayTime;
		FLOAT	flDecayHFRatio;
		LONG	lReflections;
		FLOAT	flReflectionsDelay;
		LONG	lReverb;
		FLOAT	flReverbDelay;
		FLOAT	flDiffusion;
		FLOAT	flDensity;
		FLOAT	flHFReference;

		I3DL2Listener& operator = (const I3DL2Listener& l) 
		{
			lRoom				= l.lRoom;
			lRoomHF				= l.lRoomHF;
			flRoomRolloffFactor	= l.flRoomRolloffFactor;
			flDecayTime			= l.flDecayTime;
			flDecayHFRatio		= l.flDecayHFRatio;
			lReflections		= l.lReflections;
			flReflectionsDelay	= l.flReflectionsDelay;
			lReverb				= l.lReverb;
			flReverbDelay		= l.flReverbDelay;
			flDiffusion			= l.flDiffusion;
			flDensity			= l.flDensity;
			flHFReference		= l.flHFReference;
			return *this;
		}
	};

public:

	CModule(CGraph* pGraph, Type type, ColorFlag colorFlag, LPCTSTR pName,
			int dspCycles, int yMemSize, int scratchLength, LPCTSTR dspCode, 
			LPCTSTR stateFilename, const CStringArray* pInputs, 
			const CStringArray* pOutputs);
	CModule(const CModule& module);
	CModule(void);
	virtual ~CModule(void);

	void Draw(CDC*);
	void SetRect(const CRect& rect);
	BOOL AreInputsPatched(void) const;
	BOOL AreOutputsPatched(void) const;
	BOOL IsPatched(void) const;
	void Disconnect(void);
	void DisconnectInputs(void);
	void DisconnectOutputs(void);
	BOOL IsConnected(void) const;
	LPCTSTR GetStateFilename(void) const;
	void AddConfiguration(LPCTSTR, LPCTSTR);
	void Move(const CPoint& point);
	CJack* GetJack(const CPoint& point, BOOL bSmallJacks) const;
//	CJack* GetJack(const CPatchCord* pPatchCord) const;
	void SetState(State state);
	void Write(CFile& file); // throw(CFileException);
	void FixPointers(const CGraph* pGraph);
	virtual BOOL Read(CFile& file, BYTE version);
	BOOL SetParameters(CWinDspImageBuilder* imageBuilder, const char* pFilename);
	void ShowProperties(void);
	void SetEffectName(void);
	void ChangeParameter(const CKnob* pKnob, BOOL bTransmit);
	BOOL WriteStateFile(LPCTSTR pFilePath);
	void SetActiveConfig(int config);
	BOOL AreParametersModified(void) const;
	void ParameterDialogDestroyed(void);

	// Inlined methods
	inline const CRect& GetRect(void) const { return m_rect; };
//	inline void SetType(Type type) { m_type = type; };
	inline Type GetType(void) const { return m_type; };
	inline State GetState(void) const { return m_state; };
	inline const CArray<CJack*, CJack*&>& GetInputs(void) const { return m_inputs; };
	inline const CArray<CJack*, CJack*&>& GetOutputs(void) const { return m_outputs; };
	inline LPCTSTR GetName(void) const { return (LPCTSTR)m_name; };
	inline LPCTSTR GetEffectName(void) const { return (LPCTSTR)m_effectName; };
	inline LPCTSTR GetDSPCode(void) const { return (LPCTSTR)m_dspCode; };
	inline CArray<Config, Config&>& GetConfigs(void) { return m_configs; };
	inline int GetNumConfigs(void) const { return m_configs.GetSize(); };
	inline int GetActiveConfig(void) const { return m_activeConfig; };
	inline int GetDSPCycles(void) const { return m_dspCycles; };
	inline int GetYMemSize(void) const { return m_yMemSize; };
	inline int GetScratchLength(void) const { return m_scratchLength; };
	inline void SetMixbin(int mixbin) { m_mixbin = mixbin; };
	inline int GetMixbin(void) const { return m_mixbin; };
	inline CGraph* GetGraph(void) const { return m_pGraph; };
	inline const CArray<CParameter, CParameter&>& GetParameters(void) const { return m_parameters; };

	// Note: I3DL2 cannot display properties right now (although support is there)
	inline BOOL HasProperties(void) const { return (m_bIIR2 || (m_parameters.GetSize() != 0)); };
//	inline BOOL IsIIR2(void) const { return m_bIIR2; };
	inline BOOL IsI3DL2(void) const { return m_bI3DL2; };
	inline const I3DL2Listener& GetI3DL2Listener(void) const { return m_I3DL2Listener; };
	inline LPCTSTR GetIniName(void) const { return m_iniName; };
	inline void SetIniName(LPCTSTR pName) { m_iniName = pName; };
	inline void SetIndex(DWORD index) { m_index = index; };
	inline DWORD GetIndex(void) const { return m_index; };
	inline void SetRealtime(BOOL bFlag) { m_bRealtime = bFlag; };
	inline void Highlight(BOOL bFlag) { m_bHighlight = bFlag; };

private:

	void CreatePenAndBrush(void);
	void ConvertToIIR2(void);
//	void ConvertFromIIR2(double& freq, double& q, double& gain, double a1, double a2, double b0, double b1, double b2);
	void ReorderIIR2Params(void);
	double ToDouble(DWORD dwVal);
	DWORD ToDWORD(double fVal);
	void SetI3DL2Parameters(void);
	void Initialize(void);
	void SetDisplayName(void);

private:

	// Written to file
	Type							m_type;
	State							m_state;
	CRect							m_rect;
	CString							m_name;
	CRect							m_nameRect;
	CRect							m_configRect;
	CArray<CJack*, CJack*&>			m_inputs;
	CArray<CJack*, CJack*&>			m_outputs;
	CString							m_stats;
	CRect							m_statsRect;
	CString							m_dspCode;
	CString							m_stateFilename;
	int								m_dspCycles;
	int								m_yMemSize;
	int								m_scratchLength;
	CArray<Config, Config&>			m_configs;
	int								m_activeConfig;
	ColorFlag						m_colorFlag;
	int								m_mixbin;
	CArray<CParameter, CParameter&>	m_parameters;
	CString							m_iniName;
	DWORD							m_index;
	double							m_fIIR2Frequency;
	double							m_fIIR2Q;
	double							m_fIIR2GainDB;
	double							m_fIIR2FrequencyDefault;
	double							m_fIIR2QDefault;
	double							m_fIIR2GainDBDefault;
	CString							m_effectName;

	// Not written to file
	BOOL							m_bBadData;
	CPen							m_pen;
	CBrush							m_brush;
	CBrush							m_movingBrush;
	CGraph*							m_pGraph;
	static CFont*					m_stateFont;
	static int						m_stateFontRef;
	BOOL							m_bIIR2;
	BOOL							m_bI3DL2;
	I3DL2Listener					m_I3DL2Listener;
	I3DL2Listener					m_I3DL2ListenerDefault;
	CParameterDialog*				m_pParameterDialog;
	CString							m_displayName;
	CString							m_movingName;
	BOOL							m_bRealtime;
	BOOL							m_bHighlight;
};

#endif // !defined(AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_)
