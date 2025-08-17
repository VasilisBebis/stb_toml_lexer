CFLAGS = -Wall -Wextra -ggdb 

main: main.c tokenizer.c parser.c
	gcc $(CFLAGS) -o main main.c tokenizer.c parser.c -I.

clean:
	rm -f main
