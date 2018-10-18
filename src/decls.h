/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* decls.h: Definitions for Decls, which represent declarations. */

/* Authors: P. N. Hilfinger */

#ifndef _DECLS_H_
#define _DECLS_H_

#include "ast.h"

/* Decls */

/** A declaration node, containing semantic information about a 
 *  program entity.  Subtypes of Decl refer to local variables,
 *  parameters, global variables, defined functions, methods, constants,
 *  modules, and classes. */
class Decl {
public:

    Decl (AST_Ptr id, Decl* container, Environ* members = NULL);
        
    /** My index value (if I appear explicitly), and otherwise -1 to
     *  indicate that I am unindexed. */
    int getIndex () const { return _index; }

    std::string getName () const { return _id->text (); }

    AST_Ptr getId () const { return _id; }

    /** Print THIS on OUT if it is in USED. */
    virtual void print(std::ostream& out, const Decl_Set& used) const;

    /** Print THIS on the standard output if it is in USED. */
    virtual void print (const Decl_Set& used) const;

    /** Get my container (null if none). */
    virtual Decl* getContainer () const { return _container; }

    /** Get the type of the entity I represent. */
    virtual Type_Ptr getType () const;

    /** Get my type if selected from an object, as in x.f, where x is
     *  not a class. */
    virtual Type_Ptr getSelectedType () const;

    /** My associated AST, if any.  A Decl may be associated with a
     *  particular AST that corresponds to the declaration of the
     *  entity it declares.  */
    virtual AST_Ptr getAst () const;

    /** Set getAst() to AST. */
    virtual void setAst (AST_Ptr ast);

    /** True iff I am an overloadable kind of entity (defined by def). */
    virtual bool isOverloadable () const;

    /** True iff I represent a method. */
    virtual bool isMethod () const;

    /** True iff I represent a type. */
    virtual bool isType () const;

    /** True iff I represent an entity with a type. */
    virtual bool hasType () const;

    /** True iff I represent an internally generated type variable
     *  (which should not be included in the output list). */
    virtual bool isInternal () const;

    /** A Type representing me with no type parameters. */
    virtual Type_Ptr asType () const;

    /** A Type representing me with type parameters PARAMS. */
    virtual Type_Ptr asType (const AST_Vect& params) const;

    /** A Type representing me with type parameters PARAMS. */
    virtual Type_Ptr asType (const Type_Vect& params) const;

    /** A Type representing me with the type parameters supplied at my
     *  definition. */
    virtual Type_Ptr asBaseType () const;

    /** A Type representing me with generic type parameters. */
    virtual Type_Ptr asGenericType ();

    /** If I am type, returns the number of type arguments. */
    virtual int getTypeArity () const;

    /** Get my position (in a parameter list, indexed from 0). */
    virtual int getPosition () const;

    /** A view of all member Decls I contain (modules, classes). */
    virtual const Environ* getEnviron () const;

    /** Add DECL to the end of my list of member Decls. */
    virtual void addMember (Decl* new_member);

    /** Return a declaration of ID appropriate for a variable declaration to
     *  my members. */
    virtual Decl* newVarDecl (AST_Ptr id);

    /** Create and add a declaration of ID appropriate for a variable
     *  declaration to my members.  There must not already be one. Returns the
     *  resulting declaration */
    virtual Decl* addVarDecl (AST_Ptr id);

    /** Return a declaration of ID appropriate for a 'def' with K
     *  parameters to my members. */
    virtual Decl* newDefDecl (AST_Ptr id, int k);

    /** Create and add a declaration of ID appropriate for a 'def'
     *  with K parameters to my members.  Returns the resulting declaration. */
    virtual Decl* addDefDecl (AST_Ptr id, int k);

    /** Add a new parameter declaration for ID, returning the declaration.
     *  K is the index of this parameter in its formal parameter list.
     *  If the parameter declaration already exists, indicate an error
     *  and return the previous declaration. */
    virtual Decl* addParamDecl (AST_Ptr id, int k);

    /** Add a type variable declaration for ID, if it does not already
     *  exist.  Return either the previously existing declaration or
     *  the new one, as appropriate. */
    virtual Decl* addTypeVarDecl (AST_Ptr id);

    /** True iff the entity I represent can be assigned to or defined
     *  by assignment. */
    virtual bool assignable () const;

    /** True iff my type is currently "frozen", meaning that getType is
     *  not to freshen type variables in it.  Everything in a class is
     *  frozen until processing of the class definition is finished.
     *  Likewise for outer-level functions. */
    virtual bool isFrozen () const;

    /** If my freezing status is mutable, set it to FROZEN.
     *  Otherwise, does nothing. */
    virtual void setFrozen (bool frozen);

    /** Replace all type-variable bindings reachable from me according
     *  to SUBST. */
    virtual void replaceBindings (const Unifier& subst);

    /** Add me to the possible Decls of ID (an ID node) iff I am a
     *  class that contains ID as an attribute (method or instance
     *  variable). */
    virtual void addAttributeDecls (AST_Ptr id);

protected:

    /** The name of this type of Decl for external printing purposes. */
    virtual const char* declTypeName () const;

    /** Print my container's index on OUT, if
     *  applicable, and otherwise do nothing. */
    virtual void printContainer (std::ostream& out) const;

    /** Print my position in my container, if applicable, on OUT,
     *  and otherwise do nothing. */
    virtual void printPosition (std::ostream& out) const;

    /** Print my type on OUT, if applicable, and  otherwise do nothing. */
    virtual void printType (std::ostream& out) const;

    /** Print my list of type parameters on OUT,
     *  if applicable, and otherwise do nothing. */
    virtual void printTypeParams (std::ostream& out) const;

    /** Print my index list of members or local definitions that
     *  appear in USED on OUT, if applicable, and otherwise do nothing. */
    virtual void printMembersList (std::ostream& out,
                                   const Decl_Set& used) const;

protected:
    bool _frozen;

private:
    int _index;
    const AST_Ptr _id;
    Decl* _container;
    Environ* _members;
    AST_Ptr _ast;
};

/** Declaration of local or module-level variable for ID, defined
 *  immediately inside function or module CONTAINER and having type
 *  TYPE. */
extern Decl* makeVarDecl (AST_Ptr id, Decl* container, AST_Ptr type);
/** Declaration of parameter #K (numbered from 0) named for ID in function FUNC
 *  and of type TYPE. */
extern Decl* makeParamDecl (AST_Ptr id, Decl* func, int k, AST_Ptr type);
/** Declaration of an instance variable named for ID of type TYPE 
 *  in class CLAZZ. */
extern Decl* makeInstanceDecl (AST_Ptr id, Decl* clazz, AST_Ptr type);
/** Declaration of type variable for ID. */
extern Decl* makeTypeVarDecl (AST_Ptr id);
/** Declaration of function for ID of type TYPE, nested inside the
 *  function or module CONTAINER. */
extern Decl* makeFuncDecl (AST_Ptr id, Decl* container, AST_Ptr type);
/** Declaration for a class for ID with ARITY type parameters. */
extern Decl* makeClassDecl (AST_Ptr id, AST_Ptr params);
/** Declaraton for the module named ID.  This always has sequence number 0. */
extern Decl* makeModuleDecl (AST_Ptr id);

/** Declaration for an unknown entity: intended to provide a non-null
 *  dummy declaration to use for erroneous references. If ISTYPE, the
 *  Decl is marked as a type. */
extern Decl* makeUnknownDecl (AST_Ptr id, bool isType);

/** If DECL represents a declaration of a builtin class, set the
 *  appropriate global variable to point to it. */
extern void setBuiltinDecl (Decl* decl);

/** Output all Decl nodes in USED to OUT. */
extern void outputDecls (std::ostream& out, const Decl_Set& used);

/** Decls for built-in types. */
extern Decl* intDecl;
extern Decl* listDecl;
extern Decl* tupleDecl[];
extern Decl* strDecl;
extern Decl* dictDecl;
extern Decl* boolDecl;
extern Decl* rangeDecl;

/** The declaration of the main module. */
extern Decl* mainModule;

#endif
