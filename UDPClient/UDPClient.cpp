/*
§ Civil(11) Team Internet-Programming-Project §
Team member Info
-----------------
- 연정흠
- 신용훈
- 김은빈

기능 구현이나 업데이트 사항이 있으면 UDPServer.cpp UDPClient.cpp 파일만 카카오톡으로 보내주시면 됩니다.
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <string>

//#define SERVERIP "59.31.105.35"
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512
//CRITICAL_SECTION cs;

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

DWORD WINAPI ClientRecv(LPVOID arg) {
    // 데이터 받기
    SOCKET tmp_sock = (SOCKET) arg;
    int addrlen;
    SOCKADDR_IN peeraddr;
    int retval;
    char buf[BUFSIZE + 1];
    SOCKADDR_IN serveraddr;

    while (1) {
        addrlen = sizeof(peeraddr);
        retval = recvfrom(tmp_sock, buf, BUFSIZE, 0,
            (SOCKADDR*)&peeraddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            continue;
        }
        
        if (retval == NULL) {
            break;
        }

        // 받은 데이터 출력
        buf[retval] = '\0';
        //printf("[UDP 클라이언트] %d바이트를 받았습니다.\n", retval);
        printf("[받은 데이터] %s\n", buf);
    }
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

    // 소켓 주소 구조체 초기화
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    // 데이터 통신에 사용할 변수
   // SOCKADDR_IN peeraddr;
    //int addrlen;
    char buf[BUFSIZE + 1];
    int len;
    printf("로그인 닉네임을 입력하세요. 공백x\n");
    fgets(buf, BUFSIZE + 1, stdin);

    //닉네임 공백 제거
    std::string erasedSpaceNick(buf);
    erasedSpaceNick.erase(remove(erasedSpaceNick.begin(), erasedSpaceNick.end(), ' '), erasedSpaceNick.end());
    len = erasedSpaceNick.length();
    char* sendbuf = new char[len];
    strcpy(sendbuf, erasedSpaceNick.c_str());
    // '\n' 문자 제거
    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    //char* nickbuf = new char[len];
    //std::copy(buf, buf + len, nickbuf);
    retval = sendto(sock, sendbuf, strlen(sendbuf), 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    HANDLE hThread = CreateThread(NULL, 0, ClientRecv, (LPVOID)sock, 0, NULL);
    // 서버와 데이터 통신
    while (1) {

        // 데이터 입력
        //printf("\n[보낼 데이터] ");
        if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
            break;

        // '\n' 문자 제거
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        if (strlen(buf) == 0)
            break;

        // 데이터 보내기
        retval = sendto(sock, buf, strlen(buf), 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
        if (retval == SOCKET_ERROR) {
            err_display("sendto()");
            continue;
        }
        //printf("[UDP 클라이언트] %d바이트를 보냈습니다.\n", retval);

        
        /*HANDLE hThread1 = CreateThread(NULL, 0, ClientSend, (LPVOID)sock, 0, NULL);
        DWORD waitTime = WaitForSingleObject(hThread1, 1000);
        if (waitTime == WAIT_OBJECT_0) {
            HANDLE hThread2 = CreateThread(NULL, 0, ClientRecv, (LPVOID)sock, 0, NULL);
            DWORD waitTime2 = WaitForSingleObject(hThread2, 1000);
            if (waitTime == WAIT_OBJECT_0) {
                continue;
            }
        } 
        */
    }

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
