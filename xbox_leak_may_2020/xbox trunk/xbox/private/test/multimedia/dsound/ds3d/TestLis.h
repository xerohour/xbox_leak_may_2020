#pragma once

HRESULT Test_Lis_SetAllParameters(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetDistanceFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetDopplerFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetOrientation_TopVector(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetOrientation_FrontVector(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetPosition_BoundaryCases(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetPosition_Listening(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetRolloffFactor(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_SetVelocity(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Lis_CommitDeferredSettings(char *szFileName);

HRESULT Test_INFINITY_Bug(TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
