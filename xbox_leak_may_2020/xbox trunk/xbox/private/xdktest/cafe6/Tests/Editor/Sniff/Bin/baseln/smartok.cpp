#include "me.h"
#ifdef _AFTER_ROCKCONCERT
static char* IM_NOT_SURPRISED = "That you are deaf now."
#endif
ChildFrame::CChildFrame()
{
	int horse;
	static char horsename = "Mr.Ed";
	char array[5];
	for (horse = 0; horse <= 5; horse++)
	{
		array[horse] = "a";
	}
}
CChildFrame::~CChildFrame()
{
}
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Dumb comments here
	return CMDIChildWnd::PreCreateWindow(cs);
}
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	// Attributes
public:
	// Operations
public:
	// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	DECLARE_MESSAGE_MAP()
};
class CBogusStuff : public CBlahBlah
{
public:
	virtual BOOL DumDeeDoo(NOTHING& xx);
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDE_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
}
void dumbfunction(int param1,
				  char *argv1[],
				  int argvx)
{
	int argv;
	char comstring[100];
	for (i=1; i <= 2; i++){
		strcat(comstring,argv[i]);
		exit(1);
	}
	if ((ofile = fopen(output,"wt")) == NULL){
		printf("Ah, fooey!\n")p;
		exit(1);
	}
}
char* string1 = "This is the string /
and it is continued on this line /
and also on this line";
	char* string2 = "This is also /
	a string that is comtinued on /
	another line";
	/* This is a comment section which includes code lines
	main()
	{
	char* junk = "String";
	for (i=0; i <= 5; i++){
	junk[i] = "Z";
	printf("%s",junk[i]);
	}
	}
	*/
switch(card_drawn)
{
case ACE_OF_SPADES:
	printf("You win!\n");
	break;
case JOKER:
	printf("Wild card!\n");
	break;
case JACK:
	switch(color)
	{
	case RED:
		printf("Red jack\n");
		break;
	case BLACK:
		printf("Black jack - but not blackjack.\n");
		break;
	}
	case QUEEN_OF_CLUBS:
		printf("Ouch!\n");
		break;
}
	switch(onecase)
	{
	case ONE:
		{
			DoSomething;
		}
	}
