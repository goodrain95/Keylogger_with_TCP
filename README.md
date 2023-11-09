# Keylogger
Capture keyboard input, and then send it to a server.

# How project works 
- EventHandler가 Event(key 입력 들어옴)가 발생하면 API를 사용해 window 화면과 내가 만든 secondary Program가 정보를 받음.
- 이 프로그램은 프론트 화면에서 보이지 않으며, 기기 전원이 켜지면 별다른 명령 없이 자동으로 동작함.
- 클라이언트가 수집한 데이터를 특정 서버로 전송함.(TCP 통신 사용)

# 실행 방법
- server 프로그램을 먼저 실행하고 client 서버를 그 다음에 실행할 것.
- client 프로그램은 프로젝트 속성에서 링커-시스템-하위 시스템을 창(/SUBSYSTEM:WINDOWS)으로 바꾸어야 실행 화면에 뜨지 않음. (! main()도 콘솔형과 다르게 int WINAPI WinMain 사용.)
  ![스크린샷 2023-11-09 234053](https://github.com/goodrain95/Keylogger_with_TCP/assets/143669574/82dc6301-7e2c-4de4-9692-2fcce1d22016)
- server 프로그램은 추가 설정 없이 컴파일하면 됨.

## IDE
- OS: Windows 11
- Program: VISUAL STUDIO 2022
- Language Package: C, C++
- how to compile: push Ctrl + F5
