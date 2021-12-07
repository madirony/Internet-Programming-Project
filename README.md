§ Civil(11) Team Internet-Programming-Project §
================================================
★ Team Member Info ★
-----------------
- 연정흠
- 신용훈
- 김은빈

★ Team Project Objective ★
-----------------
- UDP 서버-클라이언트 기반 다중 채팅 메신저 프로그램 구현
  - 닉네임 설정
  - 귓속말
  - 이모티콘
    - 이모지 정보로 클라이언트에서 이모지 출력
  - 친구 추가
  - GUI 구현 (NATEON 구버전 UI 오마주)
  - 부가적인 서버 기능
    - 클라이언트 강제 퇴장(x)
    - 불건전 채팅 필터링

★ Project Progress ★
------------------
![Alt "주차 별 계획과 진행 상황"](/img/plan.jpg)
- 닉네임 변경 기능 추가
- 닉네임 중복 검사 (로컬 환경에서는 ip가 같아서 불가/원격 환경에서 테스트)
  - 구현은 했지만 발표 때 제외 예정
- 이모티콘 (구현완료)
  - 이모티콘 명령어 입력 시, 누가 보낸 이모티콘인지 식별 가능하도록 구현
  - 이모티콘 클릭 시, 이모티콘 제거
- 악성 클라이언트 강퇴 기능(구현x)
  - 불건전 채팅 필터링으로 강퇴 대신 채팅 블라인드 처리
- 친구 목록
  - 친구 온라인/오프라인 여부 추가
- 귓속말 목록
  - 받은 귓속말을 저장하여 쪽지 기능을 대신 하도록 구현
- 로그인 시, 접속 효과음 재생

★ Algorithm & Function ★
------------------
- 멀티스레드로 데이터 송신과 수신을 동시에 가능하도록 구현
- 닉네임 공백 제거
  - 닉네임 변경 시에도 공백 문자가 들어간 닉네임 불가
  ![Alt "클라이언트의 닉네임 공백 제거"](/img/erasedSpace.jpg)
- 불건전 채팅 필터링
  - 클라이언트가 보낸 채팅을 서버가 분석하여 욕설이 있을 시 채팅 자체를 블라인드 처리 하는 알고리즘 함수
  ![Alt "악성 워딩 필터링 함수"](/img/filtering.jpg)
- UDP 통신에서 발생할 수 있는 문제점 해결
  - 첫 접속이 발생했을 경우 서버가 소켓을 오픈
    - 첫 접속 클라이언트를 위한 코드 추가 작성
  - 비연결 지향 프로토콜의 문제점 해결
    - 접속 여부 변수를 따로 두어 온라인 여부 판단
    - 온라인이면 전체 채팅 수신 가능
    - 친구로 등록된 유저의 접속 여부 확인 가능
- 유저 구분 알고리즘
  - ip주소/port번호/닉네임으로 유저 식별
- 채팅 식별 알고리즘
  - strtok함수로 채팅을 split하여 명령어 여부를 판단
  - 명령어(귓속말/친구추가/친구목록/이모티콘/도움말/닉네임변경/귓속말목록)
  - 잘못된 명령어나 없는 사용자에게 귓속말/친구추가를 할 경우 서버에서 관련된 시스템 메시지를 클라이언트에게 전송
- 귓속말 기능으로 1:1 통신 구현
  - 귓속말 전송 시, 다른 클라이언트에게 노출되지 않는 채팅 구현
- 신규 유저 접속 시, 서버에서 공지사항 전송
  - 소켓을 여는 첫 클라이언트는 접속 후 채팅을 보내야 공지사항 받을 수 있음
  - 로그인 소리 재생 추가
- etc...
  - 서버 채팅 추가
  - 이모티콘 송신자 식별 기능
  - x 버튼 클릭하여 클라이언트 닫을 시, 오프라인 표시(친구목록)

★ Gui Example ★
-------------
- 로그인 화면  
![Alt "로그인"](/img/login.jpg)
- 클라이언트 ui  
![Alt "client ui"](/img/clientui.jpg)

★ Test Gif ★
-------------
- 유저 로그인  
![Alt "유저로그인"](/img/user_login.gif)
- 이모티콘 사용  
![Alt "이모티콘"](/img/user_emoji.gif)
- 귓속말  
![Alt "귓속말"](/img/whisFrie.gif)
- 귓속말 리스트  
![Alt "귓속말"](/img/whisList.gif)
- 닉네임 변경  
![Alt "닉변"](/img/nickChange.gif)
- 서버 채팅  
![Alt "운영자 채팅"](/img/serverChat.gif)
- 불건전 채팅 필터링  
![Alt "운영자 채팅"](/img/badChatBlind.gif)

★ Reference ★
-------------
> :: 참고 문헌과 참고 사이트 ::  
> 김선우, 『TCP/IC 윈도우 소켓 프로그래밍』, 한빛아카데미, 2013  
> DelftStack, 『Concatenate Two Strings in C++』, DelftStack, https://www.delftstack.com/howto/cpp/how-to-concatenate-two-strings-cpp/, October-23, 2020  
> DelftStack, 『Split String by Space in C++』, DelftStack, https://www.delftstack.com/ko/howto/cpp/cpp-split-string-by-space/, March-21, 2021  
> DelftStack, 『Remove Spaces From String in C++』, DelftStack, https://www.delftstack.com/ko/howto/cpp/cpp-remove-spaces-from-string/, March-21, 2021  
> DelftStack, 『Replace a Part of the String in C++』, DelftStack, https://www.delftstack.com/howto/cpp/string-replace-cpp/, November-02, 2020  
> 코딩팩토리, 『[C언어/C++] 특정 문자열 찾기/검색 &치환 strstr 함수 사용법 & 예제』, 코딩팩토리, https://coding-factory.tistory.com/677?category=767224, 2021. 2. 13.  
> mwultong, 『문자열 치환 함수, 문자열 모두 바꾸기 - String Replace All』, mwultong Blog, http://mwultong.blogspot.com/2006/10/c-string-replace-all.html, October 03, 2006  
> Anow,『(WinAPI) editbox에 줄바꿈 삽입이 안될 때』, Anow's Opportunities, https://anow.tistory.com/176, 2013. 10. 16  