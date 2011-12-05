.PHONY: all, clean

all:
	gcc -o artteco -l ncurses *.c

clean:
	rm -rf artteco
