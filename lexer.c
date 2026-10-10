/* ID:  2022B3A70404P; Name: Vivek Raj Barnwal */
/* ID:  2022B3A70607P; Name: Abhinav Madaan */
/* ID:  2022B3A70413P; Name: Jayant Grover */
/* ID:  2022B3A70648P; Name: Gathvik Narayan Kolla */
/* ID:  2022B3A70474P; Name: Raghav Mundra */
/* ID: 2023A7PS0653P; Name: Vulli Abhinav */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexer.h"

// token name lookup - using same order as the enum
const char *tokenStrings[] = {
    "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM",
    "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE",
    "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN",
    "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT",
    "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT",
    "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ",
    "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL",
    "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT",
    "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "TK_ERROR", "TK_EOF"
};

int lineNumber = 1;



 // TWIN BUFFER IMPLEMENTATION

FILE *getStream(FILE *fp, twinBuffer B)
{
    if (B->currentBuff == 1) {
        B->bytesRead1 = fread(B->buf1, 1, BUFFER_SIZE, fp);
        B->eof_reached = (B->bytesRead1 < BUFFER_SIZE);
    } else {
        B->bytesRead2 = fread(B->buf2, 1, BUFFER_SIZE, fp);
        B->eof_reached = (B->bytesRead2 < BUFFER_SIZE);
    }
    return fp;
}

twinBuffer initBuffer(FILE *fp)
{
    twinBuffer B = (twinBuffer)malloc(sizeof(TwinBufferStruct));
    if (!B) {
        fprintf(stderr, "malloc failed for twin buffer\n");
        exit(1);
    }
    B->fp = fp;
    B->currentBuff = 1;
    B->forward = 0;
    B->bytesRead1 = 0;
    B->bytesRead2 = 0;
    B->eof_reached = false;

    //loading first buffer half using getStream 
    getStream(fp, B);

    return B;
}


char getNextChar(twinBuffer B)
{
    if (B->currentBuff == 1) {
        if (B->forward < B->bytesRead1) {
            return B->buf1[B->forward++];
        }
        // If buffer 1 is exhausted, switching to buffer2
        B->currentBuff = 2;
        B->forward = 0;
        getStream(B->fp, B);

        if (B->bytesRead2 == 0)
            return EOF;

        return B->buf2[B->forward++];
    }
    else {
        if (B->forward < B->bytesRead2) {
            return B->buf2[B->forward++];
        }
        // If buffer 2 is exhausted, switching to buffer1 
        B->currentBuff = 1;
        B->forward = 0;
        getStream(B->fp, B);

        if (B->bytesRead1 == 0)
            return EOF;

        return B->buf1[B->forward++];
    }
}


// moves forward pointer back by one character
void retractChar(twinBuffer B)
{
    if (B->forward > 0) {
        B->forward--;
    }
    else {
        if (B->currentBuff == 1) {
            B->currentBuff = 2;
            B->forward = B->bytesRead2 - 1;
        } else {
            B->currentBuff = 1;
            B->forward = B->bytesRead1 - 1;
        }
    }
}


// KEYWORD LOOKUP                          

//checks if string matches any language keyword
static bool checkKeyword(const char *str, TokenType *out)
{
    // I tried using a hash table but this was simpler and works fine 
    if (strcmp(str, "with") == 0)        { *out = TK_WITH;       return true; }
    if (strcmp(str, "parameters") == 0)  { *out = TK_PARAMETERS; return true; }
    if (strcmp(str, "end") == 0)         { *out = TK_END;        return true; }
    if (strcmp(str, "while") == 0)       { *out = TK_WHILE;      return true; }
    if (strcmp(str, "union") == 0)       { *out = TK_UNION;      return true; }
    if (strcmp(str, "endunion") == 0)    { *out = TK_ENDUNION;   return true; }
    if (strcmp(str, "definetype") == 0)  { *out = TK_DEFINETYPE; return true; }
    if (strcmp(str, "as") == 0)          { *out = TK_AS;         return true; }
    if (strcmp(str, "type") == 0)        { *out = TK_TYPE;       return true; }
    if (strcmp(str, "global") == 0)      { *out = TK_GLOBAL;     return true; }
    if (strcmp(str, "parameter") == 0)   { *out = TK_PARAMETER;  return true; }
    if (strcmp(str, "list") == 0)        { *out = TK_LIST;       return true; }
    if (strcmp(str, "input") == 0)       { *out = TK_INPUT;      return true; }
    if (strcmp(str, "output") == 0)      { *out = TK_OUTPUT;     return true; }
    if (strcmp(str, "int") == 0)         { *out = TK_INT;        return true; }
    if (strcmp(str, "real") == 0)        { *out = TK_REAL;       return true; }
    if (strcmp(str, "endwhile") == 0)    { *out = TK_ENDWHILE;   return true; }
    if (strcmp(str, "if") == 0)          { *out = TK_IF;         return true; }
    if (strcmp(str, "then") == 0)        { *out = TK_THEN;       return true; }
    if (strcmp(str, "endif") == 0)       { *out = TK_ENDIF;      return true; }
    if (strcmp(str, "read") == 0)        { *out = TK_READ;       return true; }
    if (strcmp(str, "write") == 0)       { *out = TK_WRITE;      return true; }
    if (strcmp(str, "return") == 0)      { *out = TK_RETURN;     return true; }
    if (strcmp(str, "call") == 0)        { *out = TK_CALL;       return true; }
    if (strcmp(str, "record") == 0)      { *out = TK_RECORD;     return true; }
    if (strcmp(str, "endrecord") == 0)   { *out = TK_ENDRECORD;  return true; }
    if (strcmp(str, "else") == 0)        { *out = TK_ELSE;       return true; }

    return false;
}


// DFA-BASED getNextToken

typedef enum
{
    ST_START,

    // identifiers and keywords
    ST_FIELD,          // [a-z]+ (keyword or field)
    ST_BD,             // [b-d] prefix
    ST_ID_AFTER_DIGIT, // committed to ID
    ST_ID_BD,          // middle [b-d]*
    ST_ID_TRAIL,       // trailing [2-7]*

    // Function IDs: _[a-zA-Z]*[0-9]*
    ST_FUNID_START, 
    ST_FUNID_ALPHA, 
    ST_FUNID_DIGIT, 

    // Record/Union IDs: #[a-z]*
    ST_RUID_START, // '#'
    ST_RUID,       // lowercase letters

    // Numbers & Exponents
    ST_NUM,      // [0-9]+
    ST_DOT,      // saw '.'
    ST_RNUM_D1,  // first decimal digit
    ST_RNUM_D2,  // second decimal digit
    ST_EXP,      // E
    ST_EXP_SIGN, // E followed by +/-
    ST_EXP_D1,   // first exp digit
    ST_EXP_D2,   // second exp digit

    // Operators & Assignment
    ST_LT,      // '<'
    ST_ASSIGN1, // '<-'
    ST_ASSIGN2, // '<--'
    ST_GT,      // '>'

    ST_COMMENT, // inside '%' comment

    ST_DONE
} DFAState;


tokenInfo getNextToken(twinBuffer B)
{
    tokenInfo tk = (tokenInfo)malloc(sizeof(token_info));
    tk->lexeme[0] = '\0';
    tk->lineNo = lineNumber;

    int len = 0;        
    char c;
    DFAState state = ST_START;

    while (state != ST_DONE) {
        c = getNextChar(B);

        switch (state) {

        // START STATE 
        case ST_START:
            if (c == ' ' || c == '\t' || c == '\r') {
                break;  
            }
            if (c == '\n') {
                lineNumber++;
                break;
            }
            if (c == EOF) {
                tk->tokenType = TK_EOF;
                strcpy(tk->lexeme, "$");
                state = ST_DONE;
                break;
            }

            // recording the line where the token starts
            tk->lineNo = lineNumber;

            // SINGLE CHARACTER TOKENS
            switch (c) {
                case '[':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_SQL; return tk;
                case ']':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_SQR; return tk;
                case '(':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_OP; return tk;
                case ')':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_CL; return tk;
                case ';':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_SEM; return tk;
                case ',':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_COMMA; return tk;
                case '+':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_PLUS; return tk;
                case '-':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_MINUS; return tk;
                case '*':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_MUL; return tk;
                case '/':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_DIV; return tk;
                case ':':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_COLON; return tk;
                case '.':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_DOT; return tk;
                case '~':
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    tk->tokenType = TK_NOT; return tk;
            }

            // MULTI CHARACTER OPERARTORS HANDLED INLINE

            // < : could be TK_LT, TK_LE, or TK_ASSIGNOP 
            if (c == '<') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_LT;
                break;
            }

            // > : could be TK_GT or TK_GE
            if (c == '>') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_GT;
                break;
            }

            // == 
            if (c == '=') {
                char n = getNextChar(B);
                if (n == '=') {
                    strcpy(tk->lexeme, "==");
                    tk->tokenType = TK_EQ;
                    return tk;
                } else {
                    if (n != EOF) retractChar(B);
                    printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, c);
                    tk->tokenType = TK_ERROR;
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    state = ST_DONE;
                    break;
                }
            }

            // != 
            if (c == '!') {
                char n = getNextChar(B);
                if (n == '=') {
                    strcpy(tk->lexeme, "!=");
                    tk->tokenType = TK_NE;
                    return tk;
                } else {
                    if (n != EOF) retractChar(B);
                    printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, c);
                    tk->tokenType = TK_ERROR;
                    tk->lexeme[0] = c; tk->lexeme[1] = '\0';
                    state = ST_DONE;
                    break;
                }
            }

            // &&& 
            if (c == '&') {
                char n1 = getNextChar(B);
                char n2 = getNextChar(B);
                if (n1 == '&' && n2 == '&') {
                    strcpy(tk->lexeme, "&&&");
                    tk->tokenType = TK_AND;
                    return tk;
                }
                if (n1 == '&') {
                    if (n2 != EOF) retractChar(B);
                    printf("Line %d Error: Unknown pattern <&&>\n", lineNumber);
                } else {
                    if (n2 != EOF) retractChar(B);
                    if (n1 != EOF) retractChar(B);
                    printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, c);
                }
                tk->tokenType = TK_ERROR;
                tk->lexeme[0] = '&'; tk->lexeme[1] = '\0';
                state = ST_DONE;
                break;
            }

            // @@@ 
            if (c == '@') {
                char n1 = getNextChar(B);
                char n2 = getNextChar(B);
                if (n1 == '@' && n2 == '@') {
                    strcpy(tk->lexeme, "@@@");
                    tk->tokenType = TK_OR;
                    return tk;
                }
                if (n1 == '@') {
                    if (n2 != EOF) retractChar(B);
                    printf("Line %d Error: Unknown pattern <@@>\n", lineNumber);
                } else {
                    if (n2 != EOF) retractChar(B);
                    if (n1 != EOF) retractChar(B);
                    printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, c);
                }
                tk->tokenType = TK_ERROR;
                tk->lexeme[0] = '@'; tk->lexeme[1] = '\0';
                state = ST_DONE;
                break;
            }

            // % comment 
            if (c == '%') {
                while (c != '\n' && c != EOF) {
                    c = getNextChar(B);
                }
                if (c == '\n') lineNumber++;
                strcpy(tk->lexeme, "%");
                tk->tokenType = TK_COMMENT;
                return tk;
            }


            // DFA transitions for identifiers and numbers

            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_NUM;
                break;
            }

            if (c >= 'b' && c <= 'd') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_BD;
                break;
            }

            if (c >= 'a' && c <= 'z') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_FIELD;
                break;
            }

            if (c == '_') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_FUNID_START;
                break;
            }

            if (c == '#') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_RUID_START;
                break;
            }

            // anything else is an unknown symbol 
            printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, c);
            tk->tokenType = TK_ERROR;
            tk->lexeme[0] = c; tk->lexeme[1] = '\0';
            state = ST_DONE;
            break;


        // LESS THAN / ASSIGN 
        case ST_LT:
            if (c == '=') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                tk->tokenType = TK_LE;
                return tk;
            }
            if (c == '-') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ASSIGN1;
                break;
            }
            retractChar(B);
            tk->tokenType = TK_LT;
            state = ST_DONE;
            break;

        case ST_ASSIGN1:
            // we have "<-" 
            if (c == '-') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ASSIGN2;
                break;
            }
            // double retraction and getting less than token
            retractChar(B);    
            retractChar(B);      
            len = 1;
            tk->lexeme[1] = '\0';
            tk->tokenType = TK_LT;
            state = ST_DONE;
            break;

        case ST_ASSIGN2:
            // we have "<--" 
            if (c == '-') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                tk->tokenType = TK_ASSIGNOP;
                return tk;
            }
            // <-- without third '-' is an error 
            retractChar(B);
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;


        // GREATER THAN 
        case ST_GT:
            if (c == '=') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                tk->tokenType = TK_GE;
                return tk;
            }
            retractChar(B);
            tk->tokenType = TK_GT;
            state = ST_DONE;
            break;


        // COMMENT
        case ST_COMMENT:
            state = ST_DONE;
            break;


        //FIELD ID / KEYWORD (lowercase letters) 
        case ST_FIELD:
            if (c >= 'a' && c <= 'z') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break; 
            }
            retractChar(B);
            {
                TokenType kw;
                if (checkKeyword(tk->lexeme, &kw))
                    tk->tokenType = kw;
                else
                    tk->tokenType = TK_FIELDID;
            }
            state = ST_DONE;
            break;


        //[b-d] START - could be ID or FIELDID 
        case ST_BD:
            if (c >= '2' && c <= '7') {
                // took TK_ID path: [b-d][2-7] 
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ID_AFTER_DIGIT;
                break;
            }
            if (c >= 'a' && c <= 'z') {
                // went to fieldid/keyword path 
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_FIELD;
                break;
            }
            retractChar(B);
            {
                TokenType kw;
                if (checkKeyword(tk->lexeme, &kw))
                    tk->tokenType = kw;
                else
                    tk->tokenType = TK_FIELDID;
            }
            state = ST_DONE;
            break;

        case ST_ID_AFTER_DIGIT:
            if (c >= 'b' && c <= 'd') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ID_BD;
                break;
            }
            if (c >= '2' && c <= '7') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ID_TRAIL;
                break;
            }
            retractChar(B);
            tk->tokenType = TK_ID;
            state = ST_DONE;
            break;

        case ST_ID_BD:
            if (c >= 'b' && c <= 'd') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break; 
            }
            if (c >= '2' && c <= '7') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_ID_TRAIL;
                break;
            }
            retractChar(B);
            tk->tokenType = TK_ID;
            state = ST_DONE;
            break;

        case ST_ID_TRAIL:
            if (c >= '2' && c <= '7') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break;
            }
            retractChar(B);
            tk->tokenType = TK_ID;
            state = ST_DONE;
            break;


        //FUNCTION ID: _[a-zA-Z][a-zA-Z]*[0-9]* 
        case ST_FUNID_START:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_FUNID_ALPHA;
                break;
            }
            retractChar(B);
            printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, '_');
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_FUNID_ALPHA:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break; 
            }
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_FUNID_DIGIT;
                break;
            }
            retractChar(B);
            if (strcmp(tk->lexeme, "_main") == 0)
                tk->tokenType = TK_MAIN;
            else
                tk->tokenType = TK_FUNID;
            state = ST_DONE;
            break;

        case ST_FUNID_DIGIT:
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break; 
            }
            retractChar(B);
            if (strcmp(tk->lexeme, "_main") == 0)
                tk->tokenType = TK_MAIN;
            else
                tk->tokenType = TK_FUNID;
            state = ST_DONE;
            break;


        // RECORD/UNION ID: #[a-z][a-z]*
        case ST_RUID_START:
            if (c >= 'a' && c <= 'z') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_RUID;
                break;
            }
            retractChar(B);
            printf("Line %d Error: Unknown Symbol <%c>\n", lineNumber, '#');
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_RUID:
            if (c >= 'a' && c <= 'z') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break;
            }
            retractChar(B);
            tk->tokenType = TK_RUID;
            state = ST_DONE;
            break;


        // NUMBERS 
        case ST_NUM:
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                break;
            }
            if (c == '.') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_DOT;
                break;
            }
            retractChar(B);
            tk->tokenType = TK_NUM;
            state = ST_DONE;
            break;

        case ST_DOT:
            // decimal after integer digits
            if (c >= '0' && c <= '9') {
                // first digit after decimal accept
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_RNUM_D1;
                break;
            }
            
             // Not a digit after '.'. As corrected in announcements for 23.abc -> "23." is TK_ERROR
            retractChar(B);     // retract current non-digit
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_RNUM_D1:
            //one digit after decimal
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_RNUM_D2;
                break;
            }
            // Only one digit after decimal - error
            retractChar(B);
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_RNUM_D2:
            if (c == 'E') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_EXP;
                break;
            }
            retractChar(B);
            tk->tokenType = TK_RNUM;
            state = ST_DONE;
            break;

        case ST_EXP:
            if (c == '+' || c == '-') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_EXP_SIGN;
                break;
            }
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_EXP_D1;
                break;
            }
            retractChar(B);
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_EXP_SIGN:
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_EXP_D1;
                break;
            }
            retractChar(B);
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_EXP_D1:
            if (c >= '0' && c <= '9') {
                tk->lexeme[len++] = c; tk->lexeme[len] = '\0';
                state = ST_EXP_D2;
                break;
            }
            retractChar(B);
            printf("Line %d Error: Unknown pattern <%s>\n", lineNumber, tk->lexeme);
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;

        case ST_EXP_D2:
            retractChar(B);
            tk->tokenType = TK_RNUM;
            state = ST_DONE;
            break;


        default:
            tk->tokenType = TK_ERROR;
            state = ST_DONE;
            break;
        }
    }

    // handling identifier length limits 
    if (tk->tokenType == TK_ID && (int)strlen(tk->lexeme) > 20) {
        printf("Line %d Error :Variable Identifier is longer than the prescribed length of 20 characters.\n", tk->lineNo);
        tk->tokenType = TK_ERROR;
    }
    if (tk->tokenType == TK_FUNID && (int)strlen(tk->lexeme) > 30) {
        printf("Line %d Error :Function Identifier is longer than the prescribed length of 30 characters.\n", tk->lineNo);
        tk->tokenType = TK_ERROR;
    }

    tk->lineNo = lineNumber;
    return tk;
}


// REMOVE COMMENTS

void removeComments(char *testcaseFile, char *cleanFile)
{
    FILE *src = fopen(testcaseFile, "r");
    FILE *dst = fopen(cleanFile, "w");
    if (!src || !dst) {
        fprintf(stderr, "Error opening files for comment removal\n");
        if (src) fclose(src);
        if (dst) fclose(dst);
        return;
    }

    int ch = fgetc(src);
    while (ch != EOF) {
        if (ch == '%') {
            while (ch != '\n' && ch != EOF)
                ch = fgetc(src);
            if (ch == '\n') {
                fputc('\n', dst);
                ch = fgetc(src);
            }
        } else {
            fputc(ch, dst);
            ch = fgetc(src);
        }
    }

    fclose(src);
    fclose(dst);
}
