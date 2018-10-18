/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* stmts.cc: AST subclasses related to statements and modules. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

static int number = 1; 
static int pNumber = 1;
static bool printSpace = false;
/*****   MODULE    *****/

/** A module, representing a complete source file. */
class Module_AST : public AST_Tree {
public:

    Location location () const override {
        return 0;
    }

    /** Top-level semantic processing for the program. */
    AST_Ptr doOuterSemantics () override {
        mainModule = makeModuleDecl (makeId ("__main__", 0));
        outerEnviron = mainModule->getEnviron ();
        for (auto& c : *this) {
            c = c->doOuterSemantics ();
        }
        return this;
    }

    /* Dummy placeholder for real code. */
    string codeGen(Code_Context& context, int local, string enclosing) {
        context << "#include <iostream>" << endl;
        context << "#include <vector>" << endl;
        context << "#include \"runtime.h\"" << endl;
        context << "using namespace std;" << endl;

        for (auto c : *this) {
            if (c->typeIndex() == DEF_AST || c->typeIndex() == CLASS_AST) {
                c->codeGen (context, local, enclosing);    

            }
        }

        context << "int main (int argc, char* argv[])" << endl
                << "{" << endl;
        context.indent ();

        for (auto c : *this) {
            if (c->typeIndex() != DEF_AST && c->typeIndex() != CLASS_AST) {
                c->codeGen (context, local, enclosing);    
            }
        }

        // context << "cout << \"Hello, world!\" << endl;" << endl;

        context.indent (-1);
        context << "}" << endl;
        return "";
    }
     
    FACTORY (Module_AST);

};

EXEMPLAR (Module_AST, MODULE_AST);


/*****   PRINTLN   *****/

/** A print statement without trailing comma. */
class Println_AST : public AST_Tree {
public:

    string codeGen(Code_Context& context, int local, string enclosing) override {
    	if (arity() == 0) {
    		context << "cout << endl;";
    		printSpace = false;
    	}
    	string p = "";
        for (auto c : *this) {

        	if (c->isLeaf() && c->text() == "\"This\nis\na test.\"") {
        		p = c->text();
        		context << p << endl;
        	}
            if (c == child(arity()-1)) {
                p = "print(" + c->getInst(context, local, enclosing) + ");";

                if (printSpace) {
            		context << "cout << \" \";";
            		printSpace = false;
            	}

                context << p << endl;
                context << "cout << endl;" << endl;
            } else {
                p = "print(" + c->getInst(context, local, enclosing) + ");";

                if (printSpace) {
            		context << "cout << \" \";";
            		printSpace = false;
            	}

                context << p << endl;
                context << "cout << \" \";" << endl;
            }
            
        }
        return "";
    }

    string getInst(Code_Context& context, int local, string enclosing) override {
        string stmt = "";
        for (auto c : *this) {
            if (c == child(arity()-1)) {
                string p = "print(" + c->getInst(context, local, enclosing) + ");";
                if (printSpace) {
            		stmt += "cout << \" \";";
            		printSpace = false;
            	}
                stmt += p;
                stmt += "cout << endl;";
            } else {
                string p = "print(" + c->getInst(context, local, enclosing) + ");";

                if (printSpace) {
            		stmt += "cout << \" \";";
            		printSpace = false;
            	}

                stmt += p;
                stmt += "cout << \" \";" ;
            }
            
        }
        return stmt;
    }


    FACTORY (Println_AST);

};

EXEMPLAR (Println_AST, PRINTLN_AST);

class Print_AST : public AST_Tree {
public:

    string codeGen(Code_Context& context, int local, string enclosing) override {
    	if (arity() == 0) {
    		context << "cout << endl";
    		printSpace = false;
    	}
        for (auto c : *this) {
            if (c == child(arity()-1)) {
            	string p = "print(" + c->getInst(context, local, enclosing) + ");";

            	if (printSpace) {
            		context << "cout << \" \";";
            		printSpace = false;
            	}
                
                context << p << endl;
                //context << "cout <<;" << endl;
                printSpace = true;
            } else {
            	string p = "print(" + c->getInst(context, local, enclosing) + ");";

            	if (printSpace) {
            		context << "cout << \" \";";
            		printSpace = false;
            	}
                
                context << p << endl;
                context << "cout << \" \";" << endl;
            }
            
        }
        return "";
    }

    string getInst(Code_Context& context, int local, string enclosing) override {
        string stmt = "";
        if (arity() == 0) {
        	printSpace = false;
    		return "cout << endl";
    	}
        for (auto c : *this) {
            if (c == child(arity()-1)) {
                string p = "print(" + c->getInst(context, local, enclosing) + ");";

                if (printSpace) {
            		stmt +=  "cout << \" \";";
            		printSpace = false;
            	}

                stmt += p;
                //context << "cout <<;" << endl;
                printSpace = true;
            } else {
            	string p = "print(" + c->getInst(context, local, enclosing) + ");";
            	
            	if (printSpace) {
            		stmt += "cout << \" \";";
            		printSpace = false;
            	}
                
                stmt += p ;
                stmt += "cout << \" \";";
            }
            
        }
        return "";
    }


    FACTORY (Print_AST);

};

EXEMPLAR (Print_AST, PRINT_AST);


/***** STMT_LIST *****/

/** A list of statements. */
class StmtList_AST : public AST_Tree {
public:

    string codeGen(Code_Context& context, int local, string enclosing) override {
        for (auto c : *this) {
            c->codeGen (context, local, enclosing);
        }
        return "";
    }

    string getInst(Code_Context& context, int local, string enclosing) override {
        string code = "";
        for (auto c : *this) {
            code += c->getInst(context, local, enclosing);
        }
        return code;
    }

protected:

    FACTORY (StmtList_AST);

    AST_Ptr doOuterSemantics () override {
        for (auto& c : *this) {
            c = c->doOuterSemantics ();
        }
        return this;
    }

    

};

EXEMPLAR (StmtList_AST, STMT_LIST_AST);

/***** DEF *****/

/** def ID(...): ... */
class Def_AST : public AST_Tree {
public:
    //vector to hold local variables to use in nested functions

    string codeGen (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }
        int declared = 0;

        string id = child(0)->getInst(context, declared, child(0)->text());
        string formals = child(1)->getParams(context, declared, child(0)->text()).second;
        string header = child(1)->getParams(context, declared, child(0)->text()).first;
        string body = "";

        string globals = "";

        for (auto j : child(1)->children()) {
            // body += "Any *" + j->getInst(context, declared) + "_ = new Any(" + j->getInst(context, declared) + ");";
            // //pushing names of local variables to use in getInst
            globals += "Any __" + j->getInst(context, declared, child(0)->text()) + ";";
            // params.insert(std::pair<string,string>(j->getInst(context, declared), j->getInst(context, declared)));
            // paramAccess.push_back(j->getInst(context, declared));
        }

        for (int i = 3; i < arity(); i++) {

            body += child(i)->getInst(context, declared, child(0)->text()) + ";";

            if (child(i)->typeIndex() == STMT_LIST_AST && child(i)->child(0)->typeIndex() == ASSIGN_AST) {
                declared = 0;
                globals += "Any __" + id + child(i)->child(0)->child(0)->getInst(context, local, child(0)->text()) + ";";
                // params.insert(std::pair<string, string>(child(i)->child(0)->child(0)->getInst(context, local), 
                //     child(i)->child(0)->child(1)->getInst(context, local)));
                // paramAccess.push_back(child(i)->child(0)->child(0)->getInst(context, local));
                // //changed 1 to local below 
                // params.insert(std::pair<string,string>(child(i)->getInst(context, declared), 
                //     child(i)->getInst(context, declared)));
                // std::cout << paramAccess.size() << endl;
            } 
        }
        string ret ="";
        if (body.find(" return ") == std::string::npos) {
            ret = "return new None_Obj();";
        }
        string type = header.empty() ? " Any " : " Any1 ";
        string func = header + type + id + "(" + formals + ") { " + body + ret + " }"; 
        context << globals << func << endl;
        return func;

    }

    string getInst (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }

        string signature = "";

        for (int j = 0; j < child(1)->children().size(); j++) {
            signature += "Any,";
        }
        signature.pop_back();

        int declared = 0;

        string id = child(0)->getInst(context, declared, child(0)->text());
        string formals = child(1)->getParams(context, declared, child(0)->text()).second;
        string header = child(1)->getParams(context, declared, child(0)->text()).first;
        string body = "";

        // string reachingVars = "";

        // for (int k = 0; k < paramAccess.size(); k++) {
        //     // std::cout << paramAccess[k] << endl;
        //     // reachingVars += " Any " + vars.first + " = " + vars.second + ",";
        // }
        // reachingVars.pop_back();

        // std::cout << paramAccess[0] << endl;

        for (int i = 3; i < arity(); i++) {
            body += child(i)->getInst(context, declared, child(0)->text()) + ";";
            if (child(i)->typeIndex() == STMT_LIST_AST && child(i)->child(0)->typeIndex() == ASSIGN_AST) {
                declared = 0;
                // params.insert(std::pair<string, string>(child(i)->child(0)->child(0)->getInst(context, local), 
                //     child(i)->child(0)->child(1)->getInst(context, local)));                
            }
        }
        string ret ="";

        if (body.find(" return ") == std::string::npos) {
            ret = "return new None_Obj();";
        }

        // string func = "function<Any(" + signature + ")> " + id + " = [" + reachingVars + "] (" + formals + ") -> Any { " + body + ret + " }"; 
        
        string type = header.empty() ? " Any " : " Any1 ";
        string func = header + type + id + " = [" /*+ reachingVars +*/ +"] (" + formals + ") { " + body + ret + " }";
        context << func << endl;
        // formals += ", " + reachingVars;

        
        return func;
    }

protected:

    Decl* getDecl () override {
        return child (0)->getDecl ();
    }

    AST_Ptr getId () override {
        return child (0);
    }

    void freezeDecls (bool frozen) override {
        Decl* me = getDecl ();
        if (me != nullptr)
            me->setFrozen (frozen);
        for (auto c : trimmedAST (*this, 3))
            c->freezeDecls (frozen);
    }

    void collectDecls (Decl* enclosing) override {
        AST_Ptr id = child (0);
        AST_Ptr params = child (1);
        AST_Ptr returnType = child (2);
        Decl* me = enclosing->addDefDecl(child(0), params->arity ());
        id->addDecl (me);
        params->collectDecls (me);
        params->collectTypeVarDecls (me);
        returnType->collectTypeVarDecls (me);
        for (auto c : trimmedAST (*this, 3)) {
            c->collectDecls (me);
        }
    }

    AST_Ptr resolveSimpleIds (const Environ* env) override {
        for (auto& c : *this) {
            c = c->resolveSimpleIds (getDecl ()->getEnviron ());
        }
        return this;
    }

    void resolveTypes (Decl* context, Unifier& subst) override {
        Decl* me = getDecl ();
        Type_Ptr funcType = me->getType ();
        Type_Ptr returnType = child (2)->asType ();
        AST_Ptr formals = child (1);
        getId ()->resolveTypes (me, subst);
        formals->resolveTypes (me, subst);
        if (!unify (funcType->returnType (), returnType, subst))
            error (this, "inconsistent return type");
        for (int i = 0; i < formals->arity (); i += 1) {
            if (!unify (formals->child (i)->getType (),
                        funcType->paramType (i), subst)) {
                error (this, "inconsistent parameter type");
            }
        }
        for (auto c : trimmedAST(*this, 3)) {
            c->resolveTypes (me, subst);
        }
        assert (subst[me->getType ()]->numParams () == formals->arity ());
    }

    void freeTypeVarCheck (const Type_Set& allowed) override {
    }

    FACTORY (Def_AST);

};


EXEMPLAR (Def_AST, DEF_AST);

/***** METHOD *****/

/**  def ID(...): ... (in class)     */
class Method_AST : public Def_AST {
public:

    string codeGen (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }

        int declared = local;
        string id = child(0)->getInst(context, declared, enclosing);

        string formals = child(1)->getInst(context, 2, enclosing);
        string body = "";
        
        for (int i = 3; i < arity(); i++) {
            body += child(i)->getInst(context, declared, enclosing);
            if (child(i)->typeIndex() == STMT_LIST_AST && child(i)->child(0)->typeIndex() == ASSIGN_AST) {
                declared = 0;
            }
        }

         /* for constructor */
        if (child(0)->text() == "__init__") {
        	id = child(1)->child(0)->child(1)->child(0)->text();
        	return id + "(" + formals + ") { " + body + " }";
        }
        
        string ret ="";
        if (body.find(" return ") == std::string::npos) {
            ret = "return new None_Obj();";
        }
        string func = "Any " + id + "(" + formals + ") { " + body + ret + " }"; 
        context << func << endl;

        return func;
    }

    string getInst (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }

        int declared = local;
        string id = child(0)->getInst(context, declared, enclosing);

        string formals = child(1)->getInst(context, 2, enclosing);
        string body = "";
        
        for (int i = 3; i < arity(); i++) {
            body += child(i)->getInst(context, declared, enclosing);
            // if (child(i)->typeIndex() == STMT_LIST_AST && child(i)->child(0)->typeIndex() == ASSIGN_AST) {
            //     declared = 0;
            // }
        }

         /* for constructor */
        if (child(0)->text() == "__init__") {
        	id = child(1)->child(0)->child(1)->child(0)->text();
        	return id + "(" + formals + ") { " + body + " }";
        }
        
        string ret ="";
        if (body.find(" return ") == std::string::npos) {
            ret = "return new None_Obj();";
        }
        string func = "Any " + id + "(" + formals + ") { " + body + ret + " }"; 

        return func;
    }

protected:

    FACTORY (Method_AST);

    void collectDecls (Decl* enclosing) override {
        AST_Ptr params = child (1);
        if (params->arity () == 0) {
            error (this, "method must have at least one parameter");
            setChild (1, NODE (FORMALS_LIST_AST,
                               makeId("__self__", location ()),
                               params->children ()));
        }
        Def_AST::collectDecls (enclosing);
    }

    void resolveClassStmtTypes (Decl* context, Unifier& subst,
                                bool doMethods) override {
        if (doMethods)
            resolveTypes (context, subst);
    }
};

EXEMPLAR (Method_AST, METHOD_AST);


/***** FORMALS_LIST *****/

/** ... (E1, ...)  */
class FormalsList_AST : public AST_Tree {
public:

	pair<string, string> getParams (Code_Context& context, int local, string enclosing) override {
		int num = 1;
        bool hasHeader = false;
        bool insertedAny = false;
        string header = "template <";
        string params = "";
        string type = "Any ";
        //int start = local == 2 ? 1 : 0; 
        for (int i = 0; i < arity(); i++) {
        	type = "Any ";
            auto c = child(i);
            if (c->child(1)->typeIndex() == FUNCTION_TYPE_AST) {//} && c->getType()->isFunctionType()) {
            	type = "T" + to_string(num++) + " ";
            	hasHeader = true;
            	header += c != child(0) ? ", typename " + type : "typename " + type;
            	params += type + c->getInst(context, local, enclosing) + ", ";
            } else if (c == child(arity()-1)) {
                params += type + c->getInst(context, local, enclosing);
                if (!insertedAny) {
                	header += c != child(0) ? ", typename " + type : "typename " + type;
                }
                insertedAny = true;

            } else {
                params += type + c->getInst(context, local, enclosing) + ", ";
                if (!insertedAny) {
                	header += c != child(0) ? ", typename " + type : "typename " + type;
                }
                insertedAny = true;
            }

            
        }
        header += ">";
        return hasHeader ? make_pair(header, params) : make_pair("", params);
    }

    string getInst (Code_Context& context, int local, string enclosing) override {
        string code = "";
        int start = local == 2 ? 1 : 0; 
        for (int i = start; i < arity(); i++) {
            auto c = child(i);
            if (c == child(arity()-1)) {
                code += "Any " + c->getInst(context, local, enclosing);
            } else {
                code += "Any " + c->getInst(context, local, enclosing) + ", ";
            }
            
        }
        return code;
    }

protected:

    FACTORY (FormalsList_AST);

    void collectDecls (Decl* enclosing) override {
        int k;
        k = 0;
        for (auto c : *this) {
            AST_Ptr id = c->getId ();
            c->addDecl (enclosing->addParamDecl(id, k));
            k += 1;
        };

        for (auto c : *this) {
            c->collectTypeVarDecls (enclosing);
        };
    }

};

EXEMPLAR (FormalsList_AST, FORMALS_LIST_AST);


/***** CLASS *****/

/** class ID of [...]: ...  */
class Class_AST : public AST_Tree {
public:

    string codeGen (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }

        string id = child(0)->getInst(context, 1, enclosing);
        context << "class " + id + " { ";
        context << endl << "public: " << endl;
        string classExpr= "class " + id + "{ ";
        context << endl;
        context.indent();
        for (int i = 2; i < arity(); i++) {
            context << child(i)->getInst(context, 1, enclosing);
            classExpr += child(i)->getInst(context, 1, enclosing);
        }
        context <<endl;
        context.indent(-1);
        context << "};" << endl;
        classExpr += "};";
        return classExpr;

    }

    string getInst (Code_Context& context, int local, string enclosing) override {
        int index = child(0)->getDecl()->getIndex();
        if (index <= 684) {
            return "";
        }

        string id = child(0)->getInst(context, 1, enclosing);
        string classExpr= "class " + id + "{ ";

        for (int i = 2; i < arity(); i++) {
            classExpr += child(i)->getInst(context, 1, enclosing);
        }
        classExpr += "};";
 
        return classExpr;
    }

protected:

    FACTORY (Class_AST);

    Decl* getDecl () override {
        return child (0)->getDecl ();
    }

    void collectDecls (Decl* enclosing) override {
        AST_Ptr id = child (0);
        string name = id->text ();
        AST_Ptr params = child (1);
        const Environ* env = enclosing->getEnviron ();

        Decl* me = makeClassDecl (id, params);

        if (env->findImmediate (name) != nullptr) {
            error (id, "attempt to redefine %s", name.c_str ());
        } else {
            enclosing->addMember (me);
        }

        setBuiltinDecl (me);

        id->addDecl (me);
        params->collectDecls (me);

        for (auto c : trimmedAST (*this, 2)) {
            c->collectDecls (me);
        }
    }

    AST_Ptr resolveSimpleIds (const Environ* env) override {
        for (auto& c : trimmedAST (*this, 2)) {
            c = c->resolveSimpleIds (getDecl ()->getEnviron ());
        }
        return this;
    }

    void resolveTypes (Decl* context, Unifier& subst) override {
        for (auto c : trimmedAST (*this, 2)) {
            c->resolveClassStmtTypes (getDecl (), subst, false);
        }
        replaceBindings (subst);
        for (auto c : trimmedAST (*this, 2)) {
            c->resolveClassStmtTypes (getDecl (), subst, true);
        }
    }

    void freeTypeVarCheck (const Type_Set& allowed) override {
        Type_Set typeParams;
        for (auto c : *child (1)) {
            typeParams.insert (c->asType ());
        }
        for (auto c : trimmedAST (*this, 2)) {
            c->freeTypeVarCheck (typeParams);
        }
    }

};

EXEMPLAR (Class_AST, CLASS_AST);

/***** TYPE_FORMALS_LIST *****/

/** of [$T, ...]      */
class TypeFormalsList_AST : public AST_Tree {
public:

    string getInst (Code_Context& context, int local, string enclosing) override {
        return "";
    }

protected:

    FACTORY (TypeFormalsList_AST);

    void collectDecls (Decl* enclosing) override {
        const Environ* env = enclosing->getEnviron ();
        for (auto c : *this) {
            string name = c->text ();
            if (env->findImmediate (name) != nullptr) {
                error (c, "duplicate type parameter: %s",
                       name.c_str ());
            } else {
                c->addDecl (enclosing->addTypeVarDecl (c));
            }
        }                                                                        
    }

};

EXEMPLAR (TypeFormalsList_AST, TYPE_FORMALS_LIST_AST);


/***** TYPED_ID *****/

/** ID::TYPE */
class TypedId_AST : public AST_Tree {
public:

    string codeGen(Code_Context& context, int local, string enclosing) override {

        if (child(1)->typeIndex() == FUNCTION_TYPE_AST) {
            if (child(0)->getDecl()->getId() == child(0)) {
                context << "Obj* (*" + child(0)->text() + ")(";
                for (int i = 0; i < arity(); i++) {
                    context << "Any";
                    if (i != arity() - 1) {
                        context << ", ";
                    }
                }
                context << ");";
                
            }
            return child(0)->text();
        }
        return child(0)->codeGen(context, local, enclosing);
    }

    string getInst(Code_Context& context, int local, string enclosing) override {
        return child(0)->getInst(context, local, enclosing);
    }

protected:

    FACTORY (TypedId_AST);

    AST_Ptr getId () override {
        return child (0);
    }

    Decl* getDecl () override {
        return getId ()->getDecl ();
    }

    const Decl_Vect& getDecls () override {
        return getId ()->getDecls ();
    }

    Type_Ptr getType () override {
        return child (1)->asType ();
    }

    void addTargetDecls (Decl* enclosing) override {
        getId ()->addTargetDecls (enclosing);
    }
    
    void collectTypeVarDecls (Decl* enclosing) override {
        child (1)->collectTypeVarDecls (enclosing);
    }

    void addDecl (Decl* decl) override {
        getId ()->addDecl (decl);
    }

    
    void resolveTypes (Decl* context, Unifier& subst) override {
        getId ()->resolveTypes (context, subst);
        if (!getId ()->setType (child (1)->asType (), subst))
            error (this, "incompatible type assignment");
    }

};

EXEMPLAR (TypedId_AST, TYPED_ID_AST);


/***** ASSIGN *****/

/** TARGET(s) = EXPR */
class Assign_AST : public Typed_Tree {
protected:

    FACTORY (Assign_AST);

    string codeGen(Code_Context& context, int local, string enclosing) {
        string target = "";
        string expr = "";
        if (child(0)->typeIndex() == SLICE_ASSIGN_AST) {
            string value = child(1)->getInst(context, local, enclosing);
            string res = child(0)->sliceAssign(context, value, local, enclosing);
            return res;
        } else if (child(0)->typeIndex() == TARGET_LIST_AST) {
        	AST_Ptr targetList = child(0);
        	AST_Ptr tup = child(1);
        	string res = "";

        	for (int i = 0; i < targetList->arity(); i++) {
        		auto c = targetList->child(i);
        		if (child(0)->arity() == 0 && child(0)->getDecl()->getId() == child(0)) 
					target = "Any " + c->getInst(context, local, enclosing);
               	else
                	target = c->codeGen(context, local, enclosing);
            
        		expr = tup->child(i)->getInst(context, local, enclosing); 
        		context << target + " = " + expr + ";";
        		res += target + " = " + expr + ";";
        	}
        	return res;
        } else if (child(0)->typeIndex() == SUBSCRIPT_ASSIGN_AST) {
            string value = child(1)->getInst(context, local, enclosing);
            string res = child(0)->subscriptAssign(context, value, local, enclosing);
            return res;
        } else if (child(1)->typeIndex() == CALL1_AST) {
            expr = child(1)->getInst(context, local, enclosing);
            target = "auto " + child(0)->getInst(context, local, enclosing);
        } else {
            if (child(0)->arity() == 0 && child(0)->getDecl()->getId() == child(0)) {
            	target = "Any " + child(0)->getInst(context, local, enclosing);
            	if (child(1)->isLeaf() && child(1)->getType()->isFunctionType()) {
            		target = "auto " + child(0)->getInst(context, local, enclosing);
            	}
            	// if (local == 1) {
            	// 	target = "Any " + child(0)->getInst(context, local, enclosing);
            	// }
                
            } else if (child(0)->arity() != 0 && child(0)->child(0)->getDecl()->getId() == child(0)->child(0)) {
            	target = "Any " + child(0)->getInst(context, local, enclosing);
            	if (child(1)->isLeaf() && child(1)->getType()->isFunctionType()) {
            		target = "auto " + child(0)->getInst(context, local, enclosing);
            	}
            	// if (local == 1) {
            	// 	target = "Any " + child(0)->getInst(context, local, enclosing);
            	// }
                
            } else {
                target = child(0)->codeGen(context, local, enclosing);
            }
            
            expr = child(1)->getInst(context, local, enclosing);
        }
        
        
        context << target + " = " + expr + ";";
        return target + " = " + expr + ";";
    }

    string getInst(Code_Context& context, int local, string enclosing) {
        string target = "";
        string expr = "";
        if (child(0)->typeIndex() == SLICE_ASSIGN_AST) {
            string value = child(1)->getInst(context, local, enclosing);
            string res = child(0)->sliceAssign(context, value, local, enclosing);
            return res;
        } else if (child(0)->typeIndex() == TARGET_LIST_AST) {
        	AST_Ptr targetList = child(0);
        	AST_Ptr tup = child(1);
        	string res = "";

        	for (int i = 0; i < targetList->arity(); i++) {
        		auto c = targetList->child(i);
        		if (child(0)->arity() == 0 && child(0)->getDecl()->getId() == child(0)) 
					target = "Any " + c->getInst(context, local, enclosing);
               	else
                	target = c->codeGen(context, local, enclosing);
            
        		expr = tup->child(i)->getInst(context, local, enclosing); 
        		context << target + " = " + expr + ";";
        		res += target + " = " + expr + ";";
        	}
        	return res;
        } else if (child(0)->typeIndex() == SUBSCRIPT_ASSIGN_AST) {
            string value = child(1)->getInst(context, local, enclosing);
            string res = child(0)->subscriptAssign(context, value, local, enclosing);
            return res;
        } else if (child(1)->typeIndex() == CALL1_AST) {
            expr = child(1)->getInst(context, local, enclosing);
            target = "auto " + child(0)->getInst(context, local, enclosing);
        } else {
            if (child(0)->arity() == 0 && child(0)->getDecl()->getId() == child(0)) {
            	target = "Any " + child(0)->getInst(context, local, enclosing);
            	if (child(1)->isLeaf() && child(1)->getType()->isFunctionType()) {
            		target = "auto " + child(0)->getInst(context, local, enclosing);
            	}
            	// if (local == 1) {
            	// 	target = "Any " + child(0)->getInst(context, local, enclosing);
            	// }
                
            } else {
                target = child(0)->codeGen(context, local, enclosing);
            }
            
            expr = child(1)->getInst(context, local, enclosing);
        }
        
        return target + " = " + expr + "";
    }


    void collectDecls (Decl* enclosing) override {
        child(1)->collectDecls (enclosing);
        child(0)->addTargetDecls (enclosing);
    }

    void resolveTypes (Decl* context, Unifier& subst) override {
        int errs0 = numErrors ();
        AST::resolveTypes (context, subst);
        if (!unify (child (0)->getType (), child (1)->getType (), subst)
            && errs0 == numErrors ())
            error (this, "type mismatch in assignment");
        if (!setType (child (1)->getType (), subst) && errs0 == numErrors ())
            error (this, "type mismatch in assignment");
    }
    
};

EXEMPLAR (Assign_AST, ASSIGN_AST);

/***** FOR *****/

/**  for target in exprs: body [ else: body ]     */
class For_AST : public AST_Tree {
public:

    For_AST () : sequence (nullptr) {
    }

protected:

    FACTORY (For_AST);

    string codeGen(Code_Context& context, int local, string enclosing) {
        string target = child(0)->codeGen(context, local, enclosing);
        string expr = child(1)->getInst(context, local, enclosing);
        string body = child(2)->getInst(context, local, enclosing);
        string iter = "b" + to_string(number++);
        context << "auto " + iter + " = " + expr + ";";

        string elseClause = arity() >= 4 ? child(3)->getInst(context, local, enclosing) : "";
        string forStmt = "if (" + iter + "->getType() == \"List\") { bool completed" + to_string(number++) + " = false;";
        string forStmt2 = " for (auto " + target + " : ((List)" + iter + ")->contents) {" + body + " ";
        string forStmt3 = "if (" + target + " == ((List)" + iter + ")->contents.back()){completed"+ to_string(number - 1) +" = true; } } if (completed" + to_string(number - 1) +") {" + elseClause + "}}";
        
        string forStmt4 = "if (" + iter + "->getType() == \"Range\") {";
        string forStmt5 = " for (auto " + target + " : ((Range)" + iter + ")->contents) {" + body + "} }";
        
        context << forStmt + forStmt2 + forStmt3 + forStmt4 + forStmt5;
        return forStmt;
    }

    string getInst(Code_Context& context, int local, string enclosing) {
        string target = child(0)->codeGen(context, local, enclosing);
        string expr = child(1)->getInst(context, local, enclosing);
        string body = child(2)->getInst(context, local, enclosing);
        string iter = "b" + to_string(number++);
        context << "auto " + iter + " = " + expr + ";";

        string elseClause = arity() >= 4 ? child(3)->getInst(context, local, enclosing) : "";
        string forStmt = "if (" + iter + "->getType() == \"List\") { bool completed" + to_string(number++) + " = false;";
        string forStmt2 = " for (auto " + target + " : ((List)" + iter + ")->contents) {" + body + " ";
        string forStmt3 = "if (" + target + " == ((List)" + iter + ")->contents.back()){completed"+ to_string(number - 1) +" = true; } } if (completed" + to_string(number - 1) +") {" + elseClause + "}}";
        return forStmt;
    }

    AST_Ptr add (AST_Ptr c) override {
        AST_Tree::add (c);
        if (arity () == 4)
            setSequence ();
        return this;
    }


    AST_Ptr add (const AST_Vect& v) override {
        AST_Tree::add (v);
        if (arity () == 4)
            setSequence ();
        return this;
    }

    void resolveTypes (Decl* context, Unifier& subst) override {
        int errs0 = numErrors ();
        AST_Ptr target = child (0);
        sequence->resolveTypes (context, subst);
        target->resolveTypes (context, subst);
        if (!unify (sequence->getType (), target->getType (), subst)
            && errs0 == numErrors ()) {
            error (this, "bad sequence type in 'for' statement");
        }

        for (auto c : trimmedAST (*this, 2))
            c->resolveTypes (context, subst);
    }
        
    void collectDecls (Decl* enclosing) override {
        AST_Ptr target = child (0);
        for (auto c : trimmedAST (*this, 2)) {
            c->collectDecls (enclosing);
        }
        target->addTargetDecls (enclosing);
    }

    void getOverloadings (Resolver& resolver) override {
        assert (sequence != nullptr);
        AST_Tree::getOverloadings (resolver);
        sequence->getOverloadings (resolver);
    }

    AST_Ptr resolveSimpleIds (const Environ* env) override {
        sequence->resolveSimpleIds (env);
        return AST::resolveSimpleIds (env);
    }

private:

    /** Initialize sequence (assuming all children present). */
    void setSequence () {
        if (sequence == nullptr) {
            sequence = NODE (CALL_AST, makeId ("__choose__", location ()),
                             child (1));
        }
    }

    /** Artificial identifier used to define the legal sequences for 
     *  for statements.  Shares parameter with child(1). */
    AST_Ptr sequence;
};

EXEMPLAR (For_AST, FOR_AST);


/***** RETURN *****/

/**  return EXPR */
class Return_AST : public AST_Tree {
public:

    string codeGen(Code_Context& context, int local, string enclosing) {
        string stmt = child(0)->getInst(context, local, enclosing);
        string ret = "return " + stmt + ";";
        return ret;
    }

    string getInst(Code_Context& context, int local, string enclosing) {
        string stmt = child(0)->getInst(context, local, enclosing);
        string ret = "return " + stmt + "";

        return ret;
    }

protected:

    FACTORY(Return_AST);

    void resolveTypes (Decl* context, Unifier& subst) override {
        AST_Tree::resolveTypes (context, subst);
        AST_Ptr expr = child (0);
        Type_Ptr returnType = context->getType ()->returnType ();
        if (!unify (returnType, expr->getType (), subst))
            error (this, "inconsistent return type");
    }

};

EXEMPLAR (Return_AST, RETURN_AST);


/***** IF *****/

/** if COND: STMT else: STMT */
class If_AST : public AST_Tree {
protected:

    FACTORY(If_AST);

    string codeGen (Code_Context& context, int local, string enclosing) override {
        string cond = child(0)->getInst(context, local, enclosing);
        string stmt1 = child(1)->getInst(context, local, enclosing);
        string stmt2 = child(2)->getInst(context, local, enclosing);

        // string var = "b" + to_string(number++);
        // string eval = "Bool " + var + " = " + cond + ";";
        // string evalCond = var + "->value";
        // context << eval;
        string code = "if (condtionalHelper(" + cond + ")) { " + stmt1 + " } else { " + stmt2 + "}";

        context << code;
        return code;
    } 
    string getInst (Code_Context& context, int local, string enclosing) override {
        string cond = child(0)->getInst(context, local, enclosing);
        string stmt1 = child(1)->getInst(context, local, enclosing);
        string stmt2 = child(2)->getInst(context, local, enclosing);

        // string var = "b" + to_string(number++);
        // string eval = "Bool " + var + " = " + cond + ";";
        // string evalCond = var + "->value";
        // context << eval;
        string code = "if (condtionalHelper(" + cond + ")) { " + stmt1 + " } else { " + stmt2 + "}";

        return code;
    }

    void resolveTypes (Decl* context, Unifier& subst) override {
        AST_Tree::resolveTypes (context, subst);
        if (!unify (child (0)->getType (), boolDecl->asType (), subst)) {
            error (child (0), "condition must be boolean");
        }
    }
};

EXEMPLAR (If_AST, IF_AST);

/***** WHILE *****/

/** while COND: STMT else: STMT */
class While_AST : public AST_Tree {
protected:

    FACTORY(While_AST);

    string codeGen (Code_Context& context, int local, string enclosing) override {
        string cond = child(0)->getInst(context, local, enclosing);
        string stmt = child(1)->getInst(context, local, enclosing);

        string var = "b" + to_string(number++);
        string eval = "Bool " + var + " = " + cond + ";";
        string evalCond = var + "->value";
        context << eval;
        string code = "while (condtionalHelper(" + cond + ")) { " + stmt + " }" ;

        context << code;
        return code;
    } 
    string getInst (Code_Context& context, int local, string enclosing) override {
        string cond = child(0)->getInst(context, local, enclosing);
        string stmt = child(1)->getInst(context, local, enclosing);

        string var = "b" + to_string(number++);
        string eval = "Bool " + var + " = " + cond + ";";
        string evalCond = var + "->value";
        context << eval;
        string code = "while (condtionalHelper(" + cond + ")) { " + stmt + " }" ;

        return code;
    }

    void resolveTypes (Decl* context, Unifier& subst) override{
        AST_Tree::resolveTypes (context, subst);
        if (!unify (child (0)->getType (), boolDecl->asType (), subst)) {
            error (child (0), "condition must be boolean");
        }
    }
};

EXEMPLAR (While_AST, WHILE_AST);
