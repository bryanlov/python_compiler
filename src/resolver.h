/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* resolver.h: Tool for systematically exploring overloadings. */

/* Authors: P. N. Hilfinger */

#ifndef _RESOLVER_H_
#define _RESOLVER_H_

#include <vector>
#include <utility>
#include "apyc.h"

/** A mapping of Id nodes with multiple Decls to specific Decls,
 *  allowing a systematic exploration of all possible resolutions. 
 *  Initially, contains no overloadings. */
class Resolver {
public:
    /** Add ID to the set of overloaded id instances, setting
     *  ID->getDecl () to the first overloading of ID. */
    void addId (AST_Ptr id);

    /** The number of overloaded id instances that have been added to
     *  me. */
    int numOverloadings () const;

    /** The identifier instance associated with the Ith overloading, where
     *  0 <= i < numOverloadings (). */
    AST_Ptr overloading (int i);

    /** The Decl within overloading (i) as determined by this Resolver. */
    Decl* getDecl (int i);

    /** Modify the current set of overloaded IDs to get the next
     *  combination of possible overloadings.  Returns false and
     *  restores all identifiers to their default (first) overloadings
     *  iff there are no more candidates.  Assumes that the getDecl ()
     *  values of my overloaded IDs have not been altered since the
     *  last call to nextResolution or setDecls. */
    bool nextResolution ();

    /** Set the overloadings of my identifiers to my current
     *  selections. */
    void setDecls ();

private:
    /** The sequence of (ID, K) pairs giving the overloaded IDs and
     *  the current declaration numbers selected for them. */
    std::vector< std::pair<AST_Ptr, int> > _overloadings;
};

#endif
