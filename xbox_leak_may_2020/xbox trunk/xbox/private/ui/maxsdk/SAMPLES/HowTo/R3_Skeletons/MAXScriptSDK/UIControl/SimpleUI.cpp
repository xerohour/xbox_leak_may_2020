/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			MAX Script SDK Example - adding a new UI element to the language
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 1-4-99
 | 
\*===========================================================================*/

#include "MAXScrpt.h"

// Various MAX and MXS includes
#include "Numbers.h"
#include "MAXclses.h"
#include "Streams.h"
#include "MSTime.h"
#include "MAXObj.h"
#include "Parser.h"
#include "ColorVal.h"
#include "Rollouts.h"

#include "max.h"
#include "shellapi.h"
#include "commdlg.h"
#include "utilapi.h"
#include "bmmlib.h"


// Redefine so we can export our custom class
#ifdef ScripterExport
	#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )


// flag to say when we have registered our custom controls
static BOOL SUI_Reg = FALSE;



/*===========================================================================*\
 |	We will define the name of our new window class, as well as
 |	any custom event names and internal value names we intend to use
\*===========================================================================*/

// Window Classes
#define SIMPLE_WINDOWCLASS _T("SIMPLE_WINDOWCLASS")
//-----


// My MXS Event Handlers
#define n_clicked		Name::intern(_T("clicked"))
//-----


// My MXS Custom Names
#define n_inout			Name::intern(_T("inout"))
//-----



/*===========================================================================*\
 |	Class definition of our new RolloutControl
 |	visible_class macro registers this class as exportable to MAX Script
\*===========================================================================*/

class SimpleUI_Control;
visible_class (SimpleUI_Control)


class SimpleUI_Control : public RolloutControl
{
public:

	// custom parameters
	BOOL inout;
	

		// Constructor
		SimpleUI_Control(Value* name, Value* caption, Value** keyparms, int keyparm_count)
			: RolloutControl(name, caption, keyparms, keyparm_count)  
		{ 
			tag = class_tag(SimpleUI_Control);
		}

		static RolloutControl* create(Value* name, Value* caption, Value** keyparms, int keyparm_count)
							{ return new SimpleUI_Control (name, caption, keyparms, keyparm_count); }


		classof_methods (SimpleUI_Control, RolloutControl);


		// Garbage collection
		void		collect() { delete this; }

		// Print out the internal name of the control to MXS
		void		sprin1(CharStream* s) { s->printf(_T("SimpleUI_Control:%s"), name->to_string()); }

		// Add the control itself to a rollout window
		void		add_control(Rollout *ro, int id, HWND parent, HINSTANCE hInstance, int& current_y);

		// Return the window class name 
		LPCTSTR		get_control_class() { return SIMPLE_WINDOWCLASS; }

		// Top-level call for changing rollout layout. We don't process this.
		void		compute_layout(Rollout *ro, layout_data* pos) { }

		// Handle any messages coming in from the system
		BOOL		handle_message(Rollout *ro, UINT message, WPARAM wParam, LPARAM lParam);

		// 3 Functions to handle setting and getting custom values in this control
		Value*		get_property(Value** arg_list, int count);
		Value*		set_property(Value** arg_list, int count);
		void		set_enable();
};



/*===========================================================================*\
 |	Dialog handler for our custom window class
 |	Just paint a button type thing on the window, and capture clicks -> MXS
\*===========================================================================*/

LRESULT CALLBACK SUI_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	// Get back at our stored class pointer
	SimpleUI_Control *SUI = (SimpleUI_Control*)GetWindowLong(hWnd,GWL_USERDATA);

	switch ( message ) {
	

		// We catch a mouse down, fire it back to MXS as an 'event'
		case WM_LBUTTONDOWN:
			{
				init_thread_locals();
				one_value_local(arg);
				
				// Get the location of the click
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				// Put it into a MXS acceptable value
				vl.arg = new Point2Value((float)xPos,(float)yPos);

				// Call the event handler with 'n_clicked'
				// MXS will now receive the event and pass it to the dialog rollout
				SUI->call_event_handler(SUI->parent_rollout, n_clicked, &vl.arg, 1);
				pop_value_locals();
			}
			break;


		// Do some drawing onto the control's surface
		case WM_PAINT:
			{
				// begin paint cycle
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = BeginPaint( hWnd, &ps );

				// Do some simple drawing (a 3D frame)
				Rect rect;
				GetClientRect(hWnd,&rect);
				rect.left ++;rect.top ++;rect.right --;rect.bottom --;
				Rect3D(hdc,rect,SUI->inout);
				rect.left ++;rect.top ++;rect.right --;rect.bottom --;
				Rect3D(hdc,rect,SUI->inout);


				// end paint cycle
				EndPaint( hWnd, &ps );
				return 0;
			}

	}

	return TRUE;
}

visible_class_instance (SimpleUI_Control,_T("SimpleUI_Control"));



/*===========================================================================*\
 |	Implimentations of the class functions
\*===========================================================================*/

void
SimpleUI_Control::add_control(Rollout *ro, int id, HWND parent, HINSTANCE hInstance, int& current_y)
{
	HWND	simpleUI;
	TCHAR*	text = caption->eval()->to_string();

	// We'll hang onto these...
	parent_rollout = ro;
	control_ID = id;

	// compute bounding box, apply layout params
	layout_data pos;
	setup_layout(ro, &pos, current_y);
	
	// supply some layout defaults
	Value* Wtmp = control_param(width);
	if(Wtmp==&unsupplied) pos.width = 120;
	Value* Htmp = control_param(height);
	if(Htmp==&unsupplied) pos.height = 20;


	// Load in some values from the user, if they were given...
	Value* Btmp = control_param(inout);
	if(Btmp==&unsupplied) inout = FALSE;
	else
		inout = Btmp->to_bool();


	// Pass the info back to MXS to 
	// let it calculate final position
	process_layout_params(ro, &pos, current_y);


	// Have we already registered this wndclass?
	if(!SUI_Reg)				
	{
		WNDCLASS wc;

		// Just a simple UI frame control thing
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.hInstance     = hInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = SUI_WndProc;
		wc.lpszClassName = SIMPLE_WINDOWCLASS;
		RegisterClass(&wc);	
		SUI_Reg = TRUE;
	}


	simpleUI = CreateWindow(
							SIMPLE_WINDOWCLASS,
							text,
							WS_VISIBLE | WS_CHILD | WS_GROUP,
							pos.left, pos.top, pos.width, pos.height,    
							parent, (HMENU)id, hInstance, NULL);


	// Squirrel away the class pointer, as we need this in the dlg proc
	SetWindowLong(simpleUI, GWL_USERDATA, (DWORD)this);	
}



BOOL
SimpleUI_Control::handle_message(Rollout *ro, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Deligate, Deligate...
	// All messages processing is done in our dlg proc
	if (message == WM_PAINT)
	{
   		SendMessage(GetDlgItem(ro->page, control_ID), WM_PAINT, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}



Value*
SimpleUI_Control::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];

	if (prop == n_inout)
		return inout? &true_value : &false_value;
	else
		return RolloutControl::get_property(arg_list, count);
}



Value*
SimpleUI_Control::set_property(Value** arg_list, int count)
{
	Value* val = arg_list[0];
	Value* prop = arg_list[1];

	if (prop == n_inout)
	{
		if (parent_rollout != NULL && parent_rollout->page != NULL)
		{
			inout = val->to_bool();

			HWND item = (GetDlgItem(parent_rollout->page, control_ID));

			// rebuild the dialog item
			// have to do this to update in loops on a page
			InvalidateRect(item,NULL,FALSE);
	   		SendMessage(GetDlgItem(parent_rollout->page, control_ID), WM_PAINT, 0, 0);
		}
		return val;
	}
	else return RolloutControl::set_property(arg_list, count);
}

void
SimpleUI_Control::set_enable()
{
	// Enable/disable the window item and refresh 
	HWND ctrl = GetDlgItem(parent_rollout->page, control_ID);
	if (ctrl)
	{
		EnableWindow(ctrl, enabled);
		InvalidateRect(ctrl,NULL,FALSE);
   		SendMessage(GetDlgItem(parent_rollout->page, control_ID), WM_PAINT, 0, 0);
	}

}



/*===========================================================================*\
 |	MAXScript Plugin Initialization
 |	Ask MAX Script to regester us as a new UI insertable control
\*===========================================================================*/

__declspec( dllexport ) void
LibInit() { 
	install_rollout_control(Name::intern("SimpleUI_Control"), SimpleUI_Control::create);
}
