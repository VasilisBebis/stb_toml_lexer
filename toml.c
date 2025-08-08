#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

typedef struct {
  char *string;
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
    assert(str->string != NULL && "String (re)allocation failed");
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
  const char *key;
  const char *value;
  ValueType type;
} KeyValue;

typedef struct {
  const char *key;
  size_t element_count;
  Element *elements;
} Array;

typedef struct {
  const char *key;
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

int main(void)
{
  /*
  FILE *fp;
  fp = fopen("~/probe/c-toml/test.toml", "rb");
  if (!fp) {
    fprintf(stderr, "ERROR: Could not open file!\n");
    return EXIT_FAILURE;
  }
  
  uint8_t buffer[64 * 1024] = {0};
  size_t ret = fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), fp);
  if (ferror(fp)) {
    fprintf(stderr, "ERROR: Could not read file!");
    return EXIT_FAILURE;
  }
  */

  String s = {0};

  string_append(&s, "foo\t");
  string_append(&s, "bar\t");
  string_append(&s, "baz\t");

  printf("%s\n", s.string);

  // Element element = {0};
  // element.type = KEY_VALUE;
  // element.key_value.key = "foo";
  // element.key_value.value = "bar";
  //
  // Table table = {0};
  // table.key = "test";
  // table.element_count = 1;
  // table.elements = &element;
  //
  // printf("%s\n", table.key);
  // printf("%d\n", table.elements->type);
  // printf("%s\n", table.elements->key_value.key);

  // fclose(fp);
  return 0;
}
