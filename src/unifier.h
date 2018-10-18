/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* unifier.h: Tool for recording substitutions. */

/* Authors: P. N. Hilfinger */

#ifndef _UNIFIER_H_
#define _UNIFIER_H_

#include <unordered_map>
#include "apyc.h"

/** A Unifier maps Types to Types (as pointers).  The *binding* of a
 *  type T in Unifier U (denoted U[T]) is either T itself (meaning that T is
 *  *unbound*), or an unbound type object.  Binding a type object T1
 *  to T2 in U creates a new unifier U' with U'[T1'] = U[T2], for all
 *  T1' such that U[T1'] = T1.  Unifiers can bind any type objects,
 *  not just type variables. */
class Unifier {
public:

     /** The ultimate binding of OBJ.  This will be OBJ itself if it
      *  is not bound in this Unifier. */
    Type_Ptr operator[] (Type_Ptr obj) const;

     /** Bind OBJ to TARGET in me.  Return myself. */
     Unifier& bind (Type_Ptr obj, Type_Ptr target);

     /** True iff OBJ is bound in me. */
     bool isBound (Type_Ptr obj) const {
          return (*this)[obj] != obj;
     }
      
    /** Swaps my contents with OTHER. */
    void swap (Unifier& other);

private:

    mutable std::unordered_map<Type_Ptr, Type_Ptr> binding;

};

#endif
