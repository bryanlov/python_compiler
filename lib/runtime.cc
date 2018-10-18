/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* APYC Runtime Library */

/* Authors:  YOUR NAMES HERE */

// OTHERS?

#include <vector>

#include <math.h> 
#include <string>
#include "runtime.h"

using namespace std;

// FILL IN
Obj::Obj () {}
Obj1::Obj1 () {}
Str_Obj::Str_Obj (string val) : contents (val) {}
Int_Obj::Int_Obj (int val) : contents (val) {}
Bool_Obj::Bool_Obj (string val) : contents (val) {
	if (val == "True") {
		value = true;
	}
}
List_Obj::List_Obj (vector<Any> val) : contents (val) {}
List_Obj::List_Obj () {}
Range_Obj::Range_Obj (Int l, Int h) : low (l), high(h) {
	int start = l->contents;
	int end = h->contents;
	for (int i = start; i < end; i++) {
		contents.push_back(new Int_Obj(i));
	}
}
Range_Obj::Range_Obj () {}
Dict_Obj::Dict_Obj (unordered_map<Any, Any> val, vector<Any> val1) : contents (val), order (val1) {}
Tuple0_Obj::Tuple0_Obj () {}
Tuple1_Obj::Tuple1_Obj (Any val) : contents (val) {}
Tuple2_Obj::Tuple2_Obj (Any val, Any val1) : contents (val), contents1 (val1) {}
Tuple3_Obj::Tuple3_Obj (Any val, Any val1, Any val2) : contents (val), contents1 (val1), contents2 (val2) {}
None_Obj::None_Obj () {}

Any __andHelper__ (Any x) {
	string type = x->getType();
	bool val = false;
	if (type == "Int") {
		val = ((Int)x)->contents == 0 ? false : true;
	} else if (type == "Str") {
		x->value = ((Str)x)->contents.empty() ? false : true;
	} else if (type == "Bool") {
		val = ((Bool)x)->contents == "False" ? false : true;
	} else if (type == "List") {
		val = ((List)x)->contents.empty() ? false : true;
	} else if (type == "Dict") {
		val = ((Dict)x)->order.empty() ? false : true;
	} else if (type == "Tuple0") {
		val = false;
	} else if (type == "Tuple1") {
		val = true;
	} else if (type == "Tuple2") {
		val = true;
	} else if (type == "Tuple3") {
		val = true;
	} else if (type == "None") {
		val = false;
	}
	return x;
}

Bool __truth__ (Any x) {
	string type = x->getType();
	bool val = false;
	if (type == "Int") {
		val = ((Int)x)->contents == 0 ? false : true;
	} else if (type == "Str") {
		val = ((Str)x)->contents.empty() ? false : true;
	} else if (type == "Bool") {
		val = ((Bool)x)->contents == "False" ? false : true;
	} else if (type == "List") {
		val = ((List)x)->contents.empty() ? false : true;
	} else if (type == "Dict") {
		val = ((Dict)x)->order.empty() ? false : true;
	} else if (type == "Tuple0") {
		val = false;
	} else if (type == "Tuple1") {
		val = true;
	} else if (type == "Tuple2") {
		val = true;
	} else if (type == "Tuple3") {
		val = true;
	} else if (type == "None") {
		val = false;
	}
	return !val ? new Bool_Obj("False") : new Bool_Obj("True");
}

Bool __not__ (Any x) {
	string type = x->getType();
	bool val = false;
	if (type == "Int") {
		val = ((Int)x)->contents == 0 ? false : true;
	} else if (type == "Str") {
		val = ((Str)x)->contents.empty() ? false : true;
	} else if (type == "Bool") {
		val = ((Bool)x)->contents == "False" ? false : true;
	} else if (type == "List") {
		val = ((List)x)->contents.empty() ? false : true;
	} else if (type == "Dict") {
		val = ((Dict)x)->order.empty() ? false : true;
	} else if (type == "Tuple0") {
		val = false;
	} else if (type == "Tuple1") {
		val = true;
	} else if (type == "Tuple2") {
		val = true;
	} else if (type == "Tuple3") {
		val = true;
	} else if (type == "None") {
		val = false;
	}
	return val ? new Bool_Obj("False") : new Bool_Obj("True");
}

Range __xrange__ (Int low, Int high) {
	return new Range_Obj(low, high);
}


Int __add__int__ (Int x, Int y){
	return new Int_Obj(x->contents + y->contents);
}

Int __sub__int__ (Int x, Int y){
	return new Int_Obj(x->contents - y->contents);
}

Int __mul__int__ (Int x, Int y){
	return new Int_Obj(x->contents * y->contents);
}

Int __floordiv__int__ (Int x, Int y){
	return new Int_Obj(floor(x->contents / y->contents));
}

Int __mod__int__ (Int x, Int y){
	return new Int_Obj(x->contents % y->contents);
}

Int __pow__int__ (Int x, Int y){
	x->contents = pow(x->contents, y->contents);
	return new Int_Obj(pow(x->contents, y->contents));
}

Int __neg__int__ (Int x){
	return new Int_Obj(-x->contents);
}

Int __pos__int__ (Int x){
	return new Int_Obj(+x->contents);
}

Bool __lt__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents < y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __gt__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents > y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __le__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents <= y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __ge__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents >= y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __eq__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents == y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __ne__int__ (Int x, Int y){
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents != y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}



Str __add__str__ (Str x, Str y) {
	return new Str_Obj(x->contents + y->contents);
}

Str __lmul__str__ (Str x, Int y) {
	int n = y->contents;
	string s = x->contents;
	string res = "";
	for (int i = 0; i < n ; i++) {
		res += s;
	}
	return new Str_Obj(res);
}

Str __rmul__str__ (Int x, Str y) {
	int n = x->contents;
	string s = y->contents;
	string res = "";
	for (int i = 0; i < n ; i++) {
		res += s;
	}
	return new Str_Obj(res);
}

Bool __lt__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents < y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __gt__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents > y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __le__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents <= y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __ge__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents >= y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __eq__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents == y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Bool __ne__str__ (Str x, Str y) {
	Bool res = new Bool_Obj("False");
	if (!x->value || !y->value) {
		return res;
	}
	if (x->contents != y->contents) { 
		res->contents = "True";
		res->value = true;
	}
	return res;
}

Str __getitem__str__ (Str S, Int k) {
	string s(1, S->contents[k->contents]);
	return new Str_Obj(s);
}

Str __getslice__str__ (Str S, Int L, Int U) {
	return new Str_Obj(S->contents.substr(L->contents, L->contents + U->contents));
}

Int __len__str__ (Any S) {
	return new Int_Obj(((Str)S)->contents.length());
}

Any __getitem__list__ (List S, Int k) {
	if (k->contents < 0) {
		return S->contents.at(S->contents.size() + k->contents);
	}
	return S->contents.at(k->contents);
	
}

List __getslice__list__ (List S, Int L, Int U) {
	vector<Any> vect; 
	for (int i = L->contents; i < U->contents; i++) {
		if (i >= S->contents.size()) {
			break;
		}
		vect.insert(vect.end(), S->contents.at(i));
	}
	return new List_Obj(vect);
}

Any __setitem__list__ (List S, Int k, Any val) {
	if (k->contents < 0) {
		S->contents[S->contents.size() + k->contents] = val;
		return val;
	}
	S->contents[k->contents] = val;
	return val;
}

List __setslice__list__ (List S, Int a, Int b, List val) {
	for (int i = a->contents; i < b->contents; i++) {
		if (i >= S->contents.size()) {
			break;
		}
		S->contents[i] = val->contents[i - a->contents];
	}
	return S;
}

Int __len__list__ (Any S) {
	return new Int_Obj(((List)S)->contents.size());
}

Any __getitem__dict__int__ (Dict D, Int x) {
		Int key;
		for (auto elem : D->order) {
			if (((Int)elem)->contents == x->contents) {
				key = (Int)elem;
			}
		}
		return D->contents[key];
}

Any __getitem__dict__bool__ (Dict D, Bool x) {
	Bool key;
	for (auto elem : D->order) {
		if (((Bool)elem)->contents == x->contents) {
			key = (Bool)elem;
		}
	}
	return D->contents[key];
}

Any __getitem__dict__str__ (Dict D, Str x) {
	Str key;
	for (auto elem : D->order) {
		if (((Str)elem)->contents == x->contents) {
			key = (Str)elem;
		}
	}
	return D->contents[key];
}

Any __setitem__dict__int__ (Dict S, Int k, Any val) {
	for (int i = 0; i < S->order.size(); i++) {
		auto elem = S->order[i];
		if (((Int) elem)->contents == k->contents) {
			S->order[i] = k;
			S->contents[k] = val;
			return val;
		}
	}
	S->order.push_back(k);
	S->contents[k] = val;
	return val;
}

Any __setitem__dict__bool__ (Dict S, Bool k, Any val) {
	for (int i = 0; i < S->order.size(); i++) {
		auto elem = S->order[i];
		if (((Bool) elem)->contents == k->contents) {
			S->order[i] = k;
			S->contents[k] = val;
			return val;
		}
	}
	S->order.push_back(k);
	S->contents[k] = val;
	return val;
}

Any __setitem__dict__str__ (Dict S, Str k, Any val) {
	for (int i = 0; i < S->order.size(); i++) {
		auto elem = S->order[i];
		if (((Str) elem)->contents == k->contents) {
			S->order[i] = k;
			S->contents[k] = val;
			return val;
		}
	}
	S->order.push_back(k);
	S->contents[k] = val;
	return val;
}

Int __len__dict__int__ (Dict D) {
	return new Int_Obj(D->order.size());
}

Int __len__dict__bool__ (Dict D) {
	return new Int_Obj(D->order.size());
}

Int __len__dict__str__ (Dict D) {
	return new Int_Obj(D->order.size());
}




Str __and__ (Str x, Str y) {
	// if (!x->value || !y->value) {
	// }
	if (x->contents.empty()) {
		return x;
	}
	if (y->contents.empty()) {
		return y;
	}
	return y;
}

Int __and__ (Int x, Int y) {
	// if (!x->value) {
	// 	return x;
	// }
	// if (!y->value) {
	// 	return y;
	// }
	if (!x->contents) {
		return x;
	}
	if (!y->contents) {
		return y;
	}
	return y;
}

Bool __and__ (Bool x, Bool y) {
	// if (!x->value || !y->value) {
	// 	return res;
	// }
	if (x->contents == "False") {
		return x;
	}
	if (y->contents == "False") {
		return y;
	}
	return y;
	
}

List __and__ (List x, List y) {
	// if (!x->value || !y->value) {
	// 	return res;
	// }
	// if (!x->contents.empty() && !y->contents.empty()) {
	// 	res->contents = "True";
	// 	res->value = true;
	// }
	if (x->contents.empty()) {
		return x;
	}
	if (y->contents.empty()) {
		return y;
	}
	return y;
}

Str __or__ (Str x, Str y) {
	// if (!x->value || !y->value) {
	// }
	if (!x->contents.empty()) {
		return x;
	}
	if (!y->contents.empty()) {
		return y;
	}
	return y;
}

Int __or__ (Int x, Int y) {
	// if (!x->value) {
	// 	return x;
	// }
	// if (!y->value) {
	// 	return y;
	// }
	if (x->contents) {
		return x;
	}
	if (y->contents) {
		return y;
	}
	return y;
}

Bool __or__ (Bool x, Bool y) {
	// if (!x->value || !y->value) {
	// 	return res;
	// }
	if (x->contents != "False") {
		return x;
	}
	if (y->contents != "False") {
		return y;
	}
	return y;
	
}

List __or__ (List x, List y) {
	// if (!x->value || !y->value) {
	// 	return res;
	// }
	// if (!x->contents.empty() && !y->contents.empty()) {
	// 	res->contents = "True";
	// 	res->value = true;
	// }
	if (!x->contents.empty()) {
		return x;
	}
	if (!y->contents.empty()) {
		return y;
	}
	return y;
}

void* print (Any a) {
	string type = a->getType();
	if (type == "Int") {
		cout << ((Int) a)->contents;
	} else if (type == "Str") {
		cout << ((Str) a)->contents;
	} else if (type == "Bool") {
		cout << ((Bool) a)->contents;
	} else if (type == "List") {
		print((List) a);
	} else if (type == "Dict") {
		print((Dict) a);
	} else if (type == "Tuple0") {
		print((Tuple0) a);
	} else if (type == "Tuple1") {
		print((Tuple1) a);
	} else if (type == "Tuple2") {
		print((Tuple2) a);
	} else if (type == "Tuple3") {
		print((Tuple3) a);
	} else if (type == "None") {
		print((None) a);
	} else if (type == "Range") {
		print((Range) a);
	}
 
}

void* print (Range r) {
	cout << "xrange(" + to_string((r->low)->contents) + ", " + to_string((r->high)->contents) + ")";
}

void* print (List l) {
	cout << "[";
	for (auto elem: l->contents) {
		string type = elem->getType();
		if (elem == l->contents.back()) {
			if (type == "Int") {
				cout << ((Int) elem)->contents;
			} else if (type == "Str") {
				cout << ((Str) elem)->contents;
			} else if (type == "Bool") {
				cout << ((Bool) elem)->contents;
			} 
		}
		else {
			if (type == "Int") {
				cout << ((Int) elem)->contents << ", ";
			} else if (type == "Str") {
				cout << ((Str) elem)->contents << ", ";
			} else if (type == "Bool") {
				cout << ((Bool) elem)->contents << ", ";
			}
		}
	}
	cout << "]";
}

void* print (Dict d) {
	string res = "{";
	for (auto elem: d->order) {
		string keyType = elem->getType();
		string valueType = (d->contents)[elem]->getType();
		
		if (keyType == "Int") {
			res += to_string(((Int) elem)->contents) + ": ";
		} else if (keyType == "Str") {
			res += "'" + ((Str) elem)->contents + "': ";
		} else if (keyType == "Bool") {
			res += ((Bool) elem)->contents + ": ";
		} 
		if (valueType == "Int") {
			res += to_string(((Int) d->contents[elem])->contents) + ", ";
		} else if (valueType == "Str") {
			res += "'" + ((Str) d->contents[elem])->contents + "', ";
		} else if (valueType == "Bool") {
			res += ((Bool) d->contents[elem])->contents + ", ";
		} 
	}
	res.pop_back();
	res.pop_back();
	cout << res << "}";
}

void* print (Tuple0 t) {
	cout << "()";
}

void* print (Tuple1 t) {
	cout << "(";
	if (t->contents->getType() == "Int") {
		cout << ((Int)t->contents)->contents;
	} else if (t->contents->getType() == "Str"){
		cout << "'" + ((Str)t->contents)->contents + "'";
	} else {
		cout << ((Bool)t->contents)->contents;
	}
	cout << ")";
}

void* print (Tuple2 t) {
	cout << "(";
	if (t->contents->getType() == "Int") {
		cout << ((Int)t->contents)->contents;
	} else if (t->contents->getType() == "Str"){
		cout << "'" +((Str)t->contents)->contents + "'";
	} else {
		cout << ((Bool)t->contents)->contents;
	}
	cout << ", ";

	if (t->contents1->getType() == "Int") {
		cout << ((Int)t->contents1)->contents;
	} else if (t->contents1->getType() == "Str"){
		cout << "'" + ((Str)t->contents1)->contents + "'";
	} else {
		cout << ((Bool)t->contents1)->contents;
	}
	cout << ")";
}

void* print (Tuple3 t) {
	cout << "(";
	if (t->contents->getType() == "Int") {
		cout << ((Int)t->contents)->contents;
	} else if (t->contents->getType() == "Str"){
		cout << "'" + ((Str)t->contents)->contents + "'";
	} else {
		cout << ((Bool)t->contents)->contents;
	}
	cout << ", ";

	if (t->contents1->getType() == "Int") {
		cout << ((Int)t->contents1)->contents;
	} else if (t->contents1->getType() == "Str"){
		cout << "'" + ((Str)t->contents1)->contents + "'";
	} else {
		cout << ((Bool)t->contents1)->contents;
	}
	cout << ", ";

	if (t->contents2->getType() == "Int") {
		cout << ((Int)t->contents2)->contents;
	} else if (t->contents2->getType() == "Str"){
		cout << "'" + ((Str)t->contents2)->contents + "'";
	} else {
		cout << ((Bool)t->contents2)->contents;
	}
	cout << ")";
}

void* print (None n) {
	cout << "None";
}

void* print (Int i) {
	cout << i->contents;
}

void* print (Str s) {
	cout << s->contents;
}

void* print (Bool b) {
	cout << b->contents;
}

void* print (int i) {
	cout << i;
}

void* print (string s) {
	cout << s;
}

void* print (bool b) {
	cout << b;
}

void* leftCompareHelper (Bool b, Int i) {
	if (!b->value) {
		i->value = false;
	}
}

void* leftCompareHelper (Bool b, Str s) {
	if (!b->value) {
		s->value = false;
	}
}

void* assignHelper (void* v, Int i) {
	v = static_cast<Int>(v);
}

Str assignHelper (void* v, Str s) {
	Str ptr = static_cast<Str>(v);
	return ptr;
}

void* assignHelper (void* v, Bool b) {
	v = (Bool) v;
}

void* assignHelper (void* v, List l) {
	v = (List) v;
}

bool condtionalHelper (Bool b) {
	return b->value;
}

bool condtionalHelper (Any b) {
	return ((Bool)b)->value;
}


