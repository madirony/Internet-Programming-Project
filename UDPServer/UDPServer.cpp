/*
§ Civil(11) Team Internet-Programming-Project §
Team member Info
-----------------
- 연정흠
- 신용훈
- 김은빈

기능 구현이나 업데이트 사항이 있으면 UDPServer.cpp UDPClient.cpp 파일만 카카오톡으로 보내주시면 됩니다.
*/

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

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

//유저
typedef struct USERDATA {
    std::string nickName;
    std::vector<std::string> friendsList;
} USERINFO;

std::vector<SOCKADDR_IN> v; //USER IP & PORT ADDRESS
std::vector<USERINFO> userInfo;
int per_num = 0;

//채팅
std::string command[] = { "/whisper", "/f"};

DWORD WINAPI ProcessClient(LPVOID arg) {
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];
    USERINFO tmpinfo;

    //클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
   //getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

    while (1) {
        // 데이터 받기
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
                //등록된 사용자의 채팅 전송
                // 받은 데이터 출력
                buf[retval] = '\0';
                std::cout << userInfo[i].nickName <<" ";
                printf("[UDP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
                    ntohs(clientaddr.sin_port), buf);
                std::string sendStr(buf);
                
                //명령어 확인
                char* tmpCommand = strtok(buf, " ");
                //공백으로 split된 문자열이 /
                if (tmpCommand[0] == '/') {
                    std::cout << tmpCommand << std::endl;
                    //귓속말
                    std::string forIFCommand(tmpCommand);
                    if (forIFCommand == command[0]) {
                        tmpCommand = strtok(NULL, " ");
                        for (int j = 0; j < userInfo.size(); j++) {
                            if (tmpCommand == userInfo[j].nickName) {
                                int whisUserLen = userInfo[j].nickName.length();
                                sendStr.erase(0, 10 + whisUserLen);
                                std::string sendForWhisperChat = userInfo[i].nickName + "님의 귓속말 : " + sendStr;
                                int sendSize = sendForWhisperChat.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, sendForWhisperChat.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[j], sizeof(v[j]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }
                                break;
                            }
                            else if(j==userInfo.size()-1 && tmpCommand != userInfo[j].nickName){
                                std::cout << "그런 닉네임의 사용자는 없습니다." << std::endl;
                            }
                        }

                    }
                    else if (forIFCommand == command[1]) {
                        std::cout << "들어옴"<< userInfo.size() << " " << std::endl;
                        tmpCommand = strtok(NULL, " ");
                        std::string serverMsg_friend_add;
                        for (int j = 0; j < userInfo.size(); j++) {
                            std::cout << "인자유저검색 " << tmpCommand << std::endl;
                            if (tmpCommand == userInfo[j].nickName) {
                                std::cout << "닉이 있는지 검사" << userInfo[i].friendsList.size() << std::endl;
                                if (userInfo[i].friendsList.size() == 0) {
                                    userInfo[i].friendsList.push_back(userInfo[j].nickName);
                                    serverMsg_friend_add = userInfo[j].nickName + "님을 친구 추가 '_< \n";
                                }
                                else {
                                    for (int k = 0; k < userInfo[i].friendsList.size(); k++) {
                                        std::cout << "있으면 반복문 들어옴" << std::endl;
                                        if (userInfo[i].friendsList[k] == userInfo[j].nickName) {
                                            serverMsg_friend_add = userInfo[j].nickName + "님과 이미 친구입니다.\n";
                                            std::cout << "중복" << std::endl;
                                            break;
                                        }
                                        else if ((k == userInfo[i].friendsList.size() - 1 && userInfo[i].friendsList[k] != userInfo[j].nickName)) {
                                            userInfo[i].friendsList.push_back(userInfo[j].nickName);
                                            serverMsg_friend_add = userInfo[j].nickName + "님을 친구 추가 '_< \n";
                                            std::cout << "추가" << std::endl;
                                            break;
                                        }
                                        else {
                                            std::cout << "시스템 메시지 오류?" << std::endl;
                                            break;
                                        }
                                    }
                                }
                                int sendSize = serverMsg_friend_add.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, serverMsg_friend_add.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[i], sizeof(v[i]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }

                                break;
                            }
                            else if (j == userInfo.size() - 1 && tmpCommand != userInfo[j].nickName) {
                                std::cout << "그런 닉네임의 사용자는 없습니다." << std::endl;
                            }
                            else {
                                std::cout << "오류?" << std::endl;
                            }
                        }

                    }
                    else {
                        //잘못된 커멘드
                    std::cout << "그런 커멘드는 없습니다." << std::endl;
                    }
                    break;
                }
                else {
                    //전체 채팅
                    //메시지 전송 작업
                    std::string sendForAllChat = userInfo[i].nickName + " : " + sendStr;
                    int sendSize = sendForAllChat.size();
                    char* sendbuf = new char[sendSize];
                    strcpy(sendbuf, sendForAllChat.c_str());
                    // 데이터 보내기
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

  
            }
            else if (v[i].sin_port != clientaddr.sin_port && i == v.size() - 1) {
                //신규 유저 첫 접속시 닉네임 등록
                v.push_back(clientaddr);
                buf[retval] = '\0';
                tmpinfo.nickName = buf;
                userInfo.push_back(tmpinfo);
                std::cout << "접속한 사용자 닉네임 : " << userInfo[per_num].nickName << std::endl;
                per_num++;
                break;
            }
        }
    }
    //closesocket()
    closesocket(client_sock);
    printf("-서버 종료-\n");
    /*printf("[UDP 서버] 클라이언트 종료 : IP 주소 = %s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr),
        ntohs(clientaddr.sin_port));*/

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
    USERINFO tmpinfo;

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

            //
            v.push_back(clientaddr);
            buf[retval] = '\0';
            tmpinfo.nickName = buf;
            userInfo.push_back(tmpinfo);
            std::cout << "접속한 사용자 닉네임 : " << userInfo[per_num].nickName << std::endl;
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

    // 윈속 종료
    WSACleanup();
    return 0;
}
