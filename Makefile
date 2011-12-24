.PHONY: all, clean, debug, crash

CC = gcc
CFLAGS = -Wall
LIBS = -l ncurses
SOURCES = *.c

NAME = artteco

all:
	$(CC) $(CFLAGS) $(LIBS) -o $(NAME) -D NDEBUG $(SOURCES)

debug:
	$(CC) $(CFLAGS) $(LIBS) -o $(NAME) -g $(SOURCES)

crash:
	$(CC) $(CFLAGS) $(LIBS) -o $(NAME) -g -D FLUSH $(SOURCES)

clean:
	rm -rf $(NAME)

