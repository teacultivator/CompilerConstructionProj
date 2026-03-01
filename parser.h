#ifndef PARSER_H
#define PARSER_H

#include "lexerDef.h"
#include "parserDef.h"

/* ============ FUNCTION PROTOTYPES ============ */

void initGrammar(Grammar *G);
FirstAndFollow computeFirstAndFollowSets(Grammar *G);
void createParseTable(FirstAndFollow *F, Grammar *G, ParseTable T);
ParseTree parseInputSourceCode(char *testcaseFile, ParseTable T, Grammar *G, FirstAndFollow *F);
void printParseTree(ParseTree PT, char *outfile);
void freeTree(TreeNode *node);

extern const char *nonTerminalStrings[];

#endif /* PARSER_H */
