#ifndef PCM1725_H
#define PCM1725_H

#include "..\generic\audiodac.h"
#include "..\..\..\lowlevel\hardwrio.h"

//
// PCM1725
//
class PCM1725 : public AudioDAC
	{
	friend class VirtualPCM1725;
	protected:
		BitOutputPort	*	emphasis;

		Error SetLeftVolume(WORD volume) {GNRAISE_OK;}
		Error SetRightVolume(WORD volume) {GNRAISE_OK;}
		Error SetMute(BOOL mute) {GNRAISE_OK;}
		Error SetInputFormat(DWORD format) {GNRAISE_OK;}
		Error SetOutputFormat(DWORD format) {GNRAISE_OK;}

		//
		// Update the dac control words
		//
		Error Update(void)  {GNRAISE_OK;}
	public:
		PCM1725(BitOutputPort * emphasis);
		        
		Error Configure(TAG __far * tags);
		
		virtual Error Initialize(void);

		Error	ReInitialize(void);
		
		VirtualAudioDAC * CreateVirtualAudioDAC(void); 
	};


class VirtualPCM1725 : public VirtualAudioDAC
	{
	private:
		PCM1725	*	device;
	public:
		VirtualPCM1725(PCM1725 * device) : VirtualAudioDAC(device) {this->device = device;}
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);		
	};



#endif
