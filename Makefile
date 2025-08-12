CFLAGS = -Wall -Wextra -ggdb

main: main.c tokenizer.c toml.c
	gcc $(CFLAGS) -o main main.c tokenizer.c -I.

clean:
	rm -f main
