#ifndef EEPROM_H
#define EEPROM_H

#include "library\lowlevel\hardwrio.h"


enum EEPROMOrganisation
	{
	EEPROM_ORG_8,
	EEPROM_ORG_16
	};

class EEPROM
	{
	protected:
	public:
		virtual Error Erase(int at) = 0;
		virtual Error EraseAll() = 0;
		virtual Error ProgramEnable(BOOL enable) = 0;
	};
	
class WordEEPROM : public EEPROM, public WordIndexedInOutPort
	{
	protected:
		EEPROMOrganisation org;
	public:
		WordEEPROM() {org = EEPROM_ORG_16;}

		virtual Error OutWord(int at, WORD data) = 0;
		virtual Error InWord(int at, WORD __far &data) = 0;
	};

class ByteEEPROM : public EEPROM, public ByteIndexedInOutPort
	{
	protected:
		EEPROMOrganisation org;
	public:
		ByteEEPROM() {org = EEPROM_ORG_8;}

		virtual Error OutByte(int at, BYTE data) = 0;
		virtual Error InByte(int at, BYTE __far &data) = 0;
	};

#endif


