/*
Insert the Telera TLR4-03680 platform into the linux PC and test the performance
!Need to modify the path in main.cpp first:  
                the const variable "fNamePrefix" should be your own folder path which contains the traffic file 
                the const variable "fNamePrefixOut" should be your own folder path which will store the result file 
Compile:        make
Run:            Use ./do.sh to test. Can modify it depend on your specific needs.
Default parameter:  Use the script do.sh to run the test, no need to use command line by yourself.
*/
#include <stdio.h>
#include "std_bf.h"
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <tmc/cpus.h>
#include <tmc/task.h>
#include <pthread.h>

using namespace std;

#define MAX_TILES		35						//core number
#define ELEMENT_INSERT	5000		
#define ELEMENT_QUERY	1000000					
#define ELEMENT_LEN		20

const string fNamePrefix = "/home/bloom/traffic/";	//for linux
const string fNamePrefixOut = "/home/bloom/sbf_tile/res/";	//for linux
int tile_counter = 0;	//count tile number
cpu_set_t desired_cpus;
pthread_mutex_t master_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t master_cond = PTHREAD_COND_INITIALIZER;
double maxQueryTime = 0.0;	//record the max time among the tiles: need to protect by semaphore
char stream[MAX_TILES][ELEMENT_QUERY][ELEMENT_LEN+1];
int coreUsed = MAX_TILES;
StandardBF *sBF[MAX_TILES];
int M[HashNum], N[HashNum], K[HashNum];
	
void init(const char *fName, unsigned int insertNum, unsigned int elementLen, unsigned int m, unsigned int k)	//init m, n and k using m/n*ln2=k	
{
	for(int i=0; i<HashNum; i++)
		N[i] = 5000;
	for(int i=0; i<HashNum; i++)
		K[i] = i+1;
    double express = 0.0;
	for(int i=0; i<HashNum; i++) {
		express = 0.0 - (log(2.0) / (K[i]*N[i]));   //use formula in paper
		M[i] = 1 / (1 - exp(express));
    }
		
	//initialize the 35 sbfs using same parameter
	for(int i=0; i<coreUsed; i++) {
		sBF[i] = new StandardBF(m,k);
	}
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		for(int j=0; j<coreUsed; j++) {
			sBF[j]->insert((const unsigned char *)line, elementLen);
		}
	}
	//initialize the 35 elements
	for(int i=0; i<coreUsed; i++) {
		for(int j=0; j<ELEMENT_QUERY; j++)
			fin >> stream[i][j];
	}
	delete[] line;
	fin.close();
}

void *thread_func(void *arg)
{
	unsigned int itr = 0;
	int element_num = ELEMENT_INSERT;
	int num= (int)(intptr_t)arg;
    int cpu = tmc_cpus_find_nth_cpu(&desired_cpus, num);

    // Bind to one tile.
    if (tmc_cpus_set_my_cpu(cpu) != 0)
        tmc_task_die("tmc_cpus_set_my_cpu() failed.");

	// To test the query time for the task
    double queryTime = 0.0;
	struct timeval begin, end;
	double queryTimeUsed = 0.0, queryTimeAll = 0.0;
	for(int j=0; j<100; j++) {
		gettimeofday(&begin, NULL); 
		for(int i=0; i<ELEMENT_QUERY; i++) {	//sum the 100 times, and get the average
			sBF[num]->query((const unsigned char *)stream[num][i], ELEMENT_LEN);
		}
		gettimeofday(&end, NULL);
		queryTimeUsed = end.tv_sec - begin.tv_sec + 1e-6 * (end.tv_usec - begin.tv_usec);
		queryTimeAll += queryTimeUsed;
	}
	queryTime = queryTimeAll/100;

	// To record the query performance of each Core
    pthread_mutex_lock(&master_lock);	//protect by semaphore
	cout <<"Core: " << num << ", queryTime = " << scientific << queryTime << endl;
	maxQueryTime = maxQueryTime > queryTime ? maxQueryTime : queryTime;
    tile_counter++;
    pthread_mutex_unlock(&master_lock);
    pthread_cond_signal(&master_cond);
    return (void*)NULL;
}

void parallel_exp(int n_tiles, const char *foutName, int m, int n, int k) {
    //cout << "In parallel_exp: # of cores is " << n_tiles << endl;
	
	//Get desired tiles and make sure we have enough.
    if (tmc_cpus_get_my_affinity(&desired_cpus) != 0)
        tmc_task_die("tmc_cpus_get_my_affinity() failed.");
    int avail = tmc_cpus_count(&desired_cpus);
    if (avail < n_tiles)
        tmc_task_die("Need %d cpus, but only %d specified in affinity!", n_tiles, avail);

    tile_counter = 0;
	maxQueryTime = 0.0;
    //Spawn a thread on each tile
    pthread_t threads[MAX_TILES];
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    for (int i = 0; i <= n_tiles; i++)
        pthread_create(&threads[i], &thread_attr, thread_func, (void*)(intptr_t)i);

    //Wait for all threads to be created and run.
    pthread_mutex_lock(&master_lock);
    while (tile_counter <= n_tiles)
        pthread_cond_wait(&master_cond, &master_lock);
    pthread_mutex_unlock(&master_lock);
	
	ofstream myfout(foutName, ios::app);
	if(!myfout) {
		printf("Open file %s error!\n", foutName);
		exit(-1);
	}
	myfout << n_tiles << "\t" << m << "\t" << n <<"\t" << k << "\t" << scientific << maxQueryTime << endl;
	myfout.close();
}

int main(int argc,char* argv[])
{
	string fName, foutName;
	int testK=2;
	cout << "begin" << endl;
	long long len=10;
	if(argc == 6) {       
		fName = fNamePrefix + string(argv[1]);
		len = atoi(argv[2]);
		foutName = fNamePrefixOut + string(argv[3]);
		coreUsed = atoi(argv[4]);
		testK = atoi(argv[5]);
	}else {
		fName = fNamePrefix + string("rand_10b_10M.tr");
		foutName = fNamePrefixOut + string("result");   //default output file
	}
	init(fName.c_str(), 5000, len, M[testK-1], K[testK-1]); //init(const char *fName, unsigned int insertNum, unsigned int elementLen, unsigned int m, unsigned int k)

	/*Code to run Performance Test on Many-core Platform*/
	for(int i=0; i<coreUsed; i++) {
		cout << "Core Number:  " << i+1 << endl;
		parallel_exp(i, foutName.c_str(), M[testK-1], 5000, K[testK-1]);
	}
	return 0;
}
