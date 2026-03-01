#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>
#include <stdbool.h>

/* buffer size as suggested in announcements (50 bytes each half) */
#define BUFFER_SIZE 50
#define MAX_LEXEME_SIZE 128

/* ================= TOKEN TYPES ================= */

typedef enum
{
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    TK_ERROR,
    TK_EOF
} TokenType;

/* ================= TOKEN INFO STRUCT ================= */

typedef struct token_info
{
    TokenType tokenType;
    char lexeme[MAX_LEXEME_SIZE];
    int lineNo;
} token_info;

typedef token_info *tokenInfo;

/* ================= TWIN BUFFER ================= */

typedef struct TwinBufferStruct
{
    FILE *fp;
    char buf1[BUFFER_SIZE];
    char buf2[BUFFER_SIZE];
    int currentBuff; /* 1 or 2 */
    int forward;
    int bytesRead1;
    int bytesRead2;
    bool eof_reached;
} TwinBufferStruct;

typedef TwinBufferStruct *twinBuffer;

#endif