//-------------------------------------------------------------
// Access to the Reactor Controller
//
#include "ikctrl.h"


#define REACTORFLOAT 0x717d7d1f
#define REACTORPOS 0x7ac5cae4
#define REACTORP3 0x19080908
#define REACTORROT 0x2a8734eb
#define REACTORSCALE 0x13c4451c
#define REACTORFLOAT_CLASS_ID	Class_ID(REACTORFLOAT, 0x124c173b)
#define REACTORPOS_CLASS_ID		Class_ID(REACTORPOS, 0x904a56b3)
#define REACTORP3_CLASS_ID		Class_ID(REACTORP3, 0x3b617839)
#define REACTORROT_CLASS_ID		Class_ID(REACTORROT, 0x57f47da6)
#define REACTORSCALE_CLASS_ID	Class_ID(REACTORSCALE, 0x2ccb3388)

#define EDITABLE_SURF_CLASS_ID Class_ID(0x76a11646, 0x12a822fb)

// this is the class for all biped controllers except the root and the footsteps
#define BIPSLAVE_CONTROL_CLASS_ID Class_ID(0x9154,0)
// this is the class for the center of mass, biped root controller ("Bip01")
#define BIPBODY_CONTROL_CLASS_ID  Class_ID(0x9156,0) 



#define REACTORDLG_CLASS_ID	0x75a847f9

#define FLOAT_VAR		1
#define VECTOR_VAR		2
#define QUAT_VAR		3
#define SCALE_VAR		4

class IReactor : public Control {
	public:

		virtual BOOL	CreateReaction(TCHAR *buf=NULL, TimeValue t = GetCOREInterface()->GetTime())=0;
		virtual BOOL	DeleteReaction(int i=-1)=0;
		virtual int		getSelected()=0;
		virtual int		getVarCount()=0;
		virtual TCHAR*	getVarName(int i)=0;
		virtual void	setVarName(int i, TSTR name)=0;
		virtual void	setSelected(int i)=0;
		virtual void	setrType(int i)=0;
		virtual void	reactTo(Animatable* anim, TimeValue t = GetCOREInterface()->GetTime())=0;

		virtual BOOL	assignReactObj(INode* client, int subnum)=0;
		virtual BOOL	setInfluence(int num, float inf)=0;
		virtual	BOOL	setStrength(int num, float inf)=0;
		virtual BOOL	setFalloff(int num, float inf)=0;
		virtual BOOL	setState(int num, void *val, TimeValue t=NULL)=0;
		virtual void	isABiped(BOOL bip)=0;

		virtual float	getInfluence(int num)=0;
		virtual float	getStrength(int num)=0;
		virtual float	getFalloff(int num)=0;
		virtual int 	getrType()=0;
		virtual int 	getType()=0;
		virtual void*	getState(int num)=0;
		virtual BOOL	setReactionValue(int i=-1, void *val=NULL, TimeValue t=NULL)=0;
		virtual void*	getReactionValue(int i)=0;


	};

