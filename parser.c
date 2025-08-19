#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "parser.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define TODO(message) do {fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message)} while(0)

void string_reserve(String *str, size_t wanted_capacity)
{
  if (str->capacity < wanted_capacity) {
    if (str->capacity == 0) {
      str->capacity = 256; // Initial capacity of 256 bytes
    }
    while (str->capacity < wanted_capacity) {
      str->capacity *= 2;
    }
    str->string = realloc(str->string, str->capacity * sizeof(*str->string));
    assert(str->string != NULL && "String (re)allocation failed\n");
  }
}

void string_append(String *str, char *appendant) 
{
  const char *appendant_str = appendant;
  size_t appendant_str_length = strlen(appendant_str);
  string_reserve(str, str->length + appendant_str_length);
  memcpy(str->string + str->length, appendant_str, appendant_str_length * sizeof(*str->string));
  str->length += appendant_str_length; 
}

void string_append_n(String *str, const char *appendant, size_t n) 
{
  assert(strlen(appendant) >= n);
  const char *appendant_str = appendant;
  size_t appendant_str_length = n;
  string_reserve(str, str->length + appendant_str_length);
  memcpy(str->string + str->length, appendant_str, appendant_str_length * sizeof(*str->string));
  str->length += appendant_str_length; 
}

bool read_entire_file(const char *path, String *str)
{
  bool result = false;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL)                 goto defer;
  if (fseek(fp, 0, SEEK_END) < 0) goto defer;
  long file_size = ftell(fp);
  if (file_size < 0)              goto defer;
  if (fseek(fp, 0, SEEK_SET) < 0) goto defer;

  size_t new_length = str->length + file_size;
  if (new_length > str->capacity)
    string_reserve(str, new_length);

  fread(str->string + str->length, file_size, 1, fp);
  if (ferror(fp))                 goto defer;
  str->length = new_length;
  result = true;

defer:
    if (!result) fprintf(stderr, "ERROR: Could not read file %s: %s\n", path, strerror(errno));
    if (fp) fclose(fp);
    return result;
}

bool parse_key_val(Lexer *lexer, KeyValue *kv)
{
  bool result = false;
  ExpectedToken expect_key = lexer_expect_token(lexer, TOKEN_SYMBOL, 
      TOKEN_BASIC_STRING, TOKEN_LITERAL_STRING);
  if (!expect_key.found) return false;

  ExpectedToken expect_kv_sep = lexer_expect_token(lexer, TOKEN_EQUALS);
  if (!expect_kv_sep.found) return false;

  ExpectedToken expect_value = lexer_expect_token(lexer, TOKEN_PLUS, TOKEN_SYMBOL, 
      TOKEN_BASIC_STRING, TOKEN_LITERAL_STRING, TOKEN_ML_BASIC_STRING, TOKEN_ML_LITERAL_STRING);
  if (!expect_value.found) return false;

  if (expect_value.token.kind == TOKEN_PLUS) {

    ExpectedToken expect_actual_value = lexer_expect_token(lexer, TOKEN_SYMBOL);
    if (!expect_actual_value.found) return false;

    expect_value = expect_actual_value;
  }

  String temp = {0};
  string_append_n(&temp, expect_value.token.text, expect_value.token.text_length);
  char last_char = expect_value.token.text[expect_value.token.text_length-1];
  if (last_char == 'e') {
    while (lexer_peek_token(lexer) != TOKEN_COMMENT 
        && lexer_peek_token(lexer) != TOKEN_NEWLINE) {

      ExpectedToken expected = lexer_expect_token(lexer, TOKEN_SYMBOL, TOKEN_PLUS);
      if (!expected.found) return false;

      string_append_n(&temp, expected.token.text, expected.token.text_length);

    }
  }
  
  ExpectedToken expect_newline = lexer_expect_token(lexer, TOKEN_NEWLINE, TOKEN_COMMENT);
  if (!expect_newline.found) return false;

  string_append_n(&kv->key, expect_key.token.text, expect_key.token.text_length);
  string_append_n(&kv->value, temp.string, temp.length);
  result = true;

  switch (expect_value.token.kind) {
    case TOKEN_BASIC_STRING:
    case TOKEN_ML_BASIC_STRING:
    case TOKEN_LITERAL_STRING:
    case TOKEN_ML_LITERAL_STRING:
      kv->type = STRING;
      break;
    case TOKEN_SYMBOL:
      {
        if (strchr(kv->value.string, '.') == NULL) { // no `.` found, meaning its not a float
          char* end_int;
          //TODO: handle this stupid case: `int8 = 1_2_3_4_5`
          strtoll(kv->value.string, &end_int, 0);
          if (kv->value.string != end_int) {
            kv->type = INTEGER;
            break;
          }

        }
        if (strncmp("true", kv->value.string, kv->value.length) == 0 || strncmp("false", kv->value.string, kv->value.length) == 0) {
          kv->type = BOOLEAN;
          break;
        }
        if (strncmp("inf", kv->value.string, kv->value.length) == 0 || strncmp("-inf", kv->value.string, kv->value.length) == 0) {
          kv->type = INFINITY;
          break;
        }
        if (strncmp("nan", kv->value.string, kv->value.length) == 0 || strncmp("-nan", kv->value.string, kv->value.length) == 0) {
          kv->type = NAN;
          break;
        }

        char* end_float;
        strtold(kv->value.string, &end_float); //NOTE: this can also handle NaN and inf (but i think they should be handled separately)
        if (kv->value.string != end_float) {
          kv->type = FLOAT;
          break;
        }
        //TODO: handle the datetimes

      }
      break;
    default:
      kv->type = INVALID;
  }
  return result;
}


const char *value_type_name(ValueType type)
{
  switch (type) {
    case STRING:
      return "string";
    case INTEGER:
      return "integer";
    case INFINITY:
      return "infinity";
    case NAN:
      return "nan";
    case FLOAT:
      return "float";
    case BOOLEAN:
      return "boolean";
    case OFFSET_DATETIME:
      return "offset datetime";
    case LOCAL_DATETIME:
      return "local datetime";
    case OFFSET_DATE:
      return "offset date";
    case LOCAL_DATE:
      return "local date";
    case INVALID:
      return "invalid";
    default:
      return "not implemented";
  }
}
