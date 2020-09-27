void SaveState(void);
void LoadState(void);

extern byte StateName[2048];
extern byte StateFile[2048];
extern byte CurrentState;
extern char SaveStateStatus[10];
void CheckStates(void);
