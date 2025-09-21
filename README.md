A simple [TOML](https://toml.io) lexer, written in the style of an [stb](https://github.com/nothings/stb/) header-only library.

# Usage
The implementation of the library is guarded by the `STB_TOML_LEXER_IMPLEMENTATION` flag. The header can be included many times, but the implementation should be defined only once. To do so, define the flag before including the header file. Optionally define `STBTL_DEF` flag (which goes before function declarations) as `static` or `static inline` before including the header file

# Quick Example

```c
#define STB_TOML_LEXER_IMPLEMENTATION
//optionally
//#define STBTL_DEF static inline
#include "tokenizer.h"

int main(void)
{
  const char *toml_content; // = content of a toml file 
  size_t toml_content_length; // = length of the toml_content string

  /*Get the content of a toml file from somewhere ...*/

  stbtl__lexer lexer = stbtl__lexer_new(toml_content, toml_content_length);
  stbtl__token token;
  token = stbtl__lexer_next(&lexer);
  while (token.kind != TOKEN_END) {
    /*Do something with the token ... */
    token = stbtl__lexer_next(&lexer);
  }
  printf("\n");
  return 0;
}
```
