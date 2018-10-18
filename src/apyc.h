/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* Interfaces for apyc. */

#ifndef _APYC_H_
#define _APYC_H_

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/* Forward type declarations. */

/* ast.h */

class AST;
class AST_Leaf;
class AST_Tree;
class Type;

typedef AST* AST_Ptr;
typedef Type* Type_Ptr;

typedef std::unordered_set<AST_Ptr> AST_Set;
typedef std::unordered_set<Type_Ptr> Type_Set;
typedef std::unordered_map<AST_Ptr, AST_Ptr> AST_Map;
typedef std::unordered_map<Type_Ptr, Type_Ptr> Type_Map;

typedef std::vector<AST_Ptr> AST_Vect;
typedef std::vector<Type_Ptr> Type_Vect;

/* decls.h */

class Decl;

typedef std::vector<Decl*> Decl_Vect;
typedef std::unordered_set<Decl*> Decl_Set;

/* unifier.h */

class Unifier;

/* environ.h */

class Environ;

/* resolver.h */

class Resolver;

/* code.h */

class Code_Context;

#define UNIMPLEMENTED \
    throw logic_error (string("unimplemented method: ") +  __func__)

/** A wrapper around objects for which begin() returns a random-access
 *  iterator. If sub is a subseq wrapping seq, then sub.begin () and 
 *  sub.end () access elements START and END.  */
template <class T>
class subseq {
public:
    subseq (T& obj, int start, int stop = -1) :
        obj (obj), start (start), stop (stop)
    { }

    decltype (T().begin ()) begin () {
        return obj.begin () + start;
    }

    decltype(T().end ()) end () {
        if (stop == -1)
            return obj.end ();
        else
            return obj.begin () + stop;
    }

private:
    T& obj;
    int start, stop;
};

#include "ast.h"
#include "unifier.h"
#include "environ.h"
#include "decls.h"
#include "resolver.h"
#include "code.h"
#include <exception>

/** Print an error message in standard Unix format:
 *      <filename>:<linenum>: <message>
 *  where <filename> and <linenum> denote the file and line number
 *  containing source location LOC, and <message> is composed from
 *  FORMAT and the trailing arguments as
 *  for the printf family. */
extern void error (Location loc, const char* format, ...);

/** Short for error (place->location (), format, ...). */
extern void error (AST_Ptr place, const char* format, ...);

/** Print an error message (without file or line number indications)
 *  composed from FORMAT and the trailing arguments as for the printf
 *  family. */
extern void error_no_file (const char* format, ...);

/** Report a fatal error and exit. */
extern void fatal (const char* format, ...);

/** Set printing of error reports to true iff ON is true.  When off, 
 *  numErrors() increments, but nothing is printed.  Use this to tell
 *  if a certain choice of overloaded declarations works without
 *  giving the usual message when it doesn't.  Returns previous
 *  setting. */
extern bool setErrorReporting (bool on);

/** The cumulative error count. */
extern int numErrors ();

/** Reset the cumulative error count to NUM. By saving numErrors before
 *  processing a subtree, turning off printing of error messages, and 
 *  then resetting it numErrors to its initial value, you can tell if a
 *  certain set of overloading choices (for example) worked without
 *  complaining if it doesn't.  Returns the previous value of
 *  numErrors(). */
extern int setNumErrors (int num);

/** Return a value that encodes the current line number and location
 *  in the source files. */
extern Location currentLocation ();

/** Given an encoded source location (as produced by currentLocation),
 *  return the name of the source file containing that location. */
extern std::string locationFileName (Location loc);

/** Given an encoded source location (as produced by currentLocation),
 *  return the line number in the source file containing that location. */
extern Location locationLine (Location loc);

/** (Re-) initialize the lexer to start reading from source file
 *  FILENAME.   Returns true on success, false if FILENAME can't be
 *  read. */
extern bool initLexer (const std::string& fileName);

extern AST_Ptr parse (FILE*, const std::string&);

extern AST_Ptr post1 (AST_Ptr prog, AST_Ptr prelude);

extern bool debugParser;

#endif

