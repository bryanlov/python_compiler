/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* ast.h: Top-level definitions for abstract syntax trees. */

/* Authors: P. N. Hilfinger */

#ifndef _ENVIRON_H_
#define _ENVIRON_H_

class Environ {
public:

    /** An initialiy empty environment enclosed in ENCLOSING. */
    Environ (const Environ* enclosing);

    /** An environment initialized to the (immediate) contents of 
     *  ENVIRON0, enclosed in enclosing. */
    Environ (const Environ* environ0, const Environ* enclosing);

    /** An entry for NAME, or nullptr if none.  Does not search enclosing
     *  Environs. */
    Decl* findImmediate (const std::string& name) const;

    /** Set DEFNS to a list of all my entries for NAME.  Does not search
     *  enclosing Environs. */
    void findImmediate (const std::string& name, Decl_Vect& defns) const;

    /** An entry for NAME in me or enclosing Environs, or nullptr if
     *  none. */
    Decl* find (const std::string& name) const;

    /** Set DEFNS to all definitions of NAME in the innermost Environ
     *  in me or my enclosing Environs that has any.  DEFNS is set
     *  empty if it is nowhere defined. */
    void find (const std::string& name, Decl_Vect& defns) const;

    /** Define DECL in me by its name.  */
    void define (Decl* decl);

    /** The environment enclosing me (passed to my constructor), if
     *  any. */
    const Environ* get_enclosure () const;

    /** A view of my members (not including enclosing Environs). */
    const Decl_Vect& get_members () const;

private:

    Decl_Vect members;
    const Environ* enclosure;
};

/** The current outer (__main__) environment */
extern const Environ* outerEnviron;

/** A constant empty environment. */
extern const Environ* theEmptyEnviron;

#endif
