BASE_NAME		= compiler
BASE_C 			= $(BASE_NAME).c
FRONT_END_C = front_end/*.c
BACK_END_C	= back_end/*.c
CC = gcc

$(BASE_NAME) : $(BASE_C) $(FRONT_END_C) $(BACK_END_C)
	$(CC) $(BASE_C) $(FRONT_END_C) $(BACK_END_C) -o $(BASE_NAME)

leaks : $(BASE_C) $(FRONT_END_C) $(BACK_END_C)
	clang $(BASE_C) $(FRONT_END_C) $(BACK_END_C) -o leakCheck
	leaks -atExit -- ./leakCheck
	rm leakCheck

clean: $(BASE_NAME)
	clear
	clear
	rm $(BASE_NAME)

.PHONY: leaks, clean, run
