
// FILE:      library\hardware\jpeg\generic\z016.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   05.04.95
//
// PURPOSE: The class for the Zoran ZR36016 Motion JPEG controller.
//
// HISTORY:

#include "z016.h"
#include "library\support\gfxdebug\gfxdebug.h"





// Constructor for Z016_CONFIG.

Z016_CONFIG::Z016_CONFIG (void)
	{
	compress    = FALSE;
	stillMode   = FALSE;
	thumbnail   = FALSE;
	colFiltersOn = FALSE;
	horFilterOn = FALSE;
	hDecimation = TRUE;
	vDecimation = TRUE;
	doubleClock = FALSE;
	ccirLevels  = FALSE;
	syen        = 0;
	vbusFormat  = VFMT_YUV_422;
	coderFormat = VFMT_YUV_422;
	pxoutFormat = VFMT_YUV_422;
	top    = 0;
	left   = 0;
	width  = 320;
	height = 240;
	}





	// Construct Mode register. We don't support all in/out combinations.
	// If there is an illegal combination, we do YUV 4:2:2 for in and out.

	static BYTE makeModeRegister (LP_Z016_CONFIG cfg)
		{
		BYTE mode;

		switch (cfg->coderFormat)
			{
			case VFMT_YUV_444:  mode = 0x00;  break;
			default:
			case VFMT_YUV_422:  mode = 0x01;  break;
			case VFMT_YUV_420:  mode = 0x02;  break;
			case VFMT_RGB_444:  mode = 0x04;  break;
			}
		switch (cfg->vbusFormat)
			{
			case VFMT_RGB_444:
				mode |= 0x00;
				break;
			case VFMT_YUV_444:
				mode |= 0x08;
				break;
			case VFMT_YUV_422:
				if (cfg->coderFormat == VFMT_YUV_422  ||  
				    cfg->coderFormat == VFMT_YUV_420)
					mode |= 0x10;
				else
					mode = 0x11;   // default is YUV 4:2:2
				break;
			default:
				mode = 0x11;   // default is YUV 4:2:2
				break;
			}
		switch (cfg->pxoutFormat)
			{
			case VFMT_RGB_444:  mode |= 0x00;  break;
			case VFMT_YUV_444:  mode |= 0x20;  break;
			default:
			case VFMT_YUV_422:  mode |= 0x40;  break;
			}
		if (cfg->compress)
			mode |= 0x80;   // do compression
		return mode;
		}



	// Construct Setup 1 register.

	static BYTE makeSetup1Register (LP_Z016_CONFIG cfg)
		{
		BYTE s;

		s = cfg->stillMode ? 0x00 : 0x01;
		if (cfg->thumbnail)  s |= 0x02;
		if (cfg->colFiltersOn)  s |= 0x0C;

		if (cfg->hDecimation)
			{
			s |= 0x20;
			if (cfg->horFilterOn)  s |= 0x10;
			if (cfg->doubleClock)  s |= 0x80;
			}
		if (cfg->vDecimation)  s |= 0x40;
		return s;
		}



	// Construct Setup 2 register.

	static BYTE makeSetup2Register (LP_Z016_CONFIG cfg)
		{
		BYTE s;

		// YMCS always zero.
		// We are connected to a Z050, so always unsigned data.
		s = cfg->ccirLevels ? 0x04 : 0x00;
		if (cfg->syen)  s |= 0x40;
		return s;
		}





// Write a configuration into the chip. Return success.

BOOL Z016Class::Configure (LP_Z016_CONFIG cfg)
	{
	Stop ();

	port->OutByte (1, makeModeRegister (cfg));

	// Write all indirect registers in one sequence.
	port->OutByte (2, 0x00);
	port->OutByte (3, makeSetup1Register (cfg));
	port->OutByte (3, makeSetup2Register (cfg));
	// NAX
	port->OutByte (3, LOBYTE(cfg->left));
	port->OutByte (3, HIBYTE(cfg->left) & 0x1F);
	// PAX
	port->OutByte (3, LOBYTE(cfg->width));
	port->OutByte (3, HIBYTE(cfg->width));
	// NAY
	port->OutByte (3, LOBYTE(cfg->top));
	port->OutByte (3, HIBYTE(cfg->top) & 0x1F);
	// PAY
	port->OutByte (3, LOBYTE(cfg->height));
	port->OutByte (3, HIBYTE(cfg->height));

	return TRUE;
	}
