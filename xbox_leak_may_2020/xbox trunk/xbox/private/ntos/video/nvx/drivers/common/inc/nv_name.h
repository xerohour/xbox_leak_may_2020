typedef struct _CHIP_INFO
{
	ULONG   ulDevID;
	PWSTR   pwszChip;
	PWSTR   pwszAdapterString;
} CHIP_INFO;

CHIP_INFO   sChipInfo[] =
{
 // DEVICE ID  Chip ID name              Adapter ID Name
{0x0020,	L"RIVA TNT",		L"RIVA TNT"		},
{0x0028,	L"RIVA TNT2",		L"RIVA TNT2"		},
{0x002C,	L"Vanta",		L"Vanta"		},
{0x0029,	L"RIVA TNT2 Ultra",	L"RIVA TNT2 Ultra"	},
{0x002D,	L"RIVA TNT2 Model 64",	L"RIVA TNT2 Model 64"	},
{0x00A0,	L"Aladdin TNT2",	L"Aladdin TNT2"		},
{0x0100,	L"GeForce 256",		L"GeForce 256"		},
{0x0101,	L"GeForce DDR",		L"GeForce DDR"		},
{0x0103,	L"Quadro",		L"Quadro"		},
{0x0110,	L"GeForce2 MX",		L"GeForce2 MX"		},
{0x0111,	L"GeForce2 MX",		L"GeForce2 MX"		},
{0x0113,	L"Quadro2 MXR",		L"Quadro2 MXR"		},
{0x0150,	L"GeForce2 GTS",	L"GeForce2 GTS"		},
{0x0151,	L"GeForce2 GTS",	L"GeForce2 GTS"		},
{0x0152,	L"GeForce2 Ultra",	L"GeForce2 Ultra"	},
{0x0153,	L"Quadro2 Pro",		L"Quadro2 Pro"		},
{0x0200,	L"NV20",		L"NV20"		}
};
