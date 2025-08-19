#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdbool.h>
#include "tokenizer.h"

typedef struct {
  char   *string;
  size_t length;
  size_t capacity;
} String;

typedef enum {
  STRING,
  INTEGER,
  INFINITY,
  NAN,
  FLOAT,
  BOOLEAN,
  OFFSET_DATETIME,
  LOCAL_DATETIME,
  OFFSET_DATE,
  LOCAL_DATE,
  INVALID,
} ValueType;

typedef enum {
  KEY_VALUE,
  ARRAY,
  TABLE
} ElementType;

typedef struct Element Element;

typedef struct {
  String key;
  String value;
  ValueType type;
} KeyValue;

typedef struct {
  String key;
  size_t element_count;
  Element *elements;
} Array;

typedef struct {
  String key;
  bool is_root;
  size_t element_count;
  Element *elements;
} Table;

struct Element {
  ElementType type;
  union {
    KeyValue key_value;
    Array    array;
    Table    table;
  };
};

void string_reserve(String *str, size_t wanted_capacity);
void string_append(String *str, char *appendant);
void string_append_n(String *str, const char *appendant, size_t n);
bool read_entire_file(const char *path, String *str);

bool parse_key_val(Lexer *lexer, KeyValue *kv);

const char *value_type_name(ValueType type);
#endif // PARSER_H
