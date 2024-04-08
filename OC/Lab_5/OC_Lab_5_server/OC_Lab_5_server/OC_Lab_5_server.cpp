#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

#pragma warning(disable: 4996)

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

SOCKET Connections[100];
int counter = 0;

struct Hotel
{
	int income;
	const int cheep_room_price = 500;
	const int medium_room_price = 700;
	const int luxury_room_price = 900;
	int cheep_amount;
	int medium_amount;
	int luxury_amount;
	int cheep_available;
	int medium_available;
	int luxury_available;

	Hotel()
	{
		income = 0;
		cheep_amount = 0;
		medium_amount = 0;
		luxury_amount = 0;
		cheep_available = 4;
		medium_available = 2;
		luxury_available = 2;
	}
};

void start_client()
{
	const char* navigation_command = "cd C:\\Users\\mserg\\source\\repos\\OC_Lab_2_client\\x64\\Debug";
	const char* run_command = "start C:\\Users\\mserg\\source\\repos\\OC_Lab_2_client\\x64\\Debug\\OC_Lab_2_client.exe";
	// проверка на случай непредвиденных обстоятельств
	int navigation_failure = system(navigation_command);
	if (navigation_failure)
	{
		std::cout << "Failure in system navigation!" << '\n';
		exit(1);
	}
	// запускаем собранную программу у клиента
	system(run_command);
}



int main(int argc, char* argv[])
{
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
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //сокет для прослушивания порта
	if (bind(sListen, (SOCKADDR*)&addr, sizeOfAddr) == SOCKET_ERROR) {
		//привязка адреса сокету
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(sListen);
		WSACleanup();
		return 1;
	} //подключение прослушивания с максимальной очередью
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed;\n";
		closesocket(sListen);
		WSACleanup();
		return 1;
	}
	HANDLE hSemaphore = CreateSemaphore(NULL, 2, 2, L"semaphore");
	if (!hSemaphore)
	{
		std::cout << "Failure in semaphore creation!" << '\n';
		exit(1);
	}
	int i = 0;
	hSemaphore = CreateSemaphore(NULL, glv::max_users, glv::max_users, NULL);
	SOCKET newConnection;
	for (int i = 0; i < n; ++i)
	{
		start_client();
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
		if (!newConnection)
		{
			std::cout << "Error in connect!\n";
			if (closesocket(sListen) == SOCKET_ERROR)
				std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
			WSACleanup();
			return 1;
		}
		else
		{
			std::cout << "Client Connected!\n";
			// получаем сначала длину строки
			int len;
			recv(newConnection, (char*)&len, sizeof(int), NULL);
			// затем саму строку
			char* message;
			message = new char[len + 1];
			message[len] = '\0';
			recv(newConnection, message, len, NULL);
			// проводим подсчёт символов
			Map result;
			string_check(message, result);
			// сохраняем длину отправляемого сообщения
			int nums_res_len = result.nums.size();
			int* nums = new int[nums_res_len];
			char* symbols = new char[nums_res_len];
			for (int i = 0; i < nums_res_len; ++i)
			{
				nums[i] = result.nums[i];
				symbols[i] = result.symbols[i];
			}
			char* nums_res = (char*)nums;
			// отправляем длину
			send(newConnection, (char*)&nums_res_len, sizeof(int), NULL);
			// отправляем символы отдельно
			send(newConnection, symbols, nums_res_len, NULL);
			//отправляем цифры отдельно
			send(newConnection, nums_res, sizeof(int) * nums_res_len, NULL);
			delete[] nums;
			delete[] symbols;
		}
		Connections[i] = newConnection;
		++counter;
	}
	return 0;
}