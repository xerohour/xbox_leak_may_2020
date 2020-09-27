
// FILE:      library\hardware\jpeg\generic\jcod050.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   12.01.96
//
// PURPOSE: Implementation of JPEG coder with bit rate control using the ZR36050.
//
// HISTORY:

#ifndef JCOD050_H
#define JCOD050_H

#include "library\common\prelude.h"
#include "library\lowlevel\hardwrio.h"
#include "library\hardware\video\generic\vidtypes.h"
#include "library\hardware\jpeg\generic\jcodrate.h"



#ifndef ONLY_EXTERNAL_VISIBLE


#define APP_LENGTH  64

class VirtualJCoder050;



class JCoder050 : public JCoderRated
	{
	friend class VirtualJCoder050;

	private:
		ByteIndexedInOutPort *port;
		int cfisExpansion;

		DWORD SOFMarkerSize;
		DWORD SOSMarkerSize;
		DWORD APPMarkerSize;
		DWORD COMMarkerSize;
		DWORD DRIMarkerSize;
		DWORD DQTMarkerSize;
		DWORD DHTMarkerSize;
		DWORD DNLMarkerSize;
		DWORD DQTIMarkerSize;
		DWORD DHTIMarkerSize;

	public:
		// cfisExpansion must be 0..7.
		JCoder050 (ByteIndexedInOutPort *port, int cfisExpansion)
			{this->port = port;  this->cfisExpansion = cfisExpansion;}

		virtual VirtualUnit *CreateVirtual (void);

		void Start (void);
		BOOL Check050End (void);
		void PrepareGO (void);

	protected:
		BOOL ConfigureCompression (void);
		BOOL ConfigureExpansion (void);
		virtual WORD GetLastScaleFactor (void);

		VirtualJCoder050 *GetCurrent (void) {return (VirtualJCoder050 *)(JCoderRated::GetCurrent());}

		BYTE HSampRatio[8];
		BYTE VSampRatio[8];

		void MakeMarkerSegments (void);
		DWORD MakeSOFMarker (void);
		DWORD MakeSOS1Marker (void);
		DWORD MakeDRIMarker (void);
		DWORD MakeQTable (void);
		DWORD MakeHuffTable (void);
		DWORD MakeAPPString (void);
		DWORD MakeCOMString (void);
	};



class VirtualJCoder050 : public VirtualJCoderRated
	{
	friend class JCoder050;

	protected:
		JCoder050 *jcoder;

	public:
		VirtualJCoder050 (JCoder050 *physical);
		virtual ~VirtualJCoder050 (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error InitOperation (void);

		virtual void Start (void) {jcoder->Start();}
		virtual void Stop  (void);

		WORD GetLastScaleFactor (void) {return jcoder->GetLastScaleFactor ();}

	protected:
		void SetDefaultAPP (void);

		VideoFormat format;
		BOOL thumbnail;     // Fast Preview mode ?
		BOOL twoPassMode;   // use Auto Bit Rate Control ?
		BYTE __far *qTable;
		BYTE __far *huffTable;
		BYTE __far *comString;

		BYTE appMarker[APP_LENGTH+1];
	};



#endif // ONLY_EXTERNAL_VISIBILE



#endif
