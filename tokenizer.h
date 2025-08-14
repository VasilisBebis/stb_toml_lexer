#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <stdio.h>
#include <stdbool.h>

typedef enum {
  TOKEN_END = 0,
  TOKEN_INVALID,
  TOKEN_SYMBOL,
  TOKEN_EQUALS,
  TOKEN_DOT,
  TOKEN_COMMA,
  TOKEN_OPEN_BRACKET,
  TOKEN_CLOSE_BRACKET,
  TOKEN_OPEN_CURLY,
  TOKEN_CLOSE_CURLY,
  TOKEN_COMMENT,
  TOKEN_NEWLINE,
  TOKEN_BASIC_STRING,
  TOKEN_ML_BASIC_STRING, // multi-line basic string
  TOKEN_LITERAL_STRING,
  TOKEN_ML_LITERAL_STRING, // multi-line literal string
} TokenKind;

typedef struct {
  int x;
  int y;
} Vec2;

typedef struct {
  TokenKind kind;
  const char *text;
  size_t text_length;
  Vec2 position;
} Token;

typedef struct {
  const char *content;
  size_t content_length;
  size_t cursor;
  size_t line;
  size_t beginning_of_line;
} Lexer;

typedef union {
  bool  eof;
  char  ch;
} PeekOption;

const char *token_kind_name(TokenKind kind);
Lexer lexer_new(const char *content, size_t content_length);
Token lexer_next(Lexer *lexer);
void  lexer_trim_space(Lexer *lexer);
char lexer_chop_char(Lexer *lexer);
PeekOption lexer_peek_char(Lexer *lexer);

#endif
