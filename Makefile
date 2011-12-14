.PHONY: all, clean

all:
	gcc -Wall -o artteco -l ncurses *.c

debug:
	gcc -Wall -g -o artteco -l ncurses *.c

clean:
	rm -rf artteco
