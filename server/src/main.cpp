
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 5100
#define MAXPENDING 5
#define BUFF_SIZE 10000

using namespace std;

struct ClientRequest
{
	string method;
	string filePath;
	string protocol;
	vector<string> data;
};

void dieWithError(string errorMessage); //error handling function
void handleTCPClient(int clientSocket);
ClientRequest parseRequestLine(string line);
ClientRequest parseRequestMessage(string message);
string buildResponseMessage(string status_code, string status_response, string filePathData = "");
string handleGetRequest(ClientRequest request);
string handlePostRequest(ClientRequest request);
void writeDataToFile(vector<string> data, string filePath);

void error_msg(const char *msg, bool halt_flag)
{
	perror(msg);
	if (halt_flag)
		exit(-1);
}

void *handleTCPClient(void *client_ptr)
{
	pthread_detach(pthread_self()); /* terminates on return */
	// cout << "starting new thread" << endl;
	/* read/write socket */
	int client = *((int *)client_ptr);

	/* request */
	char buffer[BUFF_SIZE + 1];
	bzero(buffer, sizeof(buffer));
	int bytes_read = recv(client, buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
		error_msg("Problem with recv call", false);

	ClientRequest request = parseRequestMessage(buffer);
	string response;
	if (request.method == "GET")
	{
		response = handleGetRequest(request);
	}
	else if (request.method == "POST")
	{
		response = handlePostRequest(request);
	}
	/* response */
	// bzero(response, sizeof(response));
	// generate_echo_response(buffer, response);
	int bytes_written = send(client, response.c_str(), response.size(), 0);
	if (bytes_written < 0)
		error_msg("Problem with send call", false);

	close(client);

	return NULL;
} /* detached thread terminates on return */

int main(int argc, char* argv[])
{
	 // Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        cerr << "Usage: " << argv[0] << " port-number" << endl;
        return 1;
    }

	int serverSocket; //Socket descriptor for server
	int clientSocket; //Socket descriptor for client

	struct sockaddr_in serverAddr; //local address
	struct sockaddr_in clientAddr; //client address

	unsigned short serverPort = stoi(argv[1]); //server port
	unsigned int clientLen;	   //length of client address data structure

	//create socket for incoming connections
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		dieWithError("socket() failed");
	}

	//construct local address structure
	// serverPort = PORT; //server port
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind to the local address
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		dieWithError("bind() failed");
	}

	//mark the socket so it will listen for incoming connections
	if (listen(serverSocket, MAXPENDING) < 0)
	{
		dieWithError("listen() failed");
	}

	cout << argv[0] << " is listening on port " << argv[1] << "!" << endl;

	while (true)
	{
		//set the size of the in-out parameter
		clientLen = sizeof(clientAddr);
		cout << "Ready to accept" << endl;
		//wait for client to connect
		if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen)) < 0)
		{
			dieWithError("accept() failed");
		}
		//clientSocket is connected to a client
		cout << "----------------------------" << endl;
		printf("Accepting connection from client %s with socket %d\n", inet_ntoa(clientAddr.sin_addr), clientSocket);
		cout << "----------------------------" << endl;
		pthread_t tid;
		int flag = pthread_create(&tid,			   /* id */
								  NULL,			   /* attributes */
								  handleTCPClient, /* routine */
								  &clientSocket);  /* routine's arg */
		if (flag < 0)
			error_msg("Problem creating thread", false);
	}
}

bool isFileExists(string filePath)
{
	ifstream f(filePath);
	bool status = f.good();
	f.close();
	return status;
}

string handleGetRequest(ClientRequest request)
{
	string response;
	// cout << "file Path: " << request.filePath << endl;
	string filePath = "../store" + request.filePath;
	if (isFileExists(filePath))
	{
		response = buildResponseMessage("200", "OK", filePath);
	}
	else
	{
		response = buildResponseMessage("404", "Not Found");
	}
	return response;
}

string handlePostRequest(ClientRequest request)
{
	string response;
	writeDataToFile(request.data, "../store" + request.filePath);
	response = buildResponseMessage("200", "OK");
	return response;
}

void writeDataToFile(vector<string> data, string filePath)
{
	ofstream file(filePath);

	// Write to the file
	for (auto line : data)
	{
		file << line;
		file << "\n";
	}
	// Close the file
	file.close();
}

ClientRequest parseRequestMessage(string message)
{
	cout << "##########################################" << endl;
	cout << "New Request comming!" << endl;
	cout << "----------------------------" << endl;
	cout << message;
	cout << "##########################################" << endl;
	istringstream f(message);
	string line;
	getline(f, line);
	ClientRequest newRequest = parseRequestLine(line);
	bool headerFinished = false;
	while (getline(f, line))
	{
		if (headerFinished)
			newRequest.data.push_back(line);

		if (line.empty())
		{
			headerFinished = true;
		}
	}
	return newRequest;
}

string buildResponseMessage(string status_code, string status_response, string filePathData)
{
	string response = "HTTP/1.1 " + status_code + ' ' + status_response + "\n\n";
	if (filePathData != "")
	{
		string line;
		ifstream dataFile(filePathData);

		while (getline(dataFile, line))
		{
			response += (line + "\n");
		}
		dataFile.close();
	}
	cout << "##########################################" << endl;
	cout << "Response message: \n";
	cout << "----------------------------" << endl;
	cout << response;
	cout << "##########################################" << endl;
	return response;
}

ClientRequest parseRequestLine(string line)
{
	// cout << "Request Line: " << line << endl;
	istringstream R(line);
	string part;
	vector<string> requestParts;
	while (getline(R, part, ' '))
	{
		requestParts.push_back(part);
		// cout << part << endl;
	}
	ClientRequest newRequest;
	if (requestParts.size() > 2)
	{
		newRequest.method = requestParts[0];
		newRequest.filePath = requestParts[1];
		newRequest.protocol = requestParts[2];
	}
	return newRequest;
}

void dieWithError(string errorMessage)
{
	cout << errorMessage << endl;
	exit(1);
}