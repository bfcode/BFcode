# This is a -*- Makefile -*-

# Default target.
all:

ifdef TILERA_ROOT

#CC = $(TILERA_ROOT)/bin/tile-gcc
CC = $(TILERA_ROOT)/bin/tile-g++

else

ifeq ($(filter tile%,$(shell uname -m)),)
$(error The 'TILERA_ROOT' environment variable is not set.)
endif

CC = g++

endif

CFLAGS = -g -O2
#CFLAGS = -g
#CFLAGS = -g -O -frepo 
#CFLAGS = -g -std=gnu99 -O2

EXE = sbf

md5.o: md5.cpp 
	$(CC) $(CFLAGS) $(XFLAGS) -c $< -pthread -o $@

sha1.o: sha1.cpp 
	$(CC) $(CFLAGS) $(XFLAGS) -c $< -pthread -o $@

hash_function.o: hash_function.cpp 
	$(CC) $(CFLAGS) $(XFLAGS) -c $< -pthread -o $@

sbf: main.cpp hash_function.o sha1.o md5.o
	$(CC) $(LDFLAGS) $(XFLAGS) $<  hash_function.o sha1.o md5.o -pthread -ltmc -o $@ 

all: $(EXE)

clean:
	rm -f *.o $(EXE)

.PHONY: all clean


FILTER = sort | sed 's/tile [0-9]*:/tile N:/'

ifdef TILERA_ROOT

TILE_MONITOR = $(TILERA_ROOT)/bin/tile-monitor

COMMON_ARGS = \
  --mkdir /opt/test \
  --cd /opt/test \
  --upload $(EXE) $(EXE) \
  --mount-same /home/wyk/ \
  --resume \
  --root \
  --tile 6x6 -- $(EXE) rand_20b_100M.tr 20 res0720_sim1.txt 35
  
COMMON_ARGS_SIM = \
  --mkdir /opt/test \
  --cd /opt/test \
  --upload $(EXE) $(EXE) \
  --mount-same /home/wyk/ \
  --root \
  --tile 2x2 -- $(EXE) rand_20b_100M.tr 20 res0720_sim1.txt 4

DEV_ARGS = $(COMMON_ARGS)

# For faster execution, also specify the "--functional" option to run the
# simulator in functional mode, as opposed to the default timing-accurate mode.
SIM_ARGS = --image 2x2 $(COMMON_ARGS_SIM)

run_dev: $(EXE)
	$(TILE_MONITOR) $(DEV_ARGS)

test_dev: $(EXE)
	set -e -o pipefail; \
	$(TILE_MONITOR) $(DEV_ARGS) \
	  | $(FILTER) > output.run; \
	cat output.txt | $(FILTER) | diff output.run -

run_sim: $(EXE)
	$(TILE_MONITOR) $(SIM_ARGS)

test_sim: $(EXE)
	set -e -o pipefail; \
	$(TILE_MONITOR) $(SIM_ARGS) \
	  | $(FILTER) > output.run; \
	cat output.txt | $(FILTER) | diff output.run -

.PHONY: run_sim test_sim run_dev test_dev single_run

else

run: $(EXE)
	./$(EXE)

test: $(EXE)
	set -e -o pipefail; \
	./$(EXE) \
	  | $(FILTER) > output.run; \
	cat output.txt | $(FILTER) | diff output.run -

.PHONY: run test

endif
