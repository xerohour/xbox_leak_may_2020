#ifndef VECTOR_H
#define VECTOR_H

//#include "xkdctest.h"


// Hungarian: vct
template<class a> class Vector {
private:
    a *items;
    int numitems;
    int maxitems;

public:
    Vector()
    {
        items = NULL;
        numitems = 0;
        maxitems = 0;
    }
    
    ~Vector()
    {
        delete [] items;
    }

    void
    DeleteAll()
    {
        for (int i = 0; i < GetCount(); i++)
        {
            delete items[i];
        }
        delete [] items;
        items = NULL;
        numitems = 0;
        maxitems = 0;
    }

    int
    GetCount() const {return numitems;}

    a *GetItems() const {return items;}
    
    BOOL
    Add(const a &item)
    {
        if (numitems+1 > maxitems)
        {
            if (!DoubleArraySize())
            {
                return FALSE;
            }
        }
        items[numitems] = item;
        numitems++;
        return TRUE;
    }

    void
    Remove(int itemnumber)
    {
        items[itemnumber] = items[numitems-1];
        numitems--;
    }
    
    BOOL
    DoubleArraySize()
    {
        BOOL fStatus = FALSE;
        if (items == NULL) {
            maxitems = 4;
            items = new a[maxitems];
            if (items)
            {
                fStatus = TRUE;
            }
        }
        else
        {
            a *newitems = new a[maxitems*2];
            if (newitems)
            {
                for (int i=0; i<numitems; i++)
                    newitems[i] = items[i];
                delete [] items;
                items = newitems;
                maxitems *= 2;
                fStatus = TRUE;
            }
        }
        return fStatus;
    }

    a &operator[](int i) const {return items[i];}

    Vector &
    operator=(const Vector &v)
    {
        maxitems = v.maxitems;
        numitems = v.numitems;
        items = new a[maxitems];
        for (int i=0; i<numitems; i++)
            items[i] = v.items[i];
        return *this;
    }
};

#endif
