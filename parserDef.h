#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"
#include <stdbool.h>

/* ============ NON-TERMINAL ENUMERATION ============ */
typedef enum {
    NT_PROGRAM = 0,
    NT_OTHER_FUNCTIONS,
    NT_FUNCTION,
    NT_MAIN_FUNCTION,
    NT_INPUT_PAR,
    NT_OUTPUT_PAR,
    NT_PARAMETER_LIST,
    NT_REMAINING_LIST,
    NT_DATA_TYPE,
    NT_PRIMITIVE_DATATYPE,
    NT_CONSTRUCTED_DATATYPE,
    NT_STMTS,
    NT_TYPE_DEFINITIONS,
    NT_ACTUAL_OR_REDEFINED,
    NT_TYPE_DEFINITION,
    NT_FIELD_DEFINITIONS,
    NT_FIELD_DEFINITION,
    NT_FIELD_TYPE,
    NT_MORE_FIELDS,
    NT_DECLARATIONS,
    NT_DECLARATION,
    NT_GLOBAL_OR_NOT,
    NT_OTHER_STMTS,
    NT_STMT,
    NT_ASSIGNMENT_STMT,
    NT_SINGLE_OR_REC_ID,
    NT_OPTION_SINGLE_CONSTRUCTED,
    NT_ONE_EXPANSION,
    NT_MORE_EXPANSIONS,
    NT_FUN_CALL_STMT,
    NT_OUTPUT_PARAMETERS,
    NT_INPUT_PARAMETERS,
    NT_ITERATIVE_STMT,
    NT_CONDITIONAL_STMT,
    NT_ELSE_PART,
    NT_IO_STMT,
    NT_RETURN_STMT,
    NT_OPT_RETURN_VAL,
    NT_BOOL_EXPR,
    NT_VAR,
    NT_LOGICAL_OP,
    NT_REL_OP,
    NT_EXPR,
    NT_EXPR_PRIME,
    NT_TERM,
    NT_TERM_PRIME,
    NT_FACTOR,
    NT_HIGH_PREC_OP,
    NT_LOW_PREC_OP,
    NT_ID_LIST,
    NT_MORE_IDS,
    NT_DEFINETYPE_STMT,
    NT_A,
    NUM_NON_TERMINALS
} NonTerminal;

/* ============ GRAMMAR SYMBOL ============ */
typedef struct {
    int isTerminal; /* 1 = terminal (TokenType), 0 = non-terminal (NonTerminal) */
    int symbol;
} GrammarSymbol;

/* ============ PRODUCTION ============ */
#define MAX_PROD_LEN  12
#define MAX_PRODUCTIONS 100

typedef struct {
    NonTerminal lhs;
    GrammarSymbol rhs[MAX_PROD_LEN];
    int rhsLen; /* 0 = epsilon production */
} Production;

/* ============ GRAMMAR ============ */
typedef struct {
    Production prods[MAX_PRODUCTIONS];
    int numProds;
} Grammar;

/* ============ FIRST AND FOLLOW SETS ============ */
#define NUM_TERMINALS (TK_EOF + 1)

typedef struct {
    bool first[NUM_NON_TERMINALS][NUM_TERMINALS];
    bool firstHasEps[NUM_NON_TERMINALS];
    bool follow[NUM_NON_TERMINALS][NUM_TERMINALS];
} FirstAndFollow;

/* ============ PARSE TABLE ============ */
/* table[NT][terminal] = production index, -1 = error entry */
typedef int ParseTable[NUM_NON_TERMINALS][NUM_TERMINALS];

/* ============ PARSE TREE NODE ============ */
#define MAX_CHILDREN 16

typedef struct TreeNode {
    int isLeaf;    /* 1 if terminal leaf, 0 if non-terminal internal node */
    int symbol;    /* TokenType if leaf, NonTerminal if internal */
    char lexeme[MAX_LEXEME_SIZE];
    int lineNo;
    TokenType tokenType;
    double numVal;   /* numeric value for TK_NUM / TK_RNUM */
    int hasNumVal;
    struct TreeNode *parent;
    struct TreeNode *children[MAX_CHILDREN];
    int numChildren;
} TreeNode;

typedef TreeNode *ParseTree;

#endif /* PARSERDEF_H */
