#include "tokenizer.h"
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

const char *token_kind_name(TokenKind type)
{
  switch (type) {
    case TOKEN_END:
      return "end of content";
    case TOKEN_INVALID:
      return "invalid token";
    case TOKEN_EQUALS:
      return "equals";
    case TOKEN_SYMBOL:
      return "symbol";
    case TOKEN_COMMENT:
      return "comment";
    case TOKEN_OPEN_BRACKET:
      return "open bracket";
    case TOKEN_CLOSE_BRACKET:
      return "close bracket";
    case TOKEN_OPEN_CURLY:
      return "open curly";
    case TOKEN_CLOSE_CURLY:
      return "close curly";
    case TOKEN_COMMA:
      return "comma";
    case TOKEN_DOT:
      return "dot";
    case TOKEN_NEWLINE:
      return "\\n";
    default:
      return "not implemented";
  }
}

Lexer lexer_new(const char *content, size_t content_length)
{
  Lexer lexer = {0};
  lexer.content = content;
  lexer.content_length = content_length;
  return lexer;
}

char lexer_chop_char(Lexer *lexer)
{
  assert(lexer->cursor < lexer->content_length);
  char chopped = lexer->content[lexer->cursor];
  lexer->cursor += 1;
  if (chopped == '\n') {
    lexer->line += 1;
    lexer->beginning_of_line = lexer->cursor;
  }
  return chopped;
}

void lexer_trim_space(Lexer *lexer)
{
  while (lexer->cursor < lexer->content_length && 
        (lexer->content[lexer->cursor] == ' ' || lexer->content[lexer->cursor] == '\t')) {
    lexer_chop_char(lexer);
  }
}

bool is_symbol(char x)
{
  return isalnum(x) || x == '_';
}

Token lexer_next(Lexer *lexer)
{
  lexer_trim_space(lexer);
  Token token = {
    .text = &lexer->content[lexer->cursor],
  };

  if (lexer->cursor >= lexer->content_length) return token;

  if (lexer->content[lexer->cursor] == '\n') {
    lexer->cursor += 1;
    lexer->line += 1;
    lexer->beginning_of_line = lexer->cursor;
    token.kind = TOKEN_NEWLINE;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '[') {
    lexer->cursor += 1;
    token.kind = TOKEN_OPEN_BRACKET;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == ']') {
    lexer->cursor += 1;
    token.kind = TOKEN_CLOSE_BRACKET;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '{') {
    lexer->cursor += 1;
    token.kind = TOKEN_OPEN_CURLY;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '}') {
    lexer->cursor += 1;
    token.kind = TOKEN_CLOSE_CURLY;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '=') {
    lexer->cursor += 1;
    token.kind = TOKEN_EQUALS;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == ',') {
    lexer->cursor += 1;
    token.kind = TOKEN_COMMA;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '.') {
    lexer->cursor += 1;
    token.kind = TOKEN_DOT;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '#') {
    token.kind = TOKEN_COMMENT;
    while (lexer->cursor < lexer->content_length && lexer->content[lexer->cursor] != '\n') {
      token.text_length += 1;
      lexer_chop_char(lexer);
    }
    if (lexer->cursor < lexer->content_length) {
      lexer_chop_char(lexer);
    }
    return token;
  }

  if (is_symbol(lexer->content[lexer->cursor])) {
    token.kind = TOKEN_SYMBOL;
    while (lexer->cursor < lexer->content_length && is_symbol(lexer->content[lexer->cursor])) {
      lexer->cursor += 1;
      token.text_length += 1;
    }
    return token;
  }

  lexer->cursor += 1;
  token.kind = TOKEN_INVALID;
  token.text_length = 1;

  return token;
}
