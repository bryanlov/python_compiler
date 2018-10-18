/* Table of information about AST node types.  Each node type is
 * identified by a symbolic constant (a C++ enumerated type), and
 * denoted in the printed representation of AST nodes by a name
 * string, as documented in the project 1 specification.
 * In addition, different types of AST nodes may be represented by
 * different subclsses of AST. This information is gathered together
 * here as a series of macro invocations of the form
 *      AST_INFO(<symbol>, <name>)
 * where <symbol> is the symbolic constant, and <name> the printed
 * name of the node type (e.g., "binop" or "slice").
 *
 * AST_INFO is an undefined name.  To use this table,
 * one uses the idiom
 *
 *     #define AST_INFO(SYM, NAME) <definition>
 *     #include "ast-types.i"
 *     #undef AST_INFO
 *
 * For example, to get a comma-separated list of all the <symbols>,
 * one could define them
 *
 *      #define AST_INFO(SYM, NAME) SYM,
 *
 */

AST_INFO(EMPTY_AST, "")
AST_INFO(AND_AST, "and")
AST_INFO(ASSIGN_AST, "assign")
AST_INFO(ATTRIBUTEREF_AST, "attributeref")
AST_INFO(BINOP_AST, "binop")
AST_INFO(BLOCK_AST, "block")
AST_INFO(BREAK_AST, "break")
AST_INFO(CALL_AST, "call")
AST_INFO(CLASS_AST, "class")
AST_INFO(COMPARE_AST, "compare")
AST_INFO(CONTINUE_AST, "continue")
AST_INFO(DEF_AST, "def")
AST_INFO(DICT_DISPLAY_AST, "dict_display")
AST_INFO(FALSE_AST, "False")
AST_INFO(FOR_AST, "for")
AST_INFO(FORMALS_LIST_AST, "formals_list")
AST_INFO(FUNCTION_TYPE_AST, "function_type")
AST_INFO(ID_AST, "id")
AST_INFO(IF_AST, "if")
AST_INFO(IF_EXPR_AST, "if_expr")
AST_INFO(INT_LITERAL_AST, "int_literal")
AST_INFO(LEFT_COMPARE_AST, "left_compare")
AST_INFO(LIST_DISPLAY_AST, "list_display")
AST_INFO(METHOD_AST, "def")         /* Artificial */
AST_INFO(MODULE_AST, "module")      /* Artificial */
AST_INFO(NATIVE_AST, "native")
AST_INFO(NEXT_VALUE_AST, "next_value")
AST_INFO(NONE_AST, "None")
AST_INFO(OR_AST, "or")
AST_INFO(PAIR_AST, "pair")
AST_INFO(PRINT_AST, "print")
AST_INFO(PRINTLN_AST, "println")   /* Artificial */
AST_INFO(RETURN_AST, "return")
AST_INFO(SLICE_AST, "slice")
AST_INFO(SLICE_ASSIGN_AST, "slice_assign")
AST_INFO(STMT_LIST_AST, "stmt_list")
AST_INFO(STRING_LITERAL_AST, "string_literal")
AST_INFO(SUBSCRIPT_AST, "subscript")
AST_INFO(SUBSCRIPT_ASSIGN_AST, "subscript_assign")
AST_INFO(TARGET_LIST_AST, "target_list")
AST_INFO(TRUE_AST, "True")
AST_INFO(TUPLE_AST, "tuple")
AST_INFO(TYPE_AST, "type")
AST_INFO(TYPE_FORMALS_LIST_AST, "type_formals_list")
AST_INFO(TYPE_LIST_AST, "type_list")
AST_INFO(TYPE_VAR_AST, "type_var")
AST_INFO(TYPED_ID_AST, "typed_id")
AST_INFO(UNOP_AST, "unop")
AST_INFO(WHILE_AST, "while")

AST_INFO(CALL1_AST, "call1")          /* Added in phase 2 */
AST_INFO(NEW_AST, "new")              /* Added in phase 2 */

AST_INFO(LAST_AST, "")
