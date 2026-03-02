/* ID:  2022B3A70404P; Name: Vivek Raj Barnwal */
/* ID:  2022B3A70607P; Name: Abhinav Madaan */
/* ID:  2022B3A70413P; Name: Jayant Grover */
/* ID:  2022B3A70648P; Name: Gathvik Narayan Kolla */
/* ID:  2022B3A70474P; Name: Raghav Mundra */
/* ID: 2023A7PS0653P; Name: Vulli Abhinav */

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

// Function prototypes as per the project specification
void ComputeFirstAndFollowSets(FirstAndFollow *F);
void createParseTable(FirstAndFollow F, table *T);
parseTree parseInputSourceCode(char *testcaseFile, table T);
void printParseTree(parseTree PT, char *outfile);

#endif
