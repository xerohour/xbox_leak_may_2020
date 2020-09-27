////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCEDLL_INCLUDED
#define RESOURCEDLL_INCLUDED



#include "library\common\profiles.h"
#include "library\common\krnlstr.h"

class ResourceDLL
	{
	private:
		BOOL libLoaded;
		HINSTANCE resInst;
		LANGID langId;
		KernelString resourceName;
	public:
		ResourceDLL(KernelString vendor, KernelString product, KernelString version, KernelString resDefName);
		~ResourceDLL();
		void Load(HINSTANCE defInst);
		void SetLanguage(LANGID langId);
		BOOL LoadStringResource(UINT resourceID, WCHAR * invalidMsg, int & invalidMsgSize);
	};

#endif
