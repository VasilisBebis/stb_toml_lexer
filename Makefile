CFLAGS = -Wall -Wextra -ggdb 

main: main.c parser.c
	gcc $(CFLAGS) -o main main.c parser.c -I.

clean:
	rm -f main
