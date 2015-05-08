/*
Produce traffic file which contains random elements without duplicates.
        Compile:  g++ -O2 traffic.cpp -o traffic
        Run:      ./traffic 64 100000
        ==>       Means produce a traffic file with 102,400,000 elements without duplicates, and each element is 20 characters in length.
        Detailed: 
                  First parameter: Bit length of the element use binary system, the actual element stored in the traffic file is in decimalism, which is 20 characters in length
                                   So input 64, means the biggest element in the traffic file is 2^64-1=18446744073709551615, 20 characters in length.
                  Second parameter: Size of the traffic file, which will multiply by 1024 in the program, so input N will produce N*1024 elements  
                  Default parameter: If no other parameter input, only run as ./traffic, the code will use 64 as the first parameter and 1 as the second parameter.
                                     This will produce a traffic file with 1,024 elements without duplicates, and each element is 20 characters in length.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <algorithm>
#include <time.h>
#include <set>
using namespace std;

#define DATA_K 1024

void genTraffic (int data_len, int data_size) {

    srand (time (NULL));
    char file_name[1024];
    sprintf (file_name, "rand_%d_%dK_%d.tr", data_len, data_size, rand());

    data_size *= DATA_K;		// use 1024 as the basic unit
    FILE *file = fopen (file_name, "w");

    double data;
    unsigned long long data_64;
    set<unsigned long long> dataStore;
    fprintf (file, "%d\n",data_size);	//the first line in the traffic file is the data size 
	
    for (int i = 0 ; i < data_size ; i ++) {
        data_64 = 0;
        for (int j = 0 ; j < data_len ; j += 16) {
            data_64 = data_64 << 16 | (rand ());		//to produce element with data_len
        }
        if(dataStore.find(data_64) == dataStore.end()) {	//only the element which is new will be added to the result file
            fprintf (file, "%llu\n", data_64);
            dataStore.insert(data_64);
        } else {
            printf("Repeat elements: %llu! Will regenerate it.\n", data_64);
            i--;
        }
    }
    fclose (file);
}

int main (int argc, char** argv) {

    if(argc == 3) {
        genTraffic (atoi (argv[1]), atoi (argv[2]));
    }else{
        genTraffic (64, 1);		//the default parameters
    }
    return 0;
}
