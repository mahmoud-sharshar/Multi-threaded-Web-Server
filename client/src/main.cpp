
#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT 5100
using namespace std;
void dieWithError(string errorMessage); //error handling function

int main()
{
	int sock;
	struct sockaddr_in serverAddr;	
	unsigned short serverPort = SERVER_PORT;
	//create a reliable stream socket using TCP
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//establish connection to the server
	if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		dieWithError("connect() failed");
	}
	char* message = "Hello world!";
	int msgLen = strlen(message);
	if(send(sock , message , msgLen , 0 ) != msgLen){
		dieWithError("send() sent a different number of bytes than expected");
	}
	cout << "connection successed";
	close(sock);
	return 0;
}

void dieWithError(string errorMessage)
{
	// fprintf(stderr, "%s\n", errorMessage);
	cout << errorMessage;
	exit(1);
}