/* Implementations of declaration and type-related classes */

/* Authors:  P. N. Hilfinger */

#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "apyc.h"

using namespace std;

/** These must be filled in with declarations from the standard
 *  prelude.  They are used to supply the types of built-in
 *  constructs.  All are initially nullptr. */
Decl* intDecl;
Decl* listDecl;
Decl* tupleDecl[4];
Decl* strDecl;
Decl* dictDecl;
Decl* boolDecl;
Decl* rangeDecl;

Decl* mainModule;

typedef unordered_map<string, Decl**> BuiltinMap;

//static struct { const char* name; Decl** var; } builtinInit[] = 
static BuiltinMap builtinMap({
    { "int", &intDecl },
    { "list", &listDecl },
    { "tuple0", &tupleDecl[0] },
    { "tuple1", &tupleDecl[1] },
    { "tuple2", &tupleDecl[2] },
    { "tuple3", &tupleDecl[3] },
    { "str", &strDecl },
    { "dict", &dictDecl },
    { "bool", &boolDecl },
    { "range", &rangeDecl },
});



/** List of declarations corresponding to the module and actual
 *  declarations in the program (as opposed to type variables
 *  introduced by freshen). */
static vector<Decl*> allDecls;

/** Utility for printing types in Decls. */
static void
outType (Type_Ptr type, ostream& out) {
    out << " ";
    if (type != nullptr) {
	type->print (out, 0);
    } else
	out << "?";
}

Decl::Decl (AST_Ptr id, Decl* container, Environ* members)
    : _frozen (true), _id (id), _container (container), _members (members) {
    _index = allDecls.size ();
    allDecls.push_back (this);
}
        
/* Print THIS on OUT. */ 
void 
Decl::print (ostream& out, const Decl_Set& used) const
{
    out << "(" << declTypeName () << " " << getIndex () 
	<< " " << getName () << " ";
    printContainer (out);
    printPosition (out);
    printType (out);
    printTypeParams (out);
    printMembersList (out, used);
    out << ")";
    out.flush ();
}

void
Decl::print (const Decl_Set& used) const
{
    print (cout, used);
}

void
Decl::printContainer (ostream& out) const
{
    if (getContainer () != nullptr) 
        out << getContainer ()->getIndex () << " ";
    else
        out << "-1 ";
}

void
Decl:: printPosition (ostream& out) const {
}

void
Decl::printType (ostream& out) const {
}

void
Decl::printMembersList (ostream& out, const Decl_Set& used) const {
    if (_members != nullptr) {
	out << " (index_list";
	const vector<Decl*>& members = getEnviron ()->get_members ();
	for (size_t i = 0; i < members.size (); i += 1)
	    if (used.count (members[i]) > 0) 
		out << " " << members[i]->getIndex ();
	out << ")";
    }
}

void
Decl::printTypeParams (ostream& out) const {
}

Type_Ptr 
Decl::getType () const
{
    return nullptr;
}

Type_Ptr
Decl::getSelectedType () const {
    return getType ();
}

int
Decl::getPosition () const
{
    return -1;
}

AST_Ptr
Decl::getAst () const
{
    return _ast;
}

void
Decl::setAst (AST_Ptr ast)
{
    _ast = ast;
}

bool
Decl::isType () const
{
    return false;
}

bool
Decl::hasType () const
{
    return false;
}

bool
Decl::isInternal () const
{
    return false;
}

bool
Decl::isOverloadable () const
{
    return false;
}

bool
Decl::isMethod () const
{
    return false;
}

Type_Ptr
Decl::asType () const
{
    return asType (Type_Vect ());
}

Type_Ptr
Decl::asType (const AST_Vect&) const
{
    UNIMPLEMENTED;
}

Type_Ptr
Decl::asType (const Type_Vect&) const
{
    UNIMPLEMENTED;
}

Type_Ptr
Decl::asBaseType () const
{
    return nullptr;
}

Type_Ptr
Decl::asGenericType ()
{
    UNIMPLEMENTED;
}

int
Decl::getTypeArity () const
{
    return 0;
}

const Environ*
Decl::getEnviron () const
{
    if (_members == nullptr)
	UNIMPLEMENTED;
    return _members;
}

void
Decl::addMember (Decl* new_member)
{
    if (_members == nullptr)
	UNIMPLEMENTED;
    _members->define (new_member);
}

Decl*
Decl::newVarDecl (AST_Ptr) {
    UNIMPLEMENTED;
}

Decl*
Decl::addVarDecl (AST_Ptr) {
    UNIMPLEMENTED;
}

Decl*
Decl::addParamDecl (AST_Ptr id, int k) {
    string name = id->text ();
    const Environ* env = getEnviron ();
    Decl* old = env->findImmediate (name);
    if (old != nullptr) {
	error (id, "multiply defined formal parameter: %s",
	       name.c_str ());
	return old;
    }
    Decl* result = makeParamDecl (id, this, k, newTypeVar ());
    addMember (result);
    return result;
}
    

Decl*
Decl::addTypeVarDecl (AST_Ptr id) {
    string name = id->text ();
    const Environ* env = getEnviron ();
    Decl* old = env->findImmediate (name);
    if (old != nullptr)
	return old;
    Decl* result = makeTypeVarDecl (id);
    addMember (result);
    return result;
}

Decl*
Decl::newDefDecl (AST_Ptr, int) {
    UNIMPLEMENTED;
}

Decl*
Decl::addDefDecl (AST_Ptr id, int k) {
    Decl* decl = newDefDecl (id, k);
    Decl* old = getEnviron ()->findImmediate (id->text());
    if (old != nullptr && !old->isOverloadable()) {
        error (id, "invalid attempt to overload %s",
               id->text().c_str());
    } else {
        addMember (decl);
    }
    return decl;
}

bool
Decl::assignable () const
{
    return false;
}

bool
Decl::isFrozen () const
{
    return _frozen;
}

void
Decl::setFrozen (bool freeze)
{
}

const char*
Decl::declTypeName () const
{
    return "?";
}

void
Decl::replaceBindings (const Unifier& subst)
{
}    

void
Decl::addAttributeDecls (AST_Ptr id)
{
}

/** The superclass of declarations that describe an entity with a type. */
class TypedDecl : public Decl {
protected:
    TypedDecl (AST_Ptr id, Decl* container, AST_Ptr type,
	       Environ* members = nullptr) 
        :  Decl (id, container, members),
           _type (type == nullptr ? nullptr : type->asType ()) {
    }

public:

    Type_Ptr getType () const override {
	if (isFrozen () || _type == nullptr)
	    return _type;
	else
	    return freshen (_type);
    }

protected:

    bool hasType () const override {
        return true;
    }

    void replaceBindings (const Unifier& subst) override {
        if (_type != nullptr)
            _type = _type->replaceBindings (subst)->asType ();
    }

    void printType (ostream& out) const override {
	outType (_type, out);
    }

private:
    Type_Ptr _type;
};

class VarDecl : public TypedDecl {
public:

    VarDecl (AST_Ptr id, Decl* container, AST_Ptr type)
        :  TypedDecl (id, container, type) {
    }

protected:

    const char* declTypeName () const override {
        return "vardecl";
    }

    bool assignable () const override {
	return true;
    }

};

Decl*
makeVarDecl (AST_Ptr id, Decl* func, AST_Ptr type)
{
    return new VarDecl (id, func, type);
}


class ParamDecl : public TypedDecl {
public:

    ParamDecl (AST_Ptr id, Decl* func, int k,
              AST_Ptr type)
        :  TypedDecl (id, func, type), _posn (k) {
    }

    int getPosition () const override {
        return _posn;
    }

protected:

    const char* declTypeName () const override {
        return "paramdecl";
    }

    void printPosition (ostream& out) const override {
        out << " " << getPosition ();
    }

    bool assignable () const override {
	return true;
    }

private:

    int _posn;

};

Decl*
makeParamDecl (AST_Ptr id, Decl* func, int k, AST_Ptr type)
{
    return new ParamDecl (id, func, k, type);
}


class InstanceDecl : public TypedDecl {
public:

    InstanceDecl (AST_Ptr id, Decl* cls, AST_Ptr type)
        :  TypedDecl (id, cls, type) {
    }

protected:

    const char* declTypeName () const override {
        return "instancedecl";
    }

    bool assignable () const override {
	return true;
    }

};

Decl*
makeInstanceDecl (AST_Ptr id, Decl* cls, AST_Ptr type)
{
    return new InstanceDecl (id, cls, type);
}

class TypeVarDecl : public Decl {
public:

    TypeVarDecl (AST_Ptr id) : Decl (id, nullptr) {
	setAst (id);
    }

protected:

    bool isInternal () const override {
	return getAst () != nullptr && getAst ()->arity () == 0;
    }

    void printContainer (ostream&) const override { 
   }

    const char* declTypeName () const override {
        return "typevardecl";
    }

    void printType (ostream& out) const override {
	outType (getAst ()->asType (), out);
    }

};

Decl*
makeTypeVarDecl (AST_Ptr id)
{
    return new TypeVarDecl (id);
}

class FuncDecl : public TypedDecl {
public:

    FuncDecl (AST_Ptr id, Decl* container, AST_Ptr type,
              Environ* env)
        :  TypedDecl (id, container, type, env) {
    }

protected:

    bool isOverloadable () const override {
        return true;
    }

    const char* declTypeName () const override {
        return "funcdecl";
    }

    void setFrozen (bool freeze) override {
        _frozen = freeze;
    }

    Decl* addVarDecl (AST_Ptr id) override {
	Decl* decl = makeVarDecl (id, this, newTypeVar ());
	addMember (decl);
	return decl;
    }

    Decl* newDefDecl (AST_Ptr id, int k) override {
	return makeFuncDecl (id, this, makeFuncType (k));
    }

};

Decl*
makeFuncDecl (AST_Ptr id, Decl* container, AST_Ptr type)
{
    return new FuncDecl (id, container, type,
			 new Environ (container->getEnviron ()));
}

class MethodDecl : public FuncDecl {
public:

    MethodDecl (AST_Ptr id, Decl* container, AST_Ptr type)
        :  FuncDecl (id, container, type,
		     new Environ (container->getEnviron ()->get_enclosure ())) {
    }

protected:

    Type_Ptr getSelectedType () const override {
        return getType ()->boundFunctionType ();
    }

    bool isMethod () const override {
	return true;
    }

};

Decl*
makeMethodDecl (AST_Ptr id, Decl* cls, AST_Ptr type)
{
    return new MethodDecl (id, cls, type);
}

class ClassDecl : public Decl {
public:

    ClassDecl (AST_Ptr id, AST_Ptr params)
        : Decl (id, nullptr, new Environ (outerEnviron)), _params (params) {
    }

protected:

    bool isType () const override {
	return true;
    }

    void printContainer (ostream&) const override {
    }

    void printTypeParams (ostream& out) const override {
        out << " (index_list";
	for (auto c : *_params) {
	    Decl* decl = c->getDecl ();
	    if (decl == nullptr)
		out << " ?";
	    else
		out << " " << decl->getIndex ();
	}
	out << ")";
    }

    const char* declTypeName () const override {
        return "classdecl";
    }

    Type_Ptr asType (const AST_Vect& params) const override {
        if (getTypeArity () != -1 && getTypeArity () != params.size ()) {
	    throw range_error ("wrong number of type parameters");
        }
        for (auto t : params) {
            if (t == nullptr || !t->isType ())
                throw domain_error ("attempt to pass null type parameter");
        }

	AST_Ptr id = makeId (getName ().c_str (), 0);
	id->addDecl (const_cast<ClassDecl*> (this));

	AST_Ptr result = NODE (TYPE_AST, id, params);
	return result->asType ();
    }

    Type_Ptr asType (const Type_Vect& tparams) const override {
        AST_Vect params;
        for (auto c : tparams)
            params.push_back (c);
        return asType (params);
    }

    Type_Ptr asBaseType () const override {
        return asType (_params->children ());
    }

    Type_Ptr asGenericType () override {
        Type_Vect params;
        for (AST_Ptr c : *_params) {
            params.push_back (newTypeVar ());
        }
        return asType (params);
    }

    Decl* addVarDecl (AST_Ptr id) override {
	Decl* decl = makeInstanceDecl (id, this, newTypeVar ());
	addMember (decl);
	return decl;
    }

    Decl* newDefDecl (AST_Ptr id, int k) override {
	return makeMethodDecl (id, this, makeMethodType (k, this));
    }

    Decl* addTypeVarDecl (AST_Ptr id) override {
	string name = id->text ();
	Decl* prev = getEnviron ()->findImmediate (name);
	if (prev != nullptr)
	    return prev;
        Decl* decl = makeTypeVarDecl (id);
	addMember (decl);
	return decl;
    }

    int getTypeArity () const override {
	return _params->arity ();
    }

    void addAttributeDecls (AST_Ptr id) override {
        Decl_Vect decls;
        getEnviron ()->findImmediate (id->text (), decls);
        for (auto d : decls) {
            id->addDecl (d);
        }
    }


    AST_Ptr _params;
};

Decl*
makeClassDecl (AST_Ptr id, AST_Ptr params)
{
    return new ClassDecl (id, params);
}

class ModuleDecl : public Decl {
public:

    ModuleDecl (AST_Ptr id)
        :  Decl (id, nullptr, new Environ (nullptr)) {
    }

protected:

    const char* declTypeName () const override {
        return "moduledecl";
    }

    void printContainer (ostream& os) const override {
    }

    Decl* addVarDecl (AST_Ptr id) override {
	Decl* decl = makeVarDecl (id, this, newTypeVar ());
	addMember (decl);
	return decl;
    }

    Decl* newDefDecl (AST_Ptr id, int k) override {
	return makeFuncDecl (id, this, makeFuncType (k));
    }

};

Decl*
makeModuleDecl (AST_Ptr id)
{
    return new ModuleDecl (id);
}

class UnknownDecl : public Decl {
public:

    UnknownDecl (AST_Ptr id, bool isType)
        :  Decl (id, mainModule, new Environ (nullptr)), _isType (isType) {
        if (!isType)
            _dummyType = newTypeVar ();
    }

protected:

    bool isType () const override {
        return _isType;
    }

    Type_Ptr getType () const override {
        return _dummyType;
    }

private:
    Type_Ptr _dummyType;
    const bool _isType;
};

Decl*
makeUnknownDecl (AST_Ptr id, bool isType)
{
    return new UnknownDecl (id, isType);
}


void
setBuiltinDecl (Decl* decl)
{
    BuiltinMap::const_iterator entryPair = builtinMap.find (decl->getName ());
    if (entryPair != builtinMap.end ()) {
        if (*entryPair->second != nullptr) {
            throw logic_error (string("multiple definition of builtin class ") 
                               + decl->getName ().c_str());
        }
        *entryPair->second = decl;
    }
}

void
outputDecls (ostream& out, const Decl_Set& used)
{
    for (size_t i = 0; i < allDecls.size (); i += 1) {
        Decl* d = allDecls[i];
        if (used.count (d) > 0) {
            allDecls[i]->print (out, used);
            out << endl;
        }
    }
}

void
DB (const Decl* decl)
{
    Decl_Set used ({ const_cast<Decl*>(decl) });
    decl->print (used);
    cerr << endl;
}
