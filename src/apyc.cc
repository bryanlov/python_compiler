/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* apyc: A PYthon Compiler. */

/* Authors:  YOUR NAMES HERE */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdarg>
#include <libgen.h>
#include <iostream>
#include <unistd.h>
#include "apyc.h"

using namespace std;

/** Cumulative error count. */
int errCount;
/** True when error messages are to be printed.  Turned off to allow
 *  overloading resolution without messages. */
static bool reportErrors;

/** Type of file generated by the code generator, in the form of a
 *  file extension.  Set to ".c" (for C), ".s" (for assembler), 
 *  or ".cc" (for C++), as appropriate. */
static const char* INTERMEDIATE_EXT = ".cc";

/** Location of library containing standard prelude and files for
 *  final compilation. */
static string LIBDIR;

static void
error1 (Location loc, const char* format, va_list ap)
{
    if (reportErrors) {
        if (loc != -1) {
            fprintf (stderr, "%s:%ld: ",
                     locationFileName (loc).c_str (), locationLine (loc));
        }
        vfprintf (stderr, format, ap);
        fprintf (stderr, "\n");
    }
    va_end (ap);
    errCount += 1;
}

void
fatal (const char* format, ...)
{
    va_list ap;
    reportErrors = true;
    va_start (ap, format);
    error1 (-1, format, ap);
    exit (1);
}

void
error (Location loc, const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    error1 (loc, format, ap);
}

void
error (AST_Ptr place, const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    error1 (place->location (), format, ap);
}

void
error_no_file (const char* format, ...) 
{
    va_list ap;
    va_start (ap, format);
    error1 (-1, format, ap);
}

int
numErrors ()
{
    return errCount;
}

int
setNumErrors (int num)
{
    int count0 = errCount;
    errCount = num;
    return count0;
}

bool
setErrorReporting (bool on)
{
    bool reportErrors0 = reportErrors;
    reportErrors = on;
    return reportErrors0;
}

static void
Usage ()
{
    fatal ("Usage: apyc --phase=(1|2) [ -dp ] [ -o OUTFILE ] DIR/BASE.py\n");
    exit (1);
}

static AST_Ptr
phase1 (const string& inFileName)
{
    FILE* inFile = fopen (inFileName.c_str (), "r");
    if (inFile == NULL) {
        error_no_file ("Could not open %s", inFileName.c_str ());
        exit (1);
    }
    AST_Ptr result = parse (inFile, inFileName);
    fclose (inFile);
    return result;
}

static AST_Ptr
phase2 (AST_Ptr tree)
{
    return tree->doOuterSemantics ();
}

static void
phase4 (const string& codeFile, const string& execFile)
{
    string command;
    command = "c++ -std=c++11 -I " + LIBDIR + " -o " + execFile
                        + " " + codeFile + " " + LIBDIR + "/runtime.cc";
    int exitCode = system (command.c_str ());
    if (exitCode != 0)
        error_no_file ("compilation errors in final assembly");
}

static void
setOutputFile(const string& fileName, const string& baseName,
              const char* ext)
{
    string outFileName;
    if (fileName == "") {
        outFileName = baseName + ext;
    } else {
        outFileName = fileName;
    }

    if (freopen (outFileName.c_str (), "w", stdout) == nullptr) {
        error_no_file ("could not write to %s", outFileName.c_str ());
        exit (1);
    }
}

/** The name of a temporary file suitable for holding generated
 *  source code.  There is a race condition, but it is not likely to
 *  matter too much. */
static string
tempOutFileName ()
{
    char name[L_tmpnam];
    strcpy (name, "apycXXXXXX");
    strcat (name, INTERMEDIATE_EXT);
    close (mkstemps (name, strlen (INTERMEDIATE_EXT)));
    return name;
}

int
main (int argc, char* argv[])
{
    int i;
    int phase;
    string outFileName;
    string preludeName;

    phase = 4;
    outFileName = "";
    LIBDIR = string(dirname(realpath(argv[0], nullptr))) + "/lib";
    preludeName = LIBDIR + "/prelude.py";
    setErrorReporting (true);

    for (i = 1; i < argc; i += 1) {
        string opt = argv[i];
        if (opt == "-o" && i < argc-1) {
            outFileName = argv[i+1];
            i += 1;
        } else if (opt.compare (0, 8, "--phase=") == 0)
            phase = atoi(opt.c_str () + 8);
        else if (opt == "-dp") 
            debugParser = true;
        else if (opt == "-S")
            phase = 3;
        else if (opt.size () == 0 || opt[0] == '-')
            Usage();
        else
            break;
    }
    errCount = 0;

    if (i != argc - 1 || phase < 1 || phase > 4)
        Usage ();

    string infileName = argv[i];
    if (infileName.size () < 4 
        || infileName.compare (infileName.size ()-3, 3, ".py") != 0) {
        error_no_file ("Unknown file type: %s\n", argv[i]);
        exit (1);
    }
    string baseFileName = string (infileName, 0, infileName.size () - 3);

    AST_Ptr tree;

    if (phase >= 1)
        tree = phase1 (infileName);

    if (errCount == 0 && phase >= 2) {
        AST_Ptr prelude = phase1(preludeName);
        
        if (errCount == 0) {
            tree = post1 (tree, prelude);
            tree = phase2 (tree);
        }
    }

    if (errCount == 0) {
        assert(tree != nullptr);
        switch (phase) {
        default:
            break;

        case 1:
            setOutputFile (outFileName, baseFileName, ".ast");
            tree->print (cout, 0);
            break;

        case 2:
            setOutputFile (outFileName, baseFileName, ".dast");
            tree->print (cout, 0);
            outputDecls (cout, tree->reachableDecls ());
            break;

        case 3: {
            Code_Context context;
            setOutputFile (outFileName, baseFileName, INTERMEDIATE_EXT);
            tree->codeGen (context, 0, "");
            break;
        }

        case 4:
            Code_Context context;
            string output = tempOutFileName ();
            setOutputFile (output, "", "");
            tree->codeGen (context, 0, "");
            if (errCount == 0) {
                phase4 (output,
                        outFileName == "" ? baseFileName : outFileName);
                remove (output.c_str ());
            }
            break;
        }
    }

    exit (errCount == 0 ? 0 : 1);
}
