typedef struct {
                char ID[5]; /*NESM^Z*/
                byte Version;
                byte TotalSongs;
                byte StartingSong;
                byte LoadAddressLow;
		byte LoadAddressHigh;
		byte InitAddressLow;
                byte InitAddressHigh;
                byte PlayAddressLow;
                byte PlayAddressHigh;
		byte SongName[32];
		byte Artist[32];
		byte Copyright[32];
		byte NTSCspeed[2];		// Unused
		byte BankSwitch[8];
		byte PALspeed[2];		// Unused
		byte VideoSystem;
		byte SoundChip;
		byte Expansion[4];
                byte reserve[8];
        } NSF_HEADER;
int NSFLoad(FILE *fp);
void NSF_write(word A, byte V);
byte NSF_read(word A);
void NSF_init(void);
extern byte CurrentSong;
extern byte SongReload;
void DrawNSF(void);
void NSFControl(int z);
extern NSF_HEADER NSFHeader;
void NSFDealloc(void);

