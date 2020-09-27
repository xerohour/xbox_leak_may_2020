
#ifndef UART5505_H
#define UART5505_H

#include "library\lowlevel\memmpdio.h"
#include "library\hardware\communic\generic\RS232Communication.h"
#include "library\lowlevel\intrctrl.h"

class UART5505CommunicationPort : public RS232CommunicationPort, 
                                  public BufferedByteCommunicationStream, 
                                  protected InterruptHandler
	{
	protected:
		MemoryMappedIO		*	port;
		InterruptServer	*	irqServer;

		DWORD controlShadow;
		DWORD intEnableShadow;

		void NotifyOutputDataAvailable(void);
		void NotifyInputSpaceAvailable(void);

		Error SetProtocol(DWORD protocol);
		Error SetBaudRate(DWORD rate);
		Error SetRS232Parameters(DWORD dataBits, DWORD stopBits, RS232CommParity parity, RS232CommHandshake handshake);

		Error InitializeConnection(void);
		Error AbortConnection(void);

	public:
		UART5505CommunicationPort(MemoryMappedIO * port, InterruptServer * irqServer);
		virtual ~UART5505CommunicationPort(void);

		virtual VirtualUnit * CreateVirtual(void);

		void Interrupt(void);
	};

#endif
