#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}


/*struct ClientInfo {
    SOCKADDR_IN clientaddr[8];
};*/
/*SOCKADDR_IN clientaddr[8];
int per_num = 0;
#define P_MAX 8;*/
std::vector<SOCKADDR_IN> v;

DWORD WINAPI ProcessClient(LPVOID arg) {
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    //클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

    while (1) {
        // 데이터 받기
        addrlen = sizeof(clientaddr);
        retval = recvfrom(client_sock, buf, BUFSIZE, 0,
            (SOCKADDR*)&clientaddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            continue;
        }

        if (retval == NULL || retval == 0) {
            break;
        }

        for (int i = 0; i < v.size(); i++) {
            if (v[i].sin_addr.s_addr == clientaddr.sin_addr.s_addr && v[i].sin_port == clientaddr.sin_port) {
                break;
            }
            else if (v[i].sin_port != clientaddr.sin_port && i == v.size() - 1) {
                v.push_back(clientaddr);
            }
        }

        // 받은 데이터 출력
        buf[retval] = '\0';
        printf("[UDP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
            ntohs(clientaddr.sin_port), buf);

        // 데이터 보내기
        for (int j = 0; j < v.size(); j++) {
            retval = sendto(client_sock, buf, retval, 0,
                (SOCKADDR*)&v[j], sizeof(v[j]));
            if (retval == SOCKET_ERROR) {
                err_display("sendto()");
                continue;
            }
        }
    }
    //closesocket()
    closesocket(client_sock);
    printf("[UDP 서버] 클라이언트 종료 : IP 주소 = %s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr),
        ntohs(clientaddr.sin_port));

    return 0;
}


int main(int argc, char *argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // 데이터 통신에 사용할 변수
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    HANDLE ThreadHandle = NULL;

    // 클라이언트와 데이터 통신
    while (1) {
        if (ThreadHandle == NULL) {
            // 데이터 받기
            addrlen = sizeof(clientaddr);
            retval = recvfrom(sock, buf, BUFSIZE, 0,
                (SOCKADDR*)&clientaddr, &addrlen);
            if (retval == SOCKET_ERROR) {
                err_display("recvfrom()");
                continue;
            }

            v.push_back(clientaddr);

            HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)sock, 0, NULL);
            ThreadHandle = hThread;
            if (hThread == NULL) { closesocket(sock); }
            else { CloseHandle(hThread); }
        }
        else {
            continue;
        }
    }

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
