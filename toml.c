#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

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
  FILE *fp;
  fp = fopen("./test.yaml", "rb");
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

  Element element = {0};
  element.type = KEY_VALUE;
  element.key_value.key = "foo";
  element.key_value.value = "bar";

  Table table = {0};
  table.key = "test";
  table.element_count = 1;
  table.elements = &element;

  printf("%s\n", table.key);
  printf("%d\n", table.elements->type);
  printf("%s\n", table.elements->key_value.key);

  fclose(fp);
  return 0;
}
