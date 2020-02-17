
#ifndef __CHUAL_CHUAZ_QIANX_BLOW_H__
#define __CHUAL_CHUAZ_QIANX_BLOW_H__
#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include "chual_chuaz_qianx_BlowFish.h"

using namespace std;

#define NUM 8
class blow{
	private:
	CBlowFish * bfa;
	CBlowFish* bfb;
	CBlowFish* bfs;
	string ka;
	string ks;
	string kb;
	
	public:
	blow(){}
	void setKeys(string a, string b, string s);
	void setAKey(string a);
	void setBKey(string b);
	void setSKey(string s);
	
	//to get blowfish
	CBlowFish* getFish(string k);
	
	//string encrpt to unsigned char*
	unsigned char* encrypt(string k, string p);
	
	//unsigned char* decrypt to unsigned char*
	unsigned char* decrypt(string k, unsigned char* cipher);
	
	//nonce function from Dr. Tan
	long f(long nonce);
	
	//string to hex string function
	string string_to_hex(const string& input);
	
	//to get file size(bytes)
	int getfilesize(string filename);
	
	void String_to_file(string s, string filename);
	//unsigned char* decrypt_file(string k,string filename,unsigned char* cipher);
	
};
#endif