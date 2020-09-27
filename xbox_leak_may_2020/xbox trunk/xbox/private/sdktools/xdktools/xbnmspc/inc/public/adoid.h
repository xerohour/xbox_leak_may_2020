//--------------------------------------------------------------------
// Microsoft ADO
//
// (c) 1998 Microsoft Corporation.  All Rights Reserved.
//
// @doc
//
// @module	adoid.h | ADO Guids
//
// @devnote None
//--------------------------------------------------------------------

//
//
//
//
//
//
// Warning!!
//
// check MAXAVAILABLEGUID and MAXAVAILABLEGUIDALL to find the next available guid
//
//
//
//
//
//

#ifndef _ADOID_H_
#define _ADOID_H_

#include "adodef.h"

#define DEFINE_ADOGUID(name, l) \
    DEFINE_GUID(name, l, 0, 0x10, 0x80,0,0,0xAA,0,0x6D,0x2E,0xA4)

// Type library
DEFINE_ADOGUID(LIBID_ADO20,				0x00000200);
DEFINE_ADOGUID(LIBID_ADOR20,			0x00000300);
DEFINE_ADOGUID(LIBID_ADO25,				0x00000205);
DEFINE_ADOGUID(LIBID_ADOR25,			0x00000305);

#define LIBID_ADO		LIBID_ADO25
#define LIBID_ADOR		LIBID_ADOR20

// defined for backwards compatibility
#define LIBID_CADO10	LIBID_ADO20
#define LIBID_CADOR10	LIBID_ADOR20

// Error
DEFINE_ADOGUID(IID_IADOError,            0x00000500);
DEFINE_ADOGUID(IID_IADOErrors,           0x00000501);

// Property
DEFINE_ADOGUID(IID_IADOProperty,         0x00000503);
DEFINE_ADOGUID(IID_IADOProperties,       0x00000504);

// Field
DEFINE_ADOGUID(CLSID_CADOField,			 0x0000053A);
DEFINE_ADOGUID(IID_IADOField15,            0x00000505);
DEFINE_ADOGUID(IID_IADOField20,            0x0000054C);
DEFINE_ADOGUID(IID_IADOField,            0x00000569);

DEFINE_ADOGUID(IID_IADOFields15,           0x00000506);
DEFINE_ADOGUID(IID_IADOFields20,           0x0000054D);
DEFINE_ADOGUID(IID_IADOFields,           0x00000564);

// Command
DEFINE_ADOGUID(CLSID_CADOCommand,		0x00000507);
DEFINE_ADOGUID(IID_IADOCommand15,			0x00000508);
DEFINE_ADOGUID(IID_IADOCommand,			0x0000054E);
DEFINE_ADOGUID(IID_IADOCommands,        0x00000509);
DEFINE_ADOGUID(IID_IADOCommandConstruction,	   0x00000517);

// Parameter
DEFINE_ADOGUID(CLSID_CADOParameter,		0x0000050B);
DEFINE_ADOGUID(IID_IADOParameter,        0x0000050C);
DEFINE_ADOGUID(IID_IADOParameters,       0x0000050D);

//Recordset
DEFINE_ADOGUID(CLSID_CADORecordset, 	 0x00000535);
DEFINE_ADOGUID(IID_IADORecordset15,		 0x0000050E);
DEFINE_ADOGUID(IID_IADORecordset20,		 0x0000054F);
DEFINE_ADOGUID(IID_IADORecordset21,		 0x00000555);
DEFINE_ADOGUID(IID_IADORecordset,		 0x00000556);

DEFINE_ADOGUID(IID_IADORecordsets,		 0x0000050F);
DEFINE_ADOGUID(IID_IADORecordsetConstruction,     0x00000283);

// Collections
DEFINE_ADOGUID(IID_IADOCollection,       0x00000512);
DEFINE_ADOGUID(IID_IADODynaCollection,   0x00000513);

// Connection
DEFINE_ADOGUID(CLSID_CADOConnection,	   0x00000514);
DEFINE_ADOGUID(IID_IADOConnection15,	   0x00000515);
DEFINE_ADOGUID(IID_IADOConnection,	   0x00000550);
DEFINE_ADOGUID(IID_IADOConnectionConstruction15,	0x00000516);
DEFINE_ADOGUID(IID_IADOConnectionConstruction,		0x00000551);
DEFINE_ADOGUID(IID_IADOConnections,		   0x00000518);

// events
// dispatch interfaces
DEFINE_ADOGUID(IID_IADORecordsetEvents,	   0x00000266);
DEFINE_ADOGUID(IID_IADOConnectionEvents,   0x00000400);
// vtable interfaces
DEFINE_ADOGUID(IID_IADORecordsetEventsVt,   0x00000403);
DEFINE_ADOGUID(IID_IADOConnectionEventsVt,  0x00000402);

DEFINE_ADOGUID(CLSID_CADORecord,			0x00000560);
DEFINE_ADOGUID(CLSID_CADORecField,			0x00000561);
DEFINE_ADOGUID(IID_IADORecord,				0x00000562);
DEFINE_ADOGUID(IID_IADORecFields,			0x00000563);
DEFINE_ADOGUID(IID_IADOStream,				0x00000565);
DEFINE_ADOGUID(CLSID_CADOStream,			0x00000566);
DEFINE_ADOGUID(IID_IADORecordConstruction,	0x00000567);
DEFINE_ADOGUID(IID_IADOStreamConstruction,	0x00000568);

#define MAXAVAILABLEGUID 0x00000570
#define MAXAVAILABLEGUIDALL 0x0000057F
#endif // _ADOID_H_
