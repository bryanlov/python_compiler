/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* APYC Runtime Library Headers */

/* Authors:  YOUR NAMES HERE */

/* The functions here are suggestions. Feel free to replace at will. 
 * Though this file is C++, we have avoided overloading and inheritance, so
 * that changing to C is relatively easy. */

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

                /* STUBS FOR BUILTIN TYPES */
class Obj {
public:
	Obj ();
	bool value = false;

	virtual string getType() {
		return "obj";
	}
	virtual string getContents() {
		return "";
	}

	// virtual vector<Int> getContents() {
	// 	return contents;
	// }
};
class Obj1 : public Obj{
public:
	Obj1 ();
	bool value = false;

	virtual string getType() {
		return "obj";
	}
	virtual string getContents() {
		return "";
	}

	// virtual vector<Int> getContents() {
	// 	return contents;
	// }
};
typedef Obj* Any;
typedef Obj1* Any1;
class Str_Obj : public Obj1 {
public:
	Str_Obj (string val);
	string contents; 
	bool value = true;
	string type = "Str";

	string getType() override {
		return type;
	}
	string getContents() override {
		return contents;
	}
};
class Int_Obj : public Obj1 {
public:
	Int_Obj (int val);
	int contents;
	bool value = true;
	string type = "Int";

	string getType() override {
		return type;
	}
};

typedef Int_Obj* Int;

class Bool_Obj : public Obj1 {
public:
	Bool_Obj (string val);
	string contents;
	bool value = false;
	string type = "Bool";

	string getType() override {
		return type;
	}
	string getContents() override {
		return contents;
	}

};
class Range_Obj : public Obj1 {
public:
	Range_Obj (Int low, Int high);
	Range_Obj ();
	vector<Int> contents;
	Int low;
	Int high;
	bool value = true;
	string type = "Range";

	string getType() override {
		return type;
	}
};
class List_Obj : public Obj1 {
public:
	List_Obj (vector<Any> val);
	List_Obj ();
	vector<Any> contents;
	bool value = true;
	string type = "List";

	string getType() override {
		return type;
	}
};
class Dict_Obj : public Obj1 {
public:
	Dict_Obj (unordered_map<Any, Any> val, vector<Any> val1);
	unordered_map<Any, Any> contents;
	vector<Any> order;
	bool value = true;
	string type = "Dict";

	string getType() override {
		return type;
	}

};
class Tuple0_Obj : public Obj1 {
public:
	Tuple0_Obj();
	string type = "Tuple0";

	string getType() override {
		return type;
	}
};
class Tuple1_Obj : public Obj1 {
public:
	Tuple1_Obj(Any val);
	Any contents;
	string type = "Tuple1";

	string getType() override {
		return type;
	}
};
class Tuple2_Obj : public Obj1 {
public:
	Tuple2_Obj(Any val, Any val1);
	Any contents;
	Any contents1;
	string type = "Tuple2";

	string getType() override {
		return type;
	}
};
class Tuple3_Obj : public Obj1 {
public:
	Tuple3_Obj(Any val, Any val1, Any val2);
	Any contents;
	Any contents1;
	Any contents2;
	string type = "Tuple2";

	string getType() override {
		return type;
	}
};

class None_Obj : public Obj1 {
public:
	None_Obj();
	string type = "None";

	string getType() override {
		return type;
	}
};

typedef Str_Obj* Str;
typedef Bool_Obj* Bool;
typedef Range_Obj* Range;
typedef List_Obj* List;
typedef Dict_Obj* Dict;
typedef Tuple0_Obj* Tuple0;
typedef Tuple1_Obj* Tuple1;
typedef Tuple2_Obj* Tuple2;
typedef Tuple3_Obj* Tuple3;
typedef None_Obj* None;



                /* NATIVE METHODS */
/* Function names are those used in 'native' statements. */

/*  Type Bool */

extern Bool __truth__ (Any x);
extern Bool __not__ (Any x);

/** The values of True and False. */
extern Bool __true__, __false__;

/*  Type Range */

extern Range __xrange__ (Int low, Int high);
extern Int __len__range__ (Range r);

/*  Type Int */

extern Int __add__int__ (Int x, Int y);
extern Int __sub__int__ (Int x, Int y);
extern Int __mul__int__ (Int x, Int y);
extern Int __floordiv__int__ (Int x, Int y);
extern Int __mod__int__ (Int x, Int y);
extern Int __pow__int__ (Int x, Int y);
extern Int __neg__int__ (Int x);
extern Int __pos__int__ (Int x);
extern Bool __lt__int__ (Int x, Int y);
extern Bool __gt__int__ (Int x, Int y);
extern Bool __le__int__ (Int x, Int y);
extern Bool __ge__int__ (Int x, Int y);
extern Bool __eq__int__ (Int x, Int y);
extern Bool __ne__int__ (Int x, Int y);

/*  toint is the replacement for 'Int' as a function in our dialect. */
extern Int __toint__str__ (Str x);

/*  Type Str */

extern Str __add__str__ (Str x, Str y);
extern Str __lmul__str__ (Str x, Int y);
extern Str __rmul__str__ (Int x, Str y);
extern Bool __lt__str__ (Str x, Str y);
extern Bool __gt__str__ (Str x, Str y);
extern Bool __le__str__ (Str x, Str y);
extern Bool __ge__str__ (Str x, Str y);
extern Bool __eq__str__ (Str x, Str y);
extern Bool __ne__str__ (Str x, Str y);
extern Str __getitem__str__ (Str S, Int k);
extern Str __getslice__str__ (Str S, Int L, Int U);
extern Int __len__str__ (Any S);

/* toStr is the replacement for 'Str' as a function in our dialect. */
extern Str __toStr__ (Any x);

/*  Type List */

extern Any __getitem__list__ (List S, Int k);
extern List __getslice__list__ (List S, Int L, Int U);
extern Any __setitem__list__ (List S, Int k, Any val);
extern List __setslice__list__ (List S, Int a, Int b, List val);
extern Int __len__list__ (Any S);

/*  Replaces sys.argv */
extern List __argv__ ();

/*  Type File */

extern Str __readline__ ();
extern Str __read__ ();

/*  Type Dict */

extern Any __getitem__dict__int__ (Dict D, Int x);
extern Any __getitem__dict__bool__ (Dict D, Bool x);
extern Any __getitem__dict__str__ (Dict D, Str x);
extern Any __setitem__dict__int__ (Dict S, Int k, Any val);
extern Any __setitem__dict__bool__ (Dict S, Bool k, Any val);
extern Any __setitem__dict__str__ (Dict S, Str k, Any val);
extern Int __len__dict__int__ (Dict D);
extern Int __len__dict__bool__ (Dict D);
extern Int __len__dict__str__ (Dict D);

/*  Backwards from regular Python */

extern Bool __contains__dict__int__ (Int x, Dict D);
extern Bool __contains__dict__bool__ (Bool x, Dict D);
extern Bool __contains__dict__str__ (Str x, Dict D);

/*  Backwards from regular Python */

extern Bool __notcontains__dict__int__ (Int x, Dict D);
extern Bool __notcontains__dict__bool__ (Bool x, Dict D);
extern Bool __notcontains__dict__str__ (Str x, Dict D);

/*  General */

extern Int __and__ (Int x, Int y);
extern Str __and__ (Str x, Str y); 
extern Bool __and__ (Bool x, Bool y); 
extern List __and__ (List x, List y);
extern Int __or__ (Int x, Int y);
extern Str __or__ (Str x, Str y); 
extern Bool __or__ (Bool x, Bool y); 
extern List __or__ (List x, List y);
extern Bool __is__ (Any x, Any y);
extern Bool __isnot__ (Any x, Any y);

/*  Extra Credit */
extern Any __gc__ ();


                /* OTHER RUNTIME SUPPORT (SUGGESTIONS) */
extern void* print (Range r);
extern void* print (Any a);
extern void* print (List l);
extern void* print (Dict d);
extern void* print (Tuple0 i);
extern void* print (Tuple1 i);
extern void* print (Tuple2 i);
extern void* print (Tuple3 i);
extern void* print (None n);
extern void* print (Int i);
extern void* print (Str s);
extern void* print (Bool b);
extern void* print (int i);
extern void* print (string s);
extern void* print (bool b);

extern void* leftCompareHelper (Bool b, Int i);

extern void* leftCompareHelper (Bool b, Str s);

extern void* assignHelper (void* v, Int i);
extern Str assignHelper (void* v, Str s);
extern void* assignHelper (void* v, Bool b);
extern void* assignHelper (void* v, List l);
extern bool condtionalHelper (Bool b);
extern bool condtionalHelper (Any b);

/** For implementing list displays.  __createList__(n, a1, ..., an) creates
 *  the list [a1, a2, ..., an]. */
// extern List __createList__ (initializer_list<Any> ls);
// extern void addToList(List l, T t);

template <typename List, typename Any>
void addToList(List l, Any t) 
{
    l->contents.push_back(t);

}

template <typename... Args>
void addToList(List l, Any t, Args... args) 
{
    l->contents.push_back(t);
    addToList(l, args...);
}


template<typename... Args>
List __createList__ (Args... args) {
	List res = new List_Obj();
	addToList(res, args...);
	return res;
}

template<typename... Args>
List __createList__ () {
	List res = new List_Obj();
	return res;
}



/** For implenting dictionary displays. 
 *  __createDict__(n, k1, v1, k2, v2, ..., kn, vn) creates the dictionary
 *      { k1 : v1, k2 : v2, ..., kn : vn  }
 */
extern Dict __createDict__ (int n, ...);

/** Allocate an object with space for N instance variables. */
extern Any __createObj__ (int n);

#endif
