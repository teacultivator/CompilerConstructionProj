#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

// ============================================================================
// HELPER ARRAYS FOR PRINTING (Mapping Enums to Strings)
// ============================================================================
const char* tokenStr[] = {
    "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", 
    "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", 
    "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN", 
    "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", 
    "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", 
    "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", 
    "TK_READ", "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", 
    "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", 
    "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE", 
    "TK_ERROR", "TK_EOF"
};

const char* nonTermStr[] = {
    "program", "mainFunction", "otherFunctions", "function", "input_par", 
    "output_par", "parameter_list", "dataType", "primitiveDatatype", 
    "constructedDatatype", "remaining_list", "stmts", "typeDefinitions", 
    "actualOrRedefined", "typeDefinition", "fieldDefinitions", 
    "fieldDefinition", "fieldType", "moreFields", "declarations", 
    "declaration", "global_or_not", "otherStmts", "stmt", "assignmentStmt", 
    "singleOrRecId", "optionSingleConstructed", "oneExpansion", 
    "moreExpansions", "funCallStmt", "outputParameters", "inputParameters", 
    "iterativeStmt", "conditionalStmt", "elsePart", "ioStmt", 
    "arithmeticExpression", "expPrime", "term", "termPrime", "factor", 
    "highPrecedenceOperators", "lowPrecedenceOperators", "booleanExpression", 
    "var", "logicalOp", "relationalOp", "returnStmt", "optionalReturn", 
    "idList", "more_ids", "definetypestmt", "A"
};

// ============================================================================
// STACK OPERATIONS
// ============================================================================
Stack createStack() {
    return NULL;
}

void push(Stack *s, GrammarSymbol sym, treeNode *tNode) {
    stackNode *newNode = (stackNode *)malloc(sizeof(stackNode));
    newNode->symbol = sym;
    newNode->treeNodePtr = tNode;
    newNode->next = *s;
    *s = newNode;
}

stackNode pop(Stack *s) {
    if (*s == NULL) {
        printf("Fatal Error: Stack Underflow.\n");
        exit(1);
    }
    stackNode *temp = *s;
    stackNode poppedData = *temp;
    *s = (*s)->next;
    free(temp);
    return poppedData;
}

int isStackEmpty(Stack s) {
    return (s == NULL);
}

// ============================================================================
// TREE NODE CREATION
// ============================================================================
treeNode* createTreeNode(GrammarSymbol sym, treeNode* parent) {
    treeNode *newNode = (treeNode *)malloc(sizeof(treeNode));
    newNode->isLeafNode = sym.isTerminal;
    newNode->symbol = sym;
    newNode->token = NULL;
    newNode->parent = parent;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// ============================================================================
// SETUP: FIRST, FOLLOW, AND PARSE TABLE
// ============================================================================
void ComputeFirstAndFollowSets(FirstAndFollow *F) {
    // Left empty for manual population or hardcoding by the team
    for(int i = 0; i < 100; i++) {
        for(int j = 0; j < 100; j++) {
            F->first[i][j] = 0;
            F->follow[i][j] = 0;
        }
    }
}

void createParseTable(FirstAndFollow F, table *T) {
    // 1. Initialize all cells to -1 (Syntax Error State)
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        for(int j = 0; j <= TK_EOF; j++) {
            T->rules[i][j] = -1;
        }
    }

    // ========================================================================
    // POPULATING THE PARSE TABLE USING LL(1) FIRST AND FOLLOW SETS
    // ========================================================================

    // Rule 1: <program> ===> <otherFunctions> <mainFunction>
    T->rules[NT_PROGRAM][TK_FUNID] = 1;
    T->rules[NT_PROGRAM][TK_MAIN] = 1;

    // Rule 2: <mainFunction> ===> TK_MAIN <stmts> TK_END
    T->rules[NT_MAIN_FUNCTION][TK_MAIN] = 2;

    // Rule 3: <otherFunctions> ===> <function> <otherFunctions>
    T->rules[NT_OTHER_FUNCTIONS][TK_FUNID] = 3;

    // Rule 4: <otherFunctions> ===> eps  (FOLLOW = TK_MAIN)
    T->rules[NT_OTHER_FUNCTIONS][TK_MAIN] = 4;

    // Rule 5: <function> ===> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
    T->rules[NT_FUNCTION][TK_FUNID] = 5;

    // Rule 6: <input_par> ===> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
    T->rules[NT_INPUT_PAR][TK_INPUT] = 6;

    // Rule 7: <output_par> ===> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
    T->rules[NT_OUTPUT_PAR][TK_OUTPUT] = 7;

    // Rule 8: <output_par> ===> eps (FOLLOW = TK_SEM)
    T->rules[NT_OUTPUT_PAR][TK_SEM] = 8;

    // Rule 9: <parameter_list> ===> <dataType> TK_ID <remaining_list>
    T->rules[NT_PARAMETER_LIST][TK_INT] = 9;
    T->rules[NT_PARAMETER_LIST][TK_REAL] = 9;
    T->rules[NT_PARAMETER_LIST][TK_RECORD] = 9;
    T->rules[NT_PARAMETER_LIST][TK_UNION] = 9;
    T->rules[NT_PARAMETER_LIST][TK_RUID] = 9;

    // Rule 10: <dataType> ===> <primitiveDatatype>
    T->rules[NT_DATA_TYPE][TK_INT] = 10;
    T->rules[NT_DATA_TYPE][TK_REAL] = 10;

    // Rule 11: <dataType> ===> <constructedDatatype>
    T->rules[NT_DATA_TYPE][TK_RECORD] = 11;
    T->rules[NT_DATA_TYPE][TK_UNION] = 11;
    T->rules[NT_DATA_TYPE][TK_RUID] = 11;

    // Rule 12: <primitiveDatatype> ===> TK_INT
    T->rules[NT_PRIMITIVE_DATATYPE][TK_INT] = 12;

    // Rule 13: <primitiveDatatype> ===> TK_REAL
    T->rules[NT_PRIMITIVE_DATATYPE][TK_REAL] = 13;

    // Rule 14: <constructedDatatype> ===> TK_RECORD TK_RUID
    T->rules[NT_CONSTRUCTED_DATATYPE][TK_RECORD] = 14;

    // Rule 15: <constructedDatatype> ===> TK_UNION TK_RUID
    T->rules[NT_CONSTRUCTED_DATATYPE][TK_UNION] = 15;

    // Rule 16: <constructedDatatype> ===> TK_RUID
    T->rules[NT_CONSTRUCTED_DATATYPE][TK_RUID] = 16;

    // Rule 17: <remaining_list> ===> TK_COMMA <parameter_list>
    T->rules[NT_REMAINING_LIST][TK_COMMA] = 17;

    // Rule 18: <remaining_list> ===> eps (FOLLOW = TK_SQR)
    T->rules[NT_REMAINING_LIST][TK_SQR] = 18;

    // Rule 19: <stmts> ===> <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    // FIRST(<stmts>) is massive
    T->rules[NT_STMTS][TK_RECORD] = 19;
    T->rules[NT_STMTS][TK_UNION] = 19;
    T->rules[NT_STMTS][TK_DEFINETYPE] = 19;
    T->rules[NT_STMTS][TK_TYPE] = 19;
    T->rules[NT_STMTS][TK_ID] = 19;
    T->rules[NT_STMTS][TK_WHILE] = 19;
    T->rules[NT_STMTS][TK_IF] = 19;
    T->rules[NT_STMTS][TK_READ] = 19;
    T->rules[NT_STMTS][TK_WRITE] = 19;
    T->rules[NT_STMTS][TK_SQL] = 19;
    T->rules[NT_STMTS][TK_CALL] = 19;
    T->rules[NT_STMTS][TK_RETURN] = 19;

    // Rule 20: <typeDefinitions> ===> <actualOrRedefined> <typeDefinitions>
    T->rules[NT_TYPE_DEFINITIONS][TK_RECORD] = 20;
    T->rules[NT_TYPE_DEFINITIONS][TK_UNION] = 20;
    T->rules[NT_TYPE_DEFINITIONS][TK_DEFINETYPE] = 20;

    // Rule 21: <typeDefinitions> ===> eps (FOLLOW = TK_TYPE, TK_ID, TK_WHILE, TK_IF...)
    T->rules[NT_TYPE_DEFINITIONS][TK_TYPE] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_ID] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_WHILE] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_IF] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_READ] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_WRITE] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_SQL] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_CALL] = 21;
    T->rules[NT_TYPE_DEFINITIONS][TK_RETURN] = 21;

    // Rule 22: <actualOrRedefined> ===> <typeDefinition>
    T->rules[NT_ACTUAL_OR_REDEFINED][TK_RECORD] = 22;
    T->rules[NT_ACTUAL_OR_REDEFINED][TK_UNION] = 22;

    // Rule 23: <actualOrRedefined> ===> <definetypestmt>
    T->rules[NT_ACTUAL_OR_REDEFINED][TK_DEFINETYPE] = 23;

    // Rule 24: <typeDefinition> ===> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD
    T->rules[NT_TYPE_DEFINITION][TK_RECORD] = 24;

    // Rule 25: <typeDefinition> ===> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
    T->rules[NT_TYPE_DEFINITION][TK_UNION] = 25;

    // Rule 26: <fieldDefinitions> ===> <fieldDefinition> <fieldDefinition> <moreFields>
    T->rules[NT_FIELD_DEFINITIONS][TK_TYPE] = 26;

    // Rule 27: <fieldDefinition> ===> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM
    T->rules[NT_FIELD_DEFINITION][TK_TYPE] = 27;

    // Rule 28: <fieldType> ===> <primitiveDatatype>
    T->rules[NT_FIELD_TYPE][TK_INT] = 28;
    T->rules[NT_FIELD_TYPE][TK_REAL] = 28;

    // Rule 29: <fieldType> ===> <constructedDatatype>
    T->rules[NT_FIELD_TYPE][TK_RECORD] = 29;
    T->rules[NT_FIELD_TYPE][TK_UNION] = 29;
    T->rules[NT_FIELD_TYPE][TK_RUID] = 29;

    // Rule 30: <moreFields> ===> <fieldDefinition> <moreFields>
    T->rules[NT_MORE_FIELDS][TK_TYPE] = 30;

    // Rule 31: <moreFields> ===> eps (FOLLOW = TK_ENDRECORD, TK_ENDUNION)
    T->rules[NT_MORE_FIELDS][TK_ENDRECORD] = 31;
    T->rules[NT_MORE_FIELDS][TK_ENDUNION] = 31;

    // Rule 32: <declarations> ===> <declaration> <declarations>
    T->rules[NT_DECLARATIONS][TK_TYPE] = 32;

    // Rule 33: <declarations> ===> eps (FOLLOW = TK_ID, TK_WHILE...)
    T->rules[NT_DECLARATIONS][TK_ID] = 33;
    T->rules[NT_DECLARATIONS][TK_WHILE] = 33;
    T->rules[NT_DECLARATIONS][TK_IF] = 33;
    T->rules[NT_DECLARATIONS][TK_READ] = 33;
    T->rules[NT_DECLARATIONS][TK_WRITE] = 33;
    T->rules[NT_DECLARATIONS][TK_SQL] = 33;
    T->rules[NT_DECLARATIONS][TK_CALL] = 33;
    T->rules[NT_DECLARATIONS][TK_RETURN] = 33;

    // Rule 34: <declaration> ===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
    T->rules[NT_DECLARATION][TK_TYPE] = 34;

    // Rule 35: <global_or_not> ===> TK_COLON TK_GLOBAL
    T->rules[NT_GLOBAL_OR_NOT][TK_COLON] = 35;

    // Rule 36: <global_or_not> ===> eps (FOLLOW = TK_SEM)
    T->rules[NT_GLOBAL_OR_NOT][TK_SEM] = 36;

    // Rule 37: <otherStmts> ===> <stmt> <otherStmts>
    T->rules[NT_OTHER_STMTS][TK_ID] = 37;
    T->rules[NT_OTHER_STMTS][TK_WHILE] = 37;
    T->rules[NT_OTHER_STMTS][TK_IF] = 37;
    T->rules[NT_OTHER_STMTS][TK_READ] = 37;
    T->rules[NT_OTHER_STMTS][TK_WRITE] = 37;
    T->rules[NT_OTHER_STMTS][TK_SQL] = 37;
    T->rules[NT_OTHER_STMTS][TK_CALL] = 37;

    // Rule 38: <otherStmts> ===> eps (FOLLOW = TK_RETURN, TK_ENDWHILE, TK_ENDIF, TK_ELSE)
    T->rules[NT_OTHER_STMTS][TK_RETURN] = 38;
    T->rules[NT_OTHER_STMTS][TK_ENDWHILE] = 38;
    T->rules[NT_OTHER_STMTS][TK_ENDIF] = 38;
    T->rules[NT_OTHER_STMTS][TK_ELSE] = 38;

    // Rule 39-43: <stmt> expansions
    T->rules[NT_STMT][TK_ID] = 39;        // <assignmentStmt>
    T->rules[NT_STMT][TK_WHILE] = 40;     // <iterativeStmt>
    T->rules[NT_STMT][TK_IF] = 41;        // <conditionalStmt>
    T->rules[NT_STMT][TK_READ] = 42;      // <ioStmt>
    T->rules[NT_STMT][TK_WRITE] = 42;     // <ioStmt>
    T->rules[NT_STMT][TK_SQL] = 43;       // <funCallStmt>
    T->rules[NT_STMT][TK_CALL] = 43;      // <funCallStmt>

    // Rule 44: <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
    T->rules[NT_ASSIGNMENT_STMT][TK_ID] = 44;

    // Rule 45: <singleOrRecId> ===> TK_ID <option_single_constructed>
    T->rules[NT_SINGLE_OR_REC_ID][TK_ID] = 45;

    // Rule 46: <option_single_constructed> ===> <oneExpansion> <moreExpansions>
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_DOT] = 46;

    // Rule 47: <option_single_constructed> ===> eps (FOLLOW = TK_ASSIGNOP, TK_CL, etc)
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_ASSIGNOP] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_CL] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_PLUS] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_MINUS] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_MUL] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_DIV] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_LT] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_LE] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_EQ] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_GT] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_GE] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_NE] = 47;
    T->rules[NT_OPTION_SINGLE_CONSTRUCTED][TK_SEM] = 47;

    // Rule 48: <oneExpansion> ===> TK_DOT TK_FIELDID
    T->rules[NT_ONE_EXPANSION][TK_DOT] = 48;

    // Rule 49: <moreExpansions> ===> <oneExpansion> <moreExpansions>
    T->rules[NT_MORE_EXPANSIONS][TK_DOT] = 49;

    // Rule 50: <moreExpansions> ===> eps (FOLLOW matches option_single_constructed)
    T->rules[NT_MORE_EXPANSIONS][TK_ASSIGNOP] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_CL] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_PLUS] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_MINUS] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_MUL] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_DIV] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_LT] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_LE] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_EQ] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_GT] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_GE] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_NE] = 50;
    T->rules[NT_MORE_EXPANSIONS][TK_SEM] = 50;

    // Rule 51: <funCallStmt> ===> <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
    T->rules[NT_FUN_CALL_STMT][TK_SQL] = 51;
    T->rules[NT_FUN_CALL_STMT][TK_CALL] = 51;

    // Rule 52: <outputParameters> ===> TK_SQL <idList> TK_SQR TK_ASSIGNOP
    T->rules[NT_OUTPUT_PARAMETERS][TK_SQL] = 52;

    // Rule 53: <outputParameters> ===> eps (FOLLOW = TK_CALL)
    T->rules[NT_OUTPUT_PARAMETERS][TK_CALL] = 53;

    // Rule 54: <inputParameters> ===> TK_SQL <idList> TK_SQR
    T->rules[NT_INPUT_PARAMETERS][TK_SQL] = 54;

    // Rule 55: <iterativeStmt> ===> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE
    T->rules[NT_ITERATIVE_STMT][TK_WHILE] = 55;

    // Rule 56: <conditionalStmt> ===> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart>
    T->rules[NT_CONDITIONAL_STMT][TK_IF] = 56;

    // Rule 57: <elsePart> ===> TK_ELSE <stmt> <otherStmts> TK_ENDIF
    T->rules[NT_ELSE_PART][TK_ELSE] = 57;

    // Rule 58: <elsePart> ===> TK_ENDIF
    T->rules[NT_ELSE_PART][TK_ENDIF] = 58;

    // Rule 59: <ioStmt> ===> TK_READ TK_OP <var> TK_CL TK_SEM
    T->rules[NT_IO_STMT][TK_READ] = 59;

    // Rule 60: <ioStmt> ===> TK_WRITE TK_OP <var> TK_CL TK_SEM
    T->rules[NT_IO_STMT][TK_WRITE] = 60;

    // Rule 61: <arithmeticExpression> ===> <term> <expPrime>
    T->rules[NT_ARITHMETIC_EXPRESSION][TK_ID] = 61;
    T->rules[NT_ARITHMETIC_EXPRESSION][TK_NUM] = 61;
    T->rules[NT_ARITHMETIC_EXPRESSION][TK_RNUM] = 61;
    T->rules[NT_ARITHMETIC_EXPRESSION][TK_OP] = 61;

    // Rule 62: <expPrime> ===> <lowPrecedenceOperators> <term> <expPrime>
    T->rules[NT_EXP_PRIME][TK_PLUS] = 62;
    T->rules[NT_EXP_PRIME][TK_MINUS] = 62;

    // Rule 63: <expPrime> ===> eps (FOLLOW = TK_SEM, TK_CL)
    T->rules[NT_EXP_PRIME][TK_SEM] = 63;
    T->rules[NT_EXP_PRIME][TK_CL] = 63;

    // Rule 64: <term> ===> <factor> <termPrime>
    T->rules[NT_TERM][TK_ID] = 64;
    T->rules[NT_TERM][TK_NUM] = 64;
    T->rules[NT_TERM][TK_RNUM] = 64;
    T->rules[NT_TERM][TK_OP] = 64;

    // Rule 65: <termPrime> ===> <highPrecedenceOperators> <factor> <termPrime>
    T->rules[NT_TERM_PRIME][TK_MUL] = 65;
    T->rules[NT_TERM_PRIME][TK_DIV] = 65;

    // Rule 66: <termPrime> ===> eps (FOLLOW matches expPrime's FOLLOW + TK_PLUS, TK_MINUS)
    T->rules[NT_TERM_PRIME][TK_PLUS] = 66;
    T->rules[NT_TERM_PRIME][TK_MINUS] = 66;
    T->rules[NT_TERM_PRIME][TK_SEM] = 66;
    T->rules[NT_TERM_PRIME][TK_CL] = 66;

    // Rule 67: <factor> ===> TK_OP <arithmeticExpression> TK_CL
    T->rules[NT_FACTOR][TK_OP] = 67;

    // Rule 68: <factor> ===> <var>
    T->rules[NT_FACTOR][TK_ID] = 68;
    T->rules[NT_FACTOR][TK_NUM] = 68;
    T->rules[NT_FACTOR][TK_RNUM] = 68;

    // Rule 69-72: Operators
    T->rules[NT_HIGH_PRECEDENCE_OPERATORS][TK_MUL] = 69;
    T->rules[NT_HIGH_PRECEDENCE_OPERATORS][TK_DIV] = 70;
    T->rules[NT_LOW_PRECEDENCE_OPERATORS][TK_PLUS] = 71;
    T->rules[NT_LOW_PRECEDENCE_OPERATORS][TK_MINUS] = 72;

    // Rule 73: <booleanExpression> ===> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
    T->rules[NT_BOOLEAN_EXPRESSION][TK_OP] = 73;

    // Rule 74: <booleanExpression> ===> <var> <relationalOp> <var>
    T->rules[NT_BOOLEAN_EXPRESSION][TK_ID] = 74;
    T->rules[NT_BOOLEAN_EXPRESSION][TK_NUM] = 74;
    T->rules[NT_BOOLEAN_EXPRESSION][TK_RNUM] = 74;

    // Rule 75: <booleanExpression> ===> TK_NOT TK_OP <booleanExpression> TK_CL
    T->rules[NT_BOOLEAN_EXPRESSION][TK_NOT] = 75;

    // Rule 76-78: <var> ===> <singleOrRecId> | TK_NUM | TK_RNUM
    T->rules[NT_VAR][TK_ID] = 76;
    T->rules[NT_VAR][TK_NUM] = 77;
    T->rules[NT_VAR][TK_RNUM] = 78;

    // Rule 79-80: Logical Operators
    T->rules[NT_LOGICAL_OP][TK_AND] = 79;
    T->rules[NT_LOGICAL_OP][TK_OR] = 80;

    // Rule 81-86: Relational Operators
    T->rules[NT_RELATIONAL_OP][TK_LT] = 81;
    T->rules[NT_RELATIONAL_OP][TK_LE] = 82;
    T->rules[NT_RELATIONAL_OP][TK_EQ] = 83;
    T->rules[NT_RELATIONAL_OP][TK_GT] = 84;
    T->rules[NT_RELATIONAL_OP][TK_GE] = 85;
    T->rules[NT_RELATIONAL_OP][TK_NE] = 86;

    // Rule 87: <returnStmt> ===> TK_RETURN <optionalReturn> TK_SEM
    T->rules[NT_RETURN_STMT][TK_RETURN] = 87;

    // Rule 88: <optionalReturn> ===> TK_SQL <idList> TK_SQR
    T->rules[NT_OPTIONAL_RETURN][TK_SQL] = 88;

    // Rule 89: <optionalReturn> ===> eps (FOLLOW = TK_SEM)
    T->rules[NT_OPTIONAL_RETURN][TK_SEM] = 89;

    // Rule 90: <idList> ===> TK_ID <more_ids>
    T->rules[NT_ID_LIST][TK_ID] = 90;

    // Rule 91: <more_ids> ===> TK_COMMA <idList>
    T->rules[NT_MORE_IDS][TK_COMMA] = 91;

    // Rule 92: <more_ids> ===> eps (FOLLOW = TK_SQR)
    T->rules[NT_MORE_IDS][TK_SQR] = 92;

    // Rule 93: <definetypestmt> ===> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
    T->rules[NT_DEFINETYPESTMT][TK_DEFINETYPE] = 93;

    // Rule 94-95: <A> ===> TK_RECORD | TK_UNION
    T->rules[NT_A][TK_RECORD] = 94;
    T->rules[NT_A][TK_UNION] = 95;
}

// ============================================================================
// PARSING ENGINE
// ============================================================================
// Helper function to build the tree and push to stack cleanly
void applyRule(Stack *stack, treeNode *parent, GrammarSymbol rhs[], int rhs_count) {
    if (rhs_count == 0) {
        // Epsilon Rule: We don't push anything to the stack.
        // Optional: You can create a dummy leaf node for "EPSILON" here 
        // if your professor wants it in the parse tree output.
        return;
    }

    treeNode *nodes[20]; // Array to hold the new child nodes

    // 1. Create all the tree nodes
    for (int i = 0; i < rhs_count; i++) {
        nodes[i] = createTreeNode(rhs[i], parent);
    }

    // 2. Link them together (Left to Right)
    parent->firstChild = nodes[0];
    for (int i = 0; i < rhs_count - 1; i++) {
        nodes[i]->nextSibling = nodes[i + 1];
    }

    // 3. Push them onto the stack (Reverse Order: Right to Left)
    for (int i = rhs_count - 1; i >= 0; i--) {
        push(stack, rhs[i], nodes[i]);
    }
}

// Helper function to filter out comments and lexical errors
tokenInfo getNextValidToken(twinBuffer B) {
    tokenInfo tk = getNextToken(B);
    
    // Keep fetching until we get a token that is NOT a comment and NOT an error
    while (tk->tokenType == TK_COMMENT || tk->tokenType == TK_ERROR) {
        tk = getNextToken(B);
    }
    
    return tk;
}

parseTree parseInputSourceCode(char *testcaseFile, table T) {
    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL) {
        printf("Error: Could not open source code file %s\n", testcaseFile);
        return NULL;
    }

    // Initialize lexer buffer
    extern twinBuffer initBuffer(FILE *fp); 
    twinBuffer B = initBuffer(fp);

    // Initialize stack
    Stack stack = createStack();

    // Push EOF
    GrammarSymbol eofSym = {1, {.term = TK_EOF}};
    push(&stack, eofSym, NULL);

    // Create Root Node and Push Start Symbol
    GrammarSymbol startSym = {0, {.nonTerm = NT_PROGRAM}};
    parseTree root = createTreeNode(startSym, NULL);
    push(&stack, startSym, root);

    // Get First Valid Token
    tokenInfo lookahead = getNextValidToken(B);
    int errorFlag = 0;
    
    // Textbook Panic Mode Recovery Flag
    int recovering = 0;

    // Main LL(1) Parsing Loop
    while (!isStackEmpty(stack)) {
        stackNode topNode = pop(&stack);
        GrammarSymbol X = topNode.symbol;
        treeNode *currentTreeNode = topNode.treeNodePtr;

        // Condition A: Top of stack is a Terminal
        if (X.isTerminal) {
            if (X.val.term == lookahead->tokenType) {
                // Match! Save the token directly into the tree node
                if (currentTreeNode != NULL) {
                    currentTreeNode->token = lookahead;
                }
                
                recovering = 0; // Successfully matched, we are out of panic mode!

                // Fetch the next valid token
                if (lookahead->tokenType != TK_EOF) {
                    lookahead = getNextValidToken(B);
                }
            } else {
                // Terminal Mismatch Error
                if (!recovering) {
                    printf("Line No %d Error: The token %s for lexeme %s does not match with the expected token %s\n", 
                           lookahead->lineNo, tokenStr[lookahead->tokenType], lookahead->lexeme, tokenStr[X.val.term]);
                    recovering = 1; // Enter panic mode to suppress cascade errors
                }
                errorFlag = 1;
                // Terminal mismatch panic recovery: We already popped the expected terminal. 
                // Do NOT advance lookahead. Let the next stack symbol try to process it.
            }
        } 
        // Condition B: Top of stack is a Non-Terminal
        else {
            int ruleNumber = T.rules[X.val.nonTerm][lookahead->tokenType];
            
            if (ruleNumber != -1) {
                // Helper macros to make the switch statement very readable
                #define T(x) (GrammarSymbol){1, {.term = x}}
                #define NT(x) (GrammarSymbol){0, {.nonTerm = x}}

                GrammarSymbol rhs[10]; 

                // ====================================================================
                // YOUR FULL 95-RULE SWITCH STATEMENT GOES HERE 
                // (I have shortened it here for readability, keep the one you pasted earlier!)
                // ====================================================================
                switch(ruleNumber) {
                    case 4:  case 8:  case 18: case 21: case 31: case 33: case 36: 
                    case 38: case 47: case 50: case 53: case 63: case 66: case 89: case 92:
                        applyRule(&stack, currentTreeNode, rhs, 0); break;
                    case 1: 
                        rhs[0] = NT(NT_OTHER_FUNCTIONS); rhs[1] = NT(NT_MAIN_FUNCTION);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 2: 
                        rhs[0] = T(TK_MAIN); rhs[1] = NT(NT_STMTS); rhs[2] = T(TK_END);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 3:
                        rhs[0] = NT(NT_FUNCTION); rhs[1] = NT(NT_OTHER_FUNCTIONS);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 5:
                        rhs[0] = T(TK_FUNID); rhs[1] = NT(NT_INPUT_PAR); rhs[2] = NT(NT_OUTPUT_PAR); 
                        rhs[3] = T(TK_SEM); rhs[4] = NT(NT_STMTS); rhs[5] = T(TK_END);
                        applyRule(&stack, currentTreeNode, rhs, 6); break;
                    case 6:
                        rhs[0] = T(TK_INPUT); rhs[1] = T(TK_PARAMETER); rhs[2] = T(TK_LIST); 
                        rhs[3] = T(TK_SQL); rhs[4] = NT(NT_PARAMETER_LIST); rhs[5] = T(TK_SQR);
                        applyRule(&stack, currentTreeNode, rhs, 6); break;
                    case 7:
                        rhs[0] = T(TK_OUTPUT); rhs[1] = T(TK_PARAMETER); rhs[2] = T(TK_LIST); 
                        rhs[3] = T(TK_SQL); rhs[4] = NT(NT_PARAMETER_LIST); rhs[5] = T(TK_SQR);
                        applyRule(&stack, currentTreeNode, rhs, 6); break;
                    case 9:
                        rhs[0] = NT(NT_DATA_TYPE); rhs[1] = T(TK_ID); rhs[2] = NT(NT_REMAINING_LIST);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 10:
                        rhs[0] = NT(NT_PRIMITIVE_DATATYPE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 11:
                        rhs[0] = NT(NT_CONSTRUCTED_DATATYPE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 12: rhs[0] = T(TK_INT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 13: rhs[0] = T(TK_REAL); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 14: rhs[0] = T(TK_RECORD); rhs[1] = T(TK_RUID); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 15: rhs[0] = T(TK_UNION); rhs[1] = T(TK_RUID); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 16: rhs[0] = T(TK_RUID); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 17:
                        rhs[0] = T(TK_COMMA); rhs[1] = NT(NT_PARAMETER_LIST);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 19:
                        rhs[0] = NT(NT_TYPE_DEFINITIONS); rhs[1] = NT(NT_DECLARATIONS); 
                        rhs[2] = NT(NT_OTHER_STMTS); rhs[3] = NT(NT_RETURN_STMT);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 20:
                        rhs[0] = NT(NT_ACTUAL_OR_REDEFINED); rhs[1] = NT(NT_TYPE_DEFINITIONS);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 22: rhs[0] = NT(NT_TYPE_DEFINITION); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 23: rhs[0] = NT(NT_DEFINETYPESTMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 24:
                        rhs[0] = T(TK_RECORD); rhs[1] = T(TK_RUID); rhs[2] = NT(NT_FIELD_DEFINITIONS); rhs[3] = T(TK_ENDRECORD);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 25:
                        rhs[0] = T(TK_UNION); rhs[1] = T(TK_RUID); rhs[2] = NT(NT_FIELD_DEFINITIONS); rhs[3] = T(TK_ENDUNION);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 26:
                        rhs[0] = NT(NT_FIELD_DEFINITION); rhs[1] = NT(NT_FIELD_DEFINITION); rhs[2] = NT(NT_MORE_FIELDS);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 27:
                        rhs[0] = T(TK_TYPE); rhs[1] = NT(NT_FIELD_TYPE); rhs[2] = T(TK_COLON); rhs[3] = T(TK_FIELDID); rhs[4] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 5); break;
                    case 28: rhs[0] = NT(NT_PRIMITIVE_DATATYPE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 29: rhs[0] = NT(NT_CONSTRUCTED_DATATYPE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 30:
                        rhs[0] = NT(NT_FIELD_DEFINITION); rhs[1] = NT(NT_MORE_FIELDS);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 32:
                        rhs[0] = NT(NT_DECLARATION); rhs[1] = NT(NT_DECLARATIONS);
                        applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 34:
                        rhs[0] = T(TK_TYPE); rhs[1] = NT(NT_DATA_TYPE); rhs[2] = T(TK_COLON); 
                        rhs[3] = T(TK_ID); rhs[4] = NT(NT_GLOBAL_OR_NOT); rhs[5] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 6); break;
                    case 35: rhs[0] = T(TK_COLON); rhs[1] = T(TK_GLOBAL); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 37: rhs[0] = NT(NT_STMT); rhs[1] = NT(NT_OTHER_STMTS); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 39: rhs[0] = NT(NT_ASSIGNMENT_STMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 40: rhs[0] = NT(NT_ITERATIVE_STMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 41: rhs[0] = NT(NT_CONDITIONAL_STMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 42: rhs[0] = NT(NT_IO_STMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 43: rhs[0] = NT(NT_FUN_CALL_STMT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 44:
                        rhs[0] = NT(NT_SINGLE_OR_REC_ID); rhs[1] = T(TK_ASSIGNOP); rhs[2] = NT(NT_ARITHMETIC_EXPRESSION); rhs[3] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 45: rhs[0] = T(TK_ID); rhs[1] = NT(NT_OPTION_SINGLE_CONSTRUCTED); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 46: rhs[0] = NT(NT_ONE_EXPANSION); rhs[1] = NT(NT_MORE_EXPANSIONS); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 48: rhs[0] = T(TK_DOT); rhs[1] = T(TK_FIELDID); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 49: rhs[0] = NT(NT_ONE_EXPANSION); rhs[1] = NT(NT_MORE_EXPANSIONS); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 51:
                        rhs[0] = NT(NT_OUTPUT_PARAMETERS); rhs[1] = T(TK_CALL); rhs[2] = T(TK_FUNID);
                        rhs[3] = T(TK_WITH); rhs[4] = T(TK_PARAMETERS); rhs[5] = NT(NT_INPUT_PARAMETERS); rhs[6] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 7); break;
                    case 52:
                        rhs[0] = T(TK_SQL); rhs[1] = NT(NT_ID_LIST); rhs[2] = T(TK_SQR); rhs[3] = T(TK_ASSIGNOP);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 54:
                        rhs[0] = T(TK_SQL); rhs[1] = NT(NT_ID_LIST); rhs[2] = T(TK_SQR);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 55:
                        rhs[0] = T(TK_WHILE); rhs[1] = T(TK_OP); rhs[2] = NT(NT_BOOLEAN_EXPRESSION);
                        rhs[3] = T(TK_CL); rhs[4] = NT(NT_STMT); rhs[5] = NT(NT_OTHER_STMTS); rhs[6] = T(TK_ENDWHILE);
                        applyRule(&stack, currentTreeNode, rhs, 7); break;
                    case 56:
                        rhs[0] = T(TK_IF); rhs[1] = T(TK_OP); rhs[2] = NT(NT_BOOLEAN_EXPRESSION); rhs[3] = T(TK_CL); 
                        rhs[4] = T(TK_THEN); rhs[5] = NT(NT_STMT); rhs[6] = NT(NT_OTHER_STMTS); rhs[7] = NT(NT_ELSE_PART);
                        applyRule(&stack, currentTreeNode, rhs, 8); break;
                    case 57:
                        rhs[0] = T(TK_ELSE); rhs[1] = NT(NT_STMT); rhs[2] = NT(NT_OTHER_STMTS); rhs[3] = T(TK_ENDIF);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 58: rhs[0] = T(TK_ENDIF); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 59:
                        rhs[0] = T(TK_READ); rhs[1] = T(TK_OP); rhs[2] = NT(NT_VAR); rhs[3] = T(TK_CL); rhs[4] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 5); break;
                    case 60:
                        rhs[0] = T(TK_WRITE); rhs[1] = T(TK_OP); rhs[2] = NT(NT_VAR); rhs[3] = T(TK_CL); rhs[4] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 5); break;
                    case 61: rhs[0] = NT(NT_TERM); rhs[1] = NT(NT_EXP_PRIME); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 62:
                        rhs[0] = NT(NT_LOW_PRECEDENCE_OPERATORS); rhs[1] = NT(NT_TERM); rhs[2] = NT(NT_EXP_PRIME);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 64: rhs[0] = NT(NT_FACTOR); rhs[1] = NT(NT_TERM_PRIME); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 65:
                        rhs[0] = NT(NT_HIGH_PRECEDENCE_OPERATORS); rhs[1] = NT(NT_FACTOR); rhs[2] = NT(NT_TERM_PRIME);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 67:
                        rhs[0] = T(TK_OP); rhs[1] = NT(NT_ARITHMETIC_EXPRESSION); rhs[2] = T(TK_CL);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 68: rhs[0] = NT(NT_VAR); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 69: rhs[0] = T(TK_MUL); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 70: rhs[0] = T(TK_DIV); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 71: rhs[0] = T(TK_PLUS); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 72: rhs[0] = T(TK_MINUS); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 73:
                        rhs[0] = T(TK_OP); rhs[1] = NT(NT_BOOLEAN_EXPRESSION); rhs[2] = T(TK_CL); 
                        rhs[3] = NT(NT_LOGICAL_OP); rhs[4] = T(TK_OP); rhs[5] = NT(NT_BOOLEAN_EXPRESSION); rhs[6] = T(TK_CL);
                        applyRule(&stack, currentTreeNode, rhs, 7); break;
                    case 74:
                        rhs[0] = NT(NT_VAR); rhs[1] = NT(NT_RELATIONAL_OP); rhs[2] = NT(NT_VAR);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 75:
                        rhs[0] = T(TK_NOT); rhs[1] = T(TK_OP); rhs[2] = NT(NT_BOOLEAN_EXPRESSION); rhs[3] = T(TK_CL);
                        applyRule(&stack, currentTreeNode, rhs, 4); break;
                    case 76: rhs[0] = NT(NT_SINGLE_OR_REC_ID); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 77: rhs[0] = T(TK_NUM); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 78: rhs[0] = T(TK_RNUM); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 79: rhs[0] = T(TK_AND); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 80: rhs[0] = T(TK_OR); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 81: rhs[0] = T(TK_LT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 82: rhs[0] = T(TK_LE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 83: rhs[0] = T(TK_EQ); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 84: rhs[0] = T(TK_GT); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 85: rhs[0] = T(TK_GE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 86: rhs[0] = T(TK_NE); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 87:
                        rhs[0] = T(TK_RETURN); rhs[1] = NT(NT_OPTIONAL_RETURN); rhs[2] = T(TK_SEM);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 88:
                        rhs[0] = T(TK_SQL); rhs[1] = NT(NT_ID_LIST); rhs[2] = T(TK_SQR);
                        applyRule(&stack, currentTreeNode, rhs, 3); break;
                    case 90: rhs[0] = T(TK_ID); rhs[1] = NT(NT_MORE_IDS); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 91: rhs[0] = T(TK_COMMA); rhs[1] = NT(NT_ID_LIST); applyRule(&stack, currentTreeNode, rhs, 2); break;
                    case 93:
                        rhs[0] = T(TK_DEFINETYPE); rhs[1] = NT(NT_A); rhs[2] = T(TK_RUID); rhs[3] = T(TK_AS); rhs[4] = T(TK_RUID);
                        applyRule(&stack, currentTreeNode, rhs, 5); break;
                    case 94: rhs[0] = T(TK_RECORD); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    case 95: rhs[0] = T(TK_UNION); applyRule(&stack, currentTreeNode, rhs, 1); break;
                    default: break;
                }
                #undef T
                #undef NT

            } else {
                // Non-Terminal Mismatch Error
                if (!recovering) {
                    printf("Line No %d Error: Invalid token %s encountered with value %s stack top %s\n", 
                           lookahead->lineNo, tokenStr[lookahead->tokenType], lookahead->lexeme, nonTermStr[X.val.nonTerm]);
                    recovering = 1;
                }
                errorFlag = 1;

                // Panic Mode Check: Is the lookahead in the synchronization set?
                TokenType t = lookahead->tokenType;
                if (t == TK_SEM || t == TK_ENDRECORD || t == TK_ENDUNION || 
                    t == TK_ENDIF || t == TK_ENDWHILE || t == TK_ELSE || 
                    t == TK_CL || t == TK_SQR || t == TK_END || t == TK_EOF) {
                    
                    // The token IS a sync token.
                    // We pop the non-terminal (already done at start of loop), 
                    // clear recovering flag, and do NOT consume the sync token.
                    // recovering = 0; 
                } else {
                    // The token is NOT a sync token.
                    // Push the non-terminal back onto the stack to try it again later,
                    // and consume (skip) this invalid token.
                    push(&stack, X, currentTreeNode);
                    lookahead = getNextValidToken(B);
                }
            }
        }
    }

    if (errorFlag == 0 && lookahead->tokenType == TK_EOF) {
        printf("\nInput source code is syntactically correct...........\n");
    }

    free(B);
    fclose(fp);
    return root;
}

// ============================================================================
// N-ARY INORDER TREE TRAVERSAL AND PRINTING
// ============================================================================
void printParseTree(parseTree PT, char *outfile) {
    if (PT == NULL) return;

    FILE *out = fopen(outfile, "a");
    if (!out) return;

    // 1. Visit Leftmost Child
    if (!PT->isLeafNode && PT->firstChild != NULL) {
        printParseTree(PT->firstChild, outfile);
    }

    // 2. Visit Parent Node (Print the 7 columns)
    
    // Column 1: Lexeme
    if (PT->isLeafNode && PT->token != NULL) {
        fprintf(out, "%-20s ", PT->token->lexeme);
    } else {
        fprintf(out, "%-20s ", "----");
    }

    // Column 2: Line Number
    if (PT->isLeafNode && PT->token != NULL) {
        fprintf(out, "%-5d ", PT->token->lineNo);
    } else {
        fprintf(out, "%-5s ", "----");
    }

    // Column 3: Token Name
    if (PT->isLeafNode && PT->token != NULL) {
        fprintf(out, "%-15s ", tokenStr[PT->token->tokenType]);
    } else {
        fprintf(out, "%-15s ", "----");
    }

    // Column 4: Value If Number
    if (PT->isLeafNode && PT->token != NULL && 
       (PT->token->tokenType == TK_NUM || PT->token->tokenType == TK_RNUM)) {
        double val = atof(PT->token->lexeme);
        fprintf(out, "%-15.4f ", val);
    } else {
        fprintf(out, "%-15s ", "----");
    }

    // Column 5: Parent Node Symbol
    if (PT->parent != NULL) {
        fprintf(out, "%-20s ", nonTermStr[PT->parent->symbol.val.nonTerm]);
    } else {
        fprintf(out, "%-20s ", "ROOT");
    }

    // Column 6: Is Leaf Node?
    if (PT->isLeafNode) {
        fprintf(out, "%-5s ", "yes");
    } else {
        fprintf(out, "%-5s ", "no");
    }

    // Column 7: Node Symbol (if not leaf)
    if (!PT->isLeafNode) {
        fprintf(out, "%-20s", nonTermStr[PT->symbol.val.nonTerm]);
    } else {
        fprintf(out, "%-20s", "----");
    }

    fprintf(out, "\n");
    fclose(out);

    // 3. Visit Remaining Siblings
    if (!PT->isLeafNode && PT->firstChild != NULL) {
        treeNode *sibling = PT->firstChild->nextSibling;
        while (sibling != NULL) {
            printParseTree(sibling, outfile);
            sibling = sibling->nextSibling;
        }
    }
}