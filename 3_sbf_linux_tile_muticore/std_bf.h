#ifndef STD_BF
#define STD_BF 

#include "hash_function.h"
#include <iostream>
#include <string>
#include <memory.h>
using namespace std;

#define HashNum	16

class StandardBF 
{
public:	
	StandardBF(unsigned int m, unsigned int k)
	{
		if(k > 18){
			cerr << "the # of hash functions cannot exceed 18" << endl;
		}
		bf_m = m;
		bf_k = k;
		bf_n = 0;
		QuerymemAccNum=0;
		bf_base = new uchar [bf_m/8+1];	//8λ
		memset(bf_base, 0, bf_m/8+1);

		//uint (* tmp_ptr[HashNum])(const unsigned char * str, uint len) = 
		//{ JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
		//APHash, BKDR, BOB, DEKHash, DJBHash, FNV32, Hsieh};

		uint (* tmp_ptr[HashNum])(const unsigned char * str, uint len) = 
		{BOB1,BOB2,BOB3,BOB4,BOB5,BOB6,BOB7,BOB8,
		 BOB9,BOB10,BOB11,BOB12,BOB13,BOB14,BOB15,BOB16};

		for(int i = 0; i < HashNum; i++){
			bf_hfp[i] = tmp_ptr[i];
		}
	
	}
	StandardBF(){
		bf_m = 0;
		bf_k = 0;
		bf_n = 0;
		QuerymemAccNum=0;
		bf_base = NULL;
		uint (* tmp_ptr[HashNum])(const unsigned char * str, uint len) = 
		{BOB1,BOB2,BOB3,BOB4,BOB5,BOB6,BOB7,BOB8,
		BOB9,BOB10,BOB11,BOB12,BOB13,BOB14,BOB15,BOB16};
		for(int i = 0; i < HashNum; i++){
			bf_hfp[i] = tmp_ptr[i];
		}
	}
	~StandardBF()
	{
		delete [] bf_base;
	}
	void initial(unsigned int m, unsigned int k){
		if(k > 18){
			cerr << "the # of hash functions cannot exceed 17" << endl;
		}
		bf_m = m;
		bf_k = k;
		bf_n = 0;
		QuerymemAccNum=0;
		bf_base = new uchar [bf_m/8+1];
		memset(bf_base, 0, bf_m/8+1);
	}
	void reset(){
		memset(bf_base, 0, bf_m);
	}
	unsigned int insert(const unsigned char * str, unsigned int len){
		unsigned int value;
		for(uint i = 0; i < bf_k; i++){
			value = bf_hfp[i](str, len) % bf_m;
			bf_base[value/8] |= (128 >> (value%8));		//set the destination to be 1
		}
		bf_n ++;
		return 1;
	}
	unsigned int query(const unsigned char * str, unsigned int len){
		register unsigned int value;
		for(register uint i = 0; i < bf_k; i++){
			value = bf_hfp[i](str, len) % bf_m;
#ifdef STATISTIC
			QuerymemAccNum++;
#endif // STATISTIC
			if(0== (bf_base[value/8] & (128 >> (value%8))))return 0;	//return 0 when there is a 0
		}
		return 1;
	}

	uint Get_bf_m(){return bf_m;}
	uint Get_bf_k(){return bf_k;}
	uint Get_bf_n(){return bf_n;}

	uint QuerymemAccNum;
	
private:
	uchar * bf_base; //bloom filter base
	
	unsigned int bf_m; //bloom filter length
	unsigned int bf_k; //hash function numbers;
	unsigned int bf_n; //# of elements inserted
	
	//pointers to hash function
	unsigned int (*bf_hfp[18])(const unsigned char * str, unsigned int len);
};
#endif