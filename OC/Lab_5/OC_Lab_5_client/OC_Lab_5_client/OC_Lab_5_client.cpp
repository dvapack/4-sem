#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#pragma warning(disable:4996)

/*
Вариант 26.
Семейный отель в Эфиопии. В отеле есть 4 номера с ценой
500 эфиопских быров за ночь, 2 номера с ценой 700 эфиопских
быров за ночь и 2 номера с ценой 900 эфиопских быров за ночь.
Туристы, решившие посетить город Лалибэлу полюбоваться
высеченными в скалах церквями, имеют определенные финансовые
ресурсы и могут снять номер, если хватает денег. Если денег не
хватает, то, пользуясь хорошей погодой, туристы идут ночевать в
саванну. Задача: промоделировать работу семейного отеля. Отель
– сервер, клиенты – отдельные процессы-клиенты. В конце работы
вывести статистику работы отеля, сколько клиентов было и какой
доход получен.
*/

#define SECOND 1000

struct Tourist
{
	int money;
	bool in_hotel;

	Tourist()
	{
		std::random_device rd; // Получение случайного устройства
		std::mt19937 gen(rd()); // Инициализация генератора случайных чисел
		std::uniform_int_distribution<> dist(0, 900); // Равномерное распределение от 0 до 900
		money = dist(gen); // Генерация случайного числа
		in_hotel = false;
	}
};

void output(int num, Tourist tourist)
{
	std::cout << "Tourist # " << num << " with " <<
		tourist.money << tourist.in_hotel ? " is in hotel" : " is in savanna";
}

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
	Tourist tourist;
	int num_of_client;
	// сначала отправляем количество денег
	send(Connection, (char*)&tourist.money, sizeof(int), NULL);
	// получаем результат
	recv(Connection, (char*)&tourist.in_hotel, sizeof(bool), NULL);
	// теперь номер клиента
	recv(Connection, (char*)&num_of_client, sizeof(int), NULL);
	output(num_of_client, tourist);
	std::cout << "Server has been stopped\n";
	if (closesocket(Connection) == SOCKET_ERROR)
		std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
	WSACleanup();
	std::cout << "Correct exit done\n";
	// чтобы консоль не закрывалась
	std::string i;
	std::getline(std::cin, i);
	return 0;
}