// ClientServerProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{
	//Initializing Winsocket
	//Creating WSA Structure
	WSADATA wsData;
	//Requesting version 2
	WORD ver = MAKEWORD(2, 2);
	//Specify version you want, identify pointer to the data by giving the address of the data
	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) {
		cerr << "ERROR: Cannot initialize Winsock. Exiting Program..." << endl;
		return;
	}

	//Creating socket
	//TCP Socket 
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	//Test listening socket
	if (listening == INVALID_SOCKET) {
		cerr << "ERROR: Cannot create a socket. Exiting Program..." << endl;
		return;
	}

	//Bind ip address and port to a socket
	//Fill out structure
	sockaddr_in hint;
	//Version 4
	hint.sin_family = AF_INET;
	//Networking is big endian, PC is little endian
	//In order to combat that there's a series of functions that are used
	hint.sin_port = htons(54000); //port 5400 host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	//Bind port to socket
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	//Now socket can be used to reference socket values

	//Tell Winsock socket its ready for listening
	//15 max connections
	listen(listening, SOMAXCONN);
	//Wait for an inbound connection
	sockaddr_in client;
	int clientsize = sizeof(client);
	//Fill in information about client
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "ERROR: Cannot create a client socket. Exiting Program..." << endl;
		return;
	}
	char host[NI_MAXHOST]; //clients remote name
	char service[NI_MAXHOST]; //port client is on
	ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << "connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port " << ntohs(client.sin_port) << endl;
	}

	//If you do not close it you can accept multiple clients
	closesocket(listening);

	char buf[4096];
	while (true) {
		ZeroMemory(buf, 4096);
		//wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "ERROR: Error in recv(). Exiting Program..." << endl;
		}
		if(bytesReceived == 0){
			cout << "Client disconnected " << endl;
			break;
		}

		//echo message and echo message back to client
		cout << string(buf, 0, bytesReceived) << endl;
		send(clientSocket, buf, bytesReceived + 1, 0);

	}

	//close socket and clean up winsock
	closesocket(clientSocket);
	WSACleanup();
}
