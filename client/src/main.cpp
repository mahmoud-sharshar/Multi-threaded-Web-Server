
#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5100
#define GET "GET"
#define POST "POST"
#define BUFF_SIZE 10000

using namespace std;

struct Request
{
	string method;
	string URI;
	string hostName;
	int port = 80;
};

void dieWithError(string errorMessage); //error handling function
Request parseCommand(string command);
vector<Request> parseCommandsFile(string file_path);
string buildRequestMessage(Request request);

void error_msg(const char *msg, bool halt_flag)
{
	perror(msg);
	if (halt_flag)
		exit(-1);
}

int main(int argc, char *argv[])
{
	// Check the number of parameters
	if (argc < 3)
	{
		// Tell the user how to run the program
		cerr << "Usage: " << argv[0] << " server_name port_number" << endl;
		return 1;
	}

	int sock;
	struct sockaddr_in serverAddr;
	unsigned short serverPort = stoi(argv[2]);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	// serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_addr.s_addr = *((unsigned long *)gethostbyname(argv[1])->h_addr_list[0]);

	vector<Request> requests = parseCommandsFile("../store/commands.txt");

	for (int i = 0; i < requests.size(); ++i)
	{
		//create a reliable stream socket using TCP
		if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		{
			dieWithError("socket() failed");
		}

		if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		{
			dieWithError("connect() failed");
		}
		string requestMsg = buildRequestMessage(requests[i]);
		char *message = new char[requestMsg.size() + 1];
		strcpy(message, requestMsg.c_str());
		int msgLen = strlen(message);
		if (send(sock, message, msgLen, 0) != msgLen)
		{
			dieWithError("send() sent a different number of bytes than expected");
		}
		char buffer[BUFF_SIZE + 1];
		bzero(buffer, sizeof(buffer));
		int bytes_read = recv(sock, buffer, sizeof(buffer), 0);
		if (bytes_read < 0)
			error_msg("Problem with recv call", false);
		cout << "##########################################" << endl;
		cout << "Response : \n";
		cout << "----------------" << endl;
		cout << buffer;
		cout << "##########################################" << endl;
		// cout << "connection successed\n";
		close(sock);
	}
	return 0;
}

vector<Request> parseCommandsFile(string file_path)
{
	string command;
	vector<Request> requests;
	// Read from the text file
	ifstream commandsFile(file_path);

	// Use a while loop together with the getline() function to read the file line by line
	while (getline(commandsFile, command))
	{
		// Output the text from the file
		requests.push_back(parseCommand(command));
	}
	return requests;
}

Request parseCommand(string command)
{
	istringstream ss(command);
	vector<string> commandParts;
	string buf;
	while (ss >> buf)
		commandParts.push_back(buf);
	Request new_request;
	if (commandParts.size() > 2)
	{
		if (commandParts[0] == "client_get")
		{
			new_request.method = GET;
		}
		else if (commandParts[0] == "client_post")
		{
			new_request.method = POST;
		}
		new_request.URI = commandParts[1];
		new_request.hostName = commandParts[2];
		if (commandParts.size() > 3)
		{
			new_request.port = stoi(commandParts[3]);
		}
	}
	return new_request;
}

// build HTTP request message
string buildRequestMessage(Request request)
{
	string message = "";
	if (request.method == GET)
	{
		message += GET;
		message += (' ' + request.URI + " HTTP/1.1\n"); // request header
		message += "\n";
	}
	else if (request.method == POST)
	{
		message += POST;
		message += (' ' + request.URI + " HTTP/1.1\n"); // request header
		message += "\n";
		ifstream file("../store" + request.URI);
		string line;
		while (getline(file, line))
		{
			message += line;
			message += "\n";
		}
	}
	cout << "##########################################" << endl;
	cout << "Sending new Request message: \n";
	cout << "---------------------------" << endl;
	cout << message;
	cout << "##########################################" << endl;
	return message;
}

void dieWithError(string errorMessage)
{
	// fprintf(stderr, "%s\n", errorMessage);
	cout << errorMessage << endl;
	exit(1);
}