#include<thread>
#include <iostream>
#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>
#include<fstream>
#include<cstdlib>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "ws2_32.lib")
using std::string;
using std::ofstream;
using std::ifstream;
using namespace std;
void readere(SOCKET soc, vector<char> redbuf) {
	while (soc != INVALID_SOCKET) {
		short pac = recv(soc, redbuf.data(), redbuf.size(), 0);
		if (pac > 0) {
			cout << redbuf.data();
		}
	}
}
int main()
{
	setlocale(LC_ALL, "rus");
	cout << "Введите IPv4\n";
	 char server_ip[] = "";
	 cin >> server_ip;
	 cout << "введите порт\n";
     short server_port = 0;	
	 cin >> server_port;
	 int error;
	in_addr ip_to_num;
	error = inet_pton(AF_INET, server_ip, &ip_to_num);
	if (!(error > 0)) {
		cout << "IP ошибка перевода\n" << endl;
		return 1;
	}
    WSADATA wsData;
	error = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error != 0) {
		cout << "ошибка ининициализации WinSock номер ";
		cout << WSAGetLastError();
		return 1;
	}
    SOCKET soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc == INVALID_SOCKET) {
		cout << "ошибка инициализации сокетаа номер " << WSAGetLastError() << endl;
		closesocket(soc);
		WSACleanup();
		return 1;
	}
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(server_port);
	error = connect(soc, (sockaddr*)&servInfo, sizeof(servInfo));
	if (error != 0) {
		cout << "соединние с сервером провалено. ошибка номер  " << WSAGetLastError() << endl;
		closesocket(soc);
		WSACleanup();
		return 1;
	}
	vector <char> redbuf(1024), buf(1024);
	short packet_size = 0;
	while (true) {
		thread lstn(readere, soc, std::ref(redbuf));
		lstn.detach();
		fgets(buf.data(), buf.size(), stdin);
		if (buf[0] == '/' && buf[1] == 'c' && buf[2] == 'l' && buf[3] == 'o' && buf[4] == 's' && buf[5] == 'e') {
			closesocket(soc);
			WSACleanup();
			return 0;
		}
		short message = send(soc, buf.data(), buf.size(), 0);
		if (message == SOCKET_ERROR) {
			cout << "нельзя отправить серверу сообщение, ошибка номер " << WSAGetLastError() << '\n';
			closesocket(soc);
			WSACleanup();
			return 1;
		}
	}

	closesocket(soc);
	WSACleanup();

	return 0;
}