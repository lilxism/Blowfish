#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <math.h>
#include <arpa/inet.h>
using namespace std;

int setupServerSocket(int portno); //server
int callServer(char* host, int portno); //client
int serverSocketAccept(int serverSocket); //server
void writeInt(int x, int socket); 
int readInt(int socket);

void writeUnsignedChar(unsigned char x, int socket);
unsigned char readUnsignedChar(int socket);

string getPeername(int node);