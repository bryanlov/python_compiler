/* -*- mode: C++; c-file-style: "stroustrup"; -*- */

/* environ.cc: Environments. */

/* Author:  Paul N. Hilfinger */

#include <cassert>
#include <string>
#include "apyc.h"

using namespace std;

const Environ* outerEnviron;
const Environ* theEmptyEnviron = new Environ (nullptr);

Environ::Environ (const Environ* enclosing)
    : enclosure (enclosing) 
{
}

Environ::Environ (const Environ* environ0, const Environ* enclosing) 
    : members (environ0->members), enclosure (enclosing)
{
}

Decl* 
Environ::findImmediate (const std::string& name) const
{
    for (auto d : members) {
	if (name == d->getName ())
	    return d;
    }
    return nullptr;
}

void
Environ::findImmediate (const std::string& name, Decl_Vect& defns) const
{
    defns.clear ();
    for (auto d : members) {
	if (name == d->getName ())
	    defns.push_back (d);
    }
}
    

Decl*
Environ::find (const std::string& name) const
{
    Decl* d = findImmediate (name);
    if (d != nullptr)
        return d;
    else if (enclosure == nullptr)
	return nullptr;
    else
        return enclosure->find (name);
}

void
Environ::find(const std::string& name, Decl_Vect& defns) const
{
    findImmediate(name, defns);
    if (!defns.empty () || enclosure == nullptr)
	return;
    enclosure->find(name, defns);
}

void 
Environ::define (Decl* decl)
{
    members.push_back (decl);
}

const Environ* 
Environ::get_enclosure () const
{
    return enclosure;
}

const Decl_Vect&
Environ::get_members () const
{
    return members;
}

/* DEBUGGING */

void
DB (const Environ* env)
{
    if (env == nullptr) {
        fprintf (stderr, "nullptr\n");
    } else {
        const char* label;
        label = "Immediate";
        while (env != nullptr) {
            const Decl_Vect& members = env->get_members ();
            fprintf (stderr, "%s:\n", label);
            for (auto member : members) {
                fprintf (stderr, "   %s @%p\n", member->getName ().c_str (),
                         member);
            }
            env = env->get_enclosure ();
            label = "Enclosed by";
        }
    }
}

