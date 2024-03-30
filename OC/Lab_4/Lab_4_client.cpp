#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#pragma warning(disable:4996)

/*
Вариант 25-26.
С процесса-сервера запускается n процессов клиентов. На
клиенте вводится текст, который затем отсылается на сервер. На
сервере происходит анализ: в полученном сообщении подсчитывается частота появления каждого символа. Далее информация отсылается на клиент.
*/

#define SECOND 1000

struct Map
{
	std::vector <int> nums;
	std::vector <char> symbols;
};





int main(int argc, char* argv[]) {
	//Загрузка библиотеки 
	WSAData wsaData; //создаем структуру для загрузки
	WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock
	if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения
		std::cerr << "Error: failed to link library.\n";
		return 1;
	}
	//Заполняем информацию об адресе сокета
	SOCKADDR_IN addr;
	int sizeOfAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	//проверка на подключение к серверу
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr))) {
		std::cerr << "Error: failed connect to the server.\n";
		return 1;
	}
	std::cout << "Client Connected to server!\n";
	std::string input;
	std::cout << "Enter string: ";
	std::getline(std::cin, input);
	int len = input.length();
	send(Connection, (char*)&len, sizeof(int), NULL);
	char* msg;
	msg = new char[len + 1];
	for (int i = 0; i < len; ++i)
		msg[i] = input[i];
	msg[len] = '\0';
	send(Connection, msg, len, NULL);
	Map result;
	int res_len;
	recv(Connection, (char*)&res_len, sizeof(int), NULL);
	char* symbols = new char[res_len];
	recv(Connection, symbols, res_len, NULL);
	res_len *= sizeof(int);
	char* res = new char[res_len];
	recv(Connection, res, res_len, NULL);
	int* nums = (int*)res;
	for (int i = 0; i < res_len/sizeof(int); ++i)
	{
		std::cout << "Symbol: " << symbols[i] <<
			" - " << nums[i] << std::endl;
	}
	std::string i;
	std::getline(std::cin, i);
	std::cout << "Server has been stopped\n";
	if (closesocket(Connection) == SOCKET_ERROR)
		std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
	WSACleanup();
	std::cout << "Correct exit done\n";
	delete[] msg;
	delete[] res;
	delete[] symbols;
	return 0;
}