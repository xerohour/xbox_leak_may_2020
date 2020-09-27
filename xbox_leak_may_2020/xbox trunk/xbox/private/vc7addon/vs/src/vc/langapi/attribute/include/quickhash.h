
#if !defined(__QUICKHASH_H__)
#define __QUICKHASH_H__

/////////////////////////////////////////////////////////////////////////////
// QuickHash
//
// NOTE:
//  To use load or operator[], VALUE must have a ctor that takes an integer argument
//

template <class KEY, class VALUE>
struct QuickHash {
    static const int hashsize;
    struct List {
        List(const KEY& d = 0, const VALUE& i = 0) { next = 0; data = d; idx = i; }
        List* next;
        KEY data;
        VALUE idx;
    };
    int size;
    List** hashtable;
    lifetime_e life;
    List* next(int* x) {
        DASSERT(x != 0 && *x >= 0 && *x <= size);
        for (int i = *x; i < size; ++i) {
            if (hashtable[i] != 0) {
                break;
            }
        }
        if (i < size) {
            *x = i;
            return hashtable[i];
        }
        return 0;
    }
    List* first(int* x) {
        DASSERT(x != 0 && *x < size);
        *x = 0;
        return next(x);
    }
    List* next(int* x, List* p) {
        DASSERT(p != 0 && x != 0 && *x < size);
        if (p->next != 0) {
            return p->next;
        } else {
            ++*x;
            return next(x);
        }
    }
    void zero() {
        memset(hashtable, 0, size*sizeof(List*));
    }
    QuickHash(lifetime_e l = M_FILELIFE) {
        size = 0;
        hashtable = 0;
        life = l;
    }
    QuickHash(int sz, lifetime_e l = M_FILELIFE) {
        init(sz, l);
    }
    QuickHash(KEY* keys, int numkeys, int sz = hashsize, lifetime_e l = M_FILELIFE) {
        load(keys, numkeys);
    }
    void init(int sz = hashsize, lifetime_e l = M_FILELIFE) {
        size = sz;
        life = l;
#if VERSP_PARSEONLY
        hashtable = new List*[size];
#else
        hashtable = CXXNEW(List*, life)[size];
#endif
        memset(hashtable, 0, size*sizeof(List*));
    }
    void load(KEY* keys, int numkeys) {
        DASSERT(numkeys == 0 || keys != 0);
        if (size == 0) {
            init();
        }
        for (int i = 0; i < numkeys; ++i) {
            insert(keys[i], VALUE(i));
        }
    }
    void clear() {
#if (VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG) && !VERSP_PARSEONLY
    // don't delete anything inside compiler
#else
        for (int i = 0; i < size; ++i) {
            List* node = hashtable[i];
            while (node != 0) {
                List* next = node->next;
                delete node;
                node = next;
            }
        }
        delete [] hashtable;
#endif
    }
    VALUE& operator[] (const KEY& k) {
        VALUE v;
        List* node = lookup(k, key(k), &v);
        if (node == 0) {
            node = insert(k, v);
        }
        DASSERT(node != 0);
        return node->idx;
    }
            
    List* insert(const KEY& k, const VALUE& idx) {
        int the_key = key(k);
        List* node = lookup(k, the_key);
        if (node == 0) {
#if VERSP_PARSEONLY
            node = new List(k, idx);
#else
            node = CXXNEW(List, life)(k, idx);
#endif
            node->next = hashtable[the_key];
            hashtable[the_key] = node;
            return node;
        }
        node->idx = idx;
        return 0;
    }
    List* lookup(const KEY& k, int the_key, VALUE* pidx = 0) {
        DASSERT(the_key >= 0 && the_key < size);
        List* node = hashtable[the_key];
        while (node != 0) {
            DASSERT(node->data != 0);
            if (node->data == k) {
                if (pidx != 0) {
                    *pidx = node->idx;
                }
                return node;
            }
            node = node->next;
        }
        return 0;
    }
    bool_t lookup(const KEY& k, VALUE* pidx = 0) {
        return (lookup(k, key(k), pidx) != 0) ? true : false;
    }
    unsigned int key(const KEY& k) const {
        return key(k, size);
    }
    static unsigned int key(const KEY& k, int the_size) {
        unsigned int nHash = 0;
        char* the_key = (char*) k;
		for (int i = 0; i < sizeof(KEY); ++i) {
            nHash += (nHash << 5) + (int)(char) the_key++;
        }
        DASSERT(the_size > 0);
        return nHash % the_size;
    }
    ~QuickHash() {
        clear();
    }
};

template <class KEY, class VALUE>
const int QuickHash<KEY, VALUE>::hashsize = 511;

//
// We really need partial specialization
//
template <>
QuickHash<char*, int>::List* QuickHash<char*, int>::lookup(char* const& k, int the_key, int* pidx) {
    DASSERT(the_key >= 0 && the_key < size);
    List* node = hashtable[the_key];
    while (node != 0) {
        DASSERT(node->data != 0);
        if (strcmp(node->data, k) == 0) {
            if (pidx != 0) {
                *pidx = node->idx;
            }
            return node;
        }
        node = node->next;
    }
    return 0;
}

//
// We really need partial specialization
//
template <>
QuickHash<char*, void*>::List* QuickHash<char*, void*>::lookup(char* const& k, int the_key, void** pidx) {
    DASSERT(the_key >= 0 && the_key < size);
    List* node = hashtable[the_key];
    while (node != 0) {
        DASSERT(node->data != 0);
        if (strcmp(node->data, k) == 0) {
            if (pidx != 0) {
                *pidx = node->idx;
            }
            return node;
        }
        node = node->next;
    }
    return 0;
}

//
// We really need partial specialization
//
template <>
unsigned int QuickHash<char*, int>::key(char* const& k, int the_size) {
    unsigned int nHash = 0;
    char* the_key = (char*) k;
    while (*the_key) {
        nHash = (nHash << 3) + nHash + *the_key++;
    }
    DASSERT(the_size > 0);
    return nHash % the_size;
}

//
// We really need partial specialization
//
template <>
unsigned int QuickHash<char*, void*>::key(char* const& k, int the_size) {
    unsigned int nHash = 0;
    char* the_key = (char*) k;
    while (*the_key) {
        nHash = (nHash << 3) + nHash + *the_key++;
    }
    DASSERT(the_size > 0);
    return nHash % the_size;
}

//
// We really need partial specialization
//
template <>
unsigned int QuickHash<char*, FastList<char*>*>::key(char* const& k, int the_size) {
    unsigned int nHash = 0;
    char* the_key = (char*) k;
    while (*the_key) {
        nHash = (nHash << 3) + nHash + *the_key++;
    }
    DASSERT(the_size > 0);
    return nHash % the_size;
}

//
// We really need partial specialization
//
template <>
unsigned int QuickHash<int, char*>::key(const int& k, int the_size) {
    unsigned int nHash = k;    
    DASSERT(the_size > 0);
    return nHash % the_size;
}

#endif // __QUICKHASH_H__
