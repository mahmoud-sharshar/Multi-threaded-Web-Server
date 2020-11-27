
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5100
#define MAXPENDING 5
#define RECVBUFSIZE 100

using namespace std;
void dieWithError(string errorMessage); //error handling function
void handleTCPClient(int clientSocket);

int main()
{
	// int sockid= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	// cout << sockid << endl;
	// int status= close(sockid);
	// cout << status << endl;
	int serverSocket;	//Socket descriptor for server
	int clientSocket;	//Socket descriptor for client

	struct sockaddr_in serverAddr;	//local address
	struct sockaddr_in clientAddr;	//client address

	unsigned short serverPort;	//server port
	unsigned int clientLen;			//length of client address data structure


	
	//create socket for incoming connections
	if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}
	
	//construct local address structure
	serverPort = PORT;	//server port
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind to the local address
	if( bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
		dieWithError("bind() failed");
	}

	//mark the socket so it will listen for incoming connections
	if(listen(serverSocket, MAXPENDING) < 0){
		dieWithError("listen() failed");
	}

	while(true){
		//set the size of the in-out parameter
		clientLen = sizeof(clientAddr);
		cout << "Ready to accept" << endl;
		//wait for client to connect
		if((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen)) < 0){
			dieWithError("accept() failed");
		}
		cout << "finish accept" << endl;
		cout << clientSocket << endl;
		// close(clientSocket);
		//clientSocket is connected to a client
		printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
		handleTCPClient(clientSocket);
	}
}

void dieWithError(string errorMessage)
{
	// fprintf(stderr, "%s\n", errorMessage);
	cout << errorMessage;
	exit(1);
}

void handleTCPClient(int clientSocket)
{
	char echoBuffer[RECVBUFSIZE];
	int msgSize;
	// char dle_etx[] = "\x10\x03";
	int sent;
	//recieve message from client
	int count = 0;

	// while(1)
	// {
				//see if there is more data to recieve
		if((msgSize = recv(clientSocket, echoBuffer, RECVBUFSIZE, 0)) < 0){
			dieWithError("recv() failed");
		}
		echoBuffer[msgSize] = '\0';
		printf("recved %s\n", echoBuffer);



	// 	if((sent = send(clientSocket, echoBuffer, msgSize, 0) )!= msgSize){
	// 		printf("ms = %d, but sent %d\n",msgSize, sent );
	// 		dieWithError("send() failed");
	// 	}

	// 	if(msgSize ==  0){
	// 		break;
	// 	}
	// 	if(strcmp(dle_etx, echoBuffer) == 0){
	// 		break;
	// 	}
	// 	count++;

	// }
	// printf("EnhancedServer echoed %d strings.\n",count);
	
	//recieve last packet to finish
	// recv(clientSocket, echoBuffer, RECVBUFSIZE, 0);


	close(clientSocket);
	// }

}
