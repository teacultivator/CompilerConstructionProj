/* ID:  2022B3A70404P; Name: Vivek Raj Barnwal */
/* ID:  2022B3A70607P; Name: Abhinav Madaan */
/* ID:  2022B3A70413P; Name: Jayant Grover */
/* ID:  2022B3A70648P; Name: Gathvik Narayan Kolla */
/* ID:  2022B3A70474P; Name: Raghav Mundra */
/* ID: 2023A7PS0653P; Name: Vulli Abhinav */

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"

typedef enum {
    NT_PROGRAM, NT_MAIN_FUNCTION, NT_OTHER_FUNCTIONS, NT_FUNCTION, NT_INPUT_PAR, 
    NT_OUTPUT_PAR, NT_PARAMETER_LIST, NT_DATA_TYPE, NT_PRIMITIVE_DATATYPE, 
    NT_CONSTRUCTED_DATATYPE, NT_REMAINING_LIST, NT_STMTS, NT_TYPE_DEFINITIONS, 
    NT_ACTUAL_OR_REDEFINED, NT_TYPE_DEFINITION, NT_FIELD_DEFINITIONS, 
    NT_FIELD_DEFINITION, NT_FIELD_TYPE, NT_MORE_FIELDS, NT_DECLARATIONS, 
    NT_DECLARATION, NT_GLOBAL_OR_NOT, NT_OTHER_STMTS, NT_STMT, NT_ASSIGNMENT_STMT, 
    NT_SINGLE_OR_REC_ID, NT_OPTION_SINGLE_CONSTRUCTED, NT_ONE_EXPANSION, 
    NT_MORE_EXPANSIONS, NT_FUN_CALL_STMT, NT_OUTPUT_PARAMETERS, NT_INPUT_PARAMETERS, 
    NT_ITERATIVE_STMT, NT_CONDITIONAL_STMT, NT_ELSE_PART, NT_IO_STMT, 
    NT_ARITHMETIC_EXPRESSION, NT_EXP_PRIME, NT_TERM, NT_TERM_PRIME, NT_FACTOR, 
    NT_HIGH_PRECEDENCE_OPERATORS, NT_LOW_PRECEDENCE_OPERATORS, NT_BOOLEAN_EXPRESSION, 
    NT_VAR, NT_LOGICAL_OP, NT_RELATIONAL_OP, NT_RETURN_STMT, NT_OPTIONAL_RETURN, 
    NT_ID_LIST, NT_MORE_IDS, NT_DEFINETYPESTMT, NT_A,
    NUM_NON_TERMINALS 
} NonTerminal;

typedef struct {
    int isTerminal; // 1 = token-type, 0 = non-terminal
    union {
        TokenType term;     
        NonTerminal nonTerm;  
    } val;
} GrammarSymbol;

// N-ary tree node (First-Child/Next-Sibling)
typedef struct treeNode {
    int isLeafNode;               
    GrammarSymbol symbol;         
    tokenInfo token;              
    struct treeNode *parent;      
    struct treeNode *firstChild;  
    struct treeNode *nextSibling; 
} treeNode;

typedef treeNode* parseTree;

typedef struct stackNode {
    GrammarSymbol symbol;
    treeNode *treeNodePtr;
    struct stackNode *next;
} stackNode;

typedef stackNode* Stack;

typedef struct {
    int first[100][100]; 
    int follow[100][100];
} FirstAndFollow;

// LL(1) Parse Table: [NonTerminal][TokenType] -> Rule Number
typedef struct {
    int rules[100][100];
} table;

#endif
