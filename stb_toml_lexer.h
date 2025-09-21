// A tiny lexer for TOML
//
// USAGE
// This file provides both the interface and the implementation.
// To instantiate the implementation,
//      #define STB_TOML_LEXER_IMPLEMENTATION
// in *ONE* source file, before #including this file.
//
// TODO
//  - Check for invalid tokens within strings
//
// LICENSE
// See end of file

#ifndef STB_TOML_LEXER_H
#define STB_TOML_LEXER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#ifndef STBTL_DEF
// Can be used to `#define STBTL_DEF static inline`
#define STBTL_DEF
#endif // STBTL_DEF

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
  TOKEN_END = 0,
  TOKEN_INVALID,
  TOKEN_SYMBOL,
  TOKEN_EQUALS,
  TOKEN_DOT,
  TOKEN_COMMA,
  TOKEN_PLUS,
  TOKEN_COLON,
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
} stbtl__token_kind;

typedef struct {
  stbtl__token_kind kind;
  const char *text;
  size_t text_length;
} stbtl__token;

typedef struct {
  const char *content;
  size_t content_length;
  size_t cursor;
  size_t line;
  size_t beginning_of_line;
} stbtl__lexer;

typedef struct {
  bool found;
  stbtl__token token;
} stbtl__expected_token;

STBTL_DEF const char         *stbtl__token_kind_name(stbtl__token_kind kind);
STBTL_DEF stbtl__lexer       stbtl__lexer_new(const char *content, size_t content_length);
STBTL_DEF stbtl__token       stbtl__lexer_next(stbtl__lexer *lexer);

STBTL_DEF stbtl__expected_token stbtl__lexer_expect_and_consume_token(stbtl__lexer *lexer, ...);
STBTL_DEF stbtl__token_kind     stbtl__lexer_peek_token(stbtl__lexer *lexer);

STBTL_DEF void stbtl__lexer_trim_space(stbtl__lexer *lexer);
STBTL_DEF char stbtl__lexer_chop_char(stbtl__lexer *lexer);
STBTL_DEF char stbtl__lexer_peek_char(stbtl__lexer *lexer);

#ifdef __cplusplus
}
#endif
#endif // STB_TOML_LEXER_H

#ifdef STB_TOML_LEXER_IMPLEMENTATION

STBTL_DEF const char *stbtl__token_kind_name(stbtl__token_kind type)
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
    case TOKEN_PLUS:
      return "plus";
    case TOKEN_COLON:
      return "colon";
    case TOKEN_NEWLINE:
#ifdef _WIN32
      return "\\r\\n";
#else 
      return "\\n";
#endif
    case TOKEN_BASIC_STRING:
      return "basic string";
    case TOKEN_ML_BASIC_STRING:
      return "multiline basic string";
    case TOKEN_LITERAL_STRING:
      return "literal string";
    case TOKEN_ML_LITERAL_STRING:
      return "multiline literal string";
    default:
      return "not implemented";
  }
}

STBTL_DEF stbtl__lexer stbtl__lexer_new(const char *content, size_t content_length)
{
  stbtl__lexer lexer = {0};
  lexer.content = content;
  lexer.content_length = content_length;
  return lexer;
}

STBTL_DEF char stbtl__lexer_chop_char(stbtl__lexer *lexer)
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

STBTL_DEF char stbtl__lexer_peek_char(stbtl__lexer *lexer)
{
  if (lexer->cursor == lexer->content_length) return 0;
  char peek = lexer->content[lexer->cursor + 1];
  if (peek == '\n') {
    lexer->line += 1;
    lexer->beginning_of_line = lexer->cursor;
  }
  return peek;
}

STBTL_DEF void stbtl__lexer_trim_space(stbtl__lexer *lexer)
{
  while (lexer->cursor < lexer->content_length && 
        (lexer->content[lexer->cursor] == ' ' || lexer->content[lexer->cursor] == '\t')) {
    stbtl__lexer_chop_char(lexer);
  }
}

STBTL_DEF bool is_symbol(char x)
{
  return isalnum(x) || x == '_' || x == '-';
}

STBTL_DEF stbtl__token stbtl__lexer_next(stbtl__lexer *lexer)
{
  stbtl__lexer_trim_space(lexer);
  stbtl__token token = {
    .text = &lexer->content[lexer->cursor],
  };

  if (lexer->cursor >= lexer->content_length) return token;

  if (lexer->content[lexer->cursor] == '\r') {
    if (stbtl__lexer_peek_char(lexer) == '\n') {
      stbtl__lexer_chop_char(lexer);
      stbtl__lexer_chop_char(lexer);
      token.kind = TOKEN_NEWLINE;
      token.text_length = 2;
    } else {
      token.kind = TOKEN_INVALID;
      token.text_length = 1;
    }
    return token;
  }

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
  
  if (lexer->content[lexer->cursor] == '+') {
    lexer->cursor += 1;
    token.kind = TOKEN_PLUS;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == ':') {
    lexer->cursor += 1;
    token.kind = TOKEN_COLON;
    token.text_length = 1;
    return token;
  }

  if (lexer->content[lexer->cursor] == '#') {
    token.kind = TOKEN_COMMENT;
    while (lexer->cursor < lexer->content_length && lexer->content[lexer->cursor] != '\n') {
      token.text_length += 1;
      stbtl__lexer_chop_char(lexer);
    }
    return token;
  }

  if (lexer->content[lexer->cursor] == '\'') {
    token.text_length += 1;
    if (stbtl__lexer_peek_char(lexer) == '\'') {
      stbtl__lexer_chop_char(lexer);
      token.text_length += 1;
      if (stbtl__lexer_peek_char(lexer) == '\'') {
        stbtl__lexer_chop_char(lexer);
        token.kind = TOKEN_ML_LITERAL_STRING;

        while (lexer->cursor < lexer->content_length) {
          stbtl__lexer_chop_char(lexer);
          token.text_length += 1;
          if (lexer->content[lexer->cursor] == '\'') {
            if (stbtl__lexer_peek_char(lexer) == '\'') {
              stbtl__lexer_chop_char(lexer);
              token.text_length += 1;
              if (stbtl__lexer_peek_char(lexer) == '\'') {
                stbtl__lexer_chop_char(lexer);
                stbtl__lexer_chop_char(lexer);
                token.text_length += 2;
                return token;
              } else {
                token.kind = TOKEN_INVALID;
                return token;
              }
            } else {
              token.kind = TOKEN_INVALID;
              return token;
            }
          }
        }
      } else {
        token.kind = TOKEN_INVALID;
        return token;
      }
    }
    token.kind = TOKEN_LITERAL_STRING;
    lexer->cursor += 1;
    while (lexer->cursor < lexer->content_length) {
      stbtl__lexer_chop_char(lexer);
      token.text_length += 1;
      if (lexer->content[lexer->cursor] == '\'') {
        stbtl__lexer_chop_char(lexer);
        token.text_length += 1;
        break;
      }
    }
    return token;
  }

  if (lexer->content[lexer->cursor] == '"') {
    token.text_length += 1;
    if (stbtl__lexer_peek_char(lexer) == '"') {
      stbtl__lexer_chop_char(lexer);
      token.text_length += 1;
      if (stbtl__lexer_peek_char(lexer) == '"') {
        stbtl__lexer_chop_char(lexer);
        token.kind = TOKEN_ML_BASIC_STRING;

        while (lexer->cursor < lexer->content_length) {
          stbtl__lexer_chop_char(lexer);
          token.text_length += 1;
          if (lexer->content[lexer->cursor] == '"') {
            if (stbtl__lexer_peek_char(lexer) == '"') {
              stbtl__lexer_chop_char(lexer);
              token.text_length += 1;
              if (stbtl__lexer_peek_char(lexer) == '"') {
                stbtl__lexer_chop_char(lexer);
                stbtl__lexer_chop_char(lexer);
                token.text_length += 2;
                return token;
              } else {
                token.kind = TOKEN_INVALID;
                return token;
              }
            } else {
              token.kind = TOKEN_INVALID;
              return token;
            }
          }
        }
      } else {
        token.kind = TOKEN_INVALID;
        return token;
      }
    }
    token.kind = TOKEN_BASIC_STRING;
    lexer->cursor += 1;
    bool escaped = false;
    while (lexer->cursor < lexer->content_length) {
      stbtl__lexer_chop_char(lexer);
      token.text_length += 1;
      if (lexer->content[lexer->cursor] == '\\' && stbtl__lexer_peek_char(lexer) == '"') {
        escaped = true;
      }
      if (lexer->content[lexer->cursor] == '"') {
        stbtl__lexer_chop_char(lexer);
        token.text_length += 1;
        if (escaped) {
          escaped = false;
          continue;
        } else 
          break;
      }
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

STBTL_DEF stbtl__expected_token stbtl__lexer_expect_and_consume_token(stbtl__lexer *lexer, ...)
{
  stbtl__expected_token et = {0};
  stbtl__token token = stbtl__lexer_next(lexer);
  va_list token_kinds;
  va_start(token_kinds, lexer);
  
  stbtl__token_kind kind;
  do {
    kind = va_arg(token_kinds, stbtl__token_kind);
    if (token.kind == kind) {
      et.found = true;
      et.token = token;
      break;
    }
  } while (kind != 0);

  return et;
}

STBTL_DEF stbtl__token_kind stbtl__lexer_peek_token(stbtl__lexer *lexer)
{
  
  size_t current_cursor = lexer->cursor;
  size_t current_line = lexer->line;
  size_t current_beginning_of_line = lexer->beginning_of_line;

  stbtl__token token = stbtl__lexer_next(lexer);
  lexer->cursor = current_cursor;
  lexer->line = current_line;
  lexer->beginning_of_line = current_beginning_of_line;

  return token.kind;
}

#endif // STB_TOML_LEXER_IMPLEMENTATION

/*LICENSE
Copyright (c) 2025 Vasilis Bempis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
