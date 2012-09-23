.PHONY: all, clean, debug

FLAGS = -Wall
LIBS = -l ncurses
SOURCES = $(shell find . -maxdepth 1 -name "*.c")
OBJECTS = $(SOURCES:%.c=%.o)

NAME = artteco

RED="\033[1;31m"
NORMAL="\033[0m"

all: DEBUG  = no
all: FLAGS := $(FLAGS) -D NDEBUG
all: touch $(NAME)

debug: DEBUG  = yes
debug: FLAGS := $(FLAGS) -ggdb3 -D FLUSH
debug: touch $(NAME)

$(NAME): $(OBJECTS)
	@echo -e linking $(RED)
	@gcc $(FLAGS) $(LIBS) -o $(NAME) $(OBJECTS)
	@echo -en $(NORMAL)

%.o: %.c
	@echo -e compile $< $(RED)
	@gcc -c $(FLAGS) $(LIBS) -o $@ $<
	@echo -en $(NORMAL)

clean:
	@rm -f $(NAME) debug
	$(shell find . -name "*.o" -exec rm -f {} \;)

touch:
	$(shell [ -f debug -a "$(DEBUG)" = "no" ] && { touch *.c; rm debug; })
	$(shell [ ! -f debug -a "$(DEBUG)" = "yes" ] && { touch *.c; touch debug; })
