/*****************************************************************************************************************

	These are flags used for the dwFlags parameter in the HSOUND struct.  3DSOUND will assert that the sound is 3D,
	ISVOICE lets us know that the sound is a voice, ISMUSIC tells us if it is music, RANDOM lets us know we should
	apply random effects to the sound within a certain range, and looping tells us if the sound loops or not

******************************************************************************************************************/

#define THREEDSND	0x00000001		//Is this a 3D Sound
#define ISVOICE		0x00000002		//Is this sound a voice
#define ISMUSIC		0x00000004		//Is this sound music
#define RANDOM		0x00000008		//Determines if we are to apply a random effect within some range

#define TWOTWOKHZ	0x00000010
#define FORFORKHZ	0x00000020

#define ADPCM		0x00000100
#define WMA			0x00000200
#define PCM			0x00000400
		
#define LOOPING		0x00001000

//Masks for checking format, sample rate, & both
#define FORMAT_MASK 0x00000f00
#define SAMPLE_MASK 0x000000f0
#define BUFFER_MASK 0x00000ff0


//Masks for deciding whether to apply Velocity, Position, & Cones to a sound played
#define VELOCITY	0x00000001
#define POSITION	0x00000010
#define CONE		0x00000100
#define VOLUME		0x00001000


//#define's for the real-world multiples of roll-off, doppler & Distance units
#define DISTANCE	1
#define DOPPLER		1
#define ROLLOFF		1

#define INVALID		-1



//JJBUG-- Get the real definition of Vector from Ronin
struct Vector
{
	float x;
	float y;
	float z;
};


typedef struct _tagVoiceInfo
{
	D3DVECTOR	pPosition;
	D3DVECTOR	pVelocity;
	
	//Cone Data
	DWORD		dwInConeAngle;
	DWORD		dwOutConeAngle;
	Vector		vConeOrientation;
	LONG		lOuterVolume;
}VOICE_INFO,*PVOICE_INFO;

//Voice & Music streams require the same info...
typedef void* HVOICE;
typedef void* HMUSIC;
typedef void* HSTREAM;
typedef void* SNDHANDLE;
typedef void* HCHANNEL3D;	//Really the same thing...except that the void* parameter doesn't matter

enum OBSTRUCTION_TYPE;			//JJBUG-- Figure out what obstruction types we need after talking w/ Ronin

//Functions

//Regular sound functions
HRESULT __stdcall xbInitAudio();
HRESULT __stdcall xbUninitAudio();													//JJBUG -- TODO- Necessary?
SNDHANDLE __stdcall xbGetHandle(void*, DWORD, DWORD);
HRESULT __stdcall xbPlaySound(SNDHANDLE, bool);								
HRESULT __stdcall xbStopSound(SNDHANDLE);				
HRESULT __stdcall xbSetVolume(SNDHANDLE, LONG);		
HRESULT __stdcall xbFadeVolume(SNDHANDLE,LONG,LONG);								//JJBUG -- NEED XDK Update
HRESULT __stdcall xbSet2DPan(SNDHANDLE, LONG);										//JJBUG -- NEED XDK Update
HRESULT __stdcall xbFade2DPan(SNDHANDLE, LONG, LONG);								//JJBUG -- NEED XDK Update
HRESULT __stdcall xbSet3DPosition(SNDHANDLE, Vector);									
HRESULT __stdcall xbSet3DVelocity(SNDHANDLE, Vector);									
HRESULT __stdcall xbSet3DCone(SNDHANDLE,DWORD,DWORD,Vector,LONG);
HRESULT __stdcall xbSetFX(SNDHANDLE);												//JJBUG -- NEED XDK Update
HRESULT __stdcall xbIsSoundPlaying(SNDHANDLE, bool*);									
HRESULT __stdcall xbGetFlags(SNDHANDLE, DWORD*);
HRESULT __stdcall xbSetFlags(SNDHANDLE, DWORD);


//2D Routing Funtions
HCHANNEL3D __stdcall xbGetChannel();
HRESULT __stdcall xbPlay3DSoundChannel(HCHANNEL3D);									
HRESULT __stdcall xbRoute2DSound(SNDHANDLE, HCHANNEL3D);


//Listener functions
HRESULT __stdcall xbSetListenerPosition(Vector);
HRESULT __stdcall xbSetListenerVelocity(Vector);
HRESULT __stdcall xbSetListenerOrientation(Vector, Vector);

//Music functions
HSTREAM __stdcall xbGetMusicStream(const char* szFileName, DWORD dwFlags);					
HRESULT __stdcall xbPlayMusic(HSTREAM);
HRESULT __stdcall xbSetMusicVolume(LONG);												
HRESULT __stdcall xbContinueMusic(HSTREAM);											//JJBUG -- TODO
HRESULT __stdcall xbFlourishMusic(HSTREAM);											//JJBUG -- TODO
HRESULT __stdcall xbStopMusic(HSTREAM);
HRESULT __stdcall xbFadeMusic(void*,LONG,DWORD);									//JJBUG -- NEED XDK UPDATE													
HRESULT __stdcall xbSetMusicRequestCallback(void WINAPI callBackFunc(HSTREAM));		
HRESULT __stdcall xbSetMusicContinueCallback(void WINAPI callBackFunc(HSTREAM));
HRESULT __stdcall xbProcess();
HRESULT __stdcall xbPauseMusic();
HRESULT __stdcall xbResumeMusic();
		
//Voice Stream functions
HVOICE  __stdcall xbPlayVoiceStream(char* szFileName, bool bDipMusic, PVOICE_INFO pVI, DWORD dwFlags);
HRESULT __stdcall xbSetVoiceVolume(LONG);
HRESULT __stdcall xbPauseVoice(void*);
HRESULT __stdcall xbResumeVoice(void*);

//Voices are 3D, music is not
HRESULT __stdcall xbSetVoicePosition(void*,Vector);
HRESULT __stdcall xbSetVoiceVelocity(void*,Vector);
HRESULT __stdcall xbSetVoiceCone(void*,DWORD,DWORD,Vector,LONG);

//Generic Stream Control
HRESULT __stdcall xbPauseAllStreams();
HRESULT __stdcall xbResumeAllStreams();


//NEED XDK UPDATE TO BE ABLE TO REALLY TEST THIS STUFF OUT
//Occlusion/Obstruction functions											
HRESULT __stdcall xbI3DClear();														//JJBUG -- TODO
HRESULT __stdcall xbI3DAddObstruction(Vector, Vector, OBSTRUCTION_TYPE);			//JJBUG -- TODO
HRESULT __stdcall xbI3DAddOcclusion(Vector, Vector, OBSTRUCTION_TYPE);				//JJBUG -- TODO
