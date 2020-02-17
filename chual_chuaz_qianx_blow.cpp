
#include "chual_chuaz_qianx_blow.h"
//setkeys // used in kdc
void blow::setKeys(string a, string b, string s){
	unsigned char* uc;
	uc=(unsigned char*)a.c_str();
	bfa=new CBlowFish(uc,strlen((char*)uc));
	uc=(unsigned char*)b.c_str();
	bfb=new CBlowFish(uc,strlen((char*)uc));
	uc=(unsigned char*)s.c_str();
	bfs=new CBlowFish(uc,strlen((char*)uc));
	ka=a;
	kb=b;
	ks=s;
}
//setkey // for Alice and Bob
void blow::setAKey(string a){
	
	unsigned char* uc=(unsigned char*)a.c_str();
	bfa=new CBlowFish(uc, strlen((char*)uc));
	ka=a;
}
void blow::setBKey(string b){
	unsigned char* uc=(unsigned char*)b.c_str();
	bfb=new CBlowFish(uc,strlen((char*)uc));
	kb=b;
}
void blow::setSKey(string s){
	unsigned char* uc=(unsigned char*)s.c_str();
	bfs=new CBlowFish(uc,strlen((char*)uc));
	ks=s;
}

//to get blowfish, used in encrypt and decrypt file
CBlowFish* blow::getFish(string k){
	if(k.compare(ka)==0){
		return bfa;
	}else if(k.compare(kb)==0){
		return bfb;
	}else if(k.compare(ks)==0){
		return bfs;
	}else{
		cout<<"e: wrong key entered"<<endl;
		exit(0);
	}
}

//encrypt
unsigned char* blow::encrypt(string k, string p){
	int length=p.size()%8;
	if(length!=0){
		length=p.size()+(NUM-length);
	} else length = p.size();
	
	unsigned char plain[length];
	strcpy((char*) plain, p.c_str());

	unsigned char* cipher=(unsigned char*) malloc (length);
	if(k.compare(ka)==0){
		bfa->Encrypt(plain,cipher,length);
	}else if(k.compare(kb)==0){
		bfb->Encrypt(plain,cipher,length);
	}else if(k.compare(ks)==0){
		bfs->Encrypt(plain,cipher,length);
	}else{
		cout<<"e: wrong key entered"<<endl;
	}
	return cipher;
}


//decrypt
unsigned char* blow::decrypt(string k, unsigned char* cipher){

	int length;
	int length2 =strlen((char*)cipher);
	int remainder=length2%8;
	
	if(remainder!=0){
		length=length2+(NUM-remainder);
	} else length = length2;
	
	unsigned char* plain=(unsigned char*) malloc (length*2);
	
	if(k.compare(ka)==0){
		bfa->Decrypt(cipher,plain,length);
	}else if(k.compare(kb)==0){
		bfb->Decrypt(cipher,plain,length);
	}else if(k.compare(ks)==0){
		bfs->Decrypt(cipher,plain,length);
	}else{
		cout<<"d: wrong key entered"<<endl;
	}
	
	return plain;
	
}

//nonce function
long blow::f(long nonce) {
    const long A = 48271;
    const long M = 2147483647;
    const long Q = M/A;
    const long R = M%A;

	static long state = 1;
	long t = A * (state % Q) - R * (state / Q);
	
	if (t > 0)
		state = t;
	else
		state = t + M;
	return (long)(((double) state/M)* nonce);
}

//string to hex function
string blow::string_to_hex(const string& input) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	
	string output;
	output.reserve(2*len);
	for(size_t i=0; i<len; i++) {
		const unsigned char c = input[i];
		output.push_back(lut[c>>4]);
		output.push_back(lut[c&15]);
	}
	return output;
}

//to get file size
int blow::getfilesize(string filename){
	streampos begin,end;
	ifstream myfile (filename, ios::binary);
	begin = myfile.tellg();
	myfile.seekg (0, ios::end);
	end = myfile.tellg();
	return (end-begin);
}

void blow::String_to_file(string s,string filename){
  ofstream myfile;
  myfile.open(filename,fstream::app);
  if(myfile << s <<" "){
	  cout<<"Write to file success."<<endl;
  }else{
	  cout<<"Too bad...write to file failed."<<endl;
  }
  myfile.close();
}
