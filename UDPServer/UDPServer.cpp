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
#include <vector>
#include <string>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512

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


/*struct ClientInfo {
    SOCKADDR_IN clientaddr[8];
};*/
/*SOCKADDR_IN clientaddr[8];
int per_num = 0;
#define P_MAX 8;*/
typedef struct USERDATA {
    std::string nickName;


} USERINFO;
std::vector<SOCKADDR_IN> v; //USER IP & PORT ADDRESS
std::vector<USERINFO> userInfo;
int per_num = 0;

DWORD WINAPI ProcessClient(LPVOID arg) {
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];
    USERINFO tmpinfo;

    //Ŭ���̾�Ʈ ���� ���
    addrlen = sizeof(clientaddr);
   //getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

    while (1) {
        // ������ �ޱ�
        addrlen = sizeof(clientaddr);
        retval = recvfrom(client_sock, buf, BUFSIZE, 0,
            (SOCKADDR*)&clientaddr, &addrlen);
        if (retval == SOCKET_ERROR) {
            err_display("recvfrom()");
            continue;
        }

        if (retval == NULL) {
            break;
        }

        for (int i = 0; i < v.size(); i++) {
            if (v[i].sin_addr.s_addr == clientaddr.sin_addr.s_addr && v[i].sin_port == clientaddr.sin_port) {
                
                // ���� ������ ���
                buf[retval] = '\0';
                std::cout << userInfo[i].nickName <<" ";
                printf("[UDP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
                    ntohs(clientaddr.sin_port), buf);
                std::string sendStr(buf);
                std::string sendForAllChat = userInfo[i].nickName + " : " + sendStr;
                int sendSize = sendForAllChat.size();
                char *sendbuf = new char[sendSize];
                strcpy(sendbuf, sendForAllChat.c_str());
                // ������ ������
                for (int j = 0; j < v.size(); j++) {
                    retval = sendto(client_sock, sendbuf, sendSize, 0,
                        (SOCKADDR*)&v[j], sizeof(v[j]));
                    if (retval == SOCKET_ERROR) {
                        err_display("sendto()");
                        continue;
                    }
                }
                break;
            }
            else if (v[i].sin_port != clientaddr.sin_port && i == v.size() - 1) {
                v.push_back(clientaddr);
                buf[retval] = '\0';
                tmpinfo.nickName = buf;
                userInfo.push_back(tmpinfo);
                std::cout << "������ ����� �г��� : " << userInfo[per_num].nickName << std::endl;
                per_num++;
                break;
            }
        }
    }
    //closesocket()
    closesocket(client_sock);
    printf("-���� ����-\n");
    /*printf("[UDP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = %s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr),
        ntohs(clientaddr.sin_port));*/

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

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // ������ ��ſ� ����� ����
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    HANDLE ThreadHandle = NULL;
    USERINFO tmpinfo;

    // Ŭ���̾�Ʈ�� ������ ���
    while (1) {
        if (ThreadHandle == NULL) {
            // ������ �ޱ�
            addrlen = sizeof(clientaddr);
            retval = recvfrom(sock, buf, BUFSIZE, 0,
                (SOCKADDR*)&clientaddr, &addrlen);
            if (retval == SOCKET_ERROR) {
                err_display("recvfrom()");
                continue;
            }

            //
            v.push_back(clientaddr);
            buf[retval] = '\0';
            tmpinfo.nickName = buf;
            userInfo.push_back(tmpinfo);
            std::cout << "������ ����� �г��� : " << userInfo[per_num].nickName << std::endl;
            per_num++;

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

    // ���� ����
    WSACleanup();
    return 0;
}
