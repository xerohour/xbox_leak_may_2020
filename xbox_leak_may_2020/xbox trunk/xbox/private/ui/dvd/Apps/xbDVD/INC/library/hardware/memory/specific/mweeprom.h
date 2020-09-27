#ifndef MWEEPROM_H
#define MWEEPROM_H

#include "..\generic\eeprom.h"

class MicroWireEEPROM : public WordEEPROM
	{
	private:
		BitOutputPort * cs;
		BitOutputPort * sk;
		BitOutputPort * dw;
		BitInputPort  * dr;

		BOOL				 programEnable;
	protected:	
		Error OutBit(BIT bit);
		Error InBit(BIT __far & bit);
	public:
		MicroWireEEPROM(BitOutputPort * cs,
							 BitOutputPort * sk,
							 BitOutputPort * dw,
							 BitInputPort  * dr);

		Error ProgramEnable(BOOL enable);

		Error OutWord(int at, WORD data);
		Error InWord(int at, WORD __far &data);
	
		Error Erase(int at) {GNRAISE(GNR_UNIMPLEMENTED);}
		Error	EraseAll() {GNRAISE(GNR_UNIMPLEMENTED);}
	};


class MicroWireEEPROMMemory
	{
	private:
		WORD			*	buffer;
		WordEEPROM	*	eeprom;
		int				size;
		BOOL				dirty;
	public:
		MicroWireEEPROMMemory(WordEEPROM * eeprom, int size);
		virtual	~MicroWireEEPROMMemory();

		void	ClearAll(WORD value);

		Error	LoadAll();
		Error	StoreAll();
		Error	StorePartial(int from, int length);

		BOOL	CheckDirty() {return dirty;}

		WORD	CalculateChecksum(int from, int length);

		WORD	GetWord(int at) {return buffer[at];}
		void	PutWord(int at, WORD data) {buffer[at] = data; dirty = TRUE;}
	};


#endif

