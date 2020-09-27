void SetNESDeemph(byte d, byte f);
#ifndef DrawState
void DrawState(void);
#endif
#ifndef DrawText
void DrawText(byte *dest, word width, byte *textmsg, byte fgcolor, byte bgcolor);
#endif
void DrawTextTrans(byte *dest, word width, byte *textmsg, byte fgcolor);
void PutImage(void);
void TrashMachine(void);
void ProtFaultBefore(void);
void CloseStuff(int signum);
int InitMachine(void);
uint32 Joysticks(void);

extern byte Exit;
extern byte ssync;
extern byte dowait;
extern byte howlong;
extern byte errmsg[];
extern byte pale;
extern byte maxline;
extern char SaveStateStatus[10];

/* Prototypes for platform interface functions follow: */

int PreInit(void);

/* Video interface */
void SetPalette(uint8 index, uint8 r, uint8 g, uint8 b);
void GetPalette(uint8 i,uint8 *r, unsigned char *g, unsigned char *b);
void BlitScreen(void);
int SetVideoMode(int vmode);
void WaitForVBlank(void);
int LockConsole(void);
int UnlockConsole(void);
void ResetVideo(void);


/* Keyboard interface */
int KeyboardInitialize(void);
void KeyboardClose(void);
int KeyboardUpdate(void);
char *KeyboardGetstate(void);

/* Joystick interface */
uint32 GetJSOr(void);
void KillJoystick(void);
char InitJoystick(void);

/* Sound interface */

int InitSound(void);
void TrashSound(void);
void FillSoundBuffer(unsigned char *Buf);

void PrintFatalError(char *s);
void PrintNonFatal(char *s);

int fceumain(void);
//int fceumain(int argc,char *argv[]);
#ifdef NETWORK
/* Network interface */

int NetworkConnect(char *Server);
int NetworkRecvByte(unsigned char *Value);
int NetworkSendByte(unsigned char *Value);
void NetworkClose(void);
#endif

void SetNESPalette(void);
void DrawTextTrans(byte *dest, word width, byte *textmsg, byte fgcolor);


#define JOY_A   1
#define JOY_B   2
#define JOY_SELECT      4
#define JOY_START       8
#define JOY_UP  0x10
#define JOY_DOWN        0x20
#define JOY_LEFT        0x40
#define JOY_RIGHT       0x80

extern int ntsccol;
extern int ntsctint;
extern int ntschue;

extern int vmode;
extern int soundon;

void DoCommand(byte c);
extern byte CommandQueue;
