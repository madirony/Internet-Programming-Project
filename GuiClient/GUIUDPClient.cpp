#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <iostream>
#include "resource.h"
#undef _WINDOWS_
#include<afxwin.h>
#include<MMSystem.h>

//#define SERVERIP   "121.130.216.94"
//����
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024

//�α��� ���ν���
BOOL CALLBACK LogProc(HWND, UINT, WPARAM, LPARAM);
// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//ģ��â ���ν���
BOOL CALLBACK FLProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
//���� ���ν���
BOOL CALLBACK WhProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
void aisplayText(char* fmt, ...);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// �г��� ���� �Լ�
int NickNameProc(char* nickName);
void send_fl(), send_wp();
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ
HWND hSendButton; // ������ ��ư
HWND hEdit1, hEdit2 , LI1, FL1, FL12, mail, Hemoji; // ���� ��Ʈ�� �κ� �Է�, �κ� ���, �α��� �г���, ģ��â ���, ģ��â ��ư, ����â ��ư
HWND hEnterButton; // ���� ��ư
//���α׷� �ν��Ͻ��� �������� ����ϱ� ���� ���� �ν��Ͻ�
HINSTANCE aInstance;

//�̸�Ƽ�� �̹���
HBITMAP smile, happy, sad, angry;
std::string emoji[] = {"smile", "happy", "sad", "angry"};
HBITMAP Bit_emoji[] = { smile, happy, sad, angry };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // �̺�Ʈ ����
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;
    aInstance = hInstance;
    // ��ȭ���� ����
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), NULL, LogProc);
    
    // �̺�Ʈ ����
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}


//�α��� ���ν���
BOOL CALLBACK LogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char nickName[BUFSIZE + 1];
    
    switch (uMsg) {
    case WM_INITDIALOG:
        LI1 = GetDlgItem(hDlg, IDC_EDIT1);//���̵�
        hEnterButton = GetDlgItem(hDlg, IDOK);//���� ��ư
        SendMessage(LI1, EM_SETLIMITTEXT, BUFSIZE, 0);
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK://����
            EnableWindow(hEnterButton, FALSE); // ������ ��ư ��Ȱ��ȭ
            WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
            GetDlgItemText(hDlg, IDC_EDIT1, nickName, BUFSIZE + 1);
            SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
            NickNameProc(nickName);// ���̵� ����
            PlaySound(TEXT("login.wav"), NULL, SND_SYNC);
            // ���� ��� ������ ����
            CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
            EndDialog(hDlg, IDOK);
            
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
            return TRUE;
        case IDCANCEL://������
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

HBITMAP bitButton;//�̹���
// ���� �ּ� ����ü �ʱ�ȭ
SOCKADDR_IN serveraddr;

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        
    case WM_INITDIALOG:
        hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);//�Է�â
        hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);//���â
        FL12 = GetDlgItem(hDlg, IDC_BUTTON2);//ģ��â
        mail = GetDlgItem(hDlg, IDC_BUTTON1);//����â
        Hemoji = GetDlgItem(hDlg, IDC_BUTTON3);//�̸�Ƽ��
        hSendButton = GetDlgItem(hDlg, IDOK);//�������ư
        SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
        //ģ��â��ư
        bitButton = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP5));//�̹��� ����
        SendMessage(FL12, BM_SETIMAGE, 0, (LPARAM)bitButton);//�̹��� ����
        //�ӼӸ�â ��ư
        bitButton = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP6));//�̹��� ����
        SendMessage(mail, BM_SETIMAGE, 0, (LPARAM)bitButton);//�̹��� ����

        MoveWindow(Hemoji, 1500, 900, 100, 100, TRUE);//�̸�Ƽ�� �˾�
        //EnableWindow(Hemoji, TRUE); // �̸��� ��ư Ȱ��ȭ
        //�̸���smile
        smile = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP7));//�̹��� ����
        //�̸���sad
        sad = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP8));//�̹��� ����
        //�̸���happy
        happy = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP9));//�̹��� ����
        //�̸���angry
        angry = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP10));//�̹��� ����



        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK://������
            EnableWindow(hSendButton, FALSE); // ������ ��ư ��Ȱ��ȭ
            WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
            GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
            SetFocus(hEdit1);
            SendMessage(hEdit1, EM_SETSEL, 0, -1);
            return TRUE;
        case IDC_BUTTON3:
            MoveWindow(Hemoji, 150, 150, 0, 0, TRUE);//�̸�Ƽ�� �˾� �Ҹ�
            return TRUE;
        case IDC_BUTTON2:
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, FLProc);//ģ��â
            return TRUE;
        case IDC_BUTTON1:
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG3), NULL, WhProc);//�ӼӸ�
            return TRUE;
        case IDCANCEL://������
            //�������� ��� �߰�
            int len, retval;
            char sendbuff[BUFSIZE + 1] = { "/off" };
            len = strlen(sendbuff);
            if (sendbuff[len - 1] == '\n')
                sendbuff[len - 1] = '\0';
            retval = sendto(sock, sendbuff, len, 0,
                (SOCKADDR*)&serveraddr, sizeof(serveraddr));
            EndDialog(hDlg, IDCANCEL);
            //DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG4), NULL, LogProc);//���� �α���â �ٽ� ����
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

HWND FLhandle;
//ģ��â ���ν���
BOOL CALLBACK FLProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int len, retval;
    // '\n' ���� ����
    char sendbuff[BUFSIZE + 1] = { "/friends_list" };
    FLhandle = hDlg;
    switch (uMsg) {
    case WM_INITDIALOG:
        FL1 = GetDlgItem(hDlg, IDC_EDIT2);//�Է�â
        len = strlen(sendbuff);
        if (sendbuff[len - 1] == '\n')
            sendbuff[len - 1] = '\0';
        retval = sendto(sock, sendbuff, len, 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDCANCEL://������
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

HWND mail_out;
BOOL CALLBACK WhProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int len, retval;
    // '\n' ���� ����
    char sendbufw[BUFSIZE + 1] = { "/whispers_list" };
    switch (uMsg) {
    case WM_INITDIALOG:
        mail_out = GetDlgItem(hDlg, IDC_EDIT1);//�Է�â
        len = strlen(sendbufw);
        if (sendbufw[len - 1] == '\n')
            sendbufw[len - 1] = '\0';
        retval = sendto(sock, sendbufw, len, 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDCANCEL://������
            
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

// �ӼӸ�â ��Ʈ�� ��� �Լ�
void wisplayText(char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(mail_out);
    SendMessage(mail_out, EM_SETSEL, nLength, nLength);
    SendMessage(mail_out, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

// ģ��â ��Ʈ�� ��� �Լ�
void aisplayText(char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(FL1);
    SendMessage(FL1, EM_SETSEL, nLength, nLength);
    SendMessage(FL1, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hEdit2);
    SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
    SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

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
    DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

DWORD WINAPI ClientRecv(LPVOID arg) {
    // ������ �ޱ�
    SOCKET tmp_sock = (SOCKET)arg;
    int addrlen;
    SOCKADDR_IN peeraddr;
    int retval;
    char buf[BUFSIZE];
    SOCKADDR_IN serveraddr;
    int FListCheck = 0;

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
        char tmp[BUFSIZE + 1];
        strcpy(tmp,buf);
        std::string FWfind = buf;
        char* tmpCommand = strtok(buf, " ");
        tmpCommand = strtok(NULL, " ");
        tmpCommand = strtok(NULL, " ");
        std::string forEMJCommand(tmpCommand);
        if (forEMJCommand == "/emoji") { // + ģ�� ����Ʈ�� ���� �ӼӸ��� ��� ���� â�� ��쵵��
            //�̸��� ID
            tmpCommand = strtok(NULL, " ");
            for (int x = 0; x < sizeof(emoji) / sizeof(std::string); x++) {
                if (emoji[x] == tmpCommand) {
                    MoveWindow(Hemoji, 30, 0, 50, 50, TRUE);//�̸�Ƽ�� �˾�
                    std::string emoji_find = tmpCommand;
                    if (emoji_find.find("smile") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)smile);//�̹��� ����
                        //���� �������� ä��â�� �߰�
                        std::string addEmo = "���� ȣȣ �����ϴ�.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }else if (emoji_find.find("sad") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)sad);//�̹��� ����
                        //���� ����� ä��â�� �߰�
                        std::string addEmo = "���� ���� ��ϴ�.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }
                    else if (emoji_find.find("happy") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)happy);//�̹��� ����
                        //���� ����� ä��â�� �߰�
                        std::string addEmo = "���� �⻵�մϴ�.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }
                    else if (emoji_find.find("angry") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)angry);//�̹��� ����
                        //���� ȭ������ ä��â�� �߰�
                        std::string addEmo = "���� ���� ȭ�� ���ϴ�.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    } 
                    break;
                }
            }
        }else if (FWfind.find(" ���� ģ�� ���") != std::string::npos) {
            SetDlgItemText(FLhandle, IDC_EDIT2, (LPCSTR)"");//ģ�� ���â �ʱ�ȭ
            aisplayText("%s\n", tmp);
            FListCheck = 1;
        }else if (FListCheck == 1 && (FWfind.find("�¶���") != std::string::npos || FWfind.find("��������") != std::string::npos)) {
            aisplayText("%s\n", tmp);
        }else if (FWfind.find("���� �ӼӸ� ���") != std::string::npos) {
            wisplayText("%s\n", tmp);
        }
        else {
            DisplayText("%s\n", tmp);
        }
    }
    return 0;
}



int NickNameProc(char nickName[BUFSIZE + 1]) {
    int len, retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    //�г��� ���� ����
    std::string erasedSpaceNick(nickName);
    erasedSpaceNick.erase(remove(erasedSpaceNick.begin(), erasedSpaceNick.end(), ' '), erasedSpaceNick.end());
    len = erasedSpaceNick.length();
    char* sendbuf = new char[len];
    strcpy(sendbuf, erasedSpaceNick.c_str());

    // '\n' ���� ����
    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    return 0;
}

/*void Send_fl() {
    int len, retval;
    // '\n' ���� ����
    char sendbuf[BUFSIZE + 1] = {"/friends_list"};

    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}

void Send_wp() {
    int len, retval;
    // '\n' ���� ����
    char sendbuf[BUFSIZE + 1] = { "/whispers_list" };

    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}*/

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;
    // ������ ��ſ� ����� ����
    int len;
    
    HANDLE hThread = CreateThread(NULL, 0, ClientRecv, (LPVOID)sock, 0, NULL);
    // ������ ������ ���
    while (1) {
        // ������ �Է�
        WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���
        if (strlen(buf) == 0) {
            EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
            SetEvent(hReadEvent); // �б� �Ϸ� �˸���
            continue;
        }

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
        EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
        SetEvent(hReadEvent); // �б� �Ϸ� �˸���
    }
    return 0;
}
