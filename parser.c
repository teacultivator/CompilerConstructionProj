#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

/* ============================================================ */
/*              NON-TERMINAL NAME STRINGS                       */
/* ============================================================ */

const char *nonTerminalStrings[] = {
    "program",              "otherFunctions",       "function",
    "mainFunction",         "input_par",            "output_par",
    "parameterList",        "remainingList",        "dataType",
    "primitiveDatatype",    "constructedDatatype",  "stmts",
    "typeDefinitions",      "actualOrRedefined",    "typeDefinition",
    "fieldDefinitions",     "fieldDefinition",      "fieldType",
    "moreFields",           "declarations",         "declaration",
    "globalOrNot",          "otherStmts",           "stmt",
    "assignmentStmt",       "singleOrRecId",        "optionSingleConstructed",
    "oneExpansion",         "moreExpansions",       "funCallStmt",
    "outputParameters",     "inputParameters",      "iterativeStmt",
    "conditionalStmt",      "elsePart",             "ioStmt",
    "returnStmt",           "optReturnVal",         "boolExpr",
    "var",                  "logicalOp",            "relOp",
    "arithmeticExpression", "exprPrime",            "term",
    "termPrime",            "factor",               "highPrecOp",
    "lowPrecOp",            "idList",               "moreIds",
    "definetypeStmt",       "A"
};

/* ============================================================ */
/*                 GRAMMAR INITIALISATION                       */
/* ============================================================ */

/* Helper: add a production to the grammar */
static void addProd(Grammar *G, NonTerminal lhs,
                    int isTerms[], int syms[], int len)
{
    Production *p = &G->prods[G->numProds++];
    p->lhs   = lhs;
    p->rhsLen = len;
    for (int i = 0; i < len; i++) {
        p->rhs[i].isTerminal = isTerms[i];
        p->rhs[i].symbol     = syms[i];
    }
}

/* Shorthand macros */
#define T  1   /* isTerminal = true  */
#define NT 0   /* isTerminal = false */

/*
 * Grammar productions (LL(1)) per Modified LL(1) Grammar document:
 *
 *  P0:  program             → otherFunctions mainFunction
 *  P1:  otherFunctions      → function otherFunctions
 *  P2:  otherFunctions      → ε
 *  P3:  function            → TK_FUNID input_par output_par TK_SEM stmts TK_END
 *  P4:  mainFunction        → TK_MAIN stmts TK_END
 *  P5:  input_par           → TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameterList TK_SQR
 *  P6:  output_par          → TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameterList TK_SQR
 *  P7:  output_par          → ε
 *  P8:  parameterList       → dataType TK_ID remainingList
 *  P9:  dataType            → primitiveDatatype
 *  P10: dataType            → constructedDatatype
 *  P11: primitiveDatatype   → TK_INT
 *  P12: primitiveDatatype   → TK_REAL
 *  P13: constructedDatatype → TK_RECORD TK_RUID
 *  P14: constructedDatatype → TK_UNION TK_RUID
 *  P15: constructedDatatype → TK_RUID
 *  P16: remainingList       → TK_COMMA parameterList
 *  P17: remainingList       → ε
 *  P18: stmts               → typeDefinitions declarations otherStmts returnStmt
 *  P19: typeDefinitions     → actualOrRedefined typeDefinitions
 *  P20: typeDefinitions     → ε
 *  P21: actualOrRedefined   → typeDefinition
 *  P22: actualOrRedefined   → definetypeStmt
 *  P23: typeDefinition      → TK_RECORD TK_RUID fieldDefinitions TK_ENDRECORD
 *  P24: typeDefinition      → TK_UNION TK_RUID fieldDefinitions TK_ENDUNION
 *  P25: fieldDefinitions    → fieldDefinition fieldDefinition moreFields
 *  P26: fieldDefinition     → TK_TYPE fieldType TK_COLON TK_FIELDID TK_SEM
 *  P27: fieldType           → primitiveDatatype
 *  P28: fieldType           → constructedDatatype
 *  P29: moreFields          → fieldDefinition moreFields
 *  P30: moreFields          → ε
 *  P31: declarations        → declaration declarations
 *  P32: declarations        → ε
 *  P33: declaration         → TK_TYPE dataType TK_COLON TK_ID globalOrNot TK_SEM
 *  P34: globalOrNot         → TK_COLON TK_GLOBAL
 *  P35: globalOrNot         → ε
 *  P36: otherStmts          → stmt otherStmts
 *  P37: otherStmts          → ε
 *  P38: stmt                → assignmentStmt
 *  P39: stmt                → iterativeStmt
 *  P40: stmt                → conditionalStmt
 *  P41: stmt                → ioStmt
 *  P42: stmt                → funCallStmt
 *  P43: assignmentStmt      → singleOrRecId TK_ASSIGNOP expr TK_SEM
 *  P44: singleOrRecId       → TK_ID optionSingleConstructed
 *  P45: optionSingleConstructed → ε
 *  P46: optionSingleConstructed → oneExpansion moreExpansions
 *  P47: oneExpansion        → TK_DOT TK_FIELDID
 *  P48: moreExpansions      → oneExpansion moreExpansions
 *  P49: moreExpansions      → ε
 *  P50: funCallStmt         → outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM
 *  P51: outputParameters    → TK_SQL idList TK_SQR TK_ASSIGNOP
 *  P52: outputParameters    → ε
 *  P53: inputParameters     → TK_SQL idList TK_SQR
 *  P54: iterativeStmt       → TK_WHILE TK_OP boolExpr TK_CL stmt otherStmts TK_ENDWHILE
 *  P55: conditionalStmt     → TK_IF TK_OP boolExpr TK_CL TK_THEN stmt otherStmts elsePart
 *  P56: elsePart            → TK_ELSE stmt otherStmts TK_ENDIF
 *  P57: elsePart            → TK_ENDIF
 *  P58: ioStmt              → TK_READ TK_OP var TK_CL TK_SEM
 *  P59: ioStmt              → TK_WRITE TK_OP var TK_CL TK_SEM
 *  P60: expr                → term exprPrime
 *  P61: exprPrime           → lowPrecOp term exprPrime
 *  P62: exprPrime           → ε
 *  P63: term                → factor termPrime
 *  P64: termPrime           → highPrecOp factor termPrime
 *  P65: termPrime           → ε
 *  P66: factor              → TK_OP expr TK_CL
 *  P67: factor              → var
 *  P68: highPrecOp          → TK_MUL
 *  P69: highPrecOp          → TK_DIV
 *  P70: lowPrecOp           → TK_PLUS
 *  P71: lowPrecOp           → TK_MINUS
 *  P72: boolExpr            → TK_OP boolExpr TK_CL logicalOp TK_OP boolExpr TK_CL
 *  P73: boolExpr            → var relOp var
 *  P74: boolExpr            → TK_NOT TK_OP boolExpr TK_CL
 *  P75: var                 → singleOrRecId
 *  P76: var                 → TK_NUM
 *  P77: var                 → TK_RNUM
 *  P78: logicalOp           → TK_AND
 *  P79: logicalOp           → TK_OR
 *  P80: relOp               → TK_LT
 *  P81: relOp               → TK_LE
 *  P82: relOp               → TK_EQ
 *  P83: relOp               → TK_GT
 *  P84: relOp               → TK_GE
 *  P85: relOp               → TK_NE
 *  P86: returnStmt          → TK_RETURN optReturnVal TK_SEM
 *  P87: optReturnVal        → TK_SQL idList TK_SQR
 *  P88: optReturnVal        → ε
 *  P89: idList              → TK_ID moreIds
 *  P90: moreIds             → TK_COMMA idList
 *  P91: moreIds             → ε
 *  P92: definetypeStmt      → TK_DEFINETYPE A TK_RUID TK_AS TK_RUID
 *  P93: A                   → TK_RECORD
 *  P94: A                   → TK_UNION
 */

void initGrammar(Grammar *G)
{
    G->numProds = 0;

    /* P0: program → otherFunctions mainFunction */
    { int it[]={NT,NT}; int sy[]={NT_OTHER_FUNCTIONS,NT_MAIN_FUNCTION};
      addProd(G, NT_PROGRAM, it, sy, 2); }

    /* P1: otherFunctions → function otherFunctions */
    { int it[]={NT,NT}; int sy[]={NT_FUNCTION,NT_OTHER_FUNCTIONS};
      addProd(G, NT_OTHER_FUNCTIONS, it, sy, 2); }

    /* P2: otherFunctions → ε */
    { addProd(G, NT_OTHER_FUNCTIONS, NULL, NULL, 0); }

    /* P3: function → TK_FUNID input_par output_par TK_SEM stmts TK_END */
    { int it[]={T,NT,NT,T,NT,T};
      int sy[]={TK_FUNID,NT_INPUT_PAR,NT_OUTPUT_PAR,TK_SEM,NT_STMTS,TK_END};
      addProd(G, NT_FUNCTION, it, sy, 6); }

    /* P4: mainFunction → TK_MAIN stmts TK_END */
    { int it[]={T,NT,T}; int sy[]={TK_MAIN,NT_STMTS,TK_END};
      addProd(G, NT_MAIN_FUNCTION, it, sy, 3); }

    /* P5: input_par → TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameterList TK_SQR */
    { int it[]={T,T,T,T,NT,T};
      int sy[]={TK_INPUT,TK_PARAMETER,TK_LIST,TK_SQL,NT_PARAMETER_LIST,TK_SQR};
      addProd(G, NT_INPUT_PAR, it, sy, 6); }

    /* P6: output_par → TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameterList TK_SQR */
    { int it[]={T,T,T,T,NT,T};
      int sy[]={TK_OUTPUT,TK_PARAMETER,TK_LIST,TK_SQL,NT_PARAMETER_LIST,TK_SQR};
      addProd(G, NT_OUTPUT_PAR, it, sy, 6); }

    /* P7: output_par → ε */
    { addProd(G, NT_OUTPUT_PAR, NULL, NULL, 0); }

    /* P8: parameterList → dataType TK_ID remainingList */
    { int it[]={NT,T,NT}; int sy[]={NT_DATA_TYPE,TK_ID,NT_REMAINING_LIST};
      addProd(G, NT_PARAMETER_LIST, it, sy, 3); }

    /* P9: dataType → primitiveDatatype */
    { int it[]={NT}; int sy[]={NT_PRIMITIVE_DATATYPE};
      addProd(G, NT_DATA_TYPE, it, sy, 1); }

    /* P10: dataType → constructedDatatype */
    { int it[]={NT}; int sy[]={NT_CONSTRUCTED_DATATYPE};
      addProd(G, NT_DATA_TYPE, it, sy, 1); }

    /* P11: primitiveDatatype → TK_INT */
    { int it[]={T}; int sy[]={TK_INT};
      addProd(G, NT_PRIMITIVE_DATATYPE, it, sy, 1); }

    /* P12: primitiveDatatype → TK_REAL */
    { int it[]={T}; int sy[]={TK_REAL};
      addProd(G, NT_PRIMITIVE_DATATYPE, it, sy, 1); }

    /* P13: constructedDatatype → TK_RECORD TK_RUID */
    { int it[]={T,T}; int sy[]={TK_RECORD,TK_RUID};
      addProd(G, NT_CONSTRUCTED_DATATYPE, it, sy, 2); }

    /* P14: constructedDatatype → TK_UNION TK_RUID */
    { int it[]={T,T}; int sy[]={TK_UNION,TK_RUID};
      addProd(G, NT_CONSTRUCTED_DATATYPE, it, sy, 2); }

    /* P15: constructedDatatype → TK_RUID */
    { int it[]={T}; int sy[]={TK_RUID};
      addProd(G, NT_CONSTRUCTED_DATATYPE, it, sy, 1); }

    /* P16: remainingList → TK_COMMA parameterList */
    { int it[]={T,NT}; int sy[]={TK_COMMA,NT_PARAMETER_LIST};
      addProd(G, NT_REMAINING_LIST, it, sy, 2); }

    /* P17: remainingList → ε */
    { addProd(G, NT_REMAINING_LIST, NULL, NULL, 0); }

    /* P18: stmts → typeDefinitions declarations otherStmts returnStmt */
    { int it[]={NT,NT,NT,NT};
      int sy[]={NT_TYPE_DEFINITIONS,NT_DECLARATIONS,NT_OTHER_STMTS,NT_RETURN_STMT};
      addProd(G, NT_STMTS, it, sy, 4); }

    /* P19: typeDefinitions → actualOrRedefined typeDefinitions */
    { int it[]={NT,NT}; int sy[]={NT_ACTUAL_OR_REDEFINED,NT_TYPE_DEFINITIONS};
      addProd(G, NT_TYPE_DEFINITIONS, it, sy, 2); }

    /* P20: typeDefinitions → ε */
    { addProd(G, NT_TYPE_DEFINITIONS, NULL, NULL, 0); }

    /* P21: actualOrRedefined → typeDefinition */
    { int it[]={NT}; int sy[]={NT_TYPE_DEFINITION};
      addProd(G, NT_ACTUAL_OR_REDEFINED, it, sy, 1); }

    /* P22: actualOrRedefined → definetypeStmt */
    { int it[]={NT}; int sy[]={NT_DEFINETYPE_STMT};
      addProd(G, NT_ACTUAL_OR_REDEFINED, it, sy, 1); }

    /* P23: typeDefinition → TK_RECORD TK_RUID fieldDefinitions TK_ENDRECORD */
    { int it[]={T,T,NT,T}; int sy[]={TK_RECORD,TK_RUID,NT_FIELD_DEFINITIONS,TK_ENDRECORD};
      addProd(G, NT_TYPE_DEFINITION, it, sy, 4); }

    /* P24: typeDefinition → TK_UNION TK_RUID fieldDefinitions TK_ENDUNION */
    { int it[]={T,T,NT,T}; int sy[]={TK_UNION,TK_RUID,NT_FIELD_DEFINITIONS,TK_ENDUNION};
      addProd(G, NT_TYPE_DEFINITION, it, sy, 4); }

    /* P25: fieldDefinitions → fieldDefinition fieldDefinition moreFields */
    { int it[]={NT,NT,NT};
      int sy[]={NT_FIELD_DEFINITION,NT_FIELD_DEFINITION,NT_MORE_FIELDS};
      addProd(G, NT_FIELD_DEFINITIONS, it, sy, 3); }

    /* P26: fieldDefinition → TK_TYPE fieldType TK_COLON TK_FIELDID TK_SEM */
    { int it[]={T,NT,T,T,T};
      int sy[]={TK_TYPE,NT_FIELD_TYPE,TK_COLON,TK_FIELDID,TK_SEM};
      addProd(G, NT_FIELD_DEFINITION, it, sy, 5); }

    /* P27: fieldType → primitiveDatatype */
    { int it[]={NT}; int sy[]={NT_PRIMITIVE_DATATYPE};
      addProd(G, NT_FIELD_TYPE, it, sy, 1); }

    /* P28: fieldType → constructedDatatype */
    { int it[]={NT}; int sy[]={NT_CONSTRUCTED_DATATYPE};
      addProd(G, NT_FIELD_TYPE, it, sy, 1); }

    /* P29: moreFields → fieldDefinition moreFields */
    { int it[]={NT,NT}; int sy[]={NT_FIELD_DEFINITION,NT_MORE_FIELDS};
      addProd(G, NT_MORE_FIELDS, it, sy, 2); }

    /* P30: moreFields → ε */
    { addProd(G, NT_MORE_FIELDS, NULL, NULL, 0); }

    /* P31: declarations → declaration declarations */
    { int it[]={NT,NT}; int sy[]={NT_DECLARATION,NT_DECLARATIONS};
      addProd(G, NT_DECLARATIONS, it, sy, 2); }

    /* P32: declarations → ε */
    { addProd(G, NT_DECLARATIONS, NULL, NULL, 0); }

    /* P33: declaration → TK_TYPE dataType TK_COLON TK_ID globalOrNot TK_SEM */
    { int it[]={T,NT,T,T,NT,T};
      int sy[]={TK_TYPE,NT_DATA_TYPE,TK_COLON,TK_ID,NT_GLOBAL_OR_NOT,TK_SEM};
      addProd(G, NT_DECLARATION, it, sy, 6); }

    /* P34: globalOrNot → TK_COLON TK_GLOBAL */
    { int it[]={T,T}; int sy[]={TK_COLON,TK_GLOBAL};
      addProd(G, NT_GLOBAL_OR_NOT, it, sy, 2); }

    /* P35: globalOrNot → ε */
    { addProd(G, NT_GLOBAL_OR_NOT, NULL, NULL, 0); }

    /* P36: otherStmts → stmt otherStmts */
    { int it[]={NT,NT}; int sy[]={NT_STMT,NT_OTHER_STMTS};
      addProd(G, NT_OTHER_STMTS, it, sy, 2); }

    /* P37: otherStmts → ε */
    { addProd(G, NT_OTHER_STMTS, NULL, NULL, 0); }

    /* P38: stmt → assignmentStmt */
    { int it[]={NT}; int sy[]={NT_ASSIGNMENT_STMT};
      addProd(G, NT_STMT, it, sy, 1); }

    /* P39: stmt → iterativeStmt */
    { int it[]={NT}; int sy[]={NT_ITERATIVE_STMT};
      addProd(G, NT_STMT, it, sy, 1); }

    /* P40: stmt → conditionalStmt */
    { int it[]={NT}; int sy[]={NT_CONDITIONAL_STMT};
      addProd(G, NT_STMT, it, sy, 1); }

    /* P41: stmt → ioStmt */
    { int it[]={NT}; int sy[]={NT_IO_STMT};
      addProd(G, NT_STMT, it, sy, 1); }

    /* P42: stmt → funCallStmt */
    { int it[]={NT}; int sy[]={NT_FUN_CALL_STMT};
      addProd(G, NT_STMT, it, sy, 1); }

    /* P43: assignmentStmt → singleOrRecId TK_ASSIGNOP expr TK_SEM */
    { int it[]={NT,T,NT,T}; int sy[]={NT_SINGLE_OR_REC_ID,TK_ASSIGNOP,NT_EXPR,TK_SEM};
      addProd(G, NT_ASSIGNMENT_STMT, it, sy, 4); }

    /* P44: singleOrRecId → TK_ID optionSingleConstructed */
    { int it[]={T,NT}; int sy[]={TK_ID,NT_OPTION_SINGLE_CONSTRUCTED};
      addProd(G, NT_SINGLE_OR_REC_ID, it, sy, 2); }

    /* P45: optionSingleConstructed → ε */
    { addProd(G, NT_OPTION_SINGLE_CONSTRUCTED, NULL, NULL, 0); }

    /* P46: optionSingleConstructed → oneExpansion moreExpansions */
    { int it[]={NT,NT}; int sy[]={NT_ONE_EXPANSION,NT_MORE_EXPANSIONS};
      addProd(G, NT_OPTION_SINGLE_CONSTRUCTED, it, sy, 2); }

    /* P47: oneExpansion → TK_DOT TK_FIELDID */
    { int it[]={T,T}; int sy[]={TK_DOT,TK_FIELDID};
      addProd(G, NT_ONE_EXPANSION, it, sy, 2); }

    /* P48: moreExpansions → oneExpansion moreExpansions */
    { int it[]={NT,NT}; int sy[]={NT_ONE_EXPANSION,NT_MORE_EXPANSIONS};
      addProd(G, NT_MORE_EXPANSIONS, it, sy, 2); }

    /* P49: moreExpansions → ε */
    { addProd(G, NT_MORE_EXPANSIONS, NULL, NULL, 0); }

    /* P50: funCallStmt → outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM */
    { int it[]={NT,T,T,T,T,NT,T};
      int sy[]={NT_OUTPUT_PARAMETERS,TK_CALL,TK_FUNID,TK_WITH,TK_PARAMETERS,NT_INPUT_PARAMETERS,TK_SEM};
      addProd(G, NT_FUN_CALL_STMT, it, sy, 7); }

    /* P51: outputParameters → TK_SQL idList TK_SQR TK_ASSIGNOP */
    { int it[]={T,NT,T,T}; int sy[]={TK_SQL,NT_ID_LIST,TK_SQR,TK_ASSIGNOP};
      addProd(G, NT_OUTPUT_PARAMETERS, it, sy, 4); }

    /* P52: outputParameters → ε */
    { addProd(G, NT_OUTPUT_PARAMETERS, NULL, NULL, 0); }

    /* P53: inputParameters → TK_SQL idList TK_SQR */
    { int it[]={T,NT,T}; int sy[]={TK_SQL,NT_ID_LIST,TK_SQR};
      addProd(G, NT_INPUT_PARAMETERS, it, sy, 3); }

    /* P54: iterativeStmt → TK_WHILE TK_OP boolExpr TK_CL stmt otherStmts TK_ENDWHILE */
    { int it[]={T,T,NT,T,NT,NT,T};
      int sy[]={TK_WHILE,TK_OP,NT_BOOL_EXPR,TK_CL,NT_STMT,NT_OTHER_STMTS,TK_ENDWHILE};
      addProd(G, NT_ITERATIVE_STMT, it, sy, 7); }

    /* P55: conditionalStmt → TK_IF TK_OP boolExpr TK_CL TK_THEN stmt otherStmts elsePart */
    { int it[]={T,T,NT,T,T,NT,NT,NT};
      int sy[]={TK_IF,TK_OP,NT_BOOL_EXPR,TK_CL,TK_THEN,NT_STMT,NT_OTHER_STMTS,NT_ELSE_PART};
      addProd(G, NT_CONDITIONAL_STMT, it, sy, 8); }

    /* P56: elsePart → TK_ELSE stmt otherStmts TK_ENDIF */
    { int it[]={T,NT,NT,T}; int sy[]={TK_ELSE,NT_STMT,NT_OTHER_STMTS,TK_ENDIF};
      addProd(G, NT_ELSE_PART, it, sy, 4); }

    /* P57: elsePart → TK_ENDIF */
    { int it[]={T}; int sy[]={TK_ENDIF};
      addProd(G, NT_ELSE_PART, it, sy, 1); }

    /* P58: ioStmt → TK_READ TK_OP var TK_CL TK_SEM */
    { int it[]={T,T,NT,T,T}; int sy[]={TK_READ,TK_OP,NT_VAR,TK_CL,TK_SEM};
      addProd(G, NT_IO_STMT, it, sy, 5); }

    /* P59: ioStmt → TK_WRITE TK_OP var TK_CL TK_SEM */
    { int it[]={T,T,NT,T,T}; int sy[]={TK_WRITE,TK_OP,NT_VAR,TK_CL,TK_SEM};
      addProd(G, NT_IO_STMT, it, sy, 5); }

    /* P60: expr → term exprPrime */
    { int it[]={NT,NT}; int sy[]={NT_TERM,NT_EXPR_PRIME};
      addProd(G, NT_EXPR, it, sy, 2); }

    /* P61: exprPrime → lowPrecOp term exprPrime */
    { int it[]={NT,NT,NT}; int sy[]={NT_LOW_PREC_OP,NT_TERM,NT_EXPR_PRIME};
      addProd(G, NT_EXPR_PRIME, it, sy, 3); }

    /* P62: exprPrime → ε */
    { addProd(G, NT_EXPR_PRIME, NULL, NULL, 0); }

    /* P63: term → factor termPrime */
    { int it[]={NT,NT}; int sy[]={NT_FACTOR,NT_TERM_PRIME};
      addProd(G, NT_TERM, it, sy, 2); }

    /* P64: termPrime → highPrecOp factor termPrime */
    { int it[]={NT,NT,NT}; int sy[]={NT_HIGH_PREC_OP,NT_FACTOR,NT_TERM_PRIME};
      addProd(G, NT_TERM_PRIME, it, sy, 3); }

    /* P65: termPrime → ε */
    { addProd(G, NT_TERM_PRIME, NULL, NULL, 0); }

    /* P66: factor → TK_OP expr TK_CL */
    { int it[]={T,NT,T}; int sy[]={TK_OP,NT_EXPR,TK_CL};
      addProd(G, NT_FACTOR, it, sy, 3); }

    /* P67: factor → var */
    { int it[]={NT}; int sy[]={NT_VAR};
      addProd(G, NT_FACTOR, it, sy, 1); }

    /* P68: highPrecOp → TK_MUL */
    { int it[]={T}; int sy[]={TK_MUL}; addProd(G, NT_HIGH_PREC_OP, it, sy, 1); }

    /* P69: highPrecOp → TK_DIV */
    { int it[]={T}; int sy[]={TK_DIV}; addProd(G, NT_HIGH_PREC_OP, it, sy, 1); }

    /* P70: lowPrecOp → TK_PLUS */
    { int it[]={T}; int sy[]={TK_PLUS}; addProd(G, NT_LOW_PREC_OP, it, sy, 1); }

    /* P71: lowPrecOp → TK_MINUS */
    { int it[]={T}; int sy[]={TK_MINUS}; addProd(G, NT_LOW_PREC_OP, it, sy, 1); }

    /* P72: boolExpr → TK_OP boolExpr TK_CL logicalOp TK_OP boolExpr TK_CL */
    { int it[]={T,NT,T,NT,T,NT,T};
      int sy[]={TK_OP,NT_BOOL_EXPR,TK_CL,NT_LOGICAL_OP,TK_OP,NT_BOOL_EXPR,TK_CL};
      addProd(G, NT_BOOL_EXPR, it, sy, 7); }

    /* P73: boolExpr → var relOp var */
    { int it[]={NT,NT,NT}; int sy[]={NT_VAR,NT_REL_OP,NT_VAR};
      addProd(G, NT_BOOL_EXPR, it, sy, 3); }

    /* P74: boolExpr → TK_NOT TK_OP boolExpr TK_CL */
    { int it[]={T,T,NT,T}; int sy[]={TK_NOT,TK_OP,NT_BOOL_EXPR,TK_CL};
      addProd(G, NT_BOOL_EXPR, it, sy, 4); }

    /* P75: var → singleOrRecId */
    { int it[]={NT}; int sy[]={NT_SINGLE_OR_REC_ID};
      addProd(G, NT_VAR, it, sy, 1); }

    /* P76: var → TK_NUM */
    { int it[]={T}; int sy[]={TK_NUM}; addProd(G, NT_VAR, it, sy, 1); }

    /* P77: var → TK_RNUM */
    { int it[]={T}; int sy[]={TK_RNUM}; addProd(G, NT_VAR, it, sy, 1); }

    /* P78: logicalOp → TK_AND */
    { int it[]={T}; int sy[]={TK_AND}; addProd(G, NT_LOGICAL_OP, it, sy, 1); }

    /* P79: logicalOp → TK_OR */
    { int it[]={T}; int sy[]={TK_OR}; addProd(G, NT_LOGICAL_OP, it, sy, 1); }

    /* P80–P85: relOp → TK_LT | TK_LE | TK_EQ | TK_GT | TK_GE | TK_NE */
    { int it[]={T}; int sy[]={TK_LT}; addProd(G, NT_REL_OP, it, sy, 1); }
    { int it[]={T}; int sy[]={TK_LE}; addProd(G, NT_REL_OP, it, sy, 1); }
    { int it[]={T}; int sy[]={TK_EQ}; addProd(G, NT_REL_OP, it, sy, 1); }
    { int it[]={T}; int sy[]={TK_GT}; addProd(G, NT_REL_OP, it, sy, 1); }
    { int it[]={T}; int sy[]={TK_GE}; addProd(G, NT_REL_OP, it, sy, 1); }
    { int it[]={T}; int sy[]={TK_NE}; addProd(G, NT_REL_OP, it, sy, 1); }

    /* P86: returnStmt → TK_RETURN optReturnVal TK_SEM */
    { int it[]={T,NT,T}; int sy[]={TK_RETURN,NT_OPT_RETURN_VAL,TK_SEM};
      addProd(G, NT_RETURN_STMT, it, sy, 3); }

    /* P87: optReturnVal → TK_SQL idList TK_SQR */
    { int it[]={T,NT,T}; int sy[]={TK_SQL,NT_ID_LIST,TK_SQR};
      addProd(G, NT_OPT_RETURN_VAL, it, sy, 3); }

    /* P88: optReturnVal → ε */
    { addProd(G, NT_OPT_RETURN_VAL, NULL, NULL, 0); }

    /* P89: idList → TK_ID moreIds */
    { int it[]={T,NT}; int sy[]={TK_ID,NT_MORE_IDS};
      addProd(G, NT_ID_LIST, it, sy, 2); }

    /* P90: moreIds → TK_COMMA idList */
    { int it[]={T,NT}; int sy[]={TK_COMMA,NT_ID_LIST};
      addProd(G, NT_MORE_IDS, it, sy, 2); }

    /* P91: moreIds → ε */
    { addProd(G, NT_MORE_IDS, NULL, NULL, 0); }

    /* P92: definetypeStmt → TK_DEFINETYPE A TK_RUID TK_AS TK_RUID */
    { int it[]={T,NT,T,T,T};
      int sy[]={TK_DEFINETYPE,NT_A,TK_RUID,TK_AS,TK_RUID};
      addProd(G, NT_DEFINETYPE_STMT, it, sy, 5); }

    /* P93: A → TK_RECORD */
    { int it[]={T}; int sy[]={TK_RECORD}; addProd(G, NT_A, it, sy, 1); }

    /* P94: A → TK_UNION */
    { int it[]={T}; int sy[]={TK_UNION}; addProd(G, NT_A, it, sy, 1); }
}

#undef T
#undef NT

/* ============================================================ */
/*            FIRST AND FOLLOW SET COMPUTATION                  */
/* ============================================================ */

/*
 * Compute FIRST of a sequence of RHS symbols starting at index `start`.
 * Writes results into `outFirst[NUM_TERMINALS]` and *outHasEps.
 */
static void firstOfSeq(Grammar *G, FirstAndFollow *F,
                       GrammarSymbol *rhs, int len, int start,
                       bool *outFirst, bool *outHasEps)
{
    *outHasEps = true; /* assume epsilon until we find a non-nullable symbol */

    for (int i = start; i < len; i++) {
        GrammarSymbol sym = rhs[i];
        if (sym.isTerminal) {
            outFirst[sym.symbol] = true;
            *outHasEps = false;
            return;
        } else {
            NonTerminal nt = (NonTerminal)sym.symbol;
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (F->first[nt][t]) outFirst[t] = true;
            }
            if (!F->firstHasEps[nt]) {
                *outHasEps = false;
                return;
            }
        }
    }
    /* reached end of sequence: epsilon is possible */
}

FirstAndFollow computeFirstAndFollowSets(Grammar *G)
{
    FirstAndFollow F;
    memset(&F, 0, sizeof(F));

    /* --- Compute FIRST sets (iterative fixpoint) --- */
    bool changed = true;
    while (changed) {
        changed = false;
        for (int p = 0; p < G->numProds; p++) {
            Production *prod = &G->prods[p];
            NonTerminal A    = prod->lhs;

            if (prod->rhsLen == 0) {
                /* epsilon production */
                if (!F.firstHasEps[A]) { F.firstHasEps[A] = true; changed = true; }
                continue;
            }

            bool seqHasEps = true;
            for (int i = 0; i < prod->rhsLen; i++) {
                GrammarSymbol sym = prod->rhs[i];
                if (sym.isTerminal) {
                    if (!F.first[A][sym.symbol]) {
                        F.first[A][sym.symbol] = true; changed = true;
                    }
                    seqHasEps = false;
                    break;
                } else {
                    NonTerminal nt = (NonTerminal)sym.symbol;
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (F.first[nt][t] && !F.first[A][t]) {
                            F.first[A][t] = true; changed = true;
                        }
                    }
                    if (!F.firstHasEps[nt]) { seqHasEps = false; break; }
                }
            }
            if (seqHasEps && !F.firstHasEps[A]) {
                F.firstHasEps[A] = true; changed = true;
            }
        }
    }

    /* --- Compute FOLLOW sets (iterative fixpoint) --- */
    /* Add $ to FOLLOW(program) */
    F.follow[NT_PROGRAM][TK_EOF] = true;

    changed = true;
    while (changed) {
        changed = false;
        for (int p = 0; p < G->numProds; p++) {
            Production *prod = &G->prods[p];
            NonTerminal A    = prod->lhs;

            for (int i = 0; i < prod->rhsLen; i++) {
                if (prod->rhs[i].isTerminal) continue;
                NonTerminal B = (NonTerminal)prod->rhs[i].symbol;

                /* compute FIRST of the suffix after B */
                bool suffixFirst[NUM_TERMINALS];
                bool suffixHasEps;
                memset(suffixFirst, 0, sizeof(suffixFirst));
                firstOfSeq(G, &F, prod->rhs, prod->rhsLen, i + 1,
                           suffixFirst, &suffixHasEps);

                for (int t = 0; t < NUM_TERMINALS; t++) {
                    if (suffixFirst[t] && !F.follow[B][t]) {
                        F.follow[B][t] = true; changed = true;
                    }
                }
                /* if suffix can derive ε, add FOLLOW(A) to FOLLOW(B) */
                if (suffixHasEps) {
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (F.follow[A][t] && !F.follow[B][t]) {
                            F.follow[B][t] = true; changed = true;
                        }
                    }
                }
            }
        }
    }

    return F;
}

/* ============================================================ */
/*                 PARSE TABLE CONSTRUCTION                     */
/* ============================================================ */

void createParseTable(FirstAndFollow *F, Grammar *G, ParseTable T)
{
    /* initialize all entries to -1 (error) */
    for (int nt = 0; nt < NUM_NON_TERMINALS; nt++)
        for (int t = 0; t < NUM_TERMINALS; t++)
            T[nt][t] = -1;

    for (int p = 0; p < G->numProds; p++) {
        Production *prod = &G->prods[p];
        NonTerminal A    = prod->lhs;

        if (prod->rhsLen == 0) {
            /* A → ε : add A → ε for each token in FOLLOW(A) */
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (F->follow[A][t]) {
                    if (T[A][t] == -1) T[A][t] = p;
                }
            }
        } else {
            /* compute FIRST of the RHS */
            bool rhsFirst[NUM_TERMINALS];
            bool rhsHasEps;
            memset(rhsFirst, 0, sizeof(rhsFirst));
            firstOfSeq(G, F, prod->rhs, prod->rhsLen, 0, rhsFirst, &rhsHasEps);

            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (rhsFirst[t]) {
                    if (T[A][t] == -1) T[A][t] = p;
                }
            }
            if (rhsHasEps) {
                for (int t = 0; t < NUM_TERMINALS; t++) {
                    if (F->follow[A][t]) {
                        if (T[A][t] == -1) T[A][t] = p;
                    }
                }
            }
        }
    }
}

/* ============================================================ */
/*                 PARSE TREE UTILITIES                         */
/* ============================================================ */

static TreeNode *newNode(int isLeaf, int symbol)
{
    TreeNode *n = (TreeNode *)malloc(sizeof(TreeNode));
    memset(n, 0, sizeof(TreeNode));
    n->isLeaf = isLeaf;
    n->symbol = symbol;
    strcpy(n->lexeme, "----");
    n->lineNo   = 0;
    n->hasNumVal = 0;
    return n;
}

void freeTree(TreeNode *node)
{
    if (!node) return;
    for (int i = 0; i < node->numChildren; i++)
        freeTree(node->children[i]);
    free(node);
}

/* ============================================================ */
/*                       PARSER                                 */
/* ============================================================ */

#define PARSE_STACK_MAX 1024
#define MAX_ERRORS_PER_LINE 2

/* Skip comments and error tokens; return first real token */
static tokenInfo getNextUsableToken(twinBuffer B)
{
    tokenInfo tk;
    do {
        tk = getNextToken(B);
    } while (tk->tokenType == TK_COMMENT || tk->tokenType == TK_ERROR);
    return tk;
}

/* Check if a token is in the synchronization set */
static int isSyncToken(TokenType t)
{
    return t == TK_SEM || t == TK_ENDRECORD || t == TK_ENDUNION ||
           t == TK_ENDIF || t == TK_ENDWHILE || t == TK_ELSE ||
           t == TK_CL || t == TK_SQR || t == TK_END;
}

ParseTree parseInputSourceCode(char *testcaseFile, ParseTable T, Grammar *G, FirstAndFollow *F)
{
    FILE *fp = fopen(testcaseFile, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open '%s'\n", testcaseFile);
        return NULL;
    }

    lineNumber = 1;
    twinBuffer B = initBuffer(fp);

    /* Allocate root node for the start symbol */
    TreeNode *root = newNode(0 /* non-leaf */, (int)NT_PROGRAM);

    /* Symbol stack and parallel tree-node stack */
    GrammarSymbol symStack[PARSE_STACK_MAX];
    TreeNode     *nodeStack[PARSE_STACK_MAX];
    int top = 0;

    /* Push EOF sentinel */
    symStack[top].isTerminal = 1;
    symStack[top].symbol     = TK_EOF;
    nodeStack[top]           = NULL;
    top++;

    /* Push start symbol */
    symStack[top].isTerminal = 0;
    symStack[top].symbol     = (int)NT_PROGRAM;
    nodeStack[top]           = root;
    top++;

    tokenInfo curTok = getNextUsableToken(B);
    int hasError = 0;
    int lastErrorLine = -1;       /* track last error line to suppress cascading */
    int errorCountOnLine = 0;     /* count of errors on the same line */

    while (top > 0) {
        GrammarSymbol X    = symStack[top - 1];
        TreeNode     *Xnod = nodeStack[top - 1];

        /* ---- EOF sentinel on top ---- */
        if (X.isTerminal && X.symbol == (int)TK_EOF) {
            if (curTok->tokenType == TK_EOF) {
                top--;
                break;  /* successful parse */
            } else {
                printf("Line %d : Syntax Error : Extra tokens after end of program\n",
                       curTok->lineNo);
                hasError = 1;
                break;
            }
        }

        /* ---- Terminal on top ---- */
        if (X.isTerminal) {
            if (curTok->tokenType == (TokenType)X.symbol) {
                /* Match! Fill in the leaf node */
                if (Xnod) {
                    strcpy(Xnod->lexeme, curTok->lexeme);
                    Xnod->lineNo    = curTok->lineNo;
                    Xnod->tokenType = curTok->tokenType;
                    Xnod->isLeaf    = 1;
                    if (curTok->tokenType == TK_NUM) {
                        Xnod->numVal    = (double)atoi(curTok->lexeme);
                        Xnod->hasNumVal = 1;
                    } else if (curTok->tokenType == TK_RNUM) {
                        Xnod->numVal    = atof(curTok->lexeme);
                        Xnod->hasNumVal = 1;
                    }
                }
                top--;
                free(curTok);
                curTok = getNextUsableToken(B);
            } else {
                /* Terminal mismatch error */
                if (curTok->lineNo != lastErrorLine) {
                    errorCountOnLine = 0;
                    lastErrorLine = curTok->lineNo;
                }
                if (errorCountOnLine < MAX_ERRORS_PER_LINE) {
                    printf("Line %d Error: The token %s for lexeme %s  does not match with the expected token %s\n",
                           curTok->lineNo,
                           tokenStrings[curTok->tokenType],
                           curTok->lexeme,
                           tokenStrings[X.symbol]);
                    errorCountOnLine++;
                }
                hasError = 1;
                /* Pop the unmatched terminal and continue */
                top--;
            }
            continue;
        }

        /* ---- Non-terminal on top ---- */
        NonTerminal nt = (NonTerminal)X.symbol;
        int prodIdx    = T[nt][curTok->tokenType];

        if (prodIdx == -1) {
            /* No production: syntax error */
            if (curTok->lineNo != lastErrorLine) {
                errorCountOnLine = 0;
                lastErrorLine = curTok->lineNo;
            }
            if (errorCountOnLine < MAX_ERRORS_PER_LINE) {
                printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                       curTok->lineNo,
                       tokenStrings[curTok->tokenType],
                       curTok->lexeme,
                       nonTerminalStrings[nt]);
                errorCountOnLine++;
            }
            hasError = 1;

            /* Panic-mode recovery using FOLLOW sets and sync tokens */
            top--;  /* pop the non-terminal */

            if (curTok->tokenType == TK_EOF) {
                /* nothing to skip */
            } else if (F->follow[nt][curTok->tokenType]) {
                /* Token is in FOLLOW(NT): keep token, continue parsing */
            } else {
                /* Skip tokens until FOLLOW(NT) or sync set or EOF */
                while (curTok->tokenType != TK_EOF &&
                       !F->follow[nt][curTok->tokenType] &&
                       !isSyncToken(curTok->tokenType)) {
                    free(curTok);
                    curTok = getNextUsableToken(B);
                }
                /* If we stopped at a sync token, pop stack symbols until
                 * we find a terminal that matches or a non-terminal that
                 * has a valid parse table entry for the current token.
                 * This reduces cascading errors. */
                if (curTok->tokenType != TK_EOF &&
                    isSyncToken(curTok->tokenType)) {
                    while (top > 0) {
                        GrammarSymbol topSym = symStack[top - 1];
                        if (topSym.isTerminal) {
                            if (topSym.symbol == (int)curTok->tokenType)
                                break;  /* found matching terminal */
                        } else {
                            int pi = T[(NonTerminal)topSym.symbol][curTok->tokenType];
                            if (pi != -1)
                                break;  /* NT can accept this token */
                        }
                        top--;  /* pop unmatched symbol */
                    }
                }
            }
            continue;
        }

        /* Apply production */
        top--;
        Production *prod = &G->prods[prodIdx];

        if (prod->rhsLen == 0) {
            /* Epsilon production: no children pushed to the stack */
            /* We mark Xnod as an internal node with 0 children */
            if (Xnod) Xnod->isLeaf = 0;
        } else {
            if (Xnod) Xnod->isLeaf = 0;

            /* Create child nodes in left-to-right order */
            TreeNode *children[MAX_PROD_LEN];
            for (int i = 0; i < prod->rhsLen; i++) {
                int childIsLeaf  = prod->rhs[i].isTerminal;
                children[i] = newNode(childIsLeaf, prod->rhs[i].symbol);
                if (Xnod) {
                    children[i]->parent = Xnod;
                    Xnod->children[Xnod->numChildren++] = children[i];
                }
            }

            /* Push children in REVERSE order so leftmost is on top */
            for (int i = prod->rhsLen - 1; i >= 0; i--) {
                if (top >= PARSE_STACK_MAX) {
                    fprintf(stderr, "Parse stack overflow!\n");
                    freeTree(root);
                    free(curTok);
                    free(B);
                    fclose(fp);
                    return NULL;
                }
                symStack[top]  = prod->rhs[i];
                nodeStack[top] = children[i];
                top++;
            }
        }
    }

    free(curTok);
    free(B);
    fclose(fp);

    if (!hasError) {
        printf("Input source code is syntactically correct...........\n");
        return root;
    } else {
        return root; /* return partial tree even on error */
    }
}

/* ============================================================ */
/*                 PARSE TREE PRINTING                          */
/* ============================================================ */

/*
 * Pre-order (DFS) traversal of the parse tree.
 *
 * Each line contains:
 *   col1: lexeme (leaf) or "----" (internal)
 *   col2: line number (leaf) or "----" (internal)
 *   col3: token name (leaf) or "----" (internal)
 *   col4: numeric value (TK_NUM/TK_RNUM) or "----"
 *   col5: parent NT name (or "ROOT" for the root)
 *   col6: "yes" / "no"
 *   col7: NT symbol name (internal) or "----" (leaf)
 */
static void traverseTree(TreeNode *node, FILE *out)
{
    if (!node) return;

    /* Determine parent symbol string */
    char parentStr[64];
    if (!node->parent) {
        strcpy(parentStr, "ROOT");
    } else if (!node->parent->isLeaf) {
        strcpy(parentStr, nonTerminalStrings[node->parent->symbol]);
    } else {
        strcpy(parentStr, "----");
    }

    if (node->isLeaf && node->lineNo > 0) {
        /* Leaf node (matched terminal) */
        char valStr[64];
        if (node->hasNumVal) {
            if (node->tokenType == TK_NUM)
                snprintf(valStr, sizeof(valStr), "%d", (int)node->numVal);
            else
                snprintf(valStr, sizeof(valStr), "%g", node->numVal);
        } else {
            strcpy(valStr, "----");
        }

        fprintf(out, "%-20s  %-4d  %-20s  %-12s  %-20s  yes   ----\n",
                node->lexeme,
                node->lineNo,
                tokenStrings[node->tokenType],
                valStr,
                parentStr);
    } else if (!node->isLeaf) {
        /* Internal (non-terminal) node */
        fprintf(out, "%-20s  %-4s  %-20s  %-12s  %-20s  no    %s\n",
                "----",
                "----",
                "----",
                "----",
                parentStr,
                nonTerminalStrings[node->symbol]);

        /* Recurse over children */
        for (int i = 0; i < node->numChildren; i++)
            traverseTree(node->children[i], out);
    }
    /* If isLeaf but lineNo == 0 (unmatched placeholder), skip */
}

void printParseTree(ParseTree PT, char *outfile)
{
    if (!PT) {
        fprintf(stderr, "Parse tree is NULL; nothing to print.\n");
        return;
    }

    FILE *out = fopen(outfile, "w");
    if (!out) {
        fprintf(stderr, "Error: cannot open output file '%s'\n", outfile);
        return;
    }

    /* Header */
    fprintf(out, "%-20s  %-4s  %-20s  %-12s  %-20s  %-5s  %s\n",
            "Lexeme", "Line", "TokenName", "Value", "ParentNT", "Leaf", "NodeNT");
    fprintf(out,
            "---------------------------------------------------------------------"
            "--------------------------------\n");

    traverseTree(PT, out);
    fclose(out);
    printf("Parse tree written to '%s'.\n", outfile);
}
