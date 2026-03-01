#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

/* ================= FUNCTION PROTOTYPES ================= */

FILE *getStream(FILE *fp, twinBuffer B);
twinBuffer initBuffer(FILE *fp);
char getNextChar(twinBuffer B);
void retractChar(twinBuffer B);
tokenInfo getNextToken(twinBuffer B);
void removeComments(char *testcaseFile, char *cleanFile);

extern const char *tokenStrings[];
extern int lineNumber;

#endif