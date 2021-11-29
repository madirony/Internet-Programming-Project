/*
�� Civil(11) Team Internet-Programming-Project ��
Team member Info
-----------------
- ������
- �ſ���
- ������

��� �����̳� ������Ʈ ������ ������ UDPServer.cpp UDPClient.cpp ���ϸ� īī�������� �����ֽø� �˴ϴ�.
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
    // ������ �ޱ�
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

        // ���� ������ ���
        buf[retval] = '\0';
        //printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
        printf("[���� ������] %s\n", buf);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // ���� �ּ� ����ü �ʱ�ȭ
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    // ������ ��ſ� ����� ����
   // SOCKADDR_IN peeraddr;
    //int addrlen;
    char buf[BUFSIZE + 1];
    int len;
    printf("�α��� �г����� �Է��ϼ���. ����x\n");
    fgets(buf, BUFSIZE + 1, stdin);

    //�г��� ���� ����
    std::string erasedSpaceNick(buf);
    erasedSpaceNick.erase(remove(erasedSpaceNick.begin(), erasedSpaceNick.end(), ' '), erasedSpaceNick.end());
    len = erasedSpaceNick.length();
    char* sendbuf = new char[len];
    strcpy(sendbuf, erasedSpaceNick.c_str());
    // '\n' ���� ����
    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    //char* nickbuf = new char[len];
    //std::copy(buf, buf + len, nickbuf);
    retval = sendto(sock, sendbuf, strlen(sendbuf), 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    HANDLE hThread = CreateThread(NULL, 0, ClientRecv, (LPVOID)sock, 0, NULL);
    // ������ ������ ���
    while (1) {

        // ������ �Է�
        //printf("\n[���� ������] ");
        if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
            break;

        // '\n' ���� ����
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        if (strlen(buf) == 0)
            break;

        // ������ ������
        retval = sendto(sock, buf, strlen(buf), 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
        if (retval == SOCKET_ERROR) {
            err_display("sendto()");
            continue;
        }
        //printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

        
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

    // ���� ����
    WSACleanup();
    return 0;
}
