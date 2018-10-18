/* -*- mode: fundamental; c-file-style: "stroustrup"; indent-tabs-mode: nil -*- */

/* apyc: Bison Parser */

/* Authors: Paul N. Hilfinger */

%{
    #include <iostream>
    #include <string>
    #include "apyc.h"
    #include "ast.h"
    #include <cassert>

    using namespace std;

    static AST_Ptr theTree;

    /* Utilities */

    /** Make a new "unop" node with function name TOKEN, operand EXPR1, and
     *  location LOC. */
    static AST_Ptr makeUnopExpr (const char* token, AST_Ptr expr1,
                                 Location loc);

    /** Make a new "binop" node with function name TOKEN, and operands EXPR1
     *  and EXPR2. */
    static AST_Ptr makeBinopExpr (const char* token,
                                  AST_Ptr expr1, AST_Ptr expr2);

    /** Type used for locations (@1, @2, etc.). */
    #define YYLTYPE Location

    #define YYLLOC_DEFAULT(Current, Rhs, N)                                 \
        do  {                                                               \
           if (N) {                                                         \
               Current = YYRHSLOC(Rhs, 1);                                  \
           } else {                                                         \
               Current = YYRHSLOC(Rhs, 0);                                  \
           }                                                                \
        } while (0)                                                         \

    static void yyerror (const char* s);

    static int yylex ();

%}

%locations
%debug

/* A token declaration such as
 *   %token INDENT
 * declares a new integer constant INDENT, which serves as a syntactic category
 * (suitable for output by the lexer) identifying a kind of token, which we
 * also often refer to as INDENT. A declaration such as
 *   %token IF "if"
 * defines a constant IF and makes it the syntactic category for the token
 * "if".  The built-in function yyexternal_token_name(...) maps between
 * syntactic categories (like IF) and the token strings defined in these
 * %token declarations (like "if").
 *
 */

/* Lexer tokens and nonterminal instances that are referred to by $$, $1,
 * $2, etc. have a semantic value, whose type is declared by the <...>
 * notations in %token and %type declarations.  The names in
 * the <...> brackets refer to members of the %union declaration (see below).
 * Symbols whose semantic value is never referenced do not need types.
 */

%token<str> ID STRING INT BIGINT TYPE_VAR

%token NEWLINE INDENT DEDENT

%token POW "**"
%token GE ">="
%token LE "<="
%token EQ "=="
%token NE "!="
%token IN "in"
%token IS "is"
%token COLONCOLON "::"
%token FUNCARROW "->"

%token AND "and"
%token BREAK "break"
%token CLASS  "class"
%token CONTINUE "continue"
%token DEF "def"
%token ELIF "elif"
%token ELSE "else"
%token FALSE "False"
%token FOR "for"
%token IF "if"
%token NATIVE "native"
%token NONE "None"
%token NOT "not"
%token OF "of"
%token OR "or"
%token PASS "pass"
%token PRINT "print"
%token RETURN "return"
%token TRUE "True"
%token WHILE "while"

%left "or"
%left "and"
%left "not"
%left '<' '>' "==" ">=" "<=" "!=" "is" "in"
%left '+' '-'
%left '%' '*' '/'
%left UNOP
%right "**"
%left BIGINT

%type<ast> assignment_stmt atom attributeref
%type<ast> break_stmt
%type<ast> call classdef compound_stmt
%type<ast> conditional_expression continue_stmt
%type<ast> dict_display
%type<ast> elifs else_clause enclosure
%type<ast> expr1 expr2 expr3 expression expression_or_list expression_stmt
%type<ast> for_stmt funcdef funcname
%type<ast> high_bound
%type<ast> identifier if_stmt
%type<ast> key_datum
%type<ast> left_compare list_display literal low_bound
%type<ast> native
%type<ast> parameter parameter_list parameterized_type parameters1
%type<ast> parenth_form primary print_stmt
%type<ast> return_stmt
%type<ast> simple_stmt slice slice_target statement stringliteral
%type<ast> subscript subscript_target suite
%type<ast> target target_list target_list2 type
%type<ast> type_annotation type_formals type_var
%type<ast> while_stmt

%type<cstr> comp_operator

%type<astVect> classbody expression_list1 expression_list_opt_comma funcbody 
%type<astVect> key_datum_list1
%type<astVect> statements stmt_list suite_stmts
%type<astVect> types type_list type_params type_var_list1

/* The following declaration provides names for possible semantic types
 * passed by the compiler.  It actually translates to a C++ union type.
 * This is like a class, but its fields are mutually exclusive.  For
 * technical reasons, all these types must be "POD (Plain Old Data) types";
 * they can't have constructors or other fancy C++ features.  Therefore,
 * they are all numeric types or pointer types. */

%union {
    std::string* str;
    AST_Ptr ast;
    AST_Vect* astVect;
    const char* cstr;
}

/* GLR parsing is useful for cases where the parser must make a reduction before
 * it has sufficient information to know which reduction to make.  For example,
 * consider a statement that begins
 *      a[i], b[j], c
 * (it's not useful, but is valid Python as it stands).  As a tuple, this
 * consists of a list of expressions.  a[i] is a subscription, and corresponds
 * to a (subscript ...) AST.  But if we have
 *      a[i], b[j], c = L
 * then the a[i] should create a subscript_assign.  If you try to handle this
 * by having two rules for subscription, one for the first case and one
 * for the second, the parser has to decide which to choose just after
 * seeing a[i], but long before seeing "=", which is how one tells which
 * translation is appropriate.  A GLR parser will hold off on this decision
 * until is reaches the =. */

%glr-parser

/* If you use the GLR features, Bison will report conflicts.  Without GLR,
 * a conflict generally means an error.  With GLR, it could mean an error,
 * or it could indicate conflicts that the GLR algorithm will correctly resolve.
 * There's really no way to know without testing.  When you get conflicts
 * THAT YOU EXPECT you can record here these expectations by reporting how
 * many reduce/reduce (%expect-rr) and shift-reduce (%expect) you expect.
 * Before increasing these counts, make sure you understand what causes your
 * conflicts and that GLR will resolve them.  Don't expect too many
 * and try to remove conflicts rather than tolerating them.
 * A conflict will occur when there is a situation where Bison cannot decide
 * on the basis of the input so far whether to perform a reduction.  The
 * conflict is OK if it will become clear later on in the input what should
 * have been done. */
%expect-rr 16
%expect 0

%%

/* See the ast.h file for declarations of NODE and LEAF.  NODE is for
 * creating AST nodes that have 0 or more children and no string content,
 * while LEAF is for creating childless nodes whose principal semantic
 * content is a string or integer value. */

program:
      %empty    { theTree = NODE (MODULE_AST); }
    | statements
                { theTree = NODE (MODULE_AST, $1); }
    ;

statements:
      statement { $$ = new AST_Vect(1, $1); }
    | statements statement
                { $$ = $1; $$->push_back($2); }
    ;

atom: identifier | literal | enclosure
    ;

identifier: ID  { $$ = makeId (*$1, @1); }
    ;

literal:
      stringliteral
    | INT       { $$ = LEAF(INT_LITERAL_AST, *$1, @1); }
    | "True"    { $$ = NODE (TRUE_AST); $$->setLoc(@1); }
    | "False"   { $$ = NODE (FALSE_AST); $$->setLoc(@1); }
    | "None"    { $$ = NODE (NONE_AST); $$->setLoc(@1); }
    ;

stringliteral:
      STRING    { $$ = LEAF(STRING_LITERAL_AST, *$1, @1); }
    | stringliteral STRING
                { $$ = $1; $$->appendText(*$2); }
    ;

enclosure: parenth_form | list_display | dict_display
    ;

parenth_form:
      '(' ')'   { $$ = NODE (TUPLE_AST); }
    | '(' expression_or_list ')'
                { $$ = $2; }
    ;

list_display: '[' expression_list_opt_comma ']'
                { $$ = NODE (LIST_DISPLAY_AST, $2); $$->setLoc (@1); }
    ;

dict_display:
      '{' key_datum_list1 opt_comma '}'
                { $$ = NODE (DICT_DISPLAY_AST, $2); $$->setLoc (@1); }
    | '{' '}'   { $$ = NODE (DICT_DISPLAY_AST); $$->setLoc (@1); }
    ;

key_datum_list1:
      key_datum { $$ = new AST_Vect(1, $1); }
    | key_datum_list1 ',' key_datum
                { $$ = $1; $$->push_back ($3); }
    ;

key_datum: expression ':' expression
                { $$ = NODE (PAIR_AST, $1, $3); }
    ;

primary: 
      atom
    | attributeref
    | subscript
    | slice
    | call
    | error BIGINT
            { $$ = LEAF (INT_LITERAL_AST, "0", @2); }
    ;

attributeref:
      primary '.' identifier
                { $$ = NODE (ATTRIBUTEREF_AST, $1, $3); }
    | parameterized_type '.' identifier
                { $$ = NODE (ATTRIBUTEREF_AST, $1, $3); }
    ;

subscript: primary '[' expression_or_list ']'
      	        { $$ = NODE (SUBSCRIPT_AST,
                             makeId("__getitem__", $1->location ()),
                             $1, $3); }
    ;

slice: primary '[' low_bound ':' high_bound ']'
                { $$ = NODE (SLICE_AST,
                             makeId ("__getslice__", $1->location ()),
                             $1, $3, $5); }
    ;

subscript_target: primary '[' expression_or_list ']'
                { $$ = NODE (SUBSCRIPT_ASSIGN_AST,
                             makeId ("__setitem__", $1->location ()),
                             $1, $3, NODE (NEXT_VALUE_AST));
                }
    ;

slice_target : primary '[' low_bound ':' high_bound ']'
                { AST_Ptr obj = $1, low = $3, high = $5;
                  $$ = NODE (SLICE_ASSIGN_AST,
                             makeId ("__setslice__", $1->location ()),
                             obj, low, high, NODE (NEXT_VALUE_AST)); 
                }
    ;

low_bound:
      expression
    | %empty
                { $$ = LEAF (INT_LITERAL_AST, "0", 0); }
    ;

high_bound:
      expression
    | %empty
                { $$ = LEAF (INT_LITERAL_AST, "0x3fffffff", 0); }
    ;

call: primary '(' expression_list_opt_comma ')'
                { $$ = NODE (CALL_AST, $1, $3); }
    | parameterized_type '(' expression_list_opt_comma ')'
                { $$ = NODE (CALL_AST, $1, $3); }
    ;

expression:
      conditional_expression
    ;

conditional_expression:
      expr1 "if" expr1 "else" expression
                { $$ = NODE (IF_EXPR_AST, $3, $1, $5);
                  $$->setLoc($1->location ()); }
    | expr1
    ;


expr1:
      expr2
    | expr1 "or" expr1
                { $$ = NODE(OR_AST, $1, $3); }
    | expr1 "and" expr1
                { $$ = NODE(AND_AST, $1, $3); }
    | "not" expr1
                { $$ = makeUnopExpr ("__not__", $2, @1); }
    ;

expression_or_list:
      expression
    | expression ','
                { $$ = NODE (TUPLE_AST, $1); }
    | expression ',' expression_list1 opt_comma
                { $$ = NODE (TUPLE_AST, $1, $3); }
    ;

comp_operator:
      '>' 	{ $$ = "__gt__"; }
    | '<' 	{ $$ = "__lt__"; }
    | ">=" 	{ $$ = "__ge__"; }
    | "<=" 	{ $$ = "__le__"; }
    | "==" 	{ $$ = "__eq__"; }
    | "!=" 	{ $$ = "__ne__"; }
    | "in" 	{ $$ = "__in__"; }
    | "is" 	{ $$ = "__is__"; }
    | "not" "in"
                { $$ = "__notin__"; }
    | "is" "not"
                { $$ = "__isnot__"; }
    ;

expr2:
      left_compare comp_operator expr3
                { $$ = NODE (COMPARE_AST, makeId($2, @1), $1, $3); }
    | expr3 comp_operator expr3
                { $$ = NODE (COMPARE_AST, makeId($2, @1), $1, $3); }
    | expr3
    ;

left_compare:
      expr3 comp_operator expr3
                { $$ = NODE (LEFT_COMPARE_AST, makeId($2, @1), $1, $3); }
    | left_compare comp_operator expr3
                { $$ = NODE (LEFT_COMPARE_AST, makeId($2, @1), $1, $3); }
    ;

expr3:
      primary
    | '-' BIGINT
                { $$ = LEAF(INT_LITERAL_AST, "-" + *$2, @1); }
    | '-' expr3 %prec UNOP
                { $$ = makeUnopExpr ("__neg__", $2, @1); }
    | '+' expr3 %prec UNOP
                { $$ = makeUnopExpr ("__pos__", $2, @1); }
    | expr3 '+' expr3
                { $$ = makeBinopExpr ("__add__", $1, $3); }
    | expr3 '-' expr3
                { $$ = makeBinopExpr ("__sub__", $1, $3); }
    | expr3 '*' expr3
                { $$ = makeBinopExpr ("__mul__", $1, $3); }
    | expr3 '/' expr3
                { $$ = makeBinopExpr ("__floordiv__", $1, $3); }
    | expr3 '%' expr3
                { $$ = makeBinopExpr ("__mod__", $1, $3); }
    | expr3 "**" expr3
                { $$ = makeBinopExpr ("__pow__", $1, $3); }
    ;

simple_stmt:
      expression_stmt
    | assignment_stmt
    | print_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
    ;

expression_stmt: expression_or_list
    ;

assignment_stmt:
      target_list '=' expression_or_list
                { $$ = NODE (ASSIGN_AST, $1, $3); }
    | target_list '=' assignment_stmt
                { $$ = NODE (ASSIGN_AST, $1, $3); }
    ;

target_list:
      target
    | '(' target ')'
                { $$ = $2; }
    | target ','
                { $$ = NODE (TARGET_LIST_AST, $1); }
    | '(' target ',' ')'
                { $$ = NODE (TARGET_LIST_AST, $2); }
    | '[' target opt_comma ']'
                { $$ = NODE (TARGET_LIST_AST, $2); $$->setLoc (@1); }
    | target_list2 opt_comma
                { $$ = $1; }
    | '(' target_list2 ')'
                { $$ = $2; $$->setLoc (@1); }
    | '[' target_list2 opt_comma ']'
                { $$ = $2; $$->setLoc (@1); }
    ;

target_list2:
      target ',' target
                { $$ = NODE (TARGET_LIST_AST, $1, $3); }
    | target_list2 ',' target
                { $$ = $1; $$->add ($3); }
    ;

target:
      identifier            /* Requires GLR for resolution vs atom */
    | identifier type_annotation
                { $$ = NODE (TYPED_ID_AST, $1, $2); }
    | attributeref          /* Requires GLR for resolution vs. primary */
    | subscript_target      /* Requires GLR for resolution vs. primary */
    | slice_target          /* Requires GLR for resolution vs. primary */
    ;

print_stmt:
      "print" expression_list1
                { $$ = NODE (PRINTLN_AST, $2); $$->setLoc (@1); }
    | "print" 
                { $$ = NODE (PRINTLN_AST); $$->setLoc (@1); }
    | "print" expression_list1 ','
                { $$ = NODE (PRINT_AST, $2); $$->setLoc (@1); }
    ;

return_stmt: "return" expression_or_list
                { $$ = NODE (RETURN_AST, $2); $$->setLoc (@1); }
    | "return"
                { $$ = NODE (RETURN_AST, NODE (NONE_AST));
                  $$->setLoc (@1); 
                }
    ;

break_stmt: "break" 
                { $$ = NODE (BREAK_AST); $$->setLoc (@1); }
    ;

continue_stmt: "continue"
                { $$ = NODE (CONTINUE_AST); $$->setLoc (@1); }
    ;

compound_stmt:
      if_stmt
    | while_stmt
    | for_stmt
    | funcdef
    | classdef
    ;

suite:
      suite_stmts
                { $$ = NODE (STMT_LIST_AST, $1); }
    ;

suite_stmts:
      stmt_list NEWLINE
                { $$ = $1; }
    | NEWLINE INDENT statements DEDENT
                { $$ = $3; }
    | error NEWLINE
                { $$ = new AST_Vect; }
    | NEWLINE INDENT error DEDENT
                { $$ = new AST_Vect; }
    ;

statement:
      stmt_list NEWLINE
                { $$ = NODE (STMT_LIST_AST, $1); }
    | compound_stmt
    ;

stmt_list:
      "pass" opt_semi
                 { $$ = new AST_Vect; }
    | simple_stmt opt_semi
                 { $$ = new AST_Vect (1, $1); }
    | "pass" ';' stmt_list
                 { $$ = $3; }
    | simple_stmt ';' stmt_list
                 { $$ = $3; $$->insert ($$->begin (), $1); }
    ;

if_stmt:
      "if" expression ':'
         suite
      elifs
                { $$ = NODE (IF_AST, $2, $4, $5);
                  $$->setLoc (@1);
                }
    ;

elifs:
      "elif" expression ':' suite elifs
                { $$ = NODE (IF_AST, $2, $4, $5); }
    | else_clause
    ;

else_clause:
      %empty   { $$ = NODE (STMT_LIST_AST); }
    | "else" ':' suite
                { $$ = $3; }
    ;

while_stmt:
      "while" expression ':'
           suite
      else_clause
                { $$ = NODE (WHILE_AST, $2, $4, $5); $$->setLoc (@1); }
    ;

for_stmt:
      "for" target_list "in" expression_or_list ':'
          suite
      else_clause
                { $$ = NODE (FOR_AST, $2, $4, $6, $7); $$->setLoc (@1); }
    ;

funcdef:
      "def" funcname parameter_list type_annotation ':'
           funcbody
                { $$ = NODE (DEF_AST, $2, $3, $4, $6);
                  $$->setLoc (@1);
                }
    | "def" funcname parameter_list ':'
           funcbody
                { $$ = NODE (DEF_AST, $2, $3, newTypeVar(), $5);
                  $$->setLoc (@1);
                }
    ;

funcbody:
      suite_stmts
    | native    { $$ = new AST_Vect (1, $1); }
    | NEWLINE INDENT native DEDENT
                { $$ = new AST_Vect (1, $3); }
    ;

native:
      "native" stringliteral opt_semi NEWLINE
                { $$ = NODE (NATIVE_AST, $2); $$->setLoc (@1); }
    ;

parameter_list:
      '(' ')'
                { $$ = NODE (FORMALS_LIST_AST); }
    | '(' parameters1 opt_comma ')'
                { $$ = $2; $$->setLoc (@1); }
    ;

parameters1:
      parameter { $$ = NODE (FORMALS_LIST_AST, AST_Vect(1, $1)); }
    | parameters1 ',' parameter
                { $$ = $1; $$->add ($3); }
    ;

parameter: 
      identifier
    | identifier type_annotation  { $$ = NODE (TYPED_ID_AST, $1, $2); }
    ;

funcname:  identifier ;

classdef: "class" identifier type_formals ':'
               classbody
                { $$ = NODE (CLASS_AST, $2, $3, $5);
                  $$->setLoc (@1);
                }
    ;

type_formals:
      %empty    { $$ = NODE (TYPE_FORMALS_LIST_AST); }
    | "of" '[' type_var_list1 ']'
                { $$ = NODE (TYPE_FORMALS_LIST_AST, $3);
                  $$->setLoc (@1);
                }
    ;

type_var_list1:
      type_var  { $$ = new AST_Vect (1, $1); }
    | type_var_list1 ',' type_var
                { $$ = $1; $$->push_back ($3); }

classbody:
      suite_stmts
    ;

type_annotation:
      "::" type { $$ = $2; }
    ;

type:
      parameterized_type
    | identifier
                { $$ = NODE (TYPE_AST, $1); }
    | type_var
    | '(' type_list ')' "->" type
                { $$ = NODE (FUNCTION_TYPE_AST, $5, $2);
                  $$->setLoc (@1);
                }
    ;

type_var:
      TYPE_VAR  { $$ = newTypeVar (*$1); $$->setLoc (@1); }
    ;

parameterized_type:
      identifier type_params { $$ = NODE (TYPE_AST, $1, $2); }
    ;

type_params:
      "of" '[' types ']'
                { $$ = $3; }
    | "of" type { $$ = new AST_Vect (1, $2); }
    ;


type_list:
      %empty    { $$ = new AST_Vect; }
    | types
    ;

types:
      type      { $$ = new AST_Vect (1, $1); }
    | types ',' type
                { $$ = $1; $1->push_back ($3); }
    ;

expression_list1:
      expression        { $$ = new AST_Vect(1, $1); }
    | expression_list1 ',' expression
                        { $$ = $1; $$->push_back ($3); }
    ;

expression_list_opt_comma:
      %empty            { $$ = new AST_Vect; }
    | expression_list1 opt_comma
                        { $$ = $1; }
    ;

opt_comma:
      %empty
    | ','
    ;

opt_semi:
      %empty
    | ';'
    ;

%%

bool debugParser;

#include "apyc-lexer.cc"

/** Make a new "unop" node with function name TOKEN, operand EXPR1, and
 *  location LOC. */
static AST_Ptr
makeUnopExpr (const char* token, AST_Ptr expr1, Location loc)
{
    return NODE (UNOP_AST, makeId(token, loc), expr1);
}

/** Make a new "binop" node with function name TOKEN, and operands EXPR1
 *  and EXPR2. */
static AST_Ptr
makeBinopExpr (const char* token, AST_Ptr expr1, AST_Ptr expr2)
{
    return NODE (BINOP_AST, makeId(token, expr1->location ()), expr1, expr2);
}

/* The following routines are used in a post-parse pass over the tree to detect
 * out-of-place constructs and to replace def nodes with method nodes
 * (for project 2) when appropriate. */

static AST_Ptr checkKids (AST_Ptr, bool, bool, bool, bool);
static AST_Ptr checkPlacement (AST_Ptr, bool, bool, bool, bool);
static void checkPlacement (AST_Ptr);


/** Apply checkPlacement (below) to each child of TREE, replacing the
 *  children's values, and returning TREE. */
static AST_Ptr
checkKids (AST_Ptr tree, bool inLoop, bool inFunc, bool inControl,
           bool inClass)
{
    for (auto& c : *tree) {
        c = checkPlacement (c, inLoop, inFunc, inControl, inClass);
    }
    return tree;
}

/** Helper for checkPlacement(TREE), which follows.  Performs the
 *  checks indicated there on TREE assuming that
 *  A. TREE is in a loop construct iff INLOOP.
 *  B. TREE is in a function definition iff INFUNC.
 *  C. TREE is in a compound control construct ('if', loop), iff INCONTROL.
 *  D. TREE is immediately within a class definition (and not in a compound
 *     statement nested within that class definition) iff INCLASS.
 *
 *  Returns TREE, suitably modified if needed. 
 */
static AST_Ptr
checkPlacement (AST_Ptr tree, bool inLoop, bool inFunc, bool inControl,
                bool inClass)
{
    assert (tree != NULL);
    switch (tree->typeIndex ()) {
    default:
        return checkKids (tree, inLoop, inFunc, inControl, inClass);
    case WHILE_AST: case FOR_AST:
        return checkKids (tree, true, inFunc, true, false);
    case IF_AST:
        return checkKids (tree, inLoop, inFunc, true, false);
    case DEF_AST: 
        if (inControl) {
            error (tree->location (), "illegally nested def statement");
        }
        checkKids (tree, false, true, false, false);
        if (inClass) {
            return NODE (METHOD_AST, tree->children ());
        } else {
            return tree;
        }
    case CLASS_AST:
        if (inClass || inControl || inFunc) {
            error (tree->location (), "illegally nested class definition");
        }
        return checkKids (tree, false, false, false, true);
    }
}

/** Check that constructs appear only in appropriate contexts in TREE.
 *
 *  1. Checks that
 *    A. break and continue occur only in loops.
 *    B. return occurs only in a functions.
 *    C. class definitions are not nested in functions, other class
 *       definitions, or compound statements.
 *    D. def statements may not be nested in compound control statements.
 *
 *  2. Converts DEF nodes to METHOD nodes in classes.
 *
 *  While these checks could be performed in the parser, it seems less
 *  obscure perform them as a post-pass over the tree.
 *
 *  Assumes that TREE is a module node.
 */
static void
checkPlacement (AST_Ptr tree)
{
    checkPlacement (tree, false, false, false, false);
}
    
AST_Ptr
parse (FILE* f, const string& name)
{
    if (debugParser)
        yydebug = 1;
    initLexer (name);
    theTree = NULL;
    yyparse ();
    if (theTree != NULL) {
        checkPlacement (theTree);
    }
    return theTree;
}

static void
yyerror (const char* s)
{
    if (yychar == BIGINT)
        error (currentLocation (), "integer literal out of range");
    else
        error (currentLocation (), s);
}



