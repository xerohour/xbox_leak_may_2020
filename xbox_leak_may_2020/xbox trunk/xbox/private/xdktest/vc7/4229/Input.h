#ifndef __INPUT_H
#define __INPUT_H

#define SANE_CONTROLLER

// MAXCONTROLLERS is defined by os.h and is a decent signal
// of whether or not it has been included.
#ifndef MAXCONTROLLERS
struct OSContPad;
#endif

#define MAX_CONTROLLERS    4
#define ACTIVE_CONTROLLERS 4

//-----------------------------------------------------------------------------
// input structure
//-----------------------------------------------------------------------------

#define CHEAT_BUFFER_SIZE (1<<5)
#define CHEAT_BUFFER_MASK (CHEAT_BUFFER_SIZE-1)
	
struct Frame;

#define FIRST_REPEAT      0.6f
#define SUBSEQUENT_REPEAT 0.2f

struct InputComponent
{
	float m_repeatTimer;
	float m_repeatInterval;
	bool  m_repeat;

	bool  m_debounced,m_debouncedNeg;

	bool  m_boolValue,m_boolValueNeg;
	int   m_intValue;
	float m_floatValue;

	int   m_digitalMin,m_digitalOrigin,m_digitalMax;

	static float m_GentleDeadSpace(float analog);

	void m_InsertDeadSpace(void) 
	{ 
		m_floatValue = m_GentleDeadSpace(m_floatValue); 
	}

	void m_ApplyDebounce(void);

#define PRESS_THRESHOLD   0.6f
#define RELEASE_THRESHOLD 0.3f

	void m_Set(int v);

	void m_Debounce(void)
	{
		if(m_boolValue)
		{
			m_debounced  = true;

			m_intValue   = 0;
			m_floatValue = 0.0f;
			m_boolValue  = false;
		}
	}
	inline void m_DebouncePos(void) { m_Debounce(); }
	void m_DebounceNeg(void)
	{
		if(m_boolValueNeg)
		{
			m_debouncedNeg = true;

			m_intValue     = 0;
			m_floatValue   = 0.0f;
			m_boolValueNeg = false;
		}
	}

	void m_SetRange(int min,int origin,int max)
	{
		m_digitalMin    = min;
		m_digitalOrigin = origin;
		m_digitalMax    = max;
	}

	void m_Flip(void)
	{
		m_floatValue   = -m_floatValue;
		m_intValue     = -m_intValue;

		bool tmp       = m_boolValue;
		m_boolValue    = m_boolValueNeg;
		m_boolValueNeg = tmp;
	}

	inline void  m_RepeatOn(void)  { m_repeat = true; }
	inline void  m_RepeatOff(void) { m_repeat = false; }

	inline int   m_AsInt(void)      { return(m_intValue); }
	inline float m_AsFloat(void)    { return(m_floatValue); }

	inline bool  m_Pressed(void)    { return(m_boolValue); }
	inline bool  m_PressedPos(void) { return(m_boolValue); }
	inline bool  m_PressedNeg(void) { return(m_boolValueNeg); }

	bool         m_ReallyPressed(void);

	inline bool  m_PressedAndDebounced(void)    { if(m_Pressed())    { m_Debounce();    return(true); } return(false); }
	inline bool  m_PressedPosAndDebounced(void) { if(m_PressedPos()) { m_DebouncePos(); return(true); } return(false); }
	inline bool  m_PressedNegAndDebounced(void) { if(m_PressedNeg()) { m_DebounceNeg(); return(true); } return(false); }

	InputComponent()
	{
		m_repeat       = false;
		m_intValue     = 0;
		m_floatValue   = 0.0f;
		m_boolValue    = false;
		m_boolValueNeg = false;
	}
};

struct InputAnalogButton : public InputComponent
{
	void m_Set(bool v)
	{
		if(m_boolValue = v)
		{
			m_intValue   = 255;
			m_floatValue = 1.0f;
		}
		else
		{
			m_intValue   = 0;
			m_floatValue = 0.0f;
		}
	}

	inline void m_Set(int v) { InputComponent::m_Set(v); }

	void m_Set(unsigned long flags,unsigned long mask)
	{
		if(flags & mask)
		{
			m_boolValue  = true;
			m_intValue   = 255;
			m_floatValue = 1.0f;
		}
		else
		{
			m_boolValue  = false;
			m_intValue   = 0;
			m_floatValue = 0.0f;
		}
	}

	void m_From(InputAnalogButton &b)
	{
		if(b.m_digitalMax    == m_digitalMax   &&
		   b.m_digitalOrigin == m_digitalOrigin)
		{
			m_intValue   = b.m_intValue;
			m_floatValue = b.m_floatValue;
			m_boolValue  = b.m_boolValue;
		}
		else
		{
			InputComponent::m_Set((int)(b.m_floatValue*(m_digitalMax-m_digitalOrigin)));
		}
	}

	InputAnalogButton() : InputComponent()
	{
		m_SetRange(0,0,255);
	}
};

struct InputAnalogAxis : public InputComponent
{
	void m_From(InputAnalogButton &min,InputAnalogButton &max)
	{
		m_Set((int)((max.m_floatValue - min.m_floatValue)*(m_digitalMax-m_digitalOrigin)));
	}

	void m_From(InputAnalogAxis &a)
	{
		if(a.m_digitalMax    == m_digitalMax   &&
		   a.m_digitalMin    == m_digitalMin   &&
		   a.m_digitalOrigin == m_digitalOrigin)
		{
			m_intValue   = a.m_intValue;
			m_floatValue = a.m_floatValue;
			m_boolValue  = a.m_boolValue;
		}
		else
		{
			InputComponent::m_Set((int)(a.m_floatValue*(m_digitalMax-m_digitalOrigin)));
		}
	}

	void m_SetRange(int min,int origin,int max)
	{
		if(min!=-max)
		{
			printf("min!=-max in analog axis\n");
			min = -max;
		}
		InputComponent::m_SetRange(min,origin,max);
	}

	InputAnalogAxis() : InputComponent()
	{
#ifdef SANE_CONTROLLER
		m_SetRange(-105,0,105);
#else
		m_SetRange(-70,0,70);
#endif
	}
};

struct InputAnalogPad
{
	// different from stick because two buttons CAN be pressed at once
	InputAnalogButton m_right;
	InputAnalogButton m_down;
	InputAnalogButton m_left;
	InputAnalogButton m_up;

	void m_From(InputAnalogPad &p)
	{ 
		m_right.m_From(p.m_right); 
		m_down.m_From(p.m_down); 
		m_left.m_From(p.m_left); 
		m_up.m_From(p.m_up); 
	}

	void m_InsertDeadSpace(void) 
	{ 
		m_right.m_InsertDeadSpace();
		m_down.m_InsertDeadSpace();
		m_left.m_InsertDeadSpace();
		m_up.m_InsertDeadSpace();
	}

	// consistent with InputAnalogStick
	inline bool m_PressedUp(void)    { return(m_up.m_Pressed()); }
	inline bool m_PressedDown(void)  { return(m_down.m_Pressed()); }
	inline bool m_PressedRight(void) { return(m_right.m_Pressed()); }
	inline bool m_PressedLeft(void)  { return(m_left.m_Pressed()); }

	void m_ApplyDebounce(void)
	{
		m_right.m_ApplyDebounce();
		m_down.m_ApplyDebounce();
		m_left.m_ApplyDebounce();
		m_up.m_ApplyDebounce();
	}
};

struct InputAnalogStick
{
	// ie input->m_leftStick.m_x.m_asFloat()
	InputAnalogAxis m_x;
	InputAnalogAxis m_y;

	void m_InsertDeadSpace(void) 
	{ 
		m_x.m_InsertDeadSpace();
		m_y.m_InsertDeadSpace();
	}

	void m_From(InputAnalogPad &p) { m_x.m_From(p.m_left,p.m_right); m_y.m_From(p.m_down,p.m_up); }
	void m_From(InputAnalogStick &s) { m_x.m_From(s.m_x); m_y.m_From(s.m_y); }

	// consistent with InputAnalogPad
	inline bool m_PressedUp(void)    { return(m_y.m_PressedPos()); }
	inline bool m_PressedDown(void)  { return(m_y.m_PressedNeg()); }
	inline bool m_PressedRight(void) { return(m_x.m_PressedPos()); }
	inline bool m_PressedLeft(void)  { return(m_x.m_PressedNeg()); }

	void m_ApplyDebounce(void)
	{
		m_x.m_ApplyDebounce();
		m_y.m_ApplyDebounce();
	}
};

struct Input
{
	int m_port;

	// make sure all of these are present in Input::ApplyDebounce()

	// real
	InputAnalogPad    m_pad;

	InputAnalogButton m_a;
	InputAnalogButton m_b;

	InputAnalogButton m_x;
	InputAnalogButton m_y;

	InputAnalogButton m_white;
	InputAnalogButton m_black;

	InputAnalogButton m_lThumb;
	InputAnalogButton m_rThumb;

	InputAnalogButton m_start;
	InputAnalogButton m_back;

	InputAnalogButton m_leftTrigger;
	InputAnalogButton m_rightTrigger;

	InputAnalogStick  m_leftStick;
	InputAnalogStick  m_rightStick;

	// virtual
	InputAnalogButton m_accelerator;
	InputAnalogButton m_reverse;
	InputAnalogButton m_brake;
	InputAnalogButton m_eBrake;

	InputAnalogButton m_gearUp;
	InputAnalogButton m_gearDown;

	InputAnalogPad    m_view;

	InputAnalogAxis   m_steering;

	// for interface
	InputAnalogButton m_left;   // combined from all three sticks
	InputAnalogButton m_right;
	InputAnalogButton m_up;
	InputAnalogButton m_down;

	InputAnalogButton m_ok;     // used for selecting items, clicking ok, etc
	InputAnalogButton m_cancel;   // back out, cancel

	// controller types (for translation from physical to virtual buttons)
	enum Type
	{
		TYPE_A=0
	};
	Type   m_type;
	bool   m_missing;
	
	u16    m_cheatBuffer[CHEAT_BUFFER_SIZE];
	int    m_cheatPos;
	
//	int port;
//	OSContPad *controller;
	
	Frame *m_forFrame;
	
	void   m_CopyBasicInfo(Input *in);
	void   m_UpdateCheats(void);
	void   m_Get(void);
    void   m_Debounce(int p);
    void   m_ApplyDebounce(void);
    void   m_UsePadForAnalog(void);
	void   m_InsertDeadSpace(void);
	void   m_Init(int port);

	// these two are the same but it's more readable to have both
//	inline int Pressed(register int mask)    { return( pad&(unsigned long)mask       ); }
//	inline int AnyPressed(register int mask) { return( pad&(unsigned long)mask       ); }
	// only valid if all are pressed
//	inline int AllPressed(register int mask) { return((pad&(unsigned long)mask)==mask); }
	// these also debounce for you if the return is nonzero
//	int PressedAndDebounced(register int mask);
//	int AllPressedAndDebounced(register int mask);
	// add in buttons
//	inline void Press(register int mask) { pad|=(unsigned long)mask; }
};

#endif
