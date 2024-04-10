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

struct Hotel
{
	int income;
	int cheep_available;
	int medium_available;
	int luxury_available;

	Hotel()
	{
		income = 0;
		cheep_available = 4;
		medium_available = 2;
		luxury_available = 2;
	}
};

void start_client()
{
	const char* navigation_command = "cd C:\\Users\\mserg\\source\\repos\\OC_Lab_5_client\\x64\\Release";
	const char* run_command = "start C:\\Users\\mserg\\source\\repos\\OC_Lab_5_client\\x64\\Release\\OC_Lab_5_client.exe";
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

void output(const Hotel& hotel, const int tourists)
{
	std::cout << "Income is: " << hotel.income << "\nThere were " <<
		tourists << " tourists in this hotel\n";
}

bool decision(int money, Hotel& hotel)
{
	if (hotel.luxury_available)
		if (money >= 900)
		{
			hotel.luxury_available--;
			hotel.income += 900;
			return true;
		}
	if (hotel.medium_available)
		if (money >= 700)
		{
			hotel.medium_available--;
			hotel.income += 700;
			return true;
		}
	if (hotel.cheep_available)
		if (money >= 500)
		{
			hotel.cheep_available--;
			hotel.income += 500;
			return true;
		}
	return false;
}

bool check_input(std::string& input) // проверка на инт
{
	std::getline(std::cin, input);
	for (int i = 0; i < input.length(); ++i)
		if (!isdigit(input[i]))
			return false;
	int n = stoi(input);
	if (n >= 1 && n <= 16)
		return true;
	return false;
}

int input()
{
	std::string inp;
	std::cout << "Enter a positive num 1 <= n <= 16: ";
	while (!check_input(inp))
		std::cout << "Error! Enter a positive num 1 <= n <= 16\n";
	return stoi(inp);
}


int main(int argc, char* argv[])
{
	//Загрузка библиотеки 
	WSAData wsaData; //создаем структуру для загрузки
	WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{ // проверка подключения
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
	if (bind(sListen, (SOCKADDR*)&addr, sizeOfAddr) == SOCKET_ERROR)
	{
		//привязка адреса сокету
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(sListen);
		WSACleanup();
		return 1;
	} //подключение прослушивания с максимальной очередью
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed;\n";
		closesocket(sListen);
		WSACleanup();
		return 1;
	}
	HANDLE hSemaphore;
	//Создание семафора, контролирующего до 8 обращений одновременно
	hSemaphore = CreateSemaphore(NULL, 8, 8, L"semaphore");
	if (hSemaphore == NULL)
		printf("CreateSemaphore error: %d\n", GetLastError());
	size_t tourist_number = 0;
	Hotel hotel;
	std::vector <SOCKET> Sockets(16); //вектор для сокетов
	size_t c_num = 0;
	int n = input(); //для количества резидентов (клиентов)
	HANDLE event = CreateEvent(NULL, FALSE, FALSE, L"Available");
	if (!event)
		std::cerr << GetLastError() << std::endl;
	for (uint8_t i = 0; i < n; i++)
	{ // окна для клиентов
		start_client();
	}
		for (uint8_t i = 0; i < n; i++)
		{ //сокеты для соединения с клиентом
			Sockets[i] = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
		}
		while (n)
		{
			//приостанавливает поток пока любой из объектов не перейдет
			// в сигнальное состояние или не закончится время ожидания
			DWORD dwResult = 1;
			while (dwResult != WAIT_OBJECT_0)
			{
				dwResult = WaitForSingleObject(hSemaphore, 1);
				Sleep(300);
			}
			//WaitForSingleObject(hSemaphore, INFINITE); // Ожидание доступа к отелю
			c_num = tourist_number;
			std::cout << "Hotel is ready to serve a tourist!\n";
			++tourist_number;
			std::cout << "Client " << c_num << " is ready to connect.\n";
			std::cout << "We receive info about tourist...\n";
			// получаем количество денег
			int money;
			if (recv(Sockets[c_num], (char*)&money, sizeof(int), NULL) == -1)
			{
				std::cerr << "Error, client num " << c_num << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			bool in_hotel = decision(money, hotel);
			// отправляем результат
			if (send(Sockets[c_num], (char*)&in_hotel, sizeof(bool), NULL) == -1)
			{
				std::cerr << "Error, client num " << c_num << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			// теперь номер клиента
			if (send(Sockets[c_num], (char*)&c_num, sizeof(size_t), NULL) == -1)
			{
				std::cerr << "Error, client num " << c_num << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			std::cout << "Tourist # " << tourist_number << " with " <<
				money;
			if (in_hotel)
				std::cout << " is in hotel\n";
			else std::cout << " is in savanna\n";
			std::cout << "Client " << c_num << " was disconnected.\n";

			// Сброс события, указывающего на готовность обслужить туриста
			SetEvent(event);
			// Задержка для симуляции времени обработки запроса
			Sleep(1000);
			ReleaseSemaphore(hSemaphore, 1, NULL);
			//отключение клиента
			--n;
		}
		output(hotel, tourist_number);
		std::cout << "Work completed successfully!\n";
		// Close thread handles.
		CloseHandle(event);
		for (uint8_t i = 0; i <= c_num; ++i)
			closesocket(Sockets[c_num]);
		CloseHandle(hSemaphore);
		if (closesocket(sListen) == SOCKET_ERROR)
			std::cerr << "Failed to terminate connection.\n Error code: "
			<< WSAGetLastError();
		WSACleanup();
		return 0;

}
