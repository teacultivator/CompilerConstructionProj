#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

// Function prototypes as per the project specification
void ComputeFirstAndFollowSets(FirstAndFollow *F);
void createParseTable(FirstAndFollow F, table *T);
parseTree parseInputSourceCode(char *testcaseFile, table T);
void printParseTree(parseTree PT, char *outfile);

#endif