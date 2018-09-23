NAME = ifj

CC = gcc
CFLAGS = -std=c11
LDFLAGS =

$(NAME): compileAll
	$(CC) $(CFLAGS) *.o -o $(NAME) $(LDFLAGS)
	rm -f *.o
compileAll:
	$(CC) $(CFLAGS) -c *.c $(LDFLAGS)
clean:
	rm -f *.o
remove:
	rm $(NAME)