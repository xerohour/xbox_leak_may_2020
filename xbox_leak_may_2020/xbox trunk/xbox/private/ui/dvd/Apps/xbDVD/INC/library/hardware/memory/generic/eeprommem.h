// FILE:      eeprommem.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:   21.09.1999
//
// PURPOSE:   General EEPROM memory classes
//
// HISTORY:   

#ifndef EEPROMMEM_H
#define EEPROMMEM_H

#include "eeprom.h"

class ByteEEPROMMemory
	{
	private:
		BYTE			*	buffer;
		ByteEEPROM	*	eeprom;
		int				size;
		BOOL				dirty;

	public:
		ByteEEPROMMemory(ByteEEPROM * eeprom, int size);
		virtual	~ByteEEPROMMemory();

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


