/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* types.cc: AST subclasses related to types. */

/* Authors:  P. N. Hilfinger */

#include <iostream>
#include "apyc.h"

using namespace std;

/***** AST *****/

/** Map of special function names to descriptions for error messages. */
static unordered_map<string, string>
   OP_MAPPING (
       {
           { "__getitem__", "indexing"},
           { "__setitem__", "indexing"},
           { "__add__", "addition"},
           { "__mul__", "subtraction"},
           { "__floordiv__", "division"},
           { "__mod__", "mod operation"},
           { "__pow__", "exponentiation"},
           { "__neg__", "negation"},
           { "__in__", "membership test"},
           { "__notin__", "membership test"},
           { "__choose__", "for statement target" }
       });

/** Report all identifiers with multiple resolutions in RESOLVED. 
 *  All Resolvers in RESOLVED must have the same sequence of
 *  overloaded identifiers. */
static void
reportAmbiguities (vector<Resolver>& resolved)
{
    if (resolved.size () <= 1)
        return;
    int numOverloadings = resolved[0].numOverloadings ();
    for (int i = 0; i < numOverloadings; i += 1) {
        Decl* decl0 = resolved[0].getDecl (i);
        for (size_t j = 1; j < resolved.size (); j += 1) {
            if (decl0 != resolved[j].getDecl (i)) {
                AST_Ptr id = resolved[0].overloading (i);
                string name = id->text ();
                if (OP_MAPPING.count (name) == 0) 
                    error (id, "ambiguous reference to %s", name.c_str ());
                else
                    error (id, "ambiguous %s",
                           OP_MAPPING[name].c_str ());
                break;
            }
        }
    }
}

void
AST::resolveTypesOuter (Decl* context) 
{
    freezeDecls (true);

    bool errorReporting0 = setErrorReporting (false);
    int errors0 = numErrors ();
    Resolver resolver;
    vector<Resolver> resolved;

    getOverloadings (resolver);

    while (resolver.nextResolution ()) {
        Unifier subst1;
        resolveTypes (context, subst1);
        if (numErrors () == errors0)
            resolved.push_back (resolver);
        setNumErrors (errors0);
    }

    setErrorReporting (errorReporting0);
    reportAmbiguities (resolved);
    if (resolved.size () != 0)
        resolver = resolved[0];
    Unifier subst;
    resolver.setDecls ();
    resolveTypes (context, subst);
    replaceBindings (subst);
    typeSpecificChecks (this);
    freeTypeVarCheck (Type_Set ());

    freezeDecls (false);
}

void
Type::typeSpecificChecks (AST_Ptr loc)
{
    for (int k = 0; k < numTypeParams (); k += 1) {
        typeParam (k)->typeSpecificChecks (loc);
    }
}

bool
Type::isValidAsKey ()
{
    return false;
}

/** As for unify, but SUBST has an undefined state if result is
 *  false. */
static bool
unify1 (Type_Ptr t0, Type_Ptr t1, Unifier& subst)
{
    t0 = subst[t0];
    t1 = subst[t1];
    if (t0 == t1)
        return true;
    if (t0->isTypeVariable ()) {
        subst.bind (t0, t1);
        return true;
    }
    subst.bind (t1, t0);
    if (t1->isTypeVariable ())
        return true;
    if (t0->getId () != t1->getId ())
        return false;
    if (t0->numTypeParams () != t1->numTypeParams ())
        return false;
    for (int i = 0; i < t0->numTypeParams (); i += 1) {
        if (!unify1 (t0->typeParam (i), t1->typeParam (i), subst))
            return false;
    }
    return true;
}

bool
unify (Type_Ptr t0, Type_Ptr t1, Unifier& subst)
{
    Unifier s = subst;
    if (unify1 (t0, t1, s)) {
        subst.swap (s);
        return true;
    }
    return false;
}

bool
unifies (Type_Ptr t0, Type_Ptr t1)
{
    Unifier s;
    return unify (t0, t1, s);
}


/*****   Typed_Tree ***/

Type_Ptr
Typed_Tree::computeType ()
{
    return newTypeVar ();
}

bool
Typed_Tree::hasType ()
{
    return true;
}

/*****    TYPE    *****/

Type_Ptr
Type::getType ()
{
    error (this, "a type may not be used as a value in this dialect");
    return newTypeVar ();
}

AST_Ptr
Type::getId ()
{
    return nullptr;
}

int
Type::numParams ()
{
    return -1;
}

Type_Ptr
Type::returnType ()
{
    return nullptr;
}

Type_Ptr
Type::paramType (int k)
{
    throw range_error ("type has no parameters");
}

int
Type::numTypeParams ()
{
    return -1;
}

Type_Ptr
Type::typeParam (int k)
{
    throw range_error ("type has no type parameters");
}

Type_Ptr
Type::setTypeParam (int k, Type_Ptr type)
{
    throw range_error ("type has no type parameters");
}
    

Type_Ptr
Type::boundFunctionType ()
{
    return nullptr;
}
    
const Environ*
Type::getEnviron ()
{
    return theEmptyEnviron;
}

bool
Type::isTypeVariable ()
{
    return false;
}

AST_Ptr
Type::replaceBindings (const Unifier& subst)
{
    return AST::replaceBindings (subst);
}

AST_Ptr
Type::replaceBindings (const Unifier& subst, AST_Set& visiting)
{
    Type_Ptr result = subst[this];
    if (result->isTypeVariable ())
        return result;
    return result->AST::replaceBindings (subst, visiting);
}

void
Type::getTypeVariables (Type_Set& typeVars, Type_Set& visited) {
    if (visited.count (this) > 0)
        return;
    visited.insert (this);
    if (isTypeVariable ())
        typeVars.insert (this);
    for (int i = 0; i < numTypeParams (); i += 1)
        typeParam (i)->getTypeVariables (typeVars, visited);
}

/** Add mappings to FRESHMAP such that applying FRESHMAP to the nodes
 *  in TREE results in a tree in which all type variables are replaced by fresh 
 *  ones, unused elesewhere.  Assumes FRESHMAP initially records previously
 *  converted nodes.  When a node is reached circularly, it will be
 *  mapped to itself; a second pass(freshed2) must be used to replace it.
 *  Returns the mapping of TREE. */
static Type_Ptr
freshen1 (Type_Ptr type, Type_Map& freshMap)
{
    auto old = freshMap.emplace (type, type);
    if (!old.second)
        return old.first->second;

    Type_Ptr result;
    if (type->isTypeVariable ())
        result = newTypeVar ();
    else {
        bool changed;
        AST_Vect newKids;

        changed = false;
        for (int i = 0; i < type->numTypeParams (); i += 1) {
            Type_Ptr c = type->typeParam (i), c0 = c;
            c = freshen1 (c, freshMap);
            newKids.push_back (c);
            changed |= (c != c0);
        }

        if (!changed) 
            result = type;
        else if (type->arity () == type->numTypeParams ())
            result = NODE (type->typeIndex (), newKids)->asType ();
        else
            result =
                NODE (type->typeIndex (), type->getId (), newKids)->asType ();
    }
    freshMap[type] = result;
    return result;
}

/** Return a version of TREE with all nodes replaced as indicated by
 *  FRESHMAP.  Assumes that subtrees mapped to themselves in FRESHMAP are
 *  fully processed. */
static Type_Ptr
freshen2 (Type_Ptr type0, Type_Map& freshMap)
{
    Type_Ptr type = freshMap[type0];
    if (type == type0 || type == nullptr)
        return type0;

    for (int i = 0; i < type->numTypeParams (); i += 1)
        type->setTypeParam (i, freshen2 (type->typeParam (i), freshMap));
    
    return type;
}

Type_Ptr
freshen (Type_Ptr tree)
{
    Type_Ptr type = tree->asType ();

    Type_Map freshMap;

    type = freshen1 (type, freshMap);
    return freshen2 (type, freshMap);
}

void
freshen (Type_Vect& types)
{
    Type_Map freshMap;

    for (auto& type : types)
        type = freshen1 (type, freshMap)->asType ();
    for (auto& type : types)
        type = freshen2 (type, freshMap)->asType ();
}

Type_Ptr
makeFuncType (int n)
{
    AST_Vect params;
    for (int i = 0; i <= n; i += 1)
        params.push_back(newTypeVar ());
    return NODE (FUNCTION_TYPE_AST, params)->asType ();
}

bool
makeFuncType (int n, Type_Ptr type, Unifier& subst)
{
    type = subst[type];
    if (!type->isTypeVariable ())
        return (type->isFunctionType () && type->numParams () == n);
    unify (type, makeFuncType (n), subst);
    return true;
}

Type_Ptr
makeMethodType (int n, Decl* clas)
{
    AST_Vect params;
    params.push_back (newTypeVar ());
    params.push_back (clas->asGenericType ());
    for (int i = 1; i < n; i += 1)
        params.push_back(newTypeVar ());
    return NODE (FUNCTION_TYPE_AST, params)->asType();
}

/*****  TYPE VARIABLES *****/

class Type_Var_AST : public Type {
    friend Type_Ptr newTypeVar(const string&);

    bool isTypeVariable () override {
        return true;
    }

    void print (AST_Printer& printer) override {
        printer.os << "(type_var " << location () << " " << text ();
        if (getDecl () != nullptr)
            printer.os << " " << getDecl ()->getIndex ();
        printer.os << ")";
    }

    string text () const override {
        return _name;
    }
           
    /** A previously unused type variable whose name is NAME. */
    static Type_Ptr newTypeVar (const string& name);
        
    Decl* getDecl () override {
        if (_me.size () == 0)
            return nullptr;
        else
            return _me[0];
    }

    const Decl_Vect& getDecls () override {
        return _me;
    }

    void addDecl (Decl* decl) override {
        assert (_me.empty ());
        _me.push_back (decl);
    }

    void reachableDecls (Decl_Set& decls) override {
        decls.insert (_me.begin (), _me.end ());
    }

    void collectTypeVarDecls (Decl* enclosing) override {
        if (getDecl () == nullptr) {
            string name = text ();
            Decl* old = enclosing->getEnviron ()->findImmediate (name);
            if (old == nullptr)
                addDecl (enclosing->addTypeVarDecl (this));
            else
                addDecl (old);
        }
    }

    AST_Ptr resolveSimpleIds (const Environ* env) override {    
        string name = text ();
        if (getDecl () != nullptr)
            return getDecl ()->getAst ();
        Decl_Vect defns;
        env->find (name, defns);
        if (defns.empty ()) {
            error (this, "undefined identifier: %s", name.c_str ());
            addDecl (makeUnknownDecl (this, false));
            return this;
        }
        assert (defns.size () == 1);
        addDecl (defns[0]);
        return defns[0]->getAst ();
    }

    Decl_Vect _me;
    string _name;
};

static int lastTypeVarUid = 0;

Type_Ptr
Type_Var_AST::newTypeVar (const string& name)
{
    Type_Var_AST* result = new Type_Var_AST;
    result->_name = name;
    return result;
}

Type_Ptr
newTypeVar (const string& name)
{
    return Type_Var_AST::newTypeVar (name);
}

Type_Ptr
newTypeVar ()
{
    lastTypeVarUid += 1;
    Type_Ptr result = newTypeVar ("$#" + to_string (lastTypeVarUid));
    result->addDecl (makeTypeVarDecl (result));
    return result;
}

/***** FUNCTION TYPES *****/

class Function_Type_AST: public Type {
protected:

    /** For function types, returns a unique artificial id node. */
    AST_Ptr getId () override {
        static AST_Ptr id = makeId ("->", 0);
        return id;
    }

    bool isFunctionType () override {
        return true;
    }

    int numParams () override {
        return arity () - 1;
    }

    Type_Ptr paramType (int k) override {
        return child (k + 1)->asType ();
    }

    Type_Ptr returnType () override {
        return child (0)->asType ();
    }

    int numTypeParams () override {
        return arity ();
    }

    Type_Ptr typeParam (int k) override {
        return child (k)->asType ();
    }

    Type_Ptr setTypeParam (int k, Type_Ptr type) override {
        return setChild (k, type)->asType ();
    }

    Type_Ptr boundFunctionType () override {
        if (numParams () == 0)
            return nullptr;
        Type_Ptr result = NODE (FUNCTION_TYPE_AST, returnType ())->asType ();
        for (int i = 1; i < numParams (); i += 1)
            ADD (result, paramType (i));
        return result;
    }

    FACTORY (Function_Type_AST);

};

EXEMPLAR (Function_Type_AST, FUNCTION_TYPE_AST);


/***** CLASSES *****/

class ClassType_AST: public Type {
protected:

    FACTORY (ClassType_AST);

    AST_Ptr getId () override {
        Decl* me = getDecl ();
        if (me == nullptr)  
            return child (0);
        else
            return me->getId ();
    }

    int numTypeParams () override {
        return arity () - 1;
    }

    Type_Ptr typeParam (int k) override {
        return child (k + 1)->asType ();
    }

    Type_Ptr setTypeParam (int k, Type_Ptr type) override {
        return setChild (k + 1, type)->asType ();
    }

    Decl* getDecl () override {
        return child (0)->getDecl ();
    }

    const Decl_Vect& getDecls () override {
        return child (0)->getDecls ();
    }

    void addDecl (Decl* decl) override {
        child (0)->addDecl (decl);
    }

    AST_Ptr resolveSimpleIds (const Environ* env) override {
        for (auto& c : trimmedAST (*this, 1)) {
            c = c->resolveSimpleIds (env);
        }

        string name = child (0)->text ();
        if (getDecl () != nullptr)
            return this;
        Decl_Vect defns;
        env->find (name, defns);
        if (defns.empty ()) {
            error (this, "undefined identifier: %s", name.c_str ());
            addDecl (makeUnknownDecl (this, true));
        } else if (defns.size () == 1 && defns[0]->isType ()) {
            if (defns[0]->getTypeArity () != numTypeParams ()) {
                error (this, "wrong number (%d) of type parameters",
                       numTypeParams ());
            }
            addDecl (defns[0]);
        } else {
            error (this, "bad type identifier: %s", name.c_str ());
            addDecl (makeUnknownDecl (this, true));
        }
        return this;
    }

    void typeSpecificChecks (AST_Ptr loc) override {
        Type::typeSpecificChecks (loc);
        if (getDecl () == dictDecl) {
            if (!typeParam (0)->isValidAsKey ()) {
                error (loc, "bad key type for dictionary");
            }
        }
    }

    bool isValidAsKey () override {
        Decl* decl = getDecl ();
        return decl == intDecl || decl == strDecl || decl == boolDecl;
    }
};

EXEMPLAR (ClassType_AST, TYPE_AST);

