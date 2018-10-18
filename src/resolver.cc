/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; * -*- */

/* resolver.cc: Implementation of the Resolver class. */

#include "apyc.h"

void
Resolver::addId (AST_Ptr id)
{
    if (id->getNumDecls () <= 1)
        return;
    for (auto p : _overloadings) {
        if (p.first == id)
            return;
    }
    _overloadings.emplace_back (id, _overloadings.size () == 0 ? -1 : 0);
    id->setDecl (0);
}

int
Resolver::numOverloadings () const
{
    return _overloadings.size ();
}

bool
Resolver::nextResolution ()
{
    for (auto& p : _overloadings) {
        if (p.second + 1 < p.first->getNumDecls ()) {
            p.second += 1;
            p.first->setDecl (p.second);
            return true;
        } else {
            p.second = 0;
            p.first->setDecl (0);
        }
    }
    return false;
}

AST_Ptr
Resolver::overloading (int i)
{
    return _overloadings.at (i).first;
}

Decl* 
Resolver::getDecl (int i)
{
    auto entry = _overloadings.at (i);
    return entry.first->getDecls ()[entry.second];
}

void
Resolver::setDecls () {
    for (auto p : _overloadings) {
        p.first->setDecl (p.second);
    }
}

