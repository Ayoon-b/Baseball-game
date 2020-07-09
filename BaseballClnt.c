#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	int strLen;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	hSocket == INVALID_SOCKET;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(50000);

	connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));

	printf("숫자 야구 게임을 시작합니다\n");

	while (1) {
		char clientNumber[5] = { 0, };
		printf("\n------------------------------------------------------------------------------------------------\n");
		printf("\n숫자를 입력하세요(ex>1 5 4 8) : "); 
		scanf(" %c %c %c %c", &clientNumber[0], &clientNumber[1], &clientNumber[2], &clientNumber[3]);
				
		send(hSocket, (char*)clientNumber, sizeof(clientNumber), 0);

		int score[3];

		recv(hSocket, (char *)score, sizeof(score), 0);
		int strike = ntohl(score[0]);
		int ball = ntohl(score[1]);
		int out = ntohl(score[2]);

		printf("%d strike %d ball %d out\n", strike, ball, out);
		if (strike == 4) {
			printf("\n------------------------------------------------------------------------------------------------\n");
			printf("정답입니다. 숫자 야구 게임을 종료합니다.\n\n");
			return 0;
		}
	}

	// 소켓 종료
	closesocket(hSocket);
	WSACleanup();
	getchar();
	return 0;
}
