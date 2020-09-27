
#ifndef RS232COMMUNICATION_H
#define RS232COMMUNICATION_H

#include "library\hardware\communic\generic\Communication.h"

enum RS232CommParity
	{
	RSCP_PARITY_NONE,
	RSCP_PARITY_EVEN,
	RSCP_PARITY_ODD
	};

enum RS232CommHandshake
	{
	RSCH_HANDSHAKE_NONE,
	RSCH_HANDSHAKE_xONxOFF,
	RSCH_HANDSHAKE_CTSDTS
	};

MKTAG(COMM_RS232_DATA_BITS,				COMMUNICATION_UNIT, 0x0101, DWORD)
MKTAG(COMM_RS232_STOP_BITS,				COMMUNICATION_UNIT, 0x0102, DWORD)
MKTAG(COMM_RS232_PARITY,					COMMUNICATION_UNIT, 0x0103, RS232CommParity)
MKTAG(COMM_RS232_HANDSHAKE,				COMMUNICATION_UNIT, 0x0104, RS232CommHandshake)


class RS232CommunicationPort : public CommunicationPort
	{
	public:
		virtual Error SetRS232Parameters(DWORD dataBits, DWORD stopBits, RS232CommParity parity, RS232CommHandshake handshake) = 0;
	};

class VirtualRS232CommunicationPort : public VirtualCommunicationPort
	{
	protected:
		DWORD ProtocolsSupported(void);

		RS232CommunicationPort * physical;

		DWORD dataBits, stopBits;
		RS232CommParity parity;
		RS232CommHandshake handshake;

		Error PreemptChange(VirtualUnit * previous);

	public:
		VirtualRS232CommunicationPort(RS232CommunicationPort * physical);
		~VirtualRS232CommunicationPort(void);

		Error Configure (TAG __far *tags);
	};



#endif
