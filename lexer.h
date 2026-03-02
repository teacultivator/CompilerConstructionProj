/* ID:  2022B3A70404P; Name: Vivek Raj Barnwal */
/* ID:  2022B3A70607P; Name: Abhinav Madaan */
/* ID:  2022B3A70413P; Name: Jayant Grover */
/* ID:  2022B3A70648P; Name: Gathvik Narayan Kolla */
/* ID:  2022B3A70474P; Name: Raghav Mundra */
/* ID: 2023A7PS0653P; Name: Vulli Abhinav */

#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

// FUNCTION PROTOTYPES

FILE *getStream(FILE *fp, twinBuffer B);
twinBuffer initBuffer(FILE *fp);
char getNextChar(twinBuffer B);
void retractChar(twinBuffer B);
tokenInfo getNextToken(twinBuffer B);
void removeComments(char *testcaseFile, char *cleanFile);

extern const char *tokenStrings[];
extern int lineNumber;

#endif
