#include "parser.h"
#include "tokenizer.h"

int main(void)
{
  String s = {0};
  // read_entire_file("~/probe/c-toml/test.toml", &s);
  read_entire_file("/home/vasilis/probe/c-toml/keyvalue.toml", &s);

  Lexer lexer = lexer_new(s.string, s.length);
#if 1
  KeyValue kv = {0};
  bool result = parse_key_val(&lexer, &kv);
  if (result)
    printf("key: %.*s\nvalue: %.*s\n", (int) kv.key.length, kv.key.string, (int) kv.value.length, kv.value.string);
#endif
#if 0
  Token token;
  token = lexer_next(&lexer);
  while (token.kind != TOKEN_END) {
    printf("\x1b[1;31m%.*s\x1b[0m(%s) ", (int) token.text_length, token.text, token_kind_name(token.kind));
    token = lexer_next(&lexer);
  }
  printf("\n");
#endif

  // String s = {0};
  //
  // string_append(&s, "foo\t");
  // string_append(&s, "bar\t");
  // string_append(&s, "baz\t");
  //
  // printf("%s\n", s.string);

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
