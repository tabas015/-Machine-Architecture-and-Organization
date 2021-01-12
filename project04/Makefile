CFLAGS = -Wall -Werror -g -Og
# CFLAGS = -Wall -g -O3
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	matsquare_print \
	matsquare_benchmark \
	search_benchmark \

all : $(PROGRAMS)

clean :
	rm -f $(PROGRAMS) *.o vgcore.* 

help :
	@echo 'Typical usage is:'
	@echo '  > make                          # build all programs'
	@echo '  > make clean                    # remove all compiled items'
	@echo '  > make zip                      # create a zip file for submission'
	@echo '  > make prob1                    # built targets associated with problem 1'
	@echo '  > make prob1 testnum=5          # run problem 1 test #5 only'
	@echo '  > make test-prob2               # run test for problem 2'
	@echo '  > make test                     # run all tests'

############################################################
# 'make zip' to create p2-code.zip for submission
AN=p4
zip : clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi


%.o : %.c
	$(CC) -c $<


# Matrix square optimization problem
matsquare_print : matsquare_print.o matvec_util.o matsquare_base.o matsquare_optm.o
	$(CC) -o $@ $^

matsquare_benchmark : matsquare_benchmark.o matvec_util.o matsquare_base.o matsquare_optm.o
	$(CC) -o $@ $^ -lm


# Search Algorithm Problem
search_benchmark : search_benchmark.o search_funcs.o
	$(CC) -o $@ $^

search_benchmark_makeup : search_benchmark_makeup.o search_funcs.o
	$(CC) -o $@ $^

search_benchmark_optlin : search_benchmark_optlin.c search_funcs.o 
	$(CC) -o $@ $^

# Testing Targets
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# run with valgrind to check for memory problems
test: test-prob1 test-prob2

test-setup :
	@chmod u+rx testy

test-prob1: matsquare_benchmark matsquare_print test-setup
	./testy test_matsquare.org 

test-prob2: search_benchmark test-setup
	./testy test_search_benchmark.org

clean-tests :
	rm -rf test-results
