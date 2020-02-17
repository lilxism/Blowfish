#include "chual_chuaz_qianx_socket.h"

int setupServerSocket(int portno){
	//Socket pointer
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		fprintf(stderr,"ERROR opening socket\n");
		exit(0);
	}
  
	//server address structure
	struct sockaddr_in serv_addr;

	//set all values in the server address to 0
	memset(&serv_addr,'0',sizeof(serv_addr));

	// Setup the type of socket (internet vs filesystem)
	serv_addr.sin_family = AF_INET;

	// Basically the machine we are on...
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Setup the port number
	// htons - is host to network byte order
	// network byte order is most sig bype first
	//   which might be host or might not be
	serv_addr.sin_port = htons(portno);

	// Bind the socket to the given port
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout<<"ERROR on binding"<<endl;
		exit(1);
	}

	// set it up to listen
	listen(sockfd,5);
	return sockfd;
}

int callServer(char* host, int portno){
	// Socket pointer
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr,"ERROR opening socket\n");
		exit(0);
	}

	// server address structure
	struct sockaddr_in serv_addr;

	// Set all the values in the server address to 0
	memset(&serv_addr, '0', sizeof(serv_addr)); 

	// Setup the type of socket (internet vs filesystem)
	serv_addr.sin_family = AF_INET;

	// Setup the port number
	// htons - is host to network byte order
	// network byte order is most sig byte first
	//   which might be host or might not be
	serv_addr.sin_port = htons(portno);


	// Setup the server host address
	struct hostent *server;
	//  server = gethostbyname("thing2.cs.uwec.edu");
	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);  /// dest, src, size

	// Connect to the server
	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout<<"ERROR connecting"<<endl;
		exit(0);
	}
	return sockfd;
}

int serverSocketAccept(int serverSocket){
	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	// Wait for a call
	printf("waiting for a call...\n");
	newsockfd = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
	cout<<"connected\n"<<endl;
	if (newsockfd < 0) {
		cout<<"ERROR on accept";
		exit(1);
	}
	return newsockfd;
}

//to read int
int readInt(int socket){
	int y=0;
	int n = read(socket, &y,sizeof(y));
	if (n < 0) {
		cout<<"ERROR reading from socket"<<endl;
		exit(0);
	}
	return y;
}

//to write int
void writeInt( int x, int socket){
	int n = write(socket,&x,sizeof(x));
	if (n < 0) {
		cout<<"ERROR writing to socket"<<endl;
		exit(0);
	}
  
}

unsigned char readUnsignedChar(int socket){
	unsigned char y=0;
	int n = read(socket, &y,sizeof(y));
	if (n < 0) {
		cout<<"ERROR reading from socket"<<endl;
		exit(0);
	}
	return y;
}

void writeUnsignedChar( unsigned char x, int socket){
	int n = write(socket,&x,sizeof(x));
  
	if (n < 0) {
		cout<<"ERROR writing to socket"<<endl;
		exit(0);
	}
}


string getPeername(int node){
	struct sockaddr_in peer;
	socklen_t len;
	len=sizeof(peer);
	getpeername(node,(struct sockaddr*)&peer,&len);
	string peername=inet_ntoa(peer.sin_addr);
	return peername;
}
