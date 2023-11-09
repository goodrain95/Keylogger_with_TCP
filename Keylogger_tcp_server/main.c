
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

//�⺻ ���� ũ��� ��Ʈ ����
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"

int __cdecl main(void)
{
    WSADATA wsaData; // Winsock ������ ����ü
    int iResult; // �Լ� ȣ�� ��� ����

    SOCKET ListenSocket = INVALID_SOCKET; // ������ ������ ����
    SOCKET ClientSocket = INVALID_SOCKET; // Ŭ���̾�Ʈ ����

    struct addrinfo* result = NULL; // getaddrinfo �Լ��� ��� ����
    struct addrinfo hints; // getaddrinfo �Լ��� ����Ʈ

    int iSendResult; // ������ ���� ����� ��
    char recvbuf[DEFAULT_BUFLEN]; // ������ ���� ����
    int recvbuflen = DEFAULT_BUFLEN; // ���۱�

    // Winsock ����
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    //addrinfo ����ü�� 0���� �ʱ�ȭ�ϰ� TCP ������ ���� ������ ��.
    ZeroMemory(&hints, sizeof(hints)); // hints ����ü�� 0���� �ʱ�ȭ��. 
    hints.ai_family = AF_INET; // IPv4 �ּ� ü��
    hints.ai_socktype = SOCK_STREAM; // ��Ʈ�� ����
    hints.ai_protocol = IPPROTO_TCP; // TCP ��������
    hints.ai_flags = AI_PASSIVE; // ���� ���� ����

    // ���� �ּҿ� ��Ʈ �ؼ�.
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) { // getaddrinfo: �־��� ȣ��Ʈ �̸��� �ش��ϴ� IP �ּҷ� ��ȯ��
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Ŭ���̾�Ʈ ������ ��� ���� ���� ����.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // TCP ������ ���� ����.(bind)
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result); //getaddrinfo �Լ����� �� �ּ� ������ ���� �Ҵ��� �޸𸮸� ����. �� �̻� �ʿ����� �ʱ� ����.

    // ������ ������ ���·� ����.(�ش� IP �ּ� �� ��Ʈ���� ���� ���� ��û�� �����ؾ� ��)
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Ŭ���̾�Ʈ ������ ������.
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("Server::Successful Connection!\nReceived: ");
    // ������ ������ ���̻� �ʿ����� �����Ƿ� ����.
    closesocket(ListenSocket);

    // Ŭ���̾�Ʈ�� ������ ������ ������ �����͸� ����.
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("%s", recvbuf);

            // ���� �����͸� Ŭ���̾�Ʈ���� �ٽ� ����.
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0) // ���� ����Ʈ ���� ���� == ���� ���� ����
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // ������ ������.
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("\nshutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // ���ҽ��� ������.
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}