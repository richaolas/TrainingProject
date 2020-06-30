// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define  _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>  

#include <iostream>
#include <thread>
#include <unordered_map>
using namespace std;

#include "SocketUtility.h"
#include "CharacterCodes.h"
#include "JsonUtility.h"

void recvthread(Client * c)
{
	while (1)
	{
		char recvBuf[1024] = "\0";
		int type;
		std::string s;

		int ret = c->recvMsg(type, s);
		if (ret < 0)
			break;

		std::string str = MBytes(s);

		cout << ret << " Get message " << s << endl;
	}

}

const int DEFAULT_PORT = 8000;

class UserClient : public Client
{
public:
	UserClient(SOCKET socket) : Client(socket) {}
	// msg handler
	virtual int handle_message(const std::string& msg)
	{
		Json::Value root;
		if (parse(msg, root) == 0)
		{
			std::string sender = MBytes(root["sender"].asString());
			std::string recvicer = MBytes(root["recvicer"].asString());
			std::string message = MBytes(root["message"].asString());
			//std::cout << sender << "->" << recvicer << ":" << message << std::endl;

			// just transfer
			cout << "[From]    " << sender << "\n"
				 << "[Message] " << message << endl;
			cout << "--------------------------------------------------------\n";
		}
		return 0;
	}
	virtual int handle_login_message(const std::string& msg)
	{
		Json::Value root;
		if (parse(msg, root) == 0)
		{
			std::string sender = MBytes(root["sender"].asString());
			//std::string recvicer = root["recvicer"].asString();
			//std::string message = root["message"].asString();
			std::cout << sender << " log in!" << endl;
			cout << "--------------------------------------------------------\n";
		}
		return 0;
	}
};




int sendMessage(const std::string& sender, const std::string& recvicer, const std::string& message)
{
	return 0;
}

int main(int argc, char* argv[])
{
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err, iLen;
	wVersionRequested = MAKEWORD(2, 2);//create 16bit data  
	//(1)Load WinSock  
	err = WSAStartup(wVersionRequested, &wsaData); //load win socket  
	if (err != 0)
	{
		cout << "Load WinSock Failed!";
		return -1;
	}
	//(2)create socket  
	SOCKET sockClt = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClt == INVALID_SOCKET) {
		cout << "socket() fail:" << WSAGetLastError() << endl;
		return -2;
	}
	//(3)IP  
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(DEFAULT_PORT);

	//(5)connect  
	err = connect(sockClt, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	if (err == INVALID_SOCKET)
	{
		cout << "connect() fail" << WSAGetLastError() << endl;
		return -1;
	}

	char sendBuf[1024], hostname[100];
	if (gethostname(hostname, 100) != 0)    //get host name  
		strcpy(hostname, "None");
	strcpy(sendBuf, hostname);
	strcat(sendBuf, "have connected to you!");

	cout << "1 " << sockClt << endl;
	UserClient c(sockClt);
	c.runHandler();
	//cout << "1" << c.
	
	//std::thread t(recvthread, &c);

	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;
	for (int i = 0; i < 10; i++)
	{
		item["key"] = i;
		arrayObj.append(item);
	}

	cout << "please input your name" << endl;
	string username = "x";
	cin >> username;
	root["user"] = username;
	//root["key2"] = "value2";
	//root["array"] = arrayObj;
	//root.toStyledString();
	std::string out = root.toStyledString();
	//std::cout << out << std::endl;

	strcpy(sendBuf, out.c_str());
	c.sendMsg(MSG_LOGIN, root.toStyledString());

	cout << "input user, and message (intput quit for quit)" << endl;

	while (1)
	{
		string name;
		string message;
		
		cin >> name;
		if (name == "quit")
			break;

		cin	>> message;
		
		Json::Value root;
		root["sender"] = MBytesToUTF8(username);
		root["recvicer"] = MBytesToUTF8(name);
		root["message"] = MBytesToUTF8(message);

		c.sendMsg(MSG_MESSAGE, root.toStyledString());
		cout << "--------------------------------------------------------\n";
	}


	//err = send(sockClt, sendBuf, strlen(sendBuf) + 1, 0);

	/*char recvBuf[1024] = "\0";
	iLen = recv(sockClt, recvBuf, 1024, 0);

	if (iLen == 0)
		return -3;
	else if (iLen == SOCKET_ERROR) {
		cout << "recv() fail:" << WSAGetLastError() << endl;
		return -4;
	}
	else
	{
		recvBuf[iLen] = '\0';
		cout << recvBuf << endl;
	}*/
	closesocket(sockClt);
	//if (t.joinable())
	//	t.join();
	WSACleanup();
	//system("PAUSE");
	//c.wait();
	return 0;
}