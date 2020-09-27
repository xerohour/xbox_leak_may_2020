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

#ifndef IDCTTABLES_H
#define IDCTTABLES_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include <math.h>

//
// ACCURATE			: fractional bits of DCT
// FREQ_PREC		: fractional bits of frequency coefficients
// DCT_PREC			: fractional bits difference second to first step
// INTER_PREC		: fractional bits in middle stage
// FIRST_SHIFT		: number of bitshifts between first and second stage


#define ACCURATE					14
#define FREQ_PREC					3
#define INTER_PREC				7
#define DCT_PREC					(INTER_PREC - FREQ_PREC)

#define ONE_SHIFT					(FREQ_PREC + ACCURATE - 16)
#define FIRST_SHIFT				(ACCURATE - DCT_PREC)
#define SECOND_SHIFT				(INTER_PREC + ACCURATE - 16)

#define ROUND_ONE					(1 << (ONE_SHIFT - 1))
#define ROUND_SINGLE				(1 << (INTER_PREC - 1))
#define ROUND_SECOND				(1 << (SECOND_SHIFT - 1))

#define FLOAT_SHIFT				(1 << ACCURATE)

static const double pi = acos(0) * 2;
static const double DC1 = sqrt(2) * cos(pi*1/16);
static const double DC2 = sqrt(2) * cos(pi*2/16);
static const double DC3 = sqrt(2) * cos(pi*3/16);
static const double DC4 = sqrt(2) * cos(pi*4/16);
static const double DC5 = sqrt(2) * cos(pi*5/16);
static const double DC6 = sqrt(2) * cos(pi*6/16);
static const double DC7 = sqrt(2) * cos(pi*7/16);
static const double DC8 = 1.0000000000;

#define FCTI(x) ((int) ( floor(((x) * FLOAT_SHIFT) + 0.5) ))
#define FCCTI(x, y) FCTI((x + y) / 2)

static const short C1 =  (short) ( floor((DC1 * FLOAT_SHIFT) + 0.5) );
static const short C2 =  (short) ( floor((DC2 * FLOAT_SHIFT) + 0.5) );
static const short C3 =  (short) ( floor((DC3 * FLOAT_SHIFT) + 0.5) );
static const short C4 =  (short) ( floor((DC4 * FLOAT_SHIFT) + 0.5) );
static const short C5 =  (short) ( floor((DC5 * FLOAT_SHIFT) + 0.5) );
static const short C6 =  (short) ( floor((DC6 * FLOAT_SHIFT) + 0.5) );
static const short C7 =  (short) ( floor((DC7 * FLOAT_SHIFT) + 0.5) );
static const short C8 =  (short) ( floor((               FLOAT_SHIFT) + 0.5) );

static const short C1a = (short) ( floor(((DC1 - 1) * FLOAT_SHIFT * 4) + 0.5) );
static const short C2a = (short) ( floor(((DC2 - 1) * FLOAT_SHIFT * 4) + 0.5) );
static const short C3a = (short) ( floor(((DC3 - 1) * FLOAT_SHIFT * 4) + 0.5) );
static const short C5a = (short) ( floor(( DC5      * FLOAT_SHIFT * 2) + 0.5) );
static const short C6a = (short) ( floor(( DC6 		* FLOAT_SHIFT * 2) + 0.5) );
static const short C7a = (short) ( floor(( DC7 		* FLOAT_SHIFT * 4) + 0.5) );

static MMXQShort pC6pC2mC2pC6 = { C6, -C2,  C2,  C6};
static MMXQShort pC7pC3pC3mC1 = {-C1,  C3,  C3,  C7};
static MMXQShort mC1pC5mC5pC7 = { C7, -C5,  C5, -C1};
static MMXQShort pC5pC7mC1mC5 = {-C5, -C1,  C7,  C5};
static MMXQShort pC3pC1mC7pC3 = { C3, -C7,  C1,  C3};

static MMXQShort pC8pC8pC8pC8 = { C8,  C8,  C8,  C8};
static MMXQShort pC1pC1pC1pC1 = { C1,  C1,  C1,  C1};
static MMXQShort pC2pC2pC2pC2 = { C2,  C2,  C2,  C2};
static MMXQShort pC3pC3pC3pC3 = { C3,  C3,  C3,  C3};
static MMXQShort pC5pC5pC5pC5 = { C5,  C5,  C5,  C5};
static MMXQShort pC6pC6pC6pC6 = { C6,  C6,  C6,  C6};
static MMXQShort pC7pC7pC7pC7 = { C7,  C7,  C7,  C7};
static MMXQShort pC8000000000 = { C8,   0,   0,   0};

static MMXQShort pC1pC1pC1pC1a = { C1a,  C1a,  C1a,  C1a};
static MMXQShort pC2pC2pC2pC2a = { C2a,  C2a,  C2a,  C2a};
static MMXQShort pC3pC3pC3pC3a = { C3a,  C3a,  C3a,  C3a};
static MMXQShort pC5pC5pC5pC5a = { C5a,  C5a,  C5a,  C5a};
static MMXQShort pC6pC6pC6pC6a = { C6a,  C6a,  C6a,  C6a};
static MMXQShort pC7pC7pC7pC7a = { C7a,  C7a,  C7a,  C7a};

static MMXQShort pC1pC3pC3mC7 = {-C7,  C3,  C3,  C1};
static MMXQShort pC5mC1pC7mC5 = {-C5,  C7, -C1,  C5};
static MMXQShort pC2pC6pC6mC2 = {-C2,  C6,  C6,  C2};
static MMXQShort pC7mC5pC5mC1 = {-C1,  C5, -C5,  C7};
static MMXQShort pC3mC1pC7pC3 = { C3,  C7, -C1,  C3};

static MMXQShort pC6pC2mC8pC8 = { C8, -C8,  C2,  C6};
static MMXQShort pC8pC8mC2pC6 = { C6, -C2,  C8,  C8};
static MMXQShort pC2mC6pC8pC8 = { C8,  C8, -C6,  C2};
static MMXQShort mC8pC8mC6mC2 = {-C2, -C6,  C8, -C8};
static MMXQShort pC7pC3mC1pC3 = { C3, -C1,  C3,  C7};
static MMXQShort pC5pC1mC5mC7 = {-C7, -C5,  C1,  C5};
static MMXQShort pC3mC1pC3pC7 = { C7,  C3, -C1,  C3};
static MMXQShort pC7pC5mC1mC5 = {-C5, -C1,  C5,  C7};

static MMXQShort pC7mC5pC3mC1 = {-C1,  C3, -C5,  C7};
static MMXQShort pC5mC1pC7pC3 = { C3,  C7, -C1,  C5};
static MMXQShort pC3mC7mC1mC5 = {-C5, -C1, -C7,  C3};
static MMXQShort pC1pC3pC5pC7 = { C7,  C5,  C3,  C1};
static MMXQShort pC6mC2pC2mC6 = {-C6,  C2, -C2,  C6};
static MMXQShort pC2pC6mC6mC2 = {-C2, -C6,  C6,  C2};
static MMXQShort pC8mC8mC8pC8 = { C8, -C8, -C8,  C8};

static MMXQShort pC8pC7pC8pC5 = { C5,  C8,  C7,  C8};
static MMXQShort pC8pC3pC8pC1 = { C1,  C8,  C3,  C8};
static MMXQShort pC8mC1pC8mC3 = {-C3,  C8, -C1,  C8};
static MMXQShort pC8mC5pC8mC7 = {-C7,  C8, -C5,  C8};

static MMXQShort mC2pC7mC6pC5 = { C5, -C6,  C7, -C2};
static MMXQShort pC6pC3pC2pC1 = { C1,  C2,  C3,  C6};
static MMXQShort pC2mC1pC6mC3 = {-C3,  C6, -C1,  C2};
static MMXQShort mC6mC5mC2mC7 = {-C7, -C2, -C5, -C6};

static MMXQShort pC6pC8pC2pC8 = { C8,  C2,  C8,  C6};
static MMXQShort mC2pC8mC6pC8 = { C8, -C6,  C8, -C2};
static MMXQShort mC7pC3pC3pC1 = { C1,  C3,  C3, -C7};
static MMXQShort mC5pC7mC1pC5 = { C5, -C1,  C7, -C5};

static MMXQShort mC1mC5pC3mC1 = {-C1,  C3, -C5, -C1};
static MMXQShort mC5mC7pC7pC3 = { C3,  C7, -C7, -C5};
static MMXQShort pC3pC7pC7pC5 = { C5,  C7,  C7,  C3};
static MMXQShort mC1pC3pC5pC1 = { C1,  C5,  C3, -C1};
static MMXQShort mC2pC6pC6pC2 = { C2,  C6,  C6, -C2};
static MMXQShort mC8pC8pC8pC8 = { C8,  C8,  C8, -C8};



static MMXQShort pC8pC8_pC8pC8_pC8pC8_pC8pC8 = {FCCTI( DC8, DC8), FCCTI( DC8, DC8), FCCTI( DC8, DC8), FCCTI( DC8, DC8)};
static MMXQShort pC2pC6_mC2mC6_mC2mC6_pC2pC6 = {FCCTI( DC6, DC2), FCCTI(-DC2,-DC6), FCCTI(-DC2,-DC6), FCCTI( DC6, DC2)};
static MMXQShort mC2pC6_pC2mC6_pC2mC6_mC2pC6 = {FCCTI(-DC2, DC6), FCCTI( DC2,-DC6), FCCTI( DC2,-DC6), FCCTI(-DC2, DC6)};

static MMXQShort mC1mC3_mC5mC7_pC5pC7_pC1pC3 = {FCCTI( DC1, DC3), FCCTI( DC5, DC7), FCCTI(-DC5,-DC7), FCCTI(-DC1,-DC3)};
static MMXQShort mC3pC7_pC1pC5_mC1mC5_pC3mC7 = {FCCTI( DC3,-DC7), FCCTI(-DC1,-DC5), FCCTI( DC1, DC5), FCCTI(-DC3, DC7)};
static MMXQShort mC5pC1_mC7mC3_pC7pC3_pC5mC1 = {FCCTI( DC5,-DC1), FCCTI( DC7, DC3), FCCTI(-DC7,-DC3), FCCTI(-DC5, DC1)};
static MMXQShort mC7pC5_mC3pC1_pC3mC1_pC7mC5 = {FCCTI( DC7,-DC5), FCCTI( DC3,-DC1), FCCTI(-DC3, DC1), FCCTI(-DC7, DC5)};

class DCTMulTable
	{
	public:
		short tpC8pC8pC8pC8[4];
		short tpC8000000000[4];
		short tpC8pC7pC8pC5[4];
		short tpC8pC3pC8pC1[4];
		short tpC8mC1pC8mC3[4];
		short tpC8mC5pC8mC7[4];
		short tmC2pC7mC6pC5[4];
		short tpC6pC3pC2pC1[4];
		short tpC2mC1pC6mC3[4];
		short tmC6mC5mC2mC7[4];
		short tpC6pC8pC2pC8[4];
		short tmC2pC8mC6pC8[4];
		short tmC7pC3pC3pC1[4];
		short tmC5pC7mC1pC5[4];
		short tmC8pC8pC8pC8[4];
		short tmC2pC6pC6pC2[4];
		short tmC1pC3pC5pC1[4];
		short tpC3pC7pC7pC5[4];
		short tmC5mC7pC7pC3[4];
		short tmC1mC5pC3mC1[4];

		short tpC8pC8_000000_pC8pC8_000000[4];
		short tpC2pC6_pC6mC2_mC2mC6_mC6pC2[4];
		short tpC1pC3_pC5mC1_pC3mC7_pC7mC5[4];
		short tpC7pC5_pC3pC7_mC5mC1_mC1pC3[4];

		short pC8pC8_tpC3pC1_pC8pC8_pC7pC5[4];
		short pC8pC8_tmC5mC7_pC8pC8_mC1mC3[4];

		short tpC2pC6_pC3pC1_mC2mC6_pC7pC5[4];
		short tmC2mC6_mC7mC5_pC2pC6_mC3mC1[4];

		short tpC8pC8_pC6pC2_pC8pC8_mC2mC6[4];
		short tpC1pC3_pC3mC7_pC7pC5_mC5mC1[4];

		void CalculateTable(int fact);
	};

#endif
