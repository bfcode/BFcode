#used for compiling the project 
all: sbf

CC = g++
INCLUDE = .
CFLAGS = -O2 -g -Wall -D LINUX_PLATFROM

sbf: main.cpp hash_function.o sha1.o md5.o
	$(CC) $(CFLAGS) $^ -o $@
hash_function.o: hash_function.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $<
sha1.o: sha1.cpp                
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $<
md5.o: md5.cpp                
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $< 

clean:
	rm -f *.o sbf
