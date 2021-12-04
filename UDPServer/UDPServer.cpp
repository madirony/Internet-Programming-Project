/*
�� Civil(11) Team Internet-Programming-Project ��
Team member Info
-----------------
- ������
- �ſ���
- ������

��� �����̳� ������Ʈ ������ ������ UDPServer.cpp UDPClient.cpp ���ϸ� īī�������� �����ֽø� �˴ϴ�.
*/

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <algorithm>

#define SERVERPORT 9000
#define BUFSIZE    1024

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
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
void err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}


/*struct ClientInfo {
    SOCKADDR_IN clientaddr[8];
};*/
/*SOCKADDR_IN clientaddr[8];
int per_num = 0;
#define P_MAX 8;*/

//����
typedef struct USERDATA {
    std::string nickName;
    std::vector<std::string> friendsList;
    std::vector<std::string> whisperList;
    int online = 0;
    int firstConnect = 0;

} USERINFO;


std::vector<SOCKADDR_IN> v; //USER IP & PORT ADDRESS
std::vector<USERINFO> userInfo;
//std::vector<SOCKADDR_IN> onlineUser;
int per_num = 0;

//ù ���� ������ ���� ����
bool first_user = false;

//ä��
std::string command[] = { "/whisper", "/w", "/friend_add","/f","/friends_list", "/emoji", "/help", "/nc", "/whispers_list"};

//�̸�Ƽ��
std::string emoji[] = { "smile", "happy", "sad", "angry" };

SOCKET client_sock;


//���͸�
std::string filtering(std::string s) { 
    std::string bad_word[] = {"fuck", "����", "����", "����", "��", "����", "�ٺ�", "���", "��", "���", "����", "���", "����", "�ֹ�", "����", "����", "����", "����", "��û��"};

    for (auto bad : bad_word) {
        if (s.find(bad) < s.size()) {
            s = "[�弳�� ���� ����ε�� ä���Դϴ�.]";
        }
    }
        return s;
}

DWORD WINAPI ProcessClient(LPVOID arg) {
    client_sock = (SOCKET)arg;
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
                //��ϵ� ������� ä�� ����
                // ���� ������ ���
                buf[retval] = '\0';
                std::cout << userInfo[i].nickName << " ";
                printf("[UDP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
                    ntohs(clientaddr.sin_port), buf);
                std::string sendStr(buf);

                //���� �ܾ� ���͸�
                sendStr = filtering(sendStr);

                // �α��� �� ù ������ ��� ���� ��� (cmd������ ���� �۵� ������ gui�� �����κ���)
                if (userInfo[i].firstConnect == 0) {

                    std::string serverMsg_welcome;
                    serverMsg_welcome = "�١�Kate-On�� ���� ���� ȯ���մϴ�.�١� \r\n������ /help Ŀ��� �Է����� ã�� �� �� �ֽ��ϴ�.\r\n[��Ģ1] : �ٸ� �� ��� ���� ����սô�. \r\n[��Ģ2] : �ߺ��� �г��� ����� �������ֽð�, \r\n         �г����� �ߺ��ȴٸ� /nc Ŀ����\r\n         �г��� ������ ���ֽñ� �ٶ��ϴ�.\r\n";
                    int tempsendSize = serverMsg_welcome.size();
                    char* tempsendbuf = new char[tempsendSize];
                    strcpy(tempsendbuf, serverMsg_welcome.c_str());
                    retval = sendto(client_sock, tempsendbuf, tempsendSize, 0,
                        (SOCKADDR*)&v[i], sizeof(v[i]));
                    userInfo[i].firstConnect = 1;

                }

                //��ɾ� Ȯ��
                char* tmpCommand = strtok(buf, " ");
                //�������� split�� ���ڿ��� /
                if (tmpCommand[0] == '/') {
                    std::cout << tmpCommand << std::endl;
                    //�ӼӸ�
                    std::string forIFCommand(tmpCommand);
                    if (forIFCommand == command[0] || forIFCommand == command[1]) {
                        int commandLen = forIFCommand.length();
                        tmpCommand = strtok(NULL, " ");
                        for (int j = 0; j < userInfo.size(); j++) {
                            if (tmpCommand == userInfo[j].nickName) {
                                int whisUserLen = userInfo[j].nickName.length();
                                if (commandLen == 8) {
                                    std::cout << sendStr << std::endl;
                                    if (sendStr[0] != '[') {
                                        sendStr.erase(0, 10 + whisUserLen);
                                    }
                                }
                                else if(commandLen ==2){
                                    std::cout << sendStr << std::endl;
                                    if (sendStr[0] != '[') {
                                        sendStr.erase(0, 4 + whisUserLen);
                                    }
                                }
                                else {
                                    std::cout << "whisper err3" << std::endl;
                                }
                                std::cout << sendStr << std::endl;
                                std::string sendForWhisperChat = userInfo[i].nickName + "���� �ӼӸ� : " + sendStr;
                                userInfo[j].whisperList.push_back(sendForWhisperChat);
                                int sendSize = sendForWhisperChat.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, sendForWhisperChat.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[j], sizeof(v[j]));
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[i], sizeof(v[i]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }
                                break;
                            }
                            else if (j == userInfo.size() - 1 && tmpCommand != userInfo[j].nickName) {
                                std::cout << "�׷� �г����� ����ڴ� �����ϴ�." << std::endl;
                                std::string serverMsg_whisper_err;
                                serverMsg_whisper_err = "�׷� �г����� ����ڴ� �����ϴ�.\r\n";
                                int sendSize = serverMsg_whisper_err.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, serverMsg_whisper_err.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[i], sizeof(v[i]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }
                            }
                        }

                    }
                    //ģ���߰�
                    else if (forIFCommand == command[2] || forIFCommand == command[3]) {
                        //std::cout << "����"<< userInfo.size() << " " << std::endl;
                        tmpCommand = strtok(NULL, " ");
                        std::string serverMsg_friend_add;
                        for (int j = 0; j < userInfo.size(); j++) {
                            //std::cout << "���������˻� " << tmpCommand << std::endl;
                            if (tmpCommand == userInfo[j].nickName) {
                                //std::cout << "���� �ִ��� �˻�" << userInfo[i].friendsList.size() << std::endl;
                                if (userInfo[i].friendsList.size() == 0) {
                                    userInfo[i].friendsList.push_back(userInfo[j].nickName);
                                    serverMsg_friend_add = userInfo[j].nickName + "���� ģ�� �߰� '_< \r\n";
                                }
                                else {
                                    for (int k = 0; k < userInfo[i].friendsList.size(); k++) {
                                        //std::cout << "������ �ݺ��� ����" << std::endl;
                                        if (userInfo[i].friendsList[k] == userInfo[j].nickName) {
                                            serverMsg_friend_add = userInfo[j].nickName + "�԰� �̹� ģ���Դϴ�.\r\n";
                                            //std::cout << "�ߺ�" << std::endl;
                                            break;
                                        }
                                        else if ((k == userInfo[i].friendsList.size() - 1 && userInfo[i].friendsList[k] != userInfo[j].nickName)) {
                                            userInfo[i].friendsList.push_back(userInfo[j].nickName);
                                            serverMsg_friend_add = userInfo[j].nickName + "���� ģ�� �߰� '_< \r\n";
                                            //std::cout << "�߰�" << std::endl;
                                            break;
                                        }
                                        else {
                                            std::cout << "�ý��� �޽��� ����?" << std::endl;
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
                                std::cout << "�׷� �г����� ����ڴ� �����ϴ�." << std::endl;
                                std::string serverMsg_friend_err;
                                serverMsg_friend_err = "�׷� �г����� ����ڴ� �����ϴ�.\r\n";
                                int sendSize = serverMsg_friend_err.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, serverMsg_friend_err.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[i], sizeof(v[i]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }
                            }
                            else {
                                std::cout << "����?" << std::endl;
                            }
                        }

                    }
                    //ģ�� ��� ����
                    else if (forIFCommand == command[4]) {
                        std::string serverMsg_my_friends_list = userInfo[i].nickName + " ���� ģ�� ���\r\n";
                        for (int j = 0; j < userInfo[i].friendsList.size(); j++) {
                            std::string onlineCheck = "��������";
                            for (int k = 0; k < userInfo.size(); k++) {
                                if (userInfo[i].friendsList[j] == userInfo[k].nickName) {
                                    onlineCheck = "�¶���";
                                }
                            }
                            serverMsg_my_friends_list += userInfo[i].friendsList[j] +" - " + onlineCheck + "\r\n";
                        }
                        int sendSize = serverMsg_my_friends_list.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_my_friends_list.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));
                        if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }


                    }
                    //�̸�Ƽ��
                    else if (forIFCommand == command[5]) {
                        tmpCommand = strtok(NULL, " ");
                        for (int j = 0; j < sizeof(emoji) / sizeof(std::string); j++) {
                            if (emoji[j] == tmpCommand) {
                                //��ü ä��
                                //�޽��� ���� �۾�
                                std::string sendForAllChat = userInfo[i].nickName + " : " + sendStr;
                                int sendSize = sendForAllChat.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, sendForAllChat.c_str());
                                // ������ ������
                                for (int j = 0; j < v.size(); j++) {
                                    if (userInfo[j].online == 1) {
                                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                                            (SOCKADDR*)&v[j], sizeof(v[j]));
                                        if (retval == SOCKET_ERROR) {
                                            err_display("sendto()");
                                            continue;
                                        }
                                    }
                                }
                                break;
                            }
                            else if (j == sizeof(emoji) / sizeof(std::string) - 1 && emoji[sizeof(emoji) / sizeof(std::string) - 1] != tmpCommand) {
                                //���� �̸���
                                std::cout << "�׷� �̸����� �����ϴ�." << std::endl;
                                std::string serverMsg_emoji_err;
                                serverMsg_emoji_err = "�׷� �̸����� �����ϴ�.\r\n";
                                int sendSize = serverMsg_emoji_err.size();
                                char* sendbuf = new char[sendSize];
                                strcpy(sendbuf, serverMsg_emoji_err.c_str());
                                retval = sendto(client_sock, sendbuf, sendSize, 0,
                                    (SOCKADDR*)&v[i], sizeof(v[i]));
                                if (retval == SOCKET_ERROR) {
                                    err_display("sendto()");
                                    continue;
                                }
                            }
                        }
                    }
                    //����
                    else if (forIFCommand == command[6]) {
                        std::string serverMsg_help;
                        serverMsg_help = "�١١�Kate-On ���򸻡١١�\r\n[�ӼӸ� ������] :: /whisper Ȥ�� /w \r\n[����] :: /whisper �г��� �ӼӸ����� Ȥ�� /w �г��� �ӼӸ�����\r\n[�ӼӸ����] :: /whispers_list\r\n[ģ�� �߰�] :: /friend_add Ȥ�� /f \r\n[����] :: /friend_add �г���  Ȥ��  /f �г���\r\n[ģ�� ����Ʈ ����] :: /friends_list\r\n[�̸�Ƽ��] :: /emoji �̸�Ƽ�ܸ�\r\n�̸�Ƽ�� ���� :: smile, happy, sad, angry \r\n[�г��� ����] :: /nc �ٲܴг���(����x)\r\n";
                        int sendSize = serverMsg_help.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_help.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));
                        if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }
                    }
                    //�г��� ����
                    else if (forIFCommand == command[7]) {
                        tmpCommand = strtok(NULL, " ");
                        //std::string erasedSpaceNick(tmpCommand);
                        //erasedSpaceNick.erase(remove(erasedSpaceNick.begin(), erasedSpaceNick.end(), ' '), erasedSpaceNick.end());
                        userInfo[i].nickName = tmpCommand;
                        /*std::string serverMsg_nickChange = "�г����� ����Ǿ����ϴ�.\n";
                        int sendSize = serverMsg_nickChange.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_nickChange.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));
                        if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }*/
                    }
                    //�ӼӸ� ���
                    else if (forIFCommand == command[8]) {
                        std::string serverMsg_my_whispers_list = userInfo[i].nickName + " ���� �ӼӸ� ���\r\n";
                        for (int j = 0; j < userInfo[i].whisperList.size(); j++) {
                            serverMsg_my_whispers_list += userInfo[i].whisperList[j] + "\r\n";
                        }
                        int sendSize = serverMsg_my_whispers_list.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_my_whispers_list.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));
                        if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }
                    }
                    else {
                        //�߸��� Ŀ���
                        std::cout << "�׷� Ŀ���� �����ϴ�." << std::endl;
                        std::string serverMsg_command_err;
                        serverMsg_command_err = "�׷� Ŀ���� �����ϴ�.\r\n";
                        int sendSize = serverMsg_command_err.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_command_err.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));
                        if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }
                    }
                    break;
                }
                else {

                    if (first_user == false && i == 0) {
                        //�ű� �������� ��Ģ/�������� ����
                         //�޽��� ���� �۾�
                        /*std::string serverMsg_welcome;
                        serverMsg_welcome = "�١�Kate-On�� ���� ���� ȯ���մϴ�.�١� \r\n������ /help Ŀ��� �Է����� ã�� �� �� �ֽ��ϴ�.\r\n[��Ģ1] : �ٸ� �� ��� ���� ����սô�. \r\n[��Ģ2] : �ߺ��� �г��� ����� �������ֽð�, \r\n         �г����� �ߺ��ȴٸ� /nc Ŀ����\r\n         �г��� ������ ���ֽñ� �ٶ��ϴ�.\r\n";
                        int sendSize = serverMsg_welcome.size();
                        char* sendbuf = new char[sendSize];
                        strcpy(sendbuf, serverMsg_welcome.c_str());
                        retval = sendto(client_sock, sendbuf, sendSize, 0,
                            (SOCKADDR*)&v[i], sizeof(v[i]));*/
                        userInfo[i].firstConnect = 1;
                        /*if (retval == SOCKET_ERROR) {
                            err_display("sendto()");
                            continue;
                        }*/
                        first_user = true;
                    }


                    //��ü ä��
                    //�޽��� ���� �۾�
                    std::string sendForAllChat = userInfo[i].nickName + " : " + sendStr;
                    int sendSize = sendForAllChat.size();
                    char* sendbuf = new char[sendSize];
                    strcpy(sendbuf, sendForAllChat.c_str());
                    // ������ ������
                    for (int j = 0; j < v.size(); j++) {
                        if (userInfo[j].online == 1) {
                            retval = sendto(client_sock, sendbuf, sendSize, 0,
                                (SOCKADDR*)&v[j], sizeof(v[j]));
                            if (retval == SOCKET_ERROR) {
                                err_display("sendto()");
                                continue;
                            }
                        }
                    }
                    break;
                }


            }
            else if (v[i].sin_port != clientaddr.sin_port && i == v.size() - 1) {
                //�ű� ���� ù ���ӽ� �г��� ���
                //127.0.0.1 ���� ȯ�濡���� �г��� �ߺ��˻� ���� �Ұ� 
                //���ݿ����� ip�� ���� �ʴ� ȯ�濡���� ������ ���̶� ����
                /*int tmpsize = userInfo.size();
                int ifNickOverlap = 0;*/

                //�г��� �ߺ� �˻�
                /*for (int j = 0; j < tmpsize; j++) {
                    if (tmpinfo.nickName == userInfo[j].nickName) {
                        if (v[j].sin_addr.s_addr == clientaddr.sin_addr.s_addr) {
                            v[j].sin_port = clientaddr.sin_port;
                            //���� ����� ����
                        }
                        else {
                            ifNickOverlap = 1;
                        }
                    }
                }*/

                v.push_back(clientaddr);
                //onlineUser.push_back(clientaddr);
                buf[retval] = '\0';
                tmpinfo.nickName = buf;
                tmpinfo.online = 1;
                userInfo.push_back(tmpinfo);
                std::cout << "������ ����� �г��� : " << userInfo[per_num].nickName << std::endl;
                tmpinfo.firstConnect;

                //�г��� �ߺ� �˻�
                /*if (ifNickOverlap == 1) {
                    std::string serverMsg_nick_overlap = "�г����� �ߺ��˴ϴ�. �г����� �������ּ���.\n";
                    int nickerrSize = serverMsg_nick_overlap.size();
                    char* nickoverBuf = new char[nickerrSize];
                    strcpy(nickoverBuf, serverMsg_nick_overlap.c_str());
                    retval = sendto(client_sock, nickoverBuf, nickerrSize, 0,
                        (SOCKADDR*)&v[per_num], sizeof(v[per_num]));
                }*/

                //�ű� �������� ��Ģ/�������� ����
                //�޽��� ���� �۾�
                /*std::string serverMsg_welcome;
                serverMsg_welcome = "�١�Kate-On�� ���� ���� ȯ���մϴ�.�١� \r\n������ /help Ŀ��� �Է����� ã�� �� �� �ֽ��ϴ�.\r\n[��Ģ1] : �ٸ� �� ��� ���� ����սô�. \r\n[��Ģ2] : �ߺ��� �г��� ����� �������ֽð�, \r\n         �г����� �ߺ��ȴٸ� /nc Ŀ����\r\n         �г��� ������ ���ֽñ� �ٶ��ϴ�.\r\n";
                int sendSize = serverMsg_welcome.size();
                char* sendbuf = new char[sendSize];
                strcpy(sendbuf, serverMsg_welcome.c_str());
                retval = sendto(client_sock, sendbuf, sendSize, 0,
                    (SOCKADDR*)&v[per_num], sizeof(v[per_num]));

                if (retval == SOCKET_ERROR) {
                    err_display("sendto()");
                    continue;
                }*/

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


int main(int argc, char* argv[])
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
    retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
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

            //ù ����
            v.push_back(clientaddr);
            //onlineUser.push_back(clientaddr);
            buf[retval] = '\0';
            tmpinfo.nickName = buf;
            tmpinfo.online = 1;
            userInfo.push_back(tmpinfo);
            std::cout << "������ ����� �г��� : " << userInfo[per_num].nickName << std::endl;
            per_num++;
            HANDLE hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)sock, 0, NULL);

            ThreadHandle = hThread;
            if (hThread == NULL) { closesocket(sock); }
            else { CloseHandle(hThread); }
        }
        else {
            //���� ����
            //���� ����� ä��
            // ������ �Է�
        //printf("\n[���� ������] ");
            if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
                break;

            // '\n' ���� ����
            int len = strlen(buf);
            if (buf[len - 1] == '\n')
                buf[len - 1] = '\0';
            if (strlen(buf) == 0)
                break;

            //��ü ä��
            //�޽��� ���� �۾�
            std::string sendStr(buf);
            std::string sendForAllChat = "[���] : " + sendStr;
            int sendSize = sendForAllChat.size();
            char* sendbuf = new char[sendSize];
            strcpy(sendbuf, sendForAllChat.c_str());
            // ������ ������
            for (int j = 0; j < v.size(); j++) {
                if (userInfo[j].online == 1) {
                    retval = sendto(client_sock, sendbuf, sendSize, 0,
                        (SOCKADDR*)&v[j], sizeof(v[j]));
                    if (retval == SOCKET_ERROR) {
                        err_display("sendto()");
                        continue;
                    }
                }
            }
            continue;
        }
    }

    // closesocket()
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}
