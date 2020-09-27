#ifndef _PARSERTOOLS_H_
#define _PARSERTOOLS_H_

typedef void (*HASHOBJ_FUNC)(void*);

/*++

Class Description:



Arguments:



Return Value:



Notes:



--*/
template <class T> class HashEntry
    {
    public:
        char *key;
        T obj;
        HashEntry <T> *next;

    public:
        HashEntry();
        HashEntry(char *k, T o);
        ~HashEntry();
    };

/*++

Class Description:



Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE> class HashTable
    {
    public:
        HashTable();
        ~HashTable();

        HASHOBJ_FUNC clean;
        size_t m_size;

    public:
        HashEntry<T> *table[SIZE];
    
        BOOL Get(char *key, T& out);
        BOOL Add(char *key, T o);
        BOOL ForAll(HASHOBJ_FUNC fn);
        void SetCleanupFunct(HASHOBJ_FUNC c) { clean = c; }
    };

class Variable
    {
    public:
        char *value;
        size_t nameLen;  // size of the buffer
        size_t valueLen; // size of the buffer

    public:
        Variable();
        Variable(char *n, char *v);
        ~Variable();
    };

extern void DestroyVariable(void *p);

/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T>
HashEntry<T>::HashEntry()
    {
    clean = NULL;
    key = NULL;
    next = NULL;
    }


/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T>
HashEntry<T>::HashEntry(char *k, T o)
    {
    key = new char[strlen(k)+1];
    strcpy(key, k);
    obj = o;
    next = NULL;
    }

/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T>
HashEntry<T>::~HashEntry()
    {
    if(key) delete[] key;
    if(next) delete next;
    }

/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE>
HashTable<T, SIZE>::HashTable()
    {
    m_size = SIZE;
    clean = NULL;
    for(size_t i=0; i<SIZE; i++)
        {
        table[i] = NULL;
        }
    }


/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE>
HashTable<T, SIZE>::~HashTable()
    {
    if(clean) ForAll(clean);
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i]) delete table[i];
        }
    }

/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::Get(char *key, T& out)
    {
    if(!key) return FALSE;

    size_t i = (key[0] + key[1]) % SIZE;

    if(!table[i]) return FALSE;

    for(HashEntry<T> *h = table[i]; h; h=h->next)
        {
        if(strcmp(h->key, key) == 0)
            {
            out = h->obj;
            return TRUE;
            }
        }

    return FALSE;
    }

/*++

Routine Description:

    Add the key and the associated object. Add the new HashEntry at the end 
    of the list to give precedence to items added first.

Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::Add(char *key, T o)
    {
    if(!key) return FALSE;
    size_t i = (key[0] + key[1]) % SIZE;
    
    if(!table[i])
        {
        table[i] = new HashEntry<T>(key, o);
        }
    else
        {
        HashEntry<T> *h = table[i];
        do
            {
            // replace entry with the same key
            if(strcmp(h->key, key) == 0)
                {
                if(clean) clean(&(h->obj));
                h->obj = o;
                return TRUE;
                }
            if(h->next) h=h->next;
            } while(h->next);

        // otherwise add it to the end
        h->next = new HashEntry<T>(key, o);
        }

    return TRUE;
    }

/*++

Routine Description:



Arguments:



Return Value:



Notes:



--*/
template <class T, size_t SIZE>
BOOL HashTable<T, SIZE>::ForAll(HASHOBJ_FUNC fn)
    {
    for(size_t i=0; i<SIZE; i++)
        {
        if(table[i]) 
            {
            for(HashEntry<T> *h = table[i]; h; h=h->next)
                {
                fn(&(h->obj));
                }
            }
        }
    return TRUE;
    }

#endif // _PARSERTOOLS_H_