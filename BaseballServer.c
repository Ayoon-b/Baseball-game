#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

#define MAX_CLNT 100
#define PORT 50000

int main() {
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKET sockets[MAX_CLNT];
	SOCKADDR_IN servAddr, clntAddr;
	WSAEVENT events[MAX_CLNT];
	WSANETWORKEVENTS netEvent;
	int numOfClnt = 0, pos, i, j, addrlen;
	char buf[100];
	int len;
	char answers[MAX_CLNT][5] = { 0, };
	int strike, ball , out;

	srand(time(0));
	
	char fromClientNumber[5] = {0,};
	int toClientScore[3];

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	servSock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servAddr, 0, sizeof(SOCKADDR_IN));
	servAddr.sin_port = htons(PORT);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(servSock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR));

	listen(servSock, 5);

	events[numOfClnt] = WSACreateEvent();
	sockets[numOfClnt] = servSock;

	WSAEventSelect(servSock, events[numOfClnt], FD_ACCEPT);
	numOfClnt++;

	while (1) {
		pos = WSAWaitForMultipleEvents(numOfClnt, events, FALSE, INFINITE, FALSE) - WSA_WAIT_EVENT_0;

		for (i = pos; i < numOfClnt; i++) {
			pos = WSAWaitForMultipleEvents(1, sockets + i, TRUE, 0, FALSE) - WSA_WAIT_EVENT_0;
			WSAEnumNetworkEvents(sockets[i], events[i], &netEvent);

			if (netEvent.lNetworkEvents & FD_ACCEPT) {
				addrlen = sizeof(SOCKADDR_IN);
				clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &addrlen);
				sockets[numOfClnt] = clntSock;
				events[numOfClnt] = WSACreateEvent();
				WSAEventSelect(clntSock, events[numOfClnt], FD_READ | FD_CLOSE);
				printf("client %d 접속\n", numOfClnt);
				answers[numOfClnt][0] = rand() % 9 + 1;

				while (1) {
					answers[numOfClnt][1] = rand() % 9 + 1;
					if (answers[numOfClnt][0] != answers[numOfClnt][1]) break;
				}
				while (1) {
					answers[numOfClnt][2] = rand() % 9 + 1;
					if (answers[numOfClnt][0] != answers[numOfClnt][2] && answers[numOfClnt][1] != answers[numOfClnt][2]) break;
				}
				while (1) {
					answers[numOfClnt][3] = rand() % 9 + 1;
					if (answers[numOfClnt][0] != answers[numOfClnt][3] && answers[numOfClnt][1] != answers[numOfClnt][3] && answers[numOfClnt][2] != answers[numOfClnt][3]) break;
				}

				answers[numOfClnt][0] += '0';
				answers[numOfClnt][1] += '0';
				answers[numOfClnt][2] += '0';
				answers[numOfClnt][3] += '0';
				printf("정답: %c %c %c %c\n\n", answers[numOfClnt][0], answers[numOfClnt][1], answers[numOfClnt][2], answers[numOfClnt][3]);
				numOfClnt++;
			}

			if (netEvent.lNetworkEvents & FD_READ) {
				strike = 0, ball = 0, out = 0;
				recv(sockets[i], (char*)fromClientNumber, sizeof(fromClientNumber), 0);
				printf("\nclient %d",i);
				printf("\n입력 받은 숫자: %c %c %c %c \n", fromClientNumber[0], fromClientNumber[1], fromClientNumber[2],fromClientNumber[3]);
				
				if (fromClientNumber[0] != answers[i][0]) { 
					if (fromClientNumber[0] == answers[i][1] || fromClientNumber[0] == answers[i][2] || fromClientNumber[0] == answers[i][3]) ball++;
					else out++;
				}
				else
					strike++;


				if (fromClientNumber[1] != answers[i][1]) {
					if (fromClientNumber[1] == answers[i][0] || fromClientNumber[1] == answers[i][2] || fromClientNumber[1] == answers[i][3]) ball++;
					else out++;
				}
				else
					strike++;


				if (fromClientNumber[2] != answers[i][2]) {
					if (fromClientNumber[2] == answers[i][1] || fromClientNumber[2] == answers[i][0] || fromClientNumber[2] == answers[i][3]) ball++;
					else out++;
				}
				else
					strike++;


				if (fromClientNumber[3] != answers[i][3]) {
					if (fromClientNumber[3] == answers[i][1] || fromClientNumber[3] == answers[i][0] || fromClientNumber[3] == answers[i][2]) ball++;
					else out++;
				}
				else
					strike++;

				printf("%d strike %d ball %d out \n", strike, ball, out);
				printf("\n------------------------------------------------------------------------------------------------\n");

				toClientScore[0] = htonl(strike);
				toClientScore[1] = htonl(ball);
				toClientScore[2] = htonl(out);

				send(sockets[i], (char*)toClientScore, sizeof(toClientScore), 0);

				if (strike == 4) {
					printf("\n정답을 맞혔습니다.\n");
					break;
				}
				
			}
			if (netEvent.lNetworkEvents & FD_CLOSE) {
				printf("client %d 종료\n", i);
				printf("\n------------------------------------------------------------------------------------------------\n");
				numOfClnt--;
				for (j = i; j < numOfClnt; j++) {
					sockets[j] = sockets[j + 1];
					events[j] = events[j + 1];
				}
			}
		}

	}
	closesocket(servSock);
	WSACleanup();
	return 0;
}
