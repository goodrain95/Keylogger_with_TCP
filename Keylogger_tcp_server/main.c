
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

//기본 버퍼 크기와 포트 정의
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"

int __cdecl main(void)
{
    WSADATA wsaData; // Winsock 데이터 구조체
    int iResult; // 함수 호출 결과 저장

    SOCKET ListenSocket = INVALID_SOCKET; // 서버의 리스닝 소켓
    SOCKET ClientSocket = INVALID_SOCKET; // 클라이언트 소켓

    struct addrinfo* result = NULL; // getaddrinfo 함수의 결과 저장
    struct addrinfo hints; // getaddrinfo 함수에 대한트

    int iSendResult; // 데이터 전송 결과를 수
    char recvbuf[DEFAULT_BUFLEN]; // 데이터 수신 버퍼
    int recvbuflen = DEFAULT_BUFLEN; // 버퍼기

    // Winsock 시작
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    //addrinfo 구조체를 0으로 초기화하고 TCP 소켓을 위한 설정을 함.
    ZeroMemory(&hints, sizeof(hints)); // hints 구조체를 0으로 초기화함. 
    hints.ai_family = AF_INET; // IPv4 주소 체계
    hints.ai_socktype = SOCK_STREAM; // 스트림 소켓
    hints.ai_protocol = IPPROTO_TCP; // TCP 프로토콜
    hints.ai_flags = AI_PASSIVE; // 서버 소켓 설정

    // 서버 주소와 포트 해석.
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) { // getaddrinfo: 주어진 호스트 이름을 해당하는 IP 주소로 변환함
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // 클라이언트 연결을 듣기 위한 소켓 생성.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // TCP 리스닝 소켓 설정.(bind)
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result); //getaddrinfo 함수에서 이 주소 정보에 대해 할당한 메모리를 해제. 더 이상 필요하지 않기 때문.

    // 소켓을 리스닝 상태로 만듦.(해당 IP 주소 및 포트에서 수신 연결 요청을 수신해야 함)
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 클라이언트 소켓을 수락함.
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("Server::Successful Connection!\nReceived: ");
    // 리스닝 소켓은 더이상 필요하지 않으므로 닫음.
    closesocket(ListenSocket);

    // 클라이언트가 연결을 종료할 때까지 데이터를 받음.
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("%s", recvbuf);

            // 받은 데이터를 클라이언트에게 다시 보냄.
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0) // 받은 바이트 수가 없음 == 연결 종료 ㄱㄱ
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // 연결을 종료함.
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("\nshutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // 리소스를 정리함.
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}