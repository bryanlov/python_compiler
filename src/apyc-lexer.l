/* -*- mode: fundamental; c-file-style: "stroustrup"; indent-tabs-mode: nil -*- */

/* apyc-lexer.l: Flex Lexer */

/* Authors: YOUR NAMES HERE */

  #define YY_DECL int yylex_raw() 

%x BOL

%option yylineno noyywrap

BLANK    [ \t\f]
DEC      [0-9]
OCT      [0-7]
HEX      [0-9a-fA-F]
IDENT    [a-zA-Z_][a-zA-Z0-9_]*
ALPHANUM [a-zA-Z_0-9]
SSIT1    [^'\\\n\r]|\\[^\r\n]|\\\r?\n
SSIT2    [^"\\\n\r]|\\[^\r\n]|\\\r?\n
LSIT1    [^'\\]|\\[^\n\r]|\\\r?\n|''?[^'\\\n\r]|''?\\[^\n\r]|''?\\\r?\n
LSIT2    [^"\\]|\\[^\n\r]|\\\r?\n|\"\"?[^"\\\n\r]|\"\"?\\[^\n\r]|\"\"?\\\r?\n

  static int processBOL (const char* indenting);
  /** Nesting level of current source position in '[', '(', and '{'
   *  brackets. */
  static int bracketDepth;

%%

  /* Rules in effect at the start of a line. */

<BOL>{BLANK}*(#.*)?\r?\n {   } 
<BOL>{BLANK}*.        { yyless (yyleng - 1); 
                        BEGIN INITIAL; 
                        int syntax = processBOL (yytext);
                        if (syntax != 0) {
                            return syntax;
                        }
                      }

  /* Normal rules. */

  /* Normally, throw away comments and whitespace other than newline. */
{BLANK}+          |
#.*               |
\\\r?\n           { }

\r?\n             { 
                    if (bracketDepth == 0) {
                        BEGIN BOL; return NEWLINE; 
                    }
                  }

    /* Integer literals. */

2147483648           |
0+20000000000        |
0[oO]0*20000000000   |
0[xX]0*80000000        {
           yylval.str = new string (yytext);
           return BIGINT;
   }

0{OCT}*              |
0[xX]{HEX}+          |
[1-9]{DEC}*          {
           yylval.str = new string (yytext);
           return INT;
   }

  /* Error cases with extraneous characters. */

[0-9]{ALPHANUM}+  { 
          error (currentLocation (), "trailing garbage in numeral");
    }

    /* Strings */

[rR]?'{SSIT1}*'      |
[rR]?\"{SSIT2}*\"    |
[rR]?'''{LSIT1}*'''  |
[rR]?\"\"\"{LSIT2}*\"\"\" {
          yylval.str = new string (yytext);
          return STRING;
    }

  /* Error cases */
[rR]?'{SSIT1}*      |
[rR]?\"{SSIT2}*     {
          error (currentLocation (), "unterminated string literal");
}
[rR]?'''{LSIT1}*  |
[rR]?\"\"\"{LSIT2}* {
          error (currentLocation (), "unterminated long string literal");
}


      /* Single-character tokens. */

  /* Single-character operators. */
[-+*/%<>]         { return yytext[0]; }

  /* Brackets. */
[(\[{]    { bracketDepth += 1; return yytext[0]; }
[)\]}]    { 
            if (bracketDepth > 0) bracketDepth -= 1; 
            return yytext[0];
          }

  /* Other single-character delimiters */
[,;:=.]   { return yytext[0]; }

  /* Single-characters tokens not in our subset. */
[&|^~@]           { error (currentLocation (), 
                           "'%s' is not in our subset", yytext);
                  }

      /* Multi-character delimiters. */

  /* Operators and punctuation. */

"<="      { return LE; }
">="      { return GE; }
"=="      { return EQ; }
"!="      { return NE; }
"::"      { return COLONCOLON; }
"->"      { return FUNCARROW; }
"//"      { return '/'; }
"**"      { return POW; }

  /* Multi-character delimiters not in our subset. */
"<<"      |
">>"      |
"+="      |
"-="      |
"*="      |
"/="      |
"//="     |
"&="      |
"|="      |
"^="      |
"%="      |
"<>"      { error (currentLocation (), 
                   "'%s' is not in our subset", yytext);
          }

  /* Keywords and identifiers. */

and	  { return AND; }
break	  { return BREAK; }
class	  { return CLASS ; }
continue  { return CONTINUE; }
def	  { return DEF; }
elif      { return ELIF; }
else      { return ELSE; }
False	  { return FALSE; }
for	  { return FOR; }
if	  { return IF; }
in        { return IN; }
is        { return IS; }
native	  { return NATIVE; }
None	  { return NONE; }
not	  { return NOT; }
of        { return OF; }
or	  { return OR; }
pass      { return PASS; }
print	  { return PRINT; }
return    { return RETURN; }
True	  { return TRUE; }
while	  { return WHILE; }

  /* Keywords not in our subset. */

as	|
assert	|
del	|
except	|
exec	|
from	|
finally	|
future	|
global	|
import	|
lambda	|
raise	|
try	|
with	|
yield	  {
            error (currentLocation (), "'%s' is not in our subset", yytext);
          }


{IDENT}   {
             yylval.str = new string (yytext);
             return ID;
          }

"$"{IDENT} {
             yylval.str = new string (yytext);
             return TYPE_VAR;
           }

<INITIAL,BOL><<EOF>>    { 
             int syntax = processBOL (yytext);
             if (syntax != 0) {
                 return syntax;
             } else {
                 return 0;
             }
           }

               

%%

#include <algorithm>

using namespace std;

static vector<string> sourceFileNames;
static vector<int> firstLines;
static int pendingDedents;
static vector<int> indentationLevels;
                   
bool
initLexer (const string& fileName)
{
    FILE* input = fopen (fileName.c_str (), "r");
    if (input == NULL) {
        return false;
    }

    firstLines.push_back (yylineno);
    sourceFileNames.push_back (fileName);
    yyin = input;
    indentationLevels.clear ();
    indentationLevels.push_back (0);
    pendingDedents = 0;
    bracketDepth = 0;
    BEGIN BOL;
    return true;
}

int
yylex ()
{
    if (pendingDedents > 0) {
        pendingDedents -= 1;
        return DEDENT;
    }

    int result = yylex_raw();
    yylloc = yylineno;
    return result;
}

string
currentFileName ()
{
    return sourceFileNames.back ();
}

Location
currentLocation ()
{
    return yylineno;
}

string
locationFileName (Location loc)
{
    if (loc == 0) {
        return "?";
    } else {
        for (size_t i = 1; i < sourceFileNames.size(); i += 1) {
             if (firstLines[i] > loc) {
                 return sourceFileNames[i - 1];
             }
         }
         return sourceFileNames.back ();
    }
}

Location
locationLine (Location loc)
{
    if (loc == 0) {
        return 0;
    } else {
        for (size_t i = 1; i < sourceFileNames.size(); i += 1) {
             if (firstLines[i] > loc) {
                 return loc - firstLines[i - 1] + 1;
             }
         }
         return loc - firstLines.back () + 1;
    }
}

/** Return the number of blanks equivalent to INDENTING, according to
 *  Python's rules. */
static int
blankLen (const char* indenting)
{
    int k;
    k = 0;
    while (*indenting == '\f') {
        indenting += 1;
    }

    for (const char* c = indenting; *c != 0; c += 1) {
        if (*c != '\t') {
            k += 1;
        } else {
            k = (k + 8) & ~7;
        }
     }
     return k;
}

/** Compute the appropriate INDENT or DEDENT tokens indicated by the prefix
 *  INDENTATION, a string of blanks and tabs. Returns the first token to
 *  return---INDENT, DEDENT, or 0 if no token is to be returned because
 *  indentation has not changed from the previous statement.  Saves the
 *  remaining number of DEDENTs to return in subsequent calls to yylex. */
static int
processBOL (const char* indenting)
{
    int indentation = blankLen (indenting);
    if (indentation == indentationLevels.back ()) {
        return 0;
    } else if (indentation > indentationLevels.back ()) {
        indentationLevels.push_back (indentation);
        return INDENT;
    } else {
        vector<int>::iterator match = 
            find (indentationLevels.begin (), indentationLevels.end (),
                  indentation);
        if (match == indentationLevels.end ()) {
            error (currentLocation (), "inconsistent indentation");
            return 0;
        }
        pendingDedents = indentationLevels.end () - match - 2;
        indentationLevels.erase (match + 1, indentationLevels.end ());
        return DEDENT;
    }
}
