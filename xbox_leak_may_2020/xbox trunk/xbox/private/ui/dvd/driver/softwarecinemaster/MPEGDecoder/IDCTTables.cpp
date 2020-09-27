////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "IDCTTables.h"

void DCTMulTable::CalculateTable(int fact)
	{
	short c1, c2, c3, c4, c5, c6, c7, c8;
	double mul = (1 << ACCURATE) * (fact / 128.);

	c1 = (short) ( floor((DC1 * mul) + 0.5) );
	c2 = (short) ( floor((DC2 * mul) + 0.5) );
	c3 = (short) ( floor((DC3 * mul) + 0.5) );
	c4 = (short) ( floor((DC4 * mul) + 0.5) );
	c5 = (short) ( floor((DC5 * mul) + 0.5) );
	c6 = (short) ( floor((DC6 * mul) + 0.5) );
	c7 = (short) ( floor((DC7 * mul) + 0.5) );
	c8 = (short) ( floor((DC8 * mul) + 0.5) );

	tpC8pC8pC8pC8[0] =  c8; tpC8pC8pC8pC8[1] =  c8;
	tpC8pC8pC8pC8[2] =  c8; tpC8pC8pC8pC8[3] =  c8;

	tpC8000000000[0] =  c8; tpC8000000000[1] =   0;
	tpC8000000000[2] =   0; tpC8000000000[3] =   0;

	tpC8pC7pC8pC5[0] =  c5; tpC8pC7pC8pC5[1] =  c8;
	tpC8pC7pC8pC5[2] =  c7; tpC8pC7pC8pC5[3] =  c8;

	tpC8pC3pC8pC1[0] =  c1; tpC8pC3pC8pC1[1] =  c8;
	tpC8pC3pC8pC1[2] =  c3; tpC8pC3pC8pC1[3] =  c8;

	tpC8mC1pC8mC3[0] = -c3; tpC8mC1pC8mC3[1] =  c8;
	tpC8mC1pC8mC3[2] = -c1; tpC8mC1pC8mC3[3] =  c8;

	tpC8mC5pC8mC7[0] = -c7; tpC8mC5pC8mC7[1] =  c8;
	tpC8mC5pC8mC7[2] = -c5; tpC8mC5pC8mC7[3] =  c8;

	tmC2pC7mC6pC5[0] =  c5; tmC2pC7mC6pC5[1] = -c6;
	tmC2pC7mC6pC5[2] =  c7; tmC2pC7mC6pC5[3] = -c2;

	tpC6pC3pC2pC1[0] =  c1; tpC6pC3pC2pC1[1] =  c2;
	tpC6pC3pC2pC1[2] =  c3; tpC6pC3pC2pC1[3] =  c6;

	tpC2mC1pC6mC3[0] = -c3; tpC2mC1pC6mC3[1] =  c6;
	tpC2mC1pC6mC3[2] = -c1; tpC2mC1pC6mC3[3] =  c2;

	tmC6mC5mC2mC7[0] = -c7; tmC6mC5mC2mC7[1] = -c2;
	tmC6mC5mC2mC7[2] = -c5; tmC6mC5mC2mC7[3] = -c6;

	tpC6pC8pC2pC8[0] =  c8; tpC6pC8pC2pC8[1] =  c2;
	tpC6pC8pC2pC8[2] =  c8; tpC6pC8pC2pC8[3] =  c6;

	tmC2pC8mC6pC8[0] =  c8; tmC2pC8mC6pC8[1] = -c6;
	tmC2pC8mC6pC8[2] =  c8; tmC2pC8mC6pC8[3] = -c2;

	tmC7pC3pC3pC1[0] =  c1; tmC7pC3pC3pC1[1] =  c3;
	tmC7pC3pC3pC1[2] =  c3; tmC7pC3pC3pC1[3] = -c7;

	tmC5pC7mC1pC5[0] =  c5; tmC5pC7mC1pC5[1] = -c1;
	tmC5pC7mC1pC5[2] =  c7; tmC5pC7mC1pC5[3] = -c5;

	tmC8pC8pC8pC8[0] =  c8; tmC8pC8pC8pC8[1] =  c8;
	tmC8pC8pC8pC8[2] =  c8; tmC8pC8pC8pC8[3] = -c8;

	tmC2pC6pC6pC2[0] =  c2; tmC2pC6pC6pC2[1] =  c6;
	tmC2pC6pC6pC2[2] =  c6; tmC2pC6pC6pC2[3] = -c2;

	tmC1pC3pC5pC1[0] =  c1; tmC1pC3pC5pC1[1] =  c5;
	tmC1pC3pC5pC1[2] =  c3; tmC1pC3pC5pC1[3] = -c1;

	tpC3pC7pC7pC5[0] =  c5; tpC3pC7pC7pC5[1] =  c7;
	tpC3pC7pC7pC5[2] =  c7; tpC3pC7pC7pC5[3] =  c3;

	tmC5mC7pC7pC3[0] =  c3; tmC5mC7pC7pC3[1] =  c7;
	tmC5mC7pC7pC3[2] = -c7; tmC5mC7pC7pC3[3] = -c5;

	tmC1mC5pC3mC1[0] = -c1; tmC1mC5pC3mC1[1] =  c3;
	tmC1mC5pC3mC1[2] = -c5; tmC1mC5pC3mC1[3] = -c1;

	tpC8pC8_000000_pC8pC8_000000[0] = c8;
	tpC8pC8_000000_pC8pC8_000000[1] = 0;
	tpC8pC8_000000_pC8pC8_000000[2] = c8;
	tpC8pC8_000000_pC8pC8_000000[3] = 0;

	tpC2pC6_pC6mC2_mC2mC6_mC6pC2[0] = (short)(( (int)c2 + (int)c6 + 1) >> 1);
	tpC2pC6_pC6mC2_mC2mC6_mC6pC2[1] = (short)((-(int)c2 + (int)c6 + 1) >> 1);
	tpC2pC6_pC6mC2_mC2mC6_mC6pC2[2] = (short)((-(int)c2 - (int)c6 + 1) >> 1);
	tpC2pC6_pC6mC2_mC2mC6_mC6pC2[3] = (short)(( (int)c2 - (int)c6 + 1) >> 1);

	tpC1pC3_pC5mC1_pC3mC7_pC7mC5[0] = (short)(( (int)c1 + (int)c3 + 1) >> 1);
	tpC1pC3_pC5mC1_pC3mC7_pC7mC5[1] = (short)(( (int)c5 - (int)c1 + 1) >> 1);
	tpC1pC3_pC5mC1_pC3mC7_pC7mC5[2] = (short)(( (int)c3 - (int)c7 + 1) >> 1);
	tpC1pC3_pC5mC1_pC3mC7_pC7mC5[3] = (short)(( (int)c7 - (int)c5 + 1) >> 1);

	tpC7pC5_pC3pC7_mC5mC1_mC1pC3[0] = (short)(( (int)c7 + (int)c5 + 1) >> 1);
	tpC7pC5_pC3pC7_mC5mC1_mC1pC3[1] = (short)(( (int)c3 + (int)c7 + 1) >> 1);
	tpC7pC5_pC3pC7_mC5mC1_mC1pC3[2] = (short)((-(int)c5 - (int)c1 + 1) >> 1);
	tpC7pC5_pC3pC7_mC5mC1_mC1pC3[3] = (short)((-(int)c1 + (int)c3 + 1) >> 1);

	pC8pC8_tpC3pC1_pC8pC8_pC7pC5[0] = c8;
	pC8pC8_tpC3pC1_pC8pC8_pC7pC5[1] = (short)(( (int)c3 + (int)c1 + 1) >> 1);
	pC8pC8_tpC3pC1_pC8pC8_pC7pC5[2] = c8;
	pC8pC8_tpC3pC1_pC8pC8_pC7pC5[3] = (short)(( (int)c7 + (int)c5 + 1) >> 1);
	pC8pC8_tmC5mC7_pC8pC8_mC1mC3[0] = c8;
	pC8pC8_tmC5mC7_pC8pC8_mC1mC3[1] = (short)((-(int)c7 - (int)c5 + 1) >> 1);
	pC8pC8_tmC5mC7_pC8pC8_mC1mC3[2] = c8;
	pC8pC8_tmC5mC7_pC8pC8_mC1mC3[3] = (short)((-(int)c3 - (int)c1 + 1) >> 1);

	tpC2pC6_pC3pC1_mC2mC6_pC7pC5[0] = (short)(( (int)c2 + (int)c6 + 1) >> 1);
	tpC2pC6_pC3pC1_mC2mC6_pC7pC5[1] = (short)(( (int)c3 + (int)c1 + 1) >> 1);
	tpC2pC6_pC3pC1_mC2mC6_pC7pC5[2] = (short)((-(int)c2 - (int)c6 + 1) >> 1);
	tpC2pC6_pC3pC1_mC2mC6_pC7pC5[3] = (short)(( (int)c7 + (int)c5 + 1) >> 1);

	tmC2mC6_mC7mC5_pC2pC6_mC3mC1[0] = (short)((-(int)c2 - (int)c6 + 1) >> 1);
	tmC2mC6_mC7mC5_pC2pC6_mC3mC1[1] = (short)((-(int)c7 - (int)c5 + 1) >> 1);
	tmC2mC6_mC7mC5_pC2pC6_mC3mC1[2] = (short)(( (int)c2 + (int)c6 + 1) >> 1);
	tmC2mC6_mC7mC5_pC2pC6_mC3mC1[3] = (short)((-(int)c3 - (int)c1 + 1) >> 1);

	tpC8pC8_pC6pC2_pC8pC8_mC2mC6[0] = c8;
	tpC8pC8_pC6pC2_pC8pC8_mC2mC6[1] = (short)(( (int)c2 + (int)c6 + 1) >> 1);
	tpC8pC8_pC6pC2_pC8pC8_mC2mC6[2] = c8;
	tpC8pC8_pC6pC2_pC8pC8_mC2mC6[3] = (short)((-(int)c2 - (int)c6 + 1) >> 1);

	tpC1pC3_pC3mC7_pC7pC5_mC5mC1[0] = (short)(( (int)c3 + (int)c1 + 1) >> 1);
	tpC1pC3_pC3mC7_pC7pC5_mC5mC1[1] = (short)(( (int)c3 - (int)c7 + 1) >> 1);
	tpC1pC3_pC3mC7_pC7pC5_mC5mC1[2] = (short)(( (int)c7 + (int)c5 + 1) >> 1);
	tpC1pC3_pC3mC7_pC7pC5_mC5mC1[3] = (short)((-(int)c5 - (int)c1 + 1) >> 1);
	}
