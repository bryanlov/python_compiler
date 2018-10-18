/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* post1.cc: Preliminary modifications of the AST received from phase 1. */

/* Authors:  P. N. Hilfinger */

#include "apyc.h"

using namespace std;

static AST_Ptr reviseTree(AST_Ptr tree, bool classp);
static AST_Ptr makeTypedId(AST_Ptr formal);

/** Assuming that PROG is the AST for a program produced by the parser
 *  compiler phase 1 and PRELUDE is the AST for the stsndard prelude
 *  produced by compiler phase 1, returns the result of destructively
 *  applying the following transformations:
 *
 *  1. The statements of PRELUDE are prepended to those of PROG.
 *  2. All "def" nodes that represent methods are converted to
 *     "method" nodes, which have the same children as defs.
 *  3. All missing return types are replaced by fresh anonymous type variables.
 *  4. All formal parameters are given fresh anonymous type variables as formal
 *     types.
 *  5. All "block" and type_list nodes are removed, and their children
 *     transferred to the enclosing node.
 *
 *  Changes 2-5 are changes that could be made in the parser itself, but may
 *  not have been.
 */ 

AST_Ptr
post1 (AST_Ptr prog, AST_Ptr prelude)
{
    prelude->add (prog->children ());
    prelude->setLoc (0);
    return reviseTree(prelude, false);
}

/** Perform phase 1 modifications (see phase1 above) on TREE,
 *  returhing modified tree. CLASSP is true iff TREE is a child
 *  of a class node block. */
static AST_Ptr
reviseTree (AST_Ptr tree, bool classp)
{
    if (tree == nullptr)
        return nullptr;

    switch (tree->typeIndex ()) {
    default:
        for (auto& c : *tree) {
            c = reviseTree (c, false);
        }
        break;

    case CLASS_AST:
        for (auto& c : *tree) {
            c = reviseTree (c, true);
        }
        break;

    case BLOCK_AST:
        for (auto& c : *tree) {
            c = reviseTree (c, classp);
        }
        break;

    }
        
    switch (tree->typeIndex ()) {
    default:
        return tree;

    case IF_AST:
    case WHILE_AST:
        if (tree->arity () < 3) {
            tree->add (NODE (STMT_LIST_AST));
        }
        return tree;

    case FOR_AST:
        if (tree->arity () < 4) {
            tree->add (NODE (STMT_LIST_AST));
        }
        return tree;

    case DEF_AST: {
        if (tree->arity () == 2 
            || tree->child (tree->arity () - 1)->typeIndex () == BLOCK_AST
            || !tree->child (2)->isType ()
            || (classp && tree->typeIndex () != METHOD_AST)) {

            int k;
            AST_Ptr newTree = NODE(classp ? METHOD_AST : DEF_AST, 
                                   tree->child (0), tree->child(1));
            newTree->setLoc (tree->location());

            if (tree->arity () == 2 || !tree->child (2)->isType ()) {
                newTree->add (newTypeVar ());
                k = 2;
            } else {
                newTree->add (tree->child (2));
                k = 3;
            }

            while (k < tree->arity ()) {
                if (tree->child (k)->typeIndex () == BLOCK_AST) {
                    newTree->add (tree->child (k)->children ());
                } else {
                    newTree->add (tree->child (k));
                }
                k += 1;
            }
            
            return newTree;
        }
        return tree;
    }

    case CLASS_AST: case FUNCTION_TYPE_AST: case TYPE_AST: {
        AST_Ptr last = tree->child (tree->arity () - 1);
        if (last->typeIndex () == BLOCK_AST 
            || last->typeIndex () == TYPE_LIST_AST) {
            AST_Ptr newTree = NODE(tree->typeIndex ());
            newTree->setLoc (tree->location ());
            for (int k = 0; k < tree->arity () - 1; k += 1) {
                newTree->add (tree->child (k));
            }
            newTree->add (last->children ());
            return newTree;
        }
        return tree;
    }

    case FORMALS_LIST_AST:
        for (auto& c : *tree) {
            c = makeTypedId (c);
        }
        return tree;
        
    }

}

/** Return FORMAL (an id or typed id) as a typed id.  This is the
 *  identity on TYPE_ID nodes.  It returns a TYPED_ID with a fresh
 *  type variable for plain ID nodes. */
static AST_Ptr
makeTypedId (AST_Ptr formal)
{
    switch (formal->typeIndex ()) {
    default:
        return formal;
    case ID_AST:
        return NODE (TYPED_ID_AST, formal, newTypeVar ());
    }
}

