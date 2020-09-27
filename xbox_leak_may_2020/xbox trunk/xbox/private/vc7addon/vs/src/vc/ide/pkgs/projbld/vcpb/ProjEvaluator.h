// CProjectEvaluator does macro expansion for property containers that are objects in the project system

#pragma once

#include <vcids.h>
#include <vccolls.h>
#include <path2.h>

class CProjectEvaluator
{
public:
	CProjectEvaluator() { m_nDepth = 0; }
	virtual ~CProjectEvaluator() {}

public:
	BOOL MapMacroNameToId(const wchar_t* pchName, int& cchName, UINT& idMacro);
	BOOL MapMacroIdToName(UINT idMacro, CStringW& strName);
	HRESULT ExpandMacros(BSTR* pbstrOut, BSTR bstrIn, IVCPropertyContainer* pPropContainer, BOOL bNoEnvVars, long idProp = 0);

protected:
	virtual BOOL GetMacroValue(UINT idMacro, CStringW& strMacroValue, IVCPropertyContainer* pPropContainer);

	BOOL GetSolutionPath(CPathW& rSolnPath);
	BOOL GetSolutionDirectory(CStringW& rstrDir);
	BOOL GetProjectDirectoryPath(IVCPropertyContainer* pPropContainer, CPathW& rProjDirPath, CStringW& rstrProjDirPath, 
		BOOL bNormalize);
	BOOL GetProjectDirectoryDir(IVCPropertyContainer* pPropContainer, CDirW& rProjDir);
	HRESULT ValidateMacro(UINT idMacro, long idProp);
	BOOL NoInheritOnMultiProp(BSTR bstrVal, LPCOLESTR szSeparator, BSTR* pbstrValue);
	HRESULT MakeMultiPropString(CComBSTR& bstrLocal, CComBSTR& bstrParent, LPCOLESTR szSeparator, BSTR* pbstrValue);
	void GetBuildEngine(IVCPropertyContainer* pPropContainer, CComPtr<IVCBuildEngine>& spBuildEngine);
	HRESULT CollapseMultiples(BSTR bstrIn, LPCOLESTR szSeparator, VARIANT_BOOL bCaseSensitive, BSTR* pbstrOut);
	int DoFindItem(CStringW& rstrItems, CStringW& rstrFindItem, CStringW& strSeparator, int nStart);
	void NormalizeDirectoryString(CStringW& strDir);
	BOOL EvalIntDirOutDir(IVCPropertyContainer* pPropContainer, BOOL bIsIntDir, CStringW& strMacroValue);
	BOOL EvalProjectMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue);
	BOOL EvalTargetMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue);
	BOOL EvalInputMacroValue(IVCPropertyContainer* pPropContainer, UINT idMacro, CStringW& strMacroValue);
	void GetDebuggerProperty(IVCPropertyContainer* pPropContainer, long idProp, CStringW& strMacroValue);
	void EvalFrameworkMacro(UINT idMacro, CStringW& strMacroValue);
private:
	int m_nDepth;
	long m_nLastMacroID;

	static CStringW s_strVCDir;
	static CStringW s_strVSDir;
	static CStringW s_strComDir;
	static CStringW s_strComVer;
	static CStringW s_strComSDKDir;
	static bool s_bComDirsInit;
};

//----------------------------------------------------------------
// primitive custom build 'tool macros'
//----------------------------------------------------------------

#define IDMACRO_PLATNAME	1000
#define IDMACRO_ENVDIR		1001
#define IDMACRO_CFGNAME		1002
#define IDMACRO_OUTDIR		1003
#define IDMACRO_INTDIR		1004

#define IDMACRO_SOLNDIR		1005
#define IDMACRO_SOLNPATH	1006
#define IDMACRO_SOLNBASE	1007
#define IDMACRO_SOLNFILE	1008
#define IDMACRO_SOLNEXT		1009

#define IDMACRO_PROJDIR		1010
#define IDMACRO_PROJPATH	1011
#define IDMACRO_PROJBASE	1012
#define IDMACRO_PROJFILE	1013
#define IDMACRO_PROJEXT		1014

#define IDMACRO_TARGDIR		1015
#define IDMACRO_TARGPATH	1016
#define IDMACRO_TARGBASE	1017
#define IDMACRO_TARGFILE	1018
#define IDMACRO_TARGEXT		1019

#define IDMACRO_INPTDIR		1020
#define IDMACRO_INPTPATH	1021
#define IDMACRO_INPTBASE	1022
#define IDMACRO_INPTFILE	1023
#define IDMACRO_INPTEXT		1024

#define IDMACRO_REMOTEMACH	1025

#define IDMACRO_VCDIR		1026
#define IDMACRO_VSDIR		1027
#define IDMACRO_COMDIR		1028
#define IDMACRO_COMSDKDIR	1029
#define IDMACRO_COMVER		1030

#define IDMACRO_INHERIT		1031
#define IDMACRO_NOINHERIT	1032

#define IDMACRO_FIRST		IDMACRO_PLATNAME
#define IDMACRO_LAST		IDMACRO_NOINHERIT


