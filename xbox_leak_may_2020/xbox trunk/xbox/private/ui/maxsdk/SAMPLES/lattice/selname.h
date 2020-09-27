#ifndef __SELNAME_H__ 

#define __SELNAME_H__

#include "Max.h"

class NVertSelSetList {
	public:
		Tab<ShapeVSel*> sets;

		~NVertSelSetList();
		ShapeVSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapeVSel &nset);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NVertSelSetList& operator=(NVertSelSetList& from);
		void DeleteSetElements(ShapeVSel &set,int m=1);
	};

class NSegSelSetList {
	public:
		Tab<ShapeSSel*> sets;

		~NSegSelSetList();
		ShapeSSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapeSSel &nset);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NSegSelSetList& operator=(NSegSelSetList& from);
		void DeleteSetElements(ShapeSSel &set,int m=1);
	};

class NPolySelSetList {
	public:
		Tab<ShapePSel*> sets;

		~NPolySelSetList();
		ShapePSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapePSel &nset);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NPolySelSetList& operator=(NPolySelSetList& from);
		void DeleteSetElements(ShapePSel &set,int m=1);
	};
#endif // __SELNAME_H__
