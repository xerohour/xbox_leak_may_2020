#pragma once
#include "help_buffer3D.h"

using namespace DMBUFFER;


//3D Mixin Tests
HRESULT DMTest_Buf_SetAllParameters                       (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetConeAngles                          (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetConeOrientation_BoundaryCases       (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetConeOrientation_Listening           (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetConeOutsideVolume                   (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetMaxDistance_BoundaryCases           (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetMaxDistance_InsideSource            (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetMinDistance_BoundaryCases           (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetMinDistance_Listening               (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetMode                                (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetPosition_BoundaryCases              (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetPosition_Listening                  (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetVelocity                            (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
//HRESULT DMTest_INFINITY_Bug                               (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);

//2D Mixin Tests
HRESULT DMTest_Buf_SetFrequency_Listening                 (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetFrequency_Range                     (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetVolume_Listening                    (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetVolume_Range                        (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetChannelVolume_Channel_Listening     (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);
HRESULT DMTest_Buf_SetChannelVolume_Mask_Listening        (BUFFER3D *pSound, TestApplies eTestApply, D3DVECTOR vRelative);





