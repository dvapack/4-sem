#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

#pragma warning(disable: 4996)

/*
Вариант 25-26.
С процесса-сервера запускается n процессов клиентов. На
клиенте вводится текст, который затем отсылается на сервер. На
сервере происходит анализ: в полученном сообщении подсчитывается частота появления каждого символа. Далее информация отсылается на клиент.
*/

SOCKET Connections[100];
int counter = 0;

struct Map 
{
	std::vector <int> nums;
	std::vector <char> symbols;
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

void string_check(char* msg, Map& result) // подсчитывает количество каждого символа в строке
{
	std::string mes(msg);
	int i = 0;
	while(mes.length() > 0)
	{
		result.symbols.push_back(mes[0]);
		result.nums.push_back(0);
		char symbol = mes[0];
		for (int j = 0; j < mes.length(); ++j)
		{
			if (symbol == mes[j])
			{
				++result.nums[i];
				mes.erase(mes.begin()+j);
				--j;
			}
		}
		++i;
	}
}

void output(int len, char* message, Map result)
{
	for (int i = 0; i < len; ++i)
	{
		std::cout << message[i];
	}
	std::cout << std::endl;
	for (int i = 0; i < result.nums.size(); ++i)
	{
		std::cout << "Symbol: " << result.symbols[i] <<
			" - " << result.nums[i] << std::endl;
	}
}

bool check_input(std::string& input) // проверка на инт
{
	std::getline(std::cin, input);
	for (int i = 0; i < input.length(); ++i)
		if (!isdigit(input[i]))
			return false;
	int n = stoi(input);
	if (n >= 1 && n <= 100)
		return true;
	return false;
}

int input()
{
	std::string inp;
	std::cout << "Enter a positive num 1 <= n <= 100: ";
	while (!check_input(inp))
		std::cout << "Error! Enter a positive num 1 <= n <= 100\n";
	return stoi(inp);
}

int main(int argc, char* argv[])
{
	int n = input();
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
	SOCKET newConnection;
	for (int i = 0; i < n; ++i)
	{
		start_client();
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
		if (!newConnection)
		{
			std::cout << "Error in connect!\n";
			if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
				std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
			if (n == 1)
			{
				WSACleanup();
				return 1;
			}
			continue;
		}
		else
		{
			std::cout << "Client Connected!\n";
			// получаем сначала длину строки
			int len;
			if (recv(newConnection, (char*)&len, sizeof(int), NULL) == -1)
			{
				std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			// затем саму строку
			char* message;
			message = new char[len + 1];
			message[len] = '\0';
			if (recv(newConnection, message, len, NULL) == -1)
			{
				std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			// проводим подсчёт символов
			Map result;
			string_check(message, result);
			output(len, message, result);
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
			if (send(newConnection, (char*)&nums_res_len, sizeof(int), NULL) == -1)
			{
				std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			// отправляем символы отдельно
			if (send(newConnection, symbols, nums_res_len, NULL) == -1)
			{
				std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			//отправляем цифры отдельно
			if (send(newConnection, nums_res, sizeof(int) * nums_res_len, NULL) == -1)
			{
				std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
				if (n == 1 && (closesocket(sListen) == SOCKET_ERROR))
					std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
				if (n == 1)
				{
					WSACleanup();
					return 1;
				}
				continue;
			}
			delete[] nums;
			delete[] symbols;
		}
		Connections[i] = newConnection;
		++counter;
	}
	if (closesocket(sListen) == SOCKET_ERROR)
		std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
	WSACleanup();
	return 0;
}
