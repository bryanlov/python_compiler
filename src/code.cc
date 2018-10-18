/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; * -*- */

/* code.cc: Implementation of the Code_Context class and
 * code-generation utilties. */

#include <iostream>
#include <iomanip>
#include "apyc.h"

using namespace std;

Code_Context::Code_Context ()
    : _indentation (0), _bol (true)
{ }


/* Code emission support. */

Code_Context&
Code_Context::operator<< (const string& code)
{
    if (_bol)
        cout << setw (_indentation) << "";
    _bol = false;
    cout << code;
    return *this;
}

Code_Context& 
Code_Context::operator<< (std::ostream& (*manip) (std::ostream&))
{
    cout << manip;
    if (manip == (ostream& (*)(ostream&)) endl)
        _bol = true;
    return *this;
}

Code_Context& 
Code_Context::operator<< (std::ios& (*manip)(std::ios&))
{
    cout << manip;
    return *this;
}
    
Code_Context& 
Code_Context::operator<< (std::ios_base& (*manip)(std::ios_base&))
{
    cout << manip;
    return *this;
}

void
Code_Context::indent (int indenting)
{
    _indentation += 4 * indenting;
}
