// message.h

// Message Dispatcher
//
//-- Generic classes ----------------------------------------------------------
// TDynamicObj        - Base class for dynamically created objects.
//                      This class holds a class factory that can
//                      create objects of the derived type.
// TDispatcher        - Base class of a dispatcher which implements
//                      just one function: the MainDispatcher.
//                      this is the function that reads the incoming
//                      buffer, converts it to a class and calls
//						the appropriate routine to dispatch it.
// CStore             - Class that "wraps" a message buffer
//                      provides the functions that allow for insertion
//                      and extraction of data in a serial buffer.
// CLoadable          - Base class of the events serialized in a buffer
//                      This class is an absolute base class which
//                      declares the Load and Store functions, implemented
//                      by the derived classes
// -- specializaions ----------------------------------------------------------
//
// pfuncCreateMsg     - Function that creates a CMessageObj 
//
// CMsgDispatcher     - Derived from CDispatcher, this class implements
//                      one dispatch routine for each incoming message.
// CMessageObj        - Class derived from CDynamicObj
//                    - This class is the base class for all classes
//                    - instantiated from incoming messages

class CMsgDispatcher;
class CMessageObj;

typedef CMessageObj* (*pfuncCreateMsg) ();
class CMessageObj : public TDynamicObj <CMessageObj, int, pfuncCreateMsg>
	{public: virtual CMessageObj* Dispatch (CMsgDispatcher*) {return NULL;}};

struct CMsgRegistrar
{
	CMsgRegistrar (int k, pfuncCreateMsg f) 
		{CMessageObj::RegisterObject (k, f);}
};

// DERIVE_CLASS macro
//
// D - derived class
// E - event class, externally declared
// 
#define DECLARE_MSGHANDLER(D,E) class D : public E, public CMessageObj \
{\
	CMessageObj* Dispatch (CMsgDispatcher *d); \
	static CMsgRegistrar m_registrar; \
	static CMessageObj* CreateObject () {return new D ();} \
}

#define IMPLEMENT_MSGHANDLER(id,D,var) \
	CMsgRegistrar D::m_registrar (id, &D::CreateObject); \
	CMessageObj* D::Dispatch (CMsgDispatcher *d) {return d->Dispatch ((D*) this);} \
	CMessageObj* CMsgDispatcher::Dispatch (D* var)

// Here we derive a class from each incoming message type.
// Then define a member function of the Dispatcher for each
// derived class.

DECLARE_MSGHANDLER (CMsgTestingStarted, CMsg::TestingStarted);
DECLARE_MSGHANDLER (CMsgTestingEnded, CMsg::TestingEnded);
	
class CMsgDispatcher:public TDispatcher <CMessageObj, int>
{
public:
	CMessageObj* Dispatch (CMsgTestingStarted* cmd);
	CMessageObj* Dispatch (CMsgTestingEnded* cmd);
};


