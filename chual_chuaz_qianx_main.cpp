
#include <iostream>
#include <string>
#include <string.h>
#include "chual_chuaz_qianx_blow.h"
#include "chual_chuaz_qianx_socket.h"
#define PORTNO 9894

using namespace std;
//thing0: 10.35.195.46 //Alice
//thing1: 10.35.195.47 //KDC
//thing2: 10.35.195.22
//thing3: 10.35.195.49 //Bob
//thing4: 10.34.40.54


//plan a:
void encrypt_file(string k,string filename,blow fish, int socket){
	ifstream myfile;
	
	myfile.open(filename, ios::binary);
	
	unsigned char plain[1025];
	unsigned char cipher[1025];
	plain[1024]='\0';
	cipher[1024]='\0';
	int filesize= fish.getfilesize(filename);
	int remain=filesize%1024;
	
	//check how many rounds to send
	if(remain>0){
		filesize=filesize/1024+1;
	}else{
		filesize=filesize/1024;
	}
	
	int extra=0;
	//make sure the remain is a multiple of 8
	if(remain%8!=0){
		extra=8-(remain%8);
		remain+=extra;
	}
	
	//for dealing with leftover
	unsigned char plain2[remain+1];
	unsigned char cipher2[remain+1];
	plain2[remain]='\0';
	cipher2[remain]='\0';
	
	//put 0 to extra
	if(extra>0){
		for(int i=remain;i>=remain-extra;i--){
			plain2[i]='\0';
			cipher2[i]='\0';
		}
	}
	
	int ssize=0; //keep count of words sent
	CBlowFish* bf=fish.getFish(k);
	while(!myfile.eof()){
		if(filesize==1&&remain>0){
			myfile.read((char*)plain2,remain);
			ssize+=myfile.gcount();
			bf->Encrypt(plain2,cipher2,remain);
			writeInt(sizeof(cipher2)-1,socket);
			for(int i=0;i<sizeof(cipher2)-1;i++){
				writeUnsignedChar(cipher2[i],socket);
			}
		}else{
			myfile.read((char*)plain,1024);
			ssize+=myfile.gcount();
			bf->Encrypt(plain,cipher,1024);
			writeInt(sizeof(cipher)-1,socket);
			for(int i=0;i<sizeof(cipher)-1;i++){
				writeUnsignedChar(cipher[i],socket);
			}
		}
		filesize--;
	}
	cout<<"All data has been read and sent: "<<ssize <<" word."<<endl;
	myfile.close();

}	

void testStringEncrypt(string ks, blow fish,int socket){
	//First:this part try with small string
	string input;
	//prompt for input
	cout<<"\nEnter String S (any length):"<<endl;
	cin>>input;
	cout<<"\nS converted to hex:"<<endl;
	cout<<fish.string_to_hex(input)<<endl;
	unsigned char* temp=fish.encrypt(ks,input);
	cout<<"\nEncrypted(Eks[S]):"<<endl;
	cout<<fish.string_to_hex((char*)temp)<<endl;
	writeInt(strlen((char*)temp),socket);
	for(int i=0;i<strlen((char*)temp);i++){
		writeUnsignedChar(temp[i],socket);
	}
	cout<<"\nSend Eks[S] -> Bob"<<endl;
	
}

void testFileEncrypt(string ks, blow fish,int socket){
	string input;
	
	//prompt for input
	cout<<"\nEnter filename: ";
	cin>>input;
	
	//getfilesize and send to Bob
	int fsize=fish.getfilesize(input);
	if(fsize%1024>0){
		fsize=fsize/1024+1;
	}else{
		fsize=fsize/1024;
	}
	writeInt(fsize,socket);
		
	//encrypt the string with session key
	cout<<"\nEncrypting file:"<<endl;
	encrypt_file(ks,input,fish, socket);
	//send the encrypted string to bob
}

void testStringDecrypt(string ks, blow fish,int node){
	string plainstr="";
	cout<<"\nReceived Eks[S]:"<<endl;
	int length=readInt(node);
	unsigned char *temp=(unsigned char*) malloc (length);
	for(int i=0;i<length;i++){
		unsigned char uchar=readUnsignedChar(node);
		temp[i]=uchar;
	}
	string str((char*)temp);
	cout<<"\nPrint (Eks[S]): "<<endl;
	cout<<fish.string_to_hex(str)<<endl;
	//unsigned char plain[length+1];
	//plain[length]='\0';
	//CBlowFish* bf=fish.getFish(ks);
	//bf->Decrypt(temp,plain,length);
	temp=fish.decrypt(ks,temp);
	cout<<"\nDecrypt and converted to hex:"<<endl;
	cout<<fish.string_to_hex((char*)temp)<<endl;
	cout<<"\nPrint S:"<<endl;
	cout<<(char*)temp<<endl;
}

void testFileDecrypt(string ks, blow fish, int node){
	//Second: This part received Encrypted message from Alice
	ofstream myfile;
	myfile.open("plain2.txt",fstream::trunc);
	string plainstr="";
	cout<<"\nReceived encrypted file from Alice"<<endl;
	int fsize=readInt(node);
	int count=0;
	unsigned char* temp;
	while(fsize>0){
		int length=readInt(node);
		count+=length;
		temp=(unsigned char*) malloc (length);
		for(int i=0;i<length;i++){
			unsigned char uchar=readUnsignedChar(node);
			temp[i]=uchar;
		}
		unsigned char plain[length+1];
		plain[length]='\0';
		CBlowFish* bf=fish.getFish(ks);
		bf->Decrypt(temp,plain,length);
		string s((char*)plain);
		myfile << s;
		cout<<s<<endl;
		fsize--;
	}
	myfile.close();
	cout<<"plain length: "<<count<<endl;
}



int main(int argc, char *argv[]) {
	blow fish;
	string ks; //session key
	string ka; //alice's key
	string kb; //bob's key
	int count=0;//for counting message
	unsigned long int nonce; //this is to store nonce
	string peername; //to keep track received from who
	char* token; //for dealing with msg 
	unsigned char* temp; //for all ecrypted and decrypted msg
	stringstream st; 
	int length=0;//length of unsigned char*
	
	//first check if run correctly
	if(argc!=2){
		cout<<"To run: ./main [KDC|Bob|Alice]"<<endl;
		exit(0);
	}
	
	cout<<"Welcome: " << argv[1] << endl;
	
	//KDC
	if(strcmp(argv[1],"KDC")==0) {
	
		//prompt user to input the keys
		cout<<"KDC: Please input secret key (KS): "<<endl;
		cin>>ks;
		
		cout<<"KDC: Please input Alice's key (KA): "<<endl;
		cin>>ka;
		
		cout<<"KDC: Please input Bob's key (KB)"<<endl;
		cin>>kb;
		
		//set keys for blowfishes
		fish.setKeys(ka,kb,ks);

		cout<<endl;
		
		//connect
		int ss=setupServerSocket(PORTNO);
		int node=serverSocketAccept(ss);
		
		//receive message from Alice
		char rec[50];
		int n = read(node, rec,sizeof(rec));
		vector<char> vc_rec(rec,rec+sizeof(rec)); //store the message in vector for future use
		
		//To check received from who
		peername=getPeername(node);
		if(peername=="10.35.195.46"){
			cout<<"Received from thing1 (Alice): "<<endl;
		}else if(peername=="10.35.195.49"){
			cout<<"Received from thing2 (Bob): "<<endl;
		}else{
			cout<<"Received from "<<peername<<endl;
		}
		
		//deal with rev msg
		//msgArr[0]=KS
		//msgArr[1]=Bob
		//msgArr[2]=nonce
		char* msgArr[3];
		token=strtok(rec,"||");
		while (token != NULL){
			if(count>=3){
				cout<<"Something wrong with received message: "<<endl;
				cout<<vc_rec.data()<<endl;
				exit(0);
			}
			msgArr[count]=token;
			token = strtok (NULL, "||");
			count++;
		}
		
		if(strcmp(msgArr[0],"KS")==0){
			cout<<" Requesting KS for "<< msgArr[1]<<endl;
			cout<<" N1 received = "<<msgArr[2]<<"\n"<<endl;
		}else{
			cout<<"It is not requesting for session key: "<<endl;
			cout<<msgArr[0]<<endl;
			exit(0);
		}			
		
		string plain; //for storing msg
		
		//ks||ida
		plain.append(ks);
		plain.append("||");
		plain.append(peername);
		
		//encrypted (ks||ida) using kb
		temp=fish.encrypt(kb,plain);
		
		//combine ks, request, e1 and encrypt
		plain=ks+"||"+vc_rec.data()+"||";
		plain.append((char*)temp);
		temp=fish.encrypt(ka,plain);
		
		//send to Alice
		cout<<"Send encrypted message to Alice"<<endl;
		length=strlen((char*)temp);
		writeInt(length,node);
		for(int i=0;i<length;i++){
			writeUnsignedChar(temp[i],node);
		}
		cout<<"KS: "<<ks<< " and N1: "<<msgArr[2]<<endl;
		
		//GO TO ALICE
		
		//close the socket
		close(ss);
		
			
	} else if(strcmp(argv[1],"Alice")==0) {
		//prompt user to input alice's secret key and nonce
		cout<<"Alice: Please input Alice's key (KA): "<<endl;
		cin>>ka;
		
		cout<<"Alice: Please input Nonce (N1): "<<endl;
		cin>>nonce;
		
		cout<<endl;
		
		//set key to blowfish
		fish.setAKey(ka); 
		
		//connect
		char* hostname=(char*)"10.35.195.47"; //KDC
		char* host=const_cast<char*>(hostname);
		int socket=callServer(host, PORTNO);
		cout<< "Node successfully connected.\n"<<endl;
		
		//request message (KS for Bob) append with N1
		st<<"KS||Bob||"<<nonce;
		string s(st.str());
		char msg[s.size()+1];
		strcpy(msg,s.c_str());
		st.str("");
		
		send(socket, msg,sizeof(msg),0);
		cout<<"Sent KDC : \n Request: KS for Bob \n N1="<<nonce<<"\n"<<endl;
		//GO TO KDC
		
		//BACK FROM KDC
		//receive encrypted msg from KDC
		length=readInt(socket);
		temp=(unsigned char*) malloc (length);
		for(int i=0;i<length;i++){
			unsigned char uchar=readUnsignedChar(socket);
			temp[i]=uchar;
		}
		cout<<"Receive encrypted message from KDC"<<endl;
		
		//decrypt the message
		cout<<"Decrypt: "<<endl;
		temp=fish.decrypt(ka,temp);
		string vc((char*)temp);
		//seg fault if ka is different
		
		//deal with decrypted msg
		//0:Ks 1:KS 2:BOB 3:NONCE 4:EKB
		char* msgArr[5];
		token=strtok((char*)temp,"||");
		//cout<<ms<<endl;
		count=0;
		while(token!=NULL&&count<=3){
			
			msgArr[count]=token;
			
			token=strtok(NULL,"||");
			count++;
		}
		//cout<<count<<endl;
		int totalMsg=strlen(msgArr[0])+strlen(msgArr[1])+strlen(msgArr[2])+strlen(msgArr[3])+8;
		
		vc.erase(0,totalMsg);
	
		char msgArr4[vc.length()];
		strcpy(msgArr4,vc.c_str()); 

	
		//make sure the message is from KDC
		st<<nonce;
		if(strcmp(msgArr[3],st.str().c_str())!=0){
			cout<<"The nonce received is "<<msgArr[3]<<". Not from KDC."<<endl;
			exit(0);
		}
		st.str("");
		fish.setSKey(msgArr[0]); //set session key
		ks = msgArr[0];
		cout<<"KS: "<<msgArr[0]<<" N1: "<<nonce<<"\n"<<endl;
		
		//connect to Bob
		cout<<"Connecting to Bob...\n"<<endl;
		//connect to Bob
		hostname=(char*)"10.35.195.49"; //Bob
		host=const_cast<char*>(hostname);
		socket=callServer(host, PORTNO);
		cout<< "Node successfully connected.\n"<<endl;
		
		//send the encrypted msg to Bob //msgArr[4]
		temp = reinterpret_cast<unsigned char*>(msgArr4);  
		
		cout<<"Send encrypted message to Bob"<<endl;
		length=strlen((char*)temp);
		writeInt(length,socket);
		for(int i=0;i<length;i++){
			writeUnsignedChar(temp[i],socket);
		}
		cout<<(char*)temp<<"\n"<<endl;
		
		//GO TO BOB
		
		//receive encryted nonce from Bob
		//write code here:
		cout<<"Received encrypted nonce from Bob"<<endl;
		length=readInt(socket);
		temp=(unsigned char*) malloc (length);
		for(int i=0;i<length;i++){
			unsigned char uchar=readUnsignedChar(socket);
			temp[i]=uchar;
		}
		cout<<temp<<endl;
		
		//Decrypted the nonce
		temp=fish.decrypt(ks,temp);
		long num=atol((char*)temp);
		cout<<"Decrypted nonce is : "<<num<<endl;
		
		//use hash function to f(N2)
		long fCode = fish.f(num);
		
		//encrypt the f(N2) with session key
		st<<fCode;
		temp=fish.encrypt(ks,st.str());
		length=strlen((char*)temp);
		
		//send the encrypted f(N2) to bob
		cout<<"\nSent Bob the hash function"<<endl;
		writeInt(length,socket);
		for(int i=0;i<length;i++){
			writeUnsignedChar(temp[i],socket);
		}
		cout<<(char*)temp<<endl;
		int loop=1;
		while(loop==1){
			string sorf;
			cout<<"\nTest string(s), Test file(f), exit(e)"<<endl;
			cin>>sorf;
			if(sorf.compare("s")==0){
				//This part test encryption with string input
				writeInt(0, socket);
				testStringEncrypt(ks,fish,socket);
				loop=0;
			}else if(sorf.compare("f")==0){
				//This part test encryption with file input
				writeInt(1, socket);
				testFileEncrypt(ks,fish,socket);
				loop=0;
			}else if(sorf.compare("e")==0){
				writeInt(-1, socket);
				exit(0);
			}else{
				cout<<"Wrong input. Try again."<<endl;
			}
		}
	
		//close the socket
		close(socket);
		
	} else if(strcmp(argv[1],"Bob")==0) {
		//prompt user to input Bob's secret key and nonce
		cout<<"Bob: Please input Bob's key (KB): "<<endl;
		cin>>kb;
		
		cout<<"Bob: Please input Nonce (N2): "<<endl;
		cin>>nonce;
		
		cout<<endl;
		
		//set key to blowfish
		fish.setBKey(kb);
		
		int ss=setupServerSocket(PORTNO);
		int node=serverSocketAccept(ss);
		
		//receive encrypted msg from Alice
		length=readInt(node);
		temp=(unsigned char*) malloc (length);
		for(int i=0;i<length;i++){
			unsigned char uchar=readUnsignedChar(node);
			temp[i]=uchar;
		}
		cout<<"\nReceived from Alice"<<endl;
		cout<<(char*)temp<<endl;
		
		cout<<"\nDecrypt and "<<endl;
		temp=fish.decrypt(kb,(unsigned char*)temp);
		
		//0:Ks 1:nonce
		char* msgArr[2];
		token=strtok((char*)temp,"||");
		while(token!=NULL){
			if(count>=2){
				cout<<"Something wrong with encrypted message.";
				cout<<(char*)temp<<endl;
				exit(0);
			}
			msgArr[count]=token;
			token=strtok(NULL,"||");
			count++;
		}
		fish.setSKey(msgArr[0]); //set session key
		ks = msgArr[0];
		cout<<"KS: "<<msgArr[0]<<" N2: "<<nonce<<"\n"<<endl;
		
		//Encrypt nonce2 using session key
		st<<nonce;
		temp=fish.encrypt(ks,st.str()); 
		st.str("");
		
		//send encrypted nonce to alice
		length=strlen((char*)temp);
		cout<<"Sent to alice the encrypted nonce "<<endl;
		writeInt(length,node);
		for(int i=0;i<length;i++){
			writeUnsignedChar(temp[i],node);
		}
		cout<<(char*)temp<<endl;
	
		//receive the hash function from alice
		cout<<"\nReceived hash function from Alice"<<endl;
		length=readInt(node);
		temp=(unsigned char*) malloc (length);
		for(int i=0;i<length;i++){
			unsigned char uchar=readUnsignedChar(node);
			temp[i]=uchar;
		}
		cout<<(char*)temp<<"\n"<<endl;
		
		//decrypt the hash function
		temp=fish.decrypt(ks,(unsigned char*)temp);
		
		//calculate the hash function
		long fCode = fish.f(nonce);
		st<<fCode;
		string fun((char*) temp);
		cout<<"Check if it is from Alice!"<<endl;
		if(st.str()==fun){
			cout<<"Yes the f(N2) matched. It is from Alice. "<<endl;
		}else{
			cout<<"No the f(N2) is not matched. "<<endl;
			cout<<"Received from Alice: "<<fun<<endl;
			cout<<"Expected from Bob: "<<st.str()<<endl;
		//exit(0);
		}
		int r=readInt(node);
		if(r==0){
			//readString
			testStringDecrypt(ks,fish,node);
		}else if(r==1){
			//readFile
			testFileDecrypt(ks,fish,node);
		}else{
			cout<<"Alice did not send file."<<endl;
		}
	
		//close the socket
		close(ss);
	}
	return 0;
		
}
