// dynamic.h


// TFactoryMap
//
// This is the most minimal class possible to support
// to object factory for associating a key with a
// pointer to the appropriate create object function.
//
template <class K, class F>
class TFactoryMap
{
	TFactoryMap<K,F> *m_next;
	K m_k;
	F m_f;
public:
	TFactoryMap (K k = 0, F f = NULL):m_k(k),m_f(f),m_next(NULL) {}
	~TFactoryMap () {if (m_next) delete m_next;}
	TFactoryMap<K,F>* Lookup (K, F*);
	TFactoryMap<K,F>* Add (K, F);
};

template <class K, class F>
TFactoryMap<K,F>*
TFactoryMap<K,F> :: Add (K k, F f)
{
	TFactoryMap<K,F>* next = this;
	while (next->m_next)
		next = m_next;
	return next->m_next = new TFactoryMap<K,F> (k, f);
}

template <class K, class F>
TFactoryMap<K,F>*
TFactoryMap<K,F> :: Lookup (K k, F* f)
{
	TFactoryMap<K,F>* next = this;
	while (next->m_next)
	{
		if (next->m_k == k))
		{
			*f = m_f;
			return next;
		}
		next = m_next;
	}
	return NULL;
}

// TObjectFactory
//
// This class stores the list of all available classes and 
// supports the ability to create an object when given a key.
//
template <class T, class K, class F>
class TObjectFactory
{
	TFactoryMap <K,F> m_map;

public:
	TObjectFactory ():m_map() {} 
	~TObjectFactory () {}

	void RegisterObject (K k, F f) {m_map.Add (k, f);}
	T *CreateObject (K k) {F f; m_map.Lookup (k, &f); return (*f)();}
};

// TDynamicObj
//
// This absolute class must be the base
// of any object that is serialized.
//
template <class T, class K, class F>
class TDynamicObj
{
	static TObjectFactory<T,K,F> m_factory;

public:
	static void RegisterObject (K k, F f)
		{m_factory.RegisterObject (k, f);}

	static T* CreateObject (K k) 
		{return m_factory.CreateObject (k);}
};

// CObjectRegistrar
//
// This class exists only for the purpose of auto registration
// Instances of this class are declared statically in each test
// The constructor is run at start-up and register the object
// in with the object factory.  The object factory is a static member
// of the CDynamicObj class.
//
template <class T, class K, class F>
struct TRegistrar
{
	TRegistrar (K k, F f) {T::RegisterObject (k, f);}
};


// CLoadable
//
class CLoadable
{
	int Type;
public:
	virtual CSerial& Store (CSerial& s) {return s << Type;}
	virtual CSerial& Load (CSerial& s) {return s;} // Type already read
};


template <class T, class K>
class TDispatcher 
{
	T* MainDispatch (LPBYTE buffer, int length);
};

template <class T, class K>
T* TDispatcher <T, K> ::
MainDispatch (LPBYTE buffer, int length)
{
	CSerial next (buffer, length);	// Convert buffer in to store
	K key;							// Declare a key
	next >> key;					// Remove the key from the store
	T* obj = T::CreateObject (key); // Use the key to create an object
	if (!obj) throw key;			// throw - return ?? punt
	obj->Load (next);				// Load the object from the store
	T* resp = obj->Dispatch (this);	// Dispatch the appropriate command
	delete obj;
	return resp;
}



