#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h" // Include your friends' lexer definitions

// ============================================================================
// 1. NON-TERMINALS ENUMERATION
// ============================================================================
// We define all the non-terminals from the grammar rules. 
// We use the NT_ prefix to separate them from the lexer's TK_ prefix.
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

// ============================================================================
// 2. GRAMMAR SYMBOL
// ============================================================================
// Our stack needs to hold both Terminals (like TK_ID) and Non-Terminals 
// (like NT_STMT). This struct lets us hold either one safely.
typedef struct {
    int isTerminal; // 1 if it is a TokenType, 0 if it is a NonTerminal
    
    union {
        TokenType term;       // Use this if isTerminal == 1
        NonTerminal nonTerm;  // Use this if isTerminal == 0
    } val;
} GrammarSymbol;


// ============================================================================
// 3. PARSE TREE NODE
// ============================================================================
// Because a grammar rule can have 1, 2, 3 or more symbols on the right side, 
// a standard binary tree won't work. We use a "First-Child / Next-Sibling" 
// approach. This is the simplest way to make an N-ary tree in C.
typedef struct treeNode {
    int isLeafNode;               // 1 if leaf (terminal), 0 if internal (non-terminal)
    GrammarSymbol symbol;         // The grammar symbol this node represents
    
    tokenInfo token;              // Pointer to the lexer token (NULL if it's an internal node)
    
    struct treeNode *parent;      // Pointer to parent (required for printing column 5)
    struct treeNode *firstChild;  // Pointer to the first child node
    struct treeNode *nextSibling; // Pointer to the next sibling node
} treeNode;

// Typedef for easier reading
typedef treeNode* parseTree;


// ============================================================================
// 4. PARSER STACK ADT
// ============================================================================
// A simple Linked-List based stack. It holds the grammar symbol to match, 
// AND a pointer to the tree node so we can attach children to it dynamically.
typedef struct stackNode {
    GrammarSymbol symbol;         // The symbol expected to be matched
    treeNode *treeNodePtr;        // Where in the tree this symbol belongs
    struct stackNode *next;       // Pointer to the next node down the stack
} stackNode;

typedef stackNode* Stack;


// ============================================================================
// 5. FIRST & FOLLOW SETS
// ============================================================================
// Simple 2D integer arrays. 
// If FIRST(NT_PROGRAM) contains TK_MAIN, then first[NT_PROGRAM][TK_MAIN] = 1.
// Otherwise, it is 0.
typedef struct {
    int first[100][100];  // [NonTerminal][TokenType]
    int follow[100][100]; // [NonTerminal][TokenType]
} FirstAndFollow;


// ============================================================================
// 6. PREDICTIVE PARSE TABLE
// ============================================================================
// A 2D array representing the parsing table.
// Rows are Non-Terminals, Columns are Terminals (Lookaheads).
// The integer stored inside is the Grammar Rule Number (1 to N).
// We will fill empty cells with -1 to indicate a Syntax Error.
typedef struct {
    int rules[100][100]; // [NonTerminal][TokenType]
} table;

#endif