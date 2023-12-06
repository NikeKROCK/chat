
#include <thread>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <inaddr.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
void readere(SOCKET soc, vector<char> redbuf) {
	while (soc!=INVALID_SOCKET){
		short pac = recv(soc, redbuf.data(), redbuf.size(), 0);
	if (pac > 0) {
		cout << redbuf.data();
	}
    }
}
int main(void)
{
	setlocale(LC_ALL, "rus");
	 vector<char> buf(1024);
     vector<char> redbuf(1024);
	 cout << "введите IPv4\n";
	 char server_ip[] = "";
	 cin >> server_ip;
	 int PORT = 0;
	 cout << "введите порт\n";
	 cin >> PORT;
	int error;
	in_addr ip;
	error = inet_pton(AF_INET, server_ip, &ip);
	if (!(error > 0)) {
		cout << "IP ошибка перевода\n" << endl;
		return 1;
	}
	WSADATA Data;
	error = WSAStartup(MAKEWORD(2, 2), &Data);
	if (error!= 0) {
		cout << "WinSock ошибка инициализации ";
		cout << WSAGetLastError()<<'\n';
		return 1;
	}
	SOCKET SerSock = socket(AF_INET, SOCK_STREAM, 0);
	if (SerSock == INVALID_SOCKET) {
		cout << "ошибка инициализации сокета " << WSAGetLastError() << '\n';
		closesocket(SerSock);
		WSACleanup();
		return 1;
	}
	sockaddr_in serInf;
	ZeroMemory(&serInf, sizeof(serInf));
	serInf.sin_family = AF_INET;
	serInf.sin_addr = ip;
	serInf.sin_port = htons(PORT);
	error = bind(SerSock, (sockaddr*)&serInf, sizeof(serInf));
	if (error != 0) {
		cout << "ошибка подключения сокета к данным клиента, номер ошибки " << WSAGetLastError() << '\n';
		closesocket(SerSock);
		WSACleanup();
		return 1;
	}
	error = listen(SerSock, SOMAXCONN);
	if (error != 0) {
		cout << "ошибка прослушивания " << WSAGetLastError() << endl;
		closesocket(SerSock);
		WSACleanup();
		return 1;
	}
	cout << "инициализация прошла";
	sockaddr_in clientInf;
	ZeroMemory(&clientInf, sizeof(clientInf));	
	int clientInf_size = sizeof(clientInf);
	SOCKET ClientCon = accept(SerSock, (sockaddr*)&clientInf, &clientInf_size);
	if (ClientCon == INVALID_SOCKET) {
		cout << "клиент есть, но не подключился, ошибка номер " << WSAGetLastError() << '\n';
		closesocket(SerSock);
		closesocket(ClientCon);
		WSACleanup();
		return 1;
	}
	char client_IP[22];
	inet_ntop(AF_INET, &clientInf.sin_addr, client_IP, INET_ADDRSTRLEN);
	cout << "клиент подключился, его IP: " << client_IP << endl;
	while (true) {
		thread lstn(readere,ClientCon,std::ref(redbuf));
		lstn.detach();
		fgets(buf.data(), buf.size(), stdin);
			if (buf[0] == '/' && buf[1] == 'c' && buf[2] == 'l' && buf[3] == 'o' && buf[4] == 's' && buf[5] == 'e') {
				shutdown(ClientCon, SD_BOTH);
				closesocket(SerSock);
				closesocket(ClientCon);
				WSACleanup();
				return 0;
			}
			short message = send(ClientCon, buf.data(), buf.size(), 0);
			if (message == SOCKET_ERROR) {
				cout << "нельзя отправить, ошибка " << WSAGetLastError() << '\n';
				closesocket(ClientCon);
				closesocket(SerSock);
				WSACleanup();
				return 1;
			}
	}
	closesocket(SerSock);
	closesocket(ClientCon);
	WSACleanup();

	return 0;

}
