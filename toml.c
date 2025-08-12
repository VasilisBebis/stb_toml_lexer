#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define TODO(message) do {fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message)} while(0)

typedef struct {
  char   *string;
  size_t length;
  size_t capacity;
} String;

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

typedef enum {
  STRING,
  INTEGER,
  FLOAT,
  BOOLEAN,
  OFFSET_DATETIME,
  LOCAL_DATETIME,
  OFFSET_DATE,
  LOCAL_DATE,
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
  struct Element *elements;
} Table;

struct Element {
  ElementType type;
  union {
    KeyValue key_value;
    Array    array;
    Table    table;
  };
};

bool read_entire_file(const char *path, String *str)
{
  bool result = false;
  FILE *fp = fopen("/home/vasilis/probe/c-toml/test.toml", "rb");
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
