CFLAGS = -Wall -g
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	thermo_main \


all : $(PROGRAMS)

clean :
	rm -f $(PROGRAMS) *.o vgcore.* hybrid_main

############################################################
# 'make zip' to create p3-code.zip for submission
AN=p3
zip : clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi

# thermometer problem
thermo_main : thermo_main.o thermo_sim.o thermo_update_asm.o 
	$(CC) -o $@ $^

thermo_main.o : thermo_main.c thermo.h
	$(CC) -c $<

thermo_sim.o : thermo_sim.c thermo.h
	$(CC) -c $<

# required assembly implementation
thermo_update_asm.o : thermo_update_asm.s thermo.h
	$(CC) -c $<


# C version of functions
thermo_update.o : thermo_update.c thermo.h
	$(CC) -c $<

# thermo_update functions testing program
test_thermo_update : test_thermo_update.o thermo_sim.o thermo_update_asm.o
	$(CC) -o $@ $^

test_thermo_update.o : test_thermo_update.c
	$(CC) -c $<


# main which uses both assmebly and C update functions for incremental
# testing
hybrid_main : thermo_main.o thermo_sim.o thermo_update_asm.o thermo_update.o
	$(CC) -o $@ $^

# hybrid test program
test_hybrid_thermo_update : test_thermo_update.o thermo_sim.o thermo_update_asm.o thermo_update.o
	$(CC) -o $@ $^

################################################################################
# Testing Targets
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

test-setup :
	@chmod u+rx testy

test: test-prob1 

test-prob1: thermo_main test_thermo_update test-setup
	./testy test_prob1.org $(testnum)

test-hybrid: hybrid_main test_hybrid_thermo_update test-setup
	./testy test_hybrid.org $(testnum)
	@echo
	@echo "WARNING: These are the hybrid tests used for incremental development."
	@echo "         Make sure to run 'make test' to run the full tests before submitting."

clean-tests : 
	rm -rf test-results/ test_thermo_update test_hybrid_thermo_update


