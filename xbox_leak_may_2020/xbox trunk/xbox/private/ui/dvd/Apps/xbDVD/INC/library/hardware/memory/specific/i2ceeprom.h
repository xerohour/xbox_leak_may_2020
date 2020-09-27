// FILE:      i2ceeprom.h
// AUTHOR:    H.Horak
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   19.12.98
//
// PURPOSE:   I2C serial EEPROM class.
//
// HISTORY:   

#ifndef I2CEEPROM_H
#define I2CEEPROM_H

#include "library\lowlevel\hwbusio.h"
#include "..\generic\eeprom.h"

class I2CEEPROM : public ByteEEPROM
	{
	private:
		AsyncByteInOutBus	*port;
		int i2cAddress;
		BOOL extendedAddressing;
		BOOL programEnable;

	public:
		I2CEEPROM (AsyncByteInOutBus *port, int i2cAddress, BOOL extendedAddressing);
		virtual ~I2CEEPROM(void);

		Error ProgramEnable(BOOL enable);

		Error OutByte(int at, BYTE data);
		Error InByte(int at, BYTE __far &data);
	
		Error Erase(int at);
		Error	EraseAll();
	};


class I2CEEPROMMemory
	{
	private:
		BYTE			*	buffer;
		ByteEEPROM	*	eeprom;
		int				size;
		BOOL				dirty;

	public:
		I2CEEPROMMemory(ByteEEPROM * eeprom, int size);
		virtual	~I2CEEPROMMemory();

		void	ClearAll(BYTE value);

		Error	LoadAll();
		Error	StoreAll();
		Error	StorePartial(int from, int length);

		BOOL	CheckDirty() {return dirty;}

		BYTE	CalculateChecksum(int from, int length);

		BYTE	GetByte(int at) {return buffer[at];}
		void	PutByte(int at, BYTE data) {buffer[at] = data; dirty = TRUE;}
	};


#endif

