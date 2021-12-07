#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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
//로컬
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024

//로그인 프로시저
BOOL CALLBACK LogProc(HWND, UINT, WPARAM, LPARAM);
// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//친구창 프로시저
BOOL CALLBACK FLProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
//쪽지 프로시저
BOOL CALLBACK WhProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
void aisplayText(char* fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 닉네임 전송 함수
int NickNameProc(char* nickName);
void send_fl(), send_wp();
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hEdit1, hEdit2 , LI1, FL1, FL12, mail, Hemoji; // 편집 컨트롤 로비 입력, 로비 출력, 로그인 닉네임, 친구창 출력, 친구창 버튼, 쪽지창 버튼
HWND hEnterButton; // 입장 버튼
//프로그램 인스턴스를 전역으로 사용하기 위한 전역 인스턴스
HINSTANCE aInstance;

//이모티콘 이미지
HBITMAP smile, happy, sad, angry;
std::string emoji[] = {"smile", "happy", "sad", "angry"};
HBITMAP Bit_emoji[] = { smile, happy, sad, angry };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // 이벤트 생성
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;
    aInstance = hInstance;
    // 대화상자 생성
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), NULL, LogProc);
    
    // 이벤트 제거
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}


//로그인 프로시저
BOOL CALLBACK LogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char nickName[BUFSIZE + 1];
    
    switch (uMsg) {
    case WM_INITDIALOG:
        LI1 = GetDlgItem(hDlg, IDC_EDIT1);//아이디
        hEnterButton = GetDlgItem(hDlg, IDOK);//입장 버튼
        SendMessage(LI1, EM_SETLIMITTEXT, BUFSIZE, 0);
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK://입장
            EnableWindow(hEnterButton, FALSE); // 보내기 버튼 비활성화
            WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
            GetDlgItemText(hDlg, IDC_EDIT1, nickName, BUFSIZE + 1);
            SetEvent(hWriteEvent); // 쓰기 완료 알리기
            NickNameProc(nickName);// 아이디 전송
            PlaySound(TEXT("login.wav"), NULL, SND_SYNC);
            // 소켓 통신 스레드 생성
            CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
            EndDialog(hDlg, IDOK);
            
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
            return TRUE;
        case IDCANCEL://끝내기
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

HBITMAP bitButton;//이미지
// 소켓 주소 구조체 초기화
SOCKADDR_IN serveraddr;

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        
    case WM_INITDIALOG:
        hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);//입력창
        hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);//출력창
        FL12 = GetDlgItem(hDlg, IDC_BUTTON2);//친구창
        mail = GetDlgItem(hDlg, IDC_BUTTON1);//쪽지창
        Hemoji = GetDlgItem(hDlg, IDC_BUTTON3);//이모티콘
        hSendButton = GetDlgItem(hDlg, IDOK);//보내기버튼
        SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
        //친구창버튼
        bitButton = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP5));//이미지 수정
        SendMessage(FL12, BM_SETIMAGE, 0, (LPARAM)bitButton);//이미지 수정
        //귓속말창 버튼
        bitButton = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP6));//이미지 수정
        SendMessage(mail, BM_SETIMAGE, 0, (LPARAM)bitButton);//이미지 수정

        MoveWindow(Hemoji, 1500, 900, 100, 100, TRUE);//이모티콘 팝업
        //EnableWindow(Hemoji, TRUE); // 이모지 버튼 활성화
        //이모지smile
        smile = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP7));//이미지 수정
        //이모지sad
        sad = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP8));//이미지 수정
        //이모지happy
        happy = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP9));//이미지 수정
        //이모지angry
        angry = LoadBitmap(aInstance, MAKEINTRESOURCE(IDB_BITMAP10));//이미지 수정



        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK://보내기
            EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
            WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
            GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // 쓰기 완료 알리기
            SetFocus(hEdit1);
            SendMessage(hEdit1, EM_SETSEL, 0, -1);
            return TRUE;
        case IDC_BUTTON3:
            MoveWindow(Hemoji, 150, 150, 0, 0, TRUE);//이모티콘 팝업 소멸
            return TRUE;
        case IDC_BUTTON2:
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, FLProc);//친구창
            return TRUE;
        case IDC_BUTTON1:
            DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG3), NULL, WhProc);//귓속말
            return TRUE;
        case IDCANCEL://끝내기
            //오프라인 기능 추가
            int len, retval;
            char sendbuff[BUFSIZE + 1] = { "/off" };
            len = strlen(sendbuff);
            if (sendbuff[len - 1] == '\n')
                sendbuff[len - 1] = '\0';
            retval = sendto(sock, sendbuff, len, 0,
                (SOCKADDR*)&serveraddr, sizeof(serveraddr));
            EndDialog(hDlg, IDCANCEL);
            //DialogBox(aInstance, MAKEINTRESOURCE(IDD_DIALOG4), NULL, LogProc);//끄고 로그인창 다시 생성
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

HWND FLhandle;
//친구창 프로시저
BOOL CALLBACK FLProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int len, retval;
    // '\n' 문자 제거
    char sendbuff[BUFSIZE + 1] = { "/friends_list" };
    FLhandle = hDlg;
    switch (uMsg) {
    case WM_INITDIALOG:
        FL1 = GetDlgItem(hDlg, IDC_EDIT2);//입력창
        len = strlen(sendbuff);
        if (sendbuff[len - 1] == '\n')
            sendbuff[len - 1] = '\0';
        retval = sendto(sock, sendbuff, len, 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDCANCEL://끝내기
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
    // '\n' 문자 제거
    char sendbufw[BUFSIZE + 1] = { "/whispers_list" };
    switch (uMsg) {
    case WM_INITDIALOG:
        mail_out = GetDlgItem(hDlg, IDC_EDIT1);//입력창
        len = strlen(sendbufw);
        if (sendbufw[len - 1] == '\n')
            sendbufw[len - 1] = '\0';
        retval = sendto(sock, sendbufw, len, 0,
            (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDCANCEL://끝내기
            
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}

// 귓속말창 컨트롤 출력 함수
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

// 친구창 컨트롤 출력 함수
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

// 편집 컨트롤 출력 함수
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
    DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

DWORD WINAPI ClientRecv(LPVOID arg) {
    // 데이터 받기
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

        // 받은 데이터 출력
        buf[retval] = '\0';
        char tmp[BUFSIZE + 1];
        strcpy(tmp,buf);
        std::string FWfind = buf;
        char* tmpCommand = strtok(buf, " ");
        tmpCommand = strtok(NULL, " ");
        tmpCommand = strtok(NULL, " ");
        std::string forEMJCommand(tmpCommand);
        if (forEMJCommand == "/emoji") { // + 친구 리스트일 경우와 귓속말일 경우 쪽지 창을 띄우도록
            //이모지 ID
            tmpCommand = strtok(NULL, " ");
            for (int x = 0; x < sizeof(emoji) / sizeof(std::string); x++) {
                if (emoji[x] == tmpCommand) {
                    MoveWindow(Hemoji, 30, 0, 50, 50, TRUE);//이모티콘 팝업
                    std::string emoji_find = tmpCommand;
                    if (emoji_find.find("smile") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)smile);//이미지 수정
                        //누가 웃었는지 채팅창에 추가
                        std::string addEmo = "님이 호호 웃습니다.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }else if (emoji_find.find("sad") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)sad);//이미지 수정
                        //누가 우는지 채팅창에 추가
                        std::string addEmo = "님이 엉엉 웁니다.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }
                    else if (emoji_find.find("happy") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)happy);//이미지 수정
                        //누가 기쁜지 채팅창에 추가
                        std::string addEmo = "님이 기뻐합니다.";
                        char* sendusernick = strtok(tmp, " ");
                        std::string strnick(sendusernick);
                        strnick += addEmo;
                        int emoSize = strnick.size();
                        char* emobuf = new char[emoSize];
                        strcpy(emobuf, strnick.c_str());
                        DisplayText("%s\r\n", emobuf);
                    }
                    else if (emoji_find.find("angry") != std::string::npos) {
                        SendMessage(Hemoji, BM_SETIMAGE, 0, (LPARAM)angry);//이미지 수정
                        //누가 화났는지 채팅창에 추가
                        std::string addEmo = "님이 버럭 화를 냅니다.";
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
        }else if (FWfind.find(" 님의 친구 목록") != std::string::npos) {
            SetDlgItemText(FLhandle, IDC_EDIT2, (LPCSTR)"");//친구 목록창 초기화
            aisplayText("%s\n", tmp);
            FListCheck = 1;
        }else if (FListCheck == 1 && (FWfind.find("온라인") != std::string::npos || FWfind.find("오프라인") != std::string::npos)) {
            aisplayText("%s\n", tmp);
        }else if (FWfind.find("님의 귓속말 목록") != std::string::npos) {
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

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    //닉네임 공백 제거
    std::string erasedSpaceNick(nickName);
    erasedSpaceNick.erase(remove(erasedSpaceNick.begin(), erasedSpaceNick.end(), ' '), erasedSpaceNick.end());
    len = erasedSpaceNick.length();
    char* sendbuf = new char[len];
    strcpy(sendbuf, erasedSpaceNick.c_str());

    // '\n' 문자 제거
    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    return 0;
}

/*void Send_fl() {
    int len, retval;
    // '\n' 문자 제거
    char sendbuf[BUFSIZE + 1] = {"/friends_list"};

    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}

void Send_wp() {
    int len, retval;
    // '\n' 문자 제거
    char sendbuf[BUFSIZE + 1] = { "/whispers_list" };

    len = strlen(sendbuf);
    if (sendbuf[len - 1] == '\n')
        sendbuf[len - 1] = '\0';
    retval = sendto(sock, sendbuf, len, 0,
        (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}*/

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;
    // 데이터 통신에 사용할 변수
    int len;
    
    HANDLE hThread = CreateThread(NULL, 0, ClientRecv, (LPVOID)sock, 0, NULL);
    // 서버와 데이터 통신
    while (1) {
        // 데이터 입력
        WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기
        if (strlen(buf) == 0) {
            EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
            SetEvent(hReadEvent); // 읽기 완료 알리기
            continue;
        }

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
        EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        SetEvent(hReadEvent); // 읽기 완료 알리기
    }
    return 0;
}
