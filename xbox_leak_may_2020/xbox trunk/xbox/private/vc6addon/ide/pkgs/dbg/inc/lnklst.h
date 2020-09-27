BOOL ListUpdateObject(DWORD index, int listId, LPSTR string, int cch);
BOOL ListGetObject(DWORD index, int listId, LPSTR string);
BOOL ListDeleteObject(DWORD index, int listId);
BOOL ListInsertObject(DWORD index, int listId, LPSTR string, int cch);
BOOL ListAddObject(int listId, LPSTR string, int cch);
DWORD ListGetCount(int listId);
void FreeList(void);
BOOL EmptyList(int listId);
BOOL InitList(int listId);
BOOL ListCopy(int newId, int oldId, LPSTR buffer, UINT cch);


