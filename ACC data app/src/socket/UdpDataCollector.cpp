//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// This file contains the logic for when to start a udp connection and
// what to do with the gathered information.
//////////////////////////////////////////////////////////////////////////////
#include "socket/UdpDataCollector.h"

#include <cstdio>
#include <mutex>
#include <WinSock2.h>
#include <vector>

#include "socket/BroadcastingProtocol.h"
#include "sessionData.h"
#include "lapData.h"

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define IP "127.0.0.1"
#define PORT 9000

std::mutex mutex;

// should we create a new socket for every session?
// or should this be done only when the app starts and then use the same socket???
void UdpDataCollector(ui_data_pair& pair, std::vector<LapData>& laps, SessionData& session)
{
	// create a socket
	// initialize winsock, this needs error checking
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		fprintf(stderr, "error wsa startup %d", WSAGetLastError());
		return;
	}

	// create socket
	sockaddr_in server;
	const SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_socket == INVALID_SOCKET)
	{
		fprintf(stderr, "error creating socket %d", WSAGetLastError());
		return;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr(IP);

	BroadcastingProtocol bp;
	const ConnectionMessage cm;
	std::string msg;
	bp.construct_conn_msg(cm, msg);

	bool new_session = true;
	while (pair.session_active)
	{
		// when we detect active session send connection msg
		if (new_session)
		{
			if (sendto(client_socket, msg.c_str(), size(msg), 0, reinterpret_cast<sockaddr*>(&server), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				fprintf(stderr, "sendto() failed with error code: %d", WSAGetLastError());
			}
			new_session = false;
		}

		// gather data into lapdata and sessiondata
		char answer[1024];
		int slen = sizeof(sockaddr_in);
		recvfrom(client_socket, answer, 1024, 0, reinterpret_cast<sockaddr*>(&server), &slen);

		////
		// TEST
		////
		if (!pair.enable_test_console)
			pair.enable_test_console = true;

		if (mutex.try_lock())
		{
			std::string answer_msg = answer;
			pair.test_msg = "answer_msg";
			mutex.unlock();
		}


		bp.process_incoming_msg(answer);

		// session end somehow merge lapdata and sessiondata with
		// those from dataCollector
	}

	// dismiss socket and everything
	closesocket(client_socket);
	WSACleanup();
}
