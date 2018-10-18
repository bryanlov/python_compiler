/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* ast.h: Top-level definitions for abstract syntax trees. */

/* Authors: */

#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/** A type representing source locations.  Source locations are
 *  represented as artificial line numbers. For locations in the first
 *  source file processed, the Location values correspond to the
 *  actual line numbers, starting from 1.  Line numbers then continue
 *  increasing in subsequent files, so that line 1 of the second file
 *  is one greater than the last line of the first.  The functions
 *  locationFileName and locationLine convert Locations back to actual
 *  line numbers and file locations. */
typedef long Location;

/** An enumeration type giving symbolic names of integer indices
 *  representing all AST types. Used for the node factories. */
enum AST_Index {

#define AST_INFO(SYM, NAME) SYM,
#include "ast-types.i"
#undef AST_INFO

};

/** The base type of all nodes in the abstract syntax tree. */
class AST {
    friend AST_Ptr NODE (AST_Index typeIndex);
    friend AST_Ptr LEAF (AST_Index typeIndex, const std::string& text,
                         Location loc);
    friend class AST_Tree;
    friend class subseq<AST>;
public:

    virtual bool isLeaf() {
        return false;
    }

    /** The encoded location associated with this AST. */
    virtual Location location () const;

    /** Set location () to LOC. */
    virtual void setLoc (Location loc);

    /** Return the identifying syntactic category associated with this
     *  node.  This category is used by AST node factories to select
     *  the subtype of AST to use and to access other information
     *  about the node. */
    virtual AST_Index typeIndex () const;
    
    /** Set typeIndex() to SYN. */
    virtual void setTypeIndex (AST_Index syn);

    /** External name of this node's type, as printed by print. */
    virtual const char* astTypeName () const;

    /** My text content.  Generally applicable to leaf nodes only. */
    virtual std::string text () const;

    /** Set text () to TEXT. */
    virtual void setText (const std::string& text);

    /** Append S to the value of text (), if allowed. */
    virtual void appendText (const std::string& s);

    /** The text denoted by THIS, if it is a string literal.  This may
     *  differ from text () in that escape sequences have been replaced
     *  with the actual characters they denote. */
    virtual std::string denotedText () const;

    /** Print my representation as an AST on OUT.  Use INDENT as the
     *  indentation for subsequent lines if my representation takes up
     *  multiple lines. */
    void print (std::ostream& out, int indent);

    /** True if this node represents a type. */
    virtual bool isType ();

    /** True iff this node represents something that has a type (as
     *  opposed to something that represents a type or module). */
    virtual bool hasType ();

    /** For nodes that represent types, return the node with a static
     *  type that reveals its Type operations.  An error on node types
     *  that do not represent types. */
    virtual Type_Ptr asType ();

    /** The number of my children. */
    virtual int arity () const;

    /** My Kth child, numbering from 0. */
    virtual AST_Ptr child (int k) const;

    /** Set my Kth child to C, returning me. */
    virtual AST_Ptr setChild (int k, AST_Ptr c);

    /** My children, as a vector. */
    virtual AST_Vect& children ();
    
    /** Add C as a new rightmost child, returning me. */
    virtual AST_Ptr add (AST_Ptr c);

    /** Add the elements of V as new rightmost children, returning me. */
    virtual AST_Ptr add (const AST_Vect& v);

    /* Methods and types to make ASTs look like STL containers whose
     * contents are the child nodes. */

    typedef AST_Vect::iterator iterator;
    typedef AST_Vect::reverse_iterator reverse_iterator;

    /** A forward iterator through my children. */
    virtual iterator begin ();
    /** A forward iterator to the end of my children. */
    virtual iterator end ();

    /** A reverse iterator through my children. */
    virtual reverse_iterator rbegin ();
    /** A reverse iterator to the beginning of my children. */
    virtual reverse_iterator rend ();

    /* End of container definitions. */

    /** Set NODE's type index to TYPEINDEX and register it as an
     *  exemplar.  Returns an arbitrary integer so that it may be used
     *  in an initializer. */
    static int registerExemplar (AST_Ptr node, AST_Index typeIndex);

    /* Static Semantics */

    /** For nodes representing formal parameters, ids, or attribute
     *  references, the id part.  For types, the class id.  */
    virtual AST_Ptr getId ();

    /** Returns my currently selected Decl, or nullptr if there is none yet
     *  assigned. */
    virtual Decl* getDecl ();

    /** The type of this entity. */
    virtual Type_Ptr getType ();
    
    /** Unify my type with TYPE in SUBST, if possible, and return true
     *  iff successful. */
    virtual bool setType (Type_Ptr type, Unifier& subst);

    /** Returns a view of all of Decls. */
    virtual const Decl_Vect& getDecls ();

    /** Returns the number of Decls I currently have. */
    virtual int getNumDecls ();

    /** Add DECL to my set of Decls. */
    virtual void addDecl (Decl* decl);

    /** Set getDecl () to getDecls ()[k]. */
    virtual void setDecl (int k);

    /** Remove my Kth Decl. */
    virtual void removeDecl (int k);

    /** The set of all declarations reachable from me. */
    Decl_Set reachableDecls ();

    /** Add all declarations reachable from me to DECLS. */
    virtual void reachableDecls (Decl_Set& decls);

    /** Set the "freeze" state of everything defined by a "def" in me to
     *  FROZEN.  The types of these entities will not be freshened if
     *  they are frozen. */
    virtual void freezeDecls (bool frozen);

    /** Replace all type-variable bindings reachable from me according
     *  to SUBST, returning the result. */
    virtual AST_Ptr replaceBindings (const Unifier& subst);

    /** Do outer-level semantic analysis on me---all scope and type
     *  analysis that applies to definitions and statements that are
     *  not nested inside classes or function definitions.  Modifies 
     *  the global environment with any definitions I represent.
     *  Returns the modified tree.  */
    virtual AST_Ptr doOuterSemantics ();

    /** Add any declarations that I represent to ENCLOSING, the
     *  environment in which I am nested.  This does not add
     *  declarations for declarative regions nested within me. */
    virtual void collectDecls (Decl* enclosing);

    /** Add declarations that result from occurrences of type
     *  variables in type attributions to ENCLOSING. */
    virtual void collectTypeVarDecls (Decl* enclosing);

    /** Assuming I am a target of an assignment, add any local
     *  declarations that would result from assignments to me to
     *  ENCLOSING, my enclosing construct.  (Used by overridings of
     *  collectDecls.) */
    virtual void addTargetDecls (Decl* enclosing);

    /** Resolve all simple (non-qualified) identifiers in me, assuming
     *  that ENV defines declarations visible at my outer level.
     *  Returns the resolved node.  This will differ from THIS when it
     *  denotes a type.  */
    virtual AST_Ptr resolveSimpleIds (const Environ* env);
    
    /** Replace any allocators in me with appropriate NEW nodes,
     *  returning the modified node. */
    virtual AST_Ptr resolveAllocators (const Environ* env);

    /** Resolve all selections of the form CLASS.ID by replacing them
     *  with ID, appropriately decorated, assuming that ENV defines
     *  all visible classes.   Returns the modified tree. */
    virtual AST_Ptr resolveStaticSelections (const Environ* env);

    /** Resolve the types of me and my outer-level subcomponents, and resolve
     *  the meanings of unresolved attribute references (OBJ.ID, where
     *  OBJ is not a class, so that the possible meanings of ID depends 
     *  on the type of OBJ).  When finished, all type variables will have
     *  been removed, save those needed for the types of polymorphic
     *  definitions, and all relevant identifiers will have exactly
     *  one declaration attached. */
    virtual void resolveTypesOuter(Decl* context);

    /** Add all overloaded identifier instances in me to RESOLVER. */
    virtual void getOverloadings (Resolver& resolver);

    /** Extend SUBST to resolve all types in me.  CONTEXT denotes my
     *  container.  */
    virtual void resolveTypes (Decl* context, Unifier& subst);

    /** If DOMETHODS, resolve all types in my methods (only).  Otherwise,
     *  resolve types in all other statements. CONTEXT denotes the 
     *  enclosing class. */
    virtual void resolveClassStmtTypes (Decl* context, Unifier& subst,
                                        bool doMethods);

    /** Miscellaneous checks, after types are fully resolved.  Use LOC
     *  as the construct to "blame" in case of error. */
    virtual void typeSpecificChecks (AST_Ptr loc);

    /** Add all Decls used in me to USED. */
    virtual void findUsedDecls (Decl_Set& used);

    /** Check for an outer-level variable whose type still contains
     *  type variables that are not in ALLOWED. */
    virtual void freeTypeVarCheck (const Type_Set& allowed);

    /** Generate code for me.  CONTEXT carries information needed to
     *  perform code generation aside from the AST itself. */

    /* local: 1-local variables, 2-method param list */

    virtual std::string codeGen (Code_Context& context, int local, std::string enclosing);

    virtual std::string getInst (Code_Context& context, int local, std::string enclosing);

    virtual std::string sliceAssign (Code_Context& context, std::string value, int local, std::string enclosing);

    virtual std::string subscriptAssign(Code_Context& context, std::string value, int local, std::string enclosing);

    virtual std::pair<std::string, std::string> getParams(Code_Context& context, int local, std::string enclosing);

protected:

    /** Normally, leave node creation to the factory methods Node and
     *  Leaf. */
    AST ();

    /* The following definition is for implementing a "node factory"
     * function.  The idea is to create one "exemplar" object
     * of each subclass of AST, and to place them in an array indexed
     * by a syntactic category index, unique to each type.  The allows one to
     * write general functions (see Node and Leaf) that, given a
     * syntactic category and sequence of child nodes, can produce a
     * node of the appropriate type by simply looking up the
     * appropriate exemplar by the syntactic category and then calling
     * these methods.
     *
     * See also the macros FACTORY and EXEMPLAR below.
     */

    /** Create a node of my type with no children.  Must be overridden
     *  in each subtype that wants to have an exemplar.  The intent is
     *  that the bodies of such an overriding in class T return new T. */
    virtual AST_Ptr make () const;

    struct AST_Printer {
        AST_Printer (std::ostream& os, int indent);

        std::ostream& os;
        int indent;
        std::unordered_map<AST_Ptr, int> nodeNums;
        std::unordered_set<AST_Ptr> seen;
    };

    /** Finds circular structures in me and numbers them in PRINTER. */
    void findCircular (AST_Printer& printer);

    /** Print my representation as an AST on OUT according to the parameters
     *  in PRINTER.
     *
     *  This is an internal printing method that handles general,
     *  potentially circular graph structures, using standard Common
     *  Lisp notation.  The first time a node beginning a circular path is
     *  encountered, it is prefixed with #N=.  On subsequent
     *  appearances, it is replaced entirely by #N#.
     */
    virtual void print (AST_Printer& printer);
                        
    /** Used by replaceBindings(SUBST).  Replace all type-variable bindings
     *  reachable from me with their referents, except for those
     *  needed to break cycles.  Assumes that the traversal started by
     *  replaceBindings() is currently visiting nodes in VISITING.
     *  Returns the modified tree. */
    virtual AST_Ptr replaceBindings (const Unifier& subst, AST_Set& visiting);

    /** Computes my type, which is then cached by getType(). */
    virtual Type_Ptr computeType ();

private:

    /** My source location. */
    Location loc;
    /** My syntactic index. */
    AST_Index index;
    /** My type, or nullptr if I have none or it has not been determined. */
    Type_Ptr _type;
};

/* Factory macros.  See the definition of Module_AST in stmts.cc, for
 * example, for how to use FACTORY and EXEMPLAR to "register" a class
 * with the system so that an instance of the class may be created by
 * referring to its AST_Index.  */

/** Use this in the class declaration of TYPE to define an appropriate make()
 *  function. */
#define FACTORY(TYPE) TYPE* make () const override { return new TYPE; }

/** Use this to create an exemplar for node type TYPE with the given TYPEINDEX.
 *  It creates a dummy static variable and initializes it in a way that 
 *  creates the exemplar.  */
#define EXEMPLAR(TYPE, TYPEINDEX)                                       \
    static int TYPEINDEX ## _EXEMPLAR = AST::registerExemplar(new TYPE, TYPEINDEX)

typedef subseq<AST> trimmedAST;

class AST_Leaf : public AST {
public:
    bool isLeaf() override;

    std::string text () const override;

    void setText (const std::string& text) override;

    void appendText (const std::string& text) override;

protected:

    void print (AST_Printer& printer) override;

private:

    std::string value;

};
    
class AST_Tree : public AST {
public:

    int arity () const override {
        return kids.size();
    }

    AST_Ptr child (int k) const override {
        return kids.at(k);
    }

    AST_Ptr setChild (int k, AST_Ptr c) override {
        kids.at(k) = c;
        return this;
    }

    AST_Vect& children () override {
        return kids;
    }
    
    AST_Ptr add (AST_Ptr c) override {
        kids.push_back (c);
        if (location () == 0)
            setLoc (c->location ());
        return this;
    }

    AST_Ptr add (const AST_Vect& v) override {
        for (auto c : v)
            add (c);
        return this;
    }

    iterator begin() override {
        return kids.begin();
    }

    iterator end() override {
        return kids.end();
    }

    reverse_iterator rbegin() override {
        return kids.rbegin();
    }

    reverse_iterator rend() override {
        return kids.rend();
    }

    AST_Ptr make () const override {
        return new AST_Tree;
    }

private:

    AST_Vect kids;

};


/* TYPES */

/** The supertype of non-leaf nodes representing expressions that have
 *  types. */

class Typed_Tree : public AST_Tree {
protected:

    Type_Ptr computeType () override;

    bool hasType () override;

};

/** The supertype of all tree nodes that represent types.  We could
 *  instead define type-specific operations in AST, but that clutters
 *  ASTs with operations that apply only to certain subtypes.  This
 *  class is intended to factor out the operations specific to types. */
class Type : public AST_Tree {
public:


    bool isType () override {
        return true;
    }

    /** Overrides AST::asType. Effectively reveals the Type-specific
     *  operations on this node.  */
    Type_Ptr asType () override {
        return this;
    }

    virtual bool isFunctionType () {
        return false;
    }

    /** Reports an erroneous type reference, since types do not have types. */
    Type_Ptr getType () override;

    /** The id_node representing my class.  For function types,
     *  returns a special dedicated ID. nullptr for unbound type
     *  variables and for types that as yet have no Decl node. For
     *  two instances of the same class type, this returns the same
     *  ("canonical") id_node. */
    virtual AST_Ptr getId () override;

    /** My arity, if I am a function type.  Otherwise -1. */
    virtual int numParams ();

    /** The type of my Kth parameter, 0 <= K < numParams(). */
    virtual Type_Ptr paramType (int k);

    /** My return type, if I am a function type.  Otherwise nullptr. */
    virtual Type_Ptr returnType ();

    /** My number of type parameters. */
    virtual int numTypeParams ();

    /** My Kth type parameter, 0 <= K < numTypeParams(),  For a
     *  function type, numTypeParams() == numParams() + 1; the 0th
     *  type parameter is the return type, and the K+1st is 
     *  paramType(K). */
    virtual Type_Ptr typeParam (int k);

    /** Set typeParam(K) to TYPE, for 0 <= K < numTypeParams(). 
     *  Returns this. */
    virtual Type_Ptr setTypeParam (int k, Type_Ptr type);

    /** For function types, the type of the closure on the first
     *  argument. If THIS is (t1, t2, ..., tn)->t0, then the result is
     *  (t2,...,tn)->t0.  Returns nullptr if applied to other kinds of type
     *  or to parameterless function types. If f is a method, then
     *  this is the type of x.f.  */
    virtual Type_Ptr boundFunctionType ();

    /** an environment defining all my attributes. */
    virtual const Environ* getEnviron ();

    /** True iff I am a type variable. */
    virtual bool isTypeVariable ();

    /** Add all type variables in me to TYPEVARS.  Assumes that
     *  VISITED contains nodes that are already processed, and adds
     *  this node and all descendents to VISITED as needed. */
    virtual void getTypeVariables (Type_Set& typeVars, Type_Set& visited);

    /** Replace all type-variable bindings reachable from me according
     *  to SUBST, returning the result. */
    AST_Ptr replaceBindings (const Unifier& subst) override;

    /** Miscellaneous checks of this type.  Use LOC as the location of
     *  any errors. */
    void typeSpecificChecks (AST_Ptr loc) override;

    /** True if I am valid as the first type parameter of a dictionary
     *  type. */
    virtual bool isValidAsKey ();

protected:
    
    AST_Ptr replaceBindings (const Unifier& subst, AST_Set& visiting) override;
};

/** Create a type variable named NAME (which should start with $). */
extern Type_Ptr newTypeVar (const std::string& name);

/** Create a previously unused type variable, decorated with a unique
 *  Decl. */
extern Type_Ptr newTypeVar ();

extern bool unify (Type_Ptr t0, Type_Ptr t1, Unifier& subst);

/** Unify a most-general function type having N formals with TYPE,
 *  returning true iff successful. */
extern bool makeFuncType (int n, Type_Ptr type, Unifier& subst);

/** Return a most-general function type having N formals. */
extern Type_Ptr makeFuncType (int n);

/** Return a most-general method type for a method of class CLAS
 *  having N>=1 formals. */
extern Type_Ptr makeMethodType (int n, Decl* clas);

/** Return a copy of TYPE such that all unbound type variables are 
 *  replaced by fresh unbound variables. */
extern Type_Ptr freshen (Type_Ptr type);

/** Simultaneously freshen all unbound type variables in TYPES,
 *  replacing each with a freshened version. */
extern void freshen (Type_Vect& types);

/** ADD(NODE, CHILD1, ...) appends CHILD1, ... to the children of
 *  AST_Ptr NODE, where each CHILDi is either an AST_Ptr or a vector 
 *  of AST_Ptr, or a pointer to a vector of AST_Ptr. */
inline AST_Ptr
ADD (AST_Ptr node) {
    return node;
}

template<typename... Args>
AST_Ptr
ADD (AST_Ptr node, AST_Ptr c0, Args... rest)
{
    node->add (c0);
    return ADD(node, rest...);
}

template<typename... Args>
AST_Ptr
ADD (AST_Ptr node, const AST_Vect& c0, Args... rest)
{
    node->add (c0);
    return ADD(node, rest...);
}

template<typename... Args>
AST_Ptr
ADD (AST_Ptr node, const AST_Vect* c0, Args... rest)
{
    node->add (*c0);
    return ADD(node, rest...);
}

/** NODE(TYPEINDEX, CHILD1, ...) returns an AST_Ptr whose type index is
 *  TYPEINDEX and whose children are CHILD1, ..., where each CHILDi is 
 *  either an AST_Ptr or a vector of AST_Ptr, or a pointer to a 
 *  vector of AST_Ptr. */
extern AST_Ptr NODE(AST_Index typeIndex);

template<typename... Args>
AST_Ptr
NODE(AST_Index typeIndex, Args... rest)
{
    AST* r = NODE (typeIndex);
    return ADD (r, rest...);
}

/** A new AST leaf node, whose type is identified by TYPEINDEX.  TEXT
 *  is the semantic content of the node. LOC is its source location. */
extern AST_Ptr LEAF (AST_Index typeIndex, const std::string& text,
                     Location loc = 0);

/* Other functions. */

/** Create a new ID token whose text is TEXT, and whose location is LOC. */
extern AST_Ptr makeId (const char* text, Location loc);
extern AST_Ptr makeId (const std::string& text, Location loc);


/** Used below. */
template<class T> struct unref     { typedef T type; };
template<class T> struct unref<T&> { typedef T type; };

#endif

