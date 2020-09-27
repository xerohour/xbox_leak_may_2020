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

#ifndef FUZZYLGK_H
#define FUZZYLGK_H

#include "prelude.h"

#define FUZZY_TRUE	10000
#define FUZZY_FALSE	0

inline WORD FZNOT(WORD a) {return FUZZY_TRUE - a;}

inline WORD FZAND(WORD a1, WORD a2) {return ScaleWord(a1, FUZZY_TRUE, a2);}
inline WORD FZAND(WORD a1, WORD a2, WORD a3) {return FZAND(FZAND(a1, a2), a3);}
inline WORD FZAND(WORD a1, WORD a2, WORD a3, WORD a4) {return FZAND(FZAND(a1, a2), FZAND(a3, a4));}

inline WORD FZOR(WORD a1, WORD a2) {return FZNOT(FZAND(FZNOT(a1), FZNOT(a2)));}
inline WORD FZOR(WORD a1, WORD a2, WORD a3) {return FZOR(FZOR(a1, a2), a3);}
inline WORD FZOR(WORD a1, WORD a2, WORD a3, WORD a4) {return FZOR(FZOR(a1, a2), FZOR(a3, a4));}

#endif
