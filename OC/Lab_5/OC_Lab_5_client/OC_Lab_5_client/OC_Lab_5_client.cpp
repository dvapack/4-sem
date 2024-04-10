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
		tourist.money;
	if (tourist.in_hotel)
		std::cout << " is in hotel\n";
	else std::cout << " is in savanna\n";
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
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr))) 
	{
		std::cerr << "Error: failed to connect to the server.\n";
		return 1;
	}
	HANDLE hSemaphore;
	//открываем семафор
	hSemaphore = OpenSemaphore(SYNCHRONIZE, FALSE, L"semaphore");
	if (hSemaphore == NULL) {
		std::cerr << "Error open semaphore.\n";
		return GetLastError();
	}
	HANDLE event = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Available");
	if (!event)
		std::cerr << GetLastError() << std::endl;
	// останавливает выполнение программы пока семафор не окажется
	// в сигнальном состоянии
	WaitForSingleObject(hSemaphore, INFINITE);
	Tourist tourist;
	int num_of_client;
	// сначала отправляем количество денег
	if (send(Connection, (char*)&tourist.money, sizeof(int), NULL) == -1)
	{
		std::cerr << "Error, server seems to be closed\n";
		if (closesocket(Connection) == SOCKET_ERROR)
			std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
		WSACleanup();
		// чтобы консоль не закрывалась
		std::string i;
		std::getline(std::cin, i);
		return 1;
	}
	std::cout << "Money sent successfully\n";
	// получаем результат
	if (recv(Connection, (char*)&tourist.in_hotel, sizeof(bool), NULL) == -1)
	{
		std::cerr << "Error, server seems to be closed\n";
		if (closesocket(Connection) == SOCKET_ERROR)
			std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
		WSACleanup();
		// чтобы консоль не закрывалась
		std::string i;
		std::getline(std::cin, i);
		return 1;
	}
	std::cout << "Res received successfully\n";
	// теперь номер клиента
	if (recv(Connection, (char*)&num_of_client, sizeof(int), NULL) == -1)
	{
		std::cerr << "Error, server seems to be closed\n";
		if (closesocket(Connection) == SOCKET_ERROR)
			std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
		WSACleanup();
		// чтобы консоль не закрывалась
		std::string i;
		std::getline(std::cin, i);
		return 1;
	}
	SetEvent(event);
	std::cout << "Client num received successfully\n";
		//std::cerr << GetLastError << std::endl;;
	output(num_of_client, tourist);
	CloseHandle(event);
	CloseHandle(hSemaphore);
	std::cout << "CLient has been stopped\n";
	if (closesocket(Connection) == SOCKET_ERROR)
		std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
	WSACleanup();
	std::cout << "Correct exit done\n";
	// чтобы консоль не закрывалась
	std::string i;
	std::getline(std::cin, i);
	return 0;
}
