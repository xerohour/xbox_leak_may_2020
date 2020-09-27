#ifndef PCM17XX_H
#define PCM17XX_H

#include "..\generic\audiodac.h"
#include "..\..\..\lowlevel\hardwrio.h"

//
// Concrete implementation of an audio DAC, Burr Brown 1717
//
class PCM17XX : public AudioDAC
	{
	friend class VirtualPCM17XX;
	protected:
		// Cached control words
		//
		WORD	cw[4];
		//
		// Changed control words (flag set)
		//
		WORD	cc;
		//		
		// Serial control lines
		//
		BitOutputPort	*	md;
		BitOutputPort	*	mc;
		BitOutputPort	*	ml;		 


		BOOL	mute,dMute;
		WORD	volumeLeft,	volumeRight;
		WORD	dVolumeLeft, dVolumeRight;
		
		//
		// Write a control word to the chip
		//
		Error WriteControlWord(WORD cw);
	protected:
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		
		//
		// Update the dac control words
		//
		Error Update(void);
	public:
		PCM17XX(BitOutputPort * md,
		        BitOutputPort * mc,
		        BitOutputPort * ml);
		        
		Error Configure(TAG __far * tags);
		
		virtual Error Initialize(void);

		Error ReInitialize(void);
		
		VirtualAudioDAC * CreateVirtualAudioDAC(void); 
	};


class VirtualPCM17XX : public VirtualAudioDAC
	{
	private:
		PCM17XX	*	device;
	public:
		VirtualPCM17XX(PCM17XX * device) : VirtualAudioDAC(device) {this->device = device;}
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);		
	};



#endif
