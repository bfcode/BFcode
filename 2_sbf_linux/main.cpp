/*
sbf_linux: All the tests described in evaluation section of the paper. (Performance test on many-core is in folder sbf_linux_tile_muticore)
           To run the specific test, you should comment out the corresponding test code in main().
           
!Need to modify the path in main.cpp first:  the const variable "fNamePrefix" should be your own folder path which contains the traffic file 
Compile:         make
Run:             ./sbf rand_20b_100M.tr(traffic file name) 20(length of the element in traffic file) 100(multiply by 1,000,000(M as a unit) in the program, so will search 100,000,000 elements)
Default parameter: 
                 If no other parameter input, only run as ./sbf, the code will run as "./sbf rand_20b_100M.tr 20 100" as described before.
*/

#include <stdio.h>
#include "std_bf.h"
#include <fstream>
#include <stdlib.h>
#include <math.h>

#ifdef LINUX_PLATFROM
#include <sys/time.h>
const string fNamePrefix = "/home/bloom/traffic/";	//for linux, need to be modified to be your own folder path which contains the traffic file 
#else
#include <windows.h>
#include <time.h>
const string fNamePrefix = "E:/traffic/";   //if use in Windows, need to be modified also. Our test are mainly done on Linux
#endif // LINUX_PLATFROM

int M[HashNum], N[HashNum], K[HashNum];
	
void init()	//init m, n and k
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
}

double falsePositiveRate(unsigned int m, unsigned int n, unsigned int k)	//Theoretical false positive rate
{
	double express = 0.0 - (n*(k+0.0)/m);
	return pow(1.0-exp(express), k+0.0);
}

//False Positive Probability Test: Basic Test
int BFs_correction_test(const char *fName, unsigned int m, unsigned int k,	
							unsigned int insertNum, long long queryNum, unsigned int elementLen)
{
	StandardBF *sBF1=new StandardBF(m,k);
	int falsePositive = 0;
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		sBF1->insert((const unsigned char *)line, elementLen);
	}
	for(int i=0; !fin.eof() && i<queryNum; i++) {	//Query numbers
		fin >> line;
		if(sBF1->query((const unsigned char *)line, elementLen) == 1)
			falsePositive++;
	}
	fin.close();
	delete[] line;
	delete sBF1;
	//printf("FalsePositive numbers: %d\n", falsePositive);
	return falsePositive;
}

//False Positive Probability Test: Error ratio vs. Querying number
void BFs_correction_test_2(const char *fName, unsigned int m, unsigned int k, 
						unsigned int insertNum, long long queryNum, unsigned int elementLen, unsigned int times)
{
	StandardBF *sBF1=new StandardBF(m,k);
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		sBF1->insert((const unsigned char *)line, elementLen);
	}

	double theoryRes = falsePositiveRate(m, insertNum, k);
	int falsePositive = 0;
	for(int i=0; !fin.eof() && i<times; i++) {	//print results many times
		for(int j=0; !fin.eof() && j<queryNum/times; j++) {
			fin >> line;
			if(sBF1->query((const unsigned char *)line, elementLen) == 1)
				falsePositive++;        //accumulated results
		}
		double res = (double)falsePositive/((i+1)*queryNum/times);
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%e\t%e\t%f%%\n", 
				m, insertNum, k, (i+1)*queryNum/times, falsePositive, theoryRes*queryNum, theoryRes, res, (res-theoryRes)/theoryRes*100);
	}

	fin.close();
	delete[] line;
	delete sBF1;
	//printf("FalsePositive numbers: %d\n", falsePositive);
}

//False Positive Probability Test: Error ratio vs. Querying number, not accumulate
void BFs_correction_test_3(const char *fName, unsigned int m, unsigned int k, 
						unsigned int insertNum, long long queryNum, unsigned int elementLen, unsigned int times)
{
	StandardBF *sBF1=new StandardBF(m,k);
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		sBF1->insert((const unsigned char *)line, elementLen);
	}

	double theoryRes = falsePositiveRate(m, insertNum, k);
	int falsePositive = 0;
	int numForeach = queryNum/((1+times)*times/2);  //get the number for query: 1+2+3+...+times
	//printf("numForeach: %d\n", numForeach);
	for(int i=0; !fin.eof() && i<times; i++) {	//print results many times
		falsePositive = 0;      //not accumulate, so clear to 0
		for(int j=0; !fin.eof() && j<numForeach*(i+1); j++) {
			fin >> line;
			if(sBF1->query((const unsigned char *)line, elementLen) == 1)
				falsePositive++;
		}
		double res = (double)falsePositive/((i+1)*numForeach);
		printf("%d\t%d\t%d\t%d\t%d\t%e\t%e\t%f%%\n", 
				m, insertNum, k, (i+1)*numForeach, falsePositive, theoryRes, res, (res-theoryRes)/theoryRes*100);
	}
	fin.close();
	delete[] line;
	delete sBF1;
	//printf("FalsePositive numbers: %d\n", falsePositive);
}

//Optimal K Formula Verification Test: m is 100,000, n is 8000, 10000, 12000. query 100M elements
void BFs_correction_test_4(const char *fName, unsigned int m, unsigned int k, 
						unsigned int insertNum, long long queryNum, unsigned int elementLen)
{
	StandardBF *sBF1=new StandardBF(m,k);
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		sBF1->insert((const unsigned char *)line, elementLen);
	}

	double theoryRes = falsePositiveRate(m, insertNum, k);
	//printf("theoryRes: %e\n",theoryRes);
	int falsePositive = 0;
	for(int i=0; !fin.eof() && i<queryNum; i++) {	//print results many times
		fin >> line;
		if(sBF1->query((const unsigned char *)line, elementLen) == 1)
			falsePositive++;
	}
	double res = (double)falsePositive /queryNum;
	int theryFalsePositiveNum = theoryRes * (double)queryNum;
	printf("%d\t%d\t%d\t%ld\t%d\t%d\t%e\t%e\t%f%%\n",
			  m, insertNum, k, queryNum, falsePositive, theryFalsePositiveNum, theoryRes, res, (res-theoryRes)/theoryRes*100);

	fin.close();
	delete[] line;
	delete sBF1;
	//printf("FalsePositive numbers: %d\n", falsePositive);
}

//Performance Test on CPU
void BFs_correction_test_5(const char *fName, unsigned int m, unsigned int k, 
						unsigned int insertNum, long long queryNum, unsigned int elementLen, unsigned int expNum)	//produce expNum result spots
{
	StandardBF *sBF1=new StandardBF(m,k);
	int falsePositive = 0;
	ifstream fin(fName);
	if(!fin) {
		printf("Open file %s error!\n", fName);
		exit(-1);
	}
	char *line = new char[elementLen+1];	//last '\0'
	fin >> line;	//omit first line
	for(int i=0; !fin.eof() && i<insertNum; i++) {	//Insert numbers
		fin >> line;
		sBF1->insert((const unsigned char *)line, elementLen);
	}

	long long eachNum = queryNum / expNum;
	char **stream = new char* [eachNum];    //distribute the traffic file to expNum pieces, each piece is eachNum in length
	for(int i=0; i<eachNum; i++)
		stream[i] = new char[elementLen+1];

	//begin querying, and get the cost cpu time
	for(int nn=0; nn<expNum; nn++) {	//expNum 
		for(int i=0; !fin.eof() && i<eachNum; i++) {	//Query numbers = eachNum, store in stream first
			fin >> stream[i];
		}
#ifdef LINUX_PLATFROM	// linux platform to calculate time, LINUX_PLATFROM is defined in Makefile
		struct timeval begin, end;
		double queryTimeUsed = 0.0, queryTimeAll = 0.0;
		for(int j=0; j<100; j++) {
			gettimeofday(&begin, NULL); 
			for(int i=0; i<eachNum; i++) {	//sum the 100 times, and get the average
				sBF1->query((const unsigned char *)stream[i], elementLen);
			}
			gettimeofday(&end, NULL);
			queryTimeUsed = end.tv_sec - begin.tv_sec + 1e-6 * (end.tv_usec - begin.tv_usec);
			queryTimeAll += queryTimeUsed;
		}
		printf("%d\t%d\t%d\t%d\t%ld\t%e\n",	//print the average
				nn+1, m, insertNum, k, eachNum, queryTimeAll/100);
		cout << nn+1 << "\t" << m << "\t" << insertNum <<"\t" << k << "\t" << eachNum << "\t" << scientific << queryTimeAll/100 << endl;
#else   // windows platform to calculate time
		LARGE_INTEGER frequence,privious,privious1;
		if(!QueryPerformanceFrequency(&frequence))
			return;
		//printf("\tfrequency=%u\n",frequence.QuadPart);//3020585
		QueryPerformanceCounter(&privious);
		for (int j=0;j<100;j++) {
			for(int i=0; i<eachNum; i++) {	    //sum the 100 times, and get the average
				sBF1->query((const unsigned char *)stream[i], elementLen);
			}
		}
		QueryPerformanceCounter(&privious1);
		double duration = (privious1.QuadPart-privious.QuadPart)/frequence.QuadPart;
		cout << nn+1 << "\t" << m << "\t" << insertNum <<"\t" << k << "\t" << eachNum << "\t" << scientific << duration/100 << endl;
#endif
	}

	fin.close();
	delete[] line;
	for(int i=0; i<eachNum; i++)	//free space
		delete []stream[i];
	delete []stream;
	delete sBF1;
}

int main(int argc,char* argv[])
{
	string fName;
	long long len=20, queryNum=1000000;		//use long long, in case of overflow
	if(argc == 4) {       
		fName = fNamePrefix + string(argv[1]);
		len = atoi(argv[2]);
		queryNum *=  atoi(argv[3]);
	}else {
		fName = fNamePrefix + string("rand_20b_100M.tr");
		queryNum *= 100;    //default parameter
	}
	init();
    
	/*Code to run basic test*/
	//for(int i=0; i<16; i++) {	    //k up to 16
	//	double theoryRes = falsePositiveRate(M[i], N[i], K[i]);
	//	int falseNum = BFs_correction_test(fName.c_str(), M[i], K[i], N[i], queryNum, len);//file name, m, k, insertNum, queryNum, elementLen
	//	double res = (double)falseNum/queryNum;
	//	printf("%d\t%d\t%d\t%d\t%e\t%e\t%f%%\n", 
	//			M[i], N[i], K[i], falseNum, theoryRes, res, (res-theoryRes)/theoryRes*100);
	//}
    
	/*Code to run False Positive Probability Test: Error ratio vs. Querying number*/
	//for(int i=3; i<16; i++) {	//k=4,5,6,8,11,14,16
	//	if(i==6 || i==8 || i==9 || i==11 || i==12 || i==14)
	//		continue;
	//	BFs_correction_test_2(fName.c_str(), M[i], K[i], N[i], queryNum, len, 100);
	//	printf("\n");
	//}
    
	/*Code to run False Positive Probability Test: Error ratio vs. Querying number, not accumulate*/
	//printf("\nNot accumulation\n");
	//for(int i=3; i<6; i++) {	//k=4,5,6,8,11,14,16		//347138467 elements
	//	if(i==6 || i==8 || i==9 || i==11 || i==12 || i==14)
	//		continue;
	//	BFs_correction_test_3(fName.c_str(), M[i], K[i], N[i], queryNum, len, 100);
	//	printf("\n");
	//}

	/*Optimal K Formula Veri?cation Test: m is 100,000, n is from 4000 to 14000. query 100M elements, each time get 16 spots*/
	/*printf("M: %d\tN: %d\n", 100000, 4000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 4000, queryNum, len);
	printf("M: %d\tN: %d\n", 100000, 6000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 6000, queryNum, len);
	printf("M: %d\tN: %d\n", 100000, 8000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 8000, queryNum, len);*/
	/*printf("M: %d\tN: %d\n", 100000, 10000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 10000, queryNum, len);
	printf("M: %d\tN: %d\n", 100000, 12000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 12000, queryNum, len);
	printf("M: %d\tN: %d\n", 100000, 14000);
	for(int i=0; i<16; i++)
		BFs_correction_test_4(fName.c_str(), 100000, K[i], 14000, queryNum, len);*/

	/*Code to run Performance Test on CPU*/
	//BFs_correction_test_5(fName.c_str(), M[3], K[3], 5000, queryNum, len, 100);
	//BFs_correction_test_5(fName.c_str(), M[5], K[5], 5000, queryNum, len, 100);
	//BFs_correction_test_5(fName.c_str(), M[7], K[7], 5000, queryNum, len, 100);

	return 0;
}
