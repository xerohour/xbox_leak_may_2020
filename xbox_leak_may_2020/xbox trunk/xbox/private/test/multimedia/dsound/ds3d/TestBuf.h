#pragma once

HRESULT Test_Buf_SetAllParameters                       (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetConeAngles                          (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetConeOrientation_BoundaryCases       (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetConeOrientation_Listening           (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetConeOutsideVolume                   (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetMaxDistance_BoundaryCases           (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetMaxDistance_InsideSource            (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetMinDistance_BoundaryCases           (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetMinDistance_Listening               (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetMode                                (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetPosition_BoundaryCases              (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetPosition_Listening                  (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
HRESULT Test_Buf_SetVelocity                            (TestApplies eTestApply, D3DVECTOR vRelative, char *szFileName);
