#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>



namespace OCLab6server {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^ header_background;
	private: System::Windows::Forms::Label^ header_server;
	private: System::Windows::Forms::Button^ button_close;
	private: System::Windows::Forms::TextBox^ input_client_num;



	private: System::Windows::Forms::PictureBox^ client_num_background;
	private: System::Windows::Forms::Label^ text_input_client_num;
	private: System::Windows::Forms::PictureBox^ current_client_result;
	private: System::Windows::Forms::Label^ text_current_client;
	private: System::Windows::Forms::TextBox^ current_client_output;
	private: System::Windows::Forms::PictureBox^ log_background;
	private: System::Windows::Forms::Label^ text_log;


	private: System::Windows::Forms::TextBox^ log_output;

	private: System::Windows::Forms::Label^ text_clients_max_amount;

	private: System::Windows::Forms::Button^ clients_amount_input_button;

		int clients_num;
		int counter = 0;
		array< SOCKET >^ connections = gcnew array< SOCKET >(6);

		List <int> nums;
		List <char> symbols;

		void output(int len, char* message)
		{
			current_client_output->Text = "String: ";
			log_output->Text += "#########Client num #" + System::Convert::ToString(counter) + " #########";
			log_output->Text += "\nString: ";
			for (int i = 0; i < len; ++i)
			{
				current_client_output->Text += message[i];
				log_output->Text += message[i];
			}
			for (int i = 0; i < nums.Count; ++i)
			{
				current_client_output->Text += "\nSymbol: " + symbols[i] +
					" - " + System::Convert::ToString(nums[i]) + "\n";
				log_output->Text += "\nSymbol: " + symbols[i] +
					" - " + System::Convert::ToString(nums[i]) + "\n";
			}
		}

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

		void string_check(char* msg) // подсчитывает количество каждого символа в строке
		{
			std::string mes(msg);
			int i = 0;
			while (mes.length() > 0)
			{
				symbols.Add(mes[0]);
				nums.Add(0);
				char symbol = mes[0];
				for (int j = 0; j < mes.length(); ++j)
				{
					if (symbol == mes[j])
					{
						++nums[i];
						mes.erase(mes.begin() + j);
						--j;
					}
				}
				++i;
			}
		}
		
		bool check_input(String^& input) // проверка на инт
		{
			input = input_client_num->Text;
			for (int i = 0; i < input->Length; ++i)
				if (!isdigit(input[i]))
					return false;
			int n = System::Convert::ToInt32(input);
			if (n >= 1 && n <= 6)
				return true;
			return false;
		}

		void main_function()
		{
			//Загрузка библиотеки 
			WSAData wsaData; //создаем структуру для загрузки
			WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock
			if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения
				//std::cerr << "Error: failed to link library.\n";
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
				//printf("Error bind %d\n", WSAGetLastError());
				closesocket(sListen);
				WSACleanup();
			} //подключение прослушивания с максимальной очередью
			if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) {
				//std::cout << "Listen failed;\n";
				closesocket(sListen);
				WSACleanup();
			}
			SOCKET newConnection;
			for (int i = 0; i < clients_num; ++i)
			{
				start_client();
				newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
				if (!newConnection)
				{
					//std::cout << "Error in connect!\n";
					if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
						std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
					if (clients_num == 1)
					{
						WSACleanup();
						//return 1;
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
						//std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
						if (clients_num == 1)
						{
							WSACleanup();
							//return 1;
						}
						continue;
					}
					// затем саму строку
					char* message;
					message = new char[len + 1];
					message[len] = '\0';
					if (recv(newConnection, message, len, NULL) == -1)
					{
						//std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
						if (clients_num == 1)
						{
							WSACleanup();
							//return 1;
						}
						continue;
					}
					// проводим подсчёт символов
					string_check(message);
					output(len, message);
					// сохраняем длину отправляемого сообщения
					int nums_res_len = nums.Count;
					int* numbers = new int[nums_res_len];
					char* Symbols = new char[nums_res_len];
					for (int i = 0; i < nums_res_len; ++i)
					{
						numbers[i] = nums[i];
						Symbols[i] = symbols[i];
					}
					char* nums_res = (char*)numbers;
					// отправляем длину
					if (send(newConnection, (char*)&nums_res_len, sizeof(int), NULL) == -1)
					{
						//std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
						if (clients_num == 1)
						{
							WSACleanup();
							//return 1;
						}
						continue;
					}
					// отправляем символы отдельно
					if (send(newConnection, Symbols, nums_res_len, NULL) == -1)
					{
						//std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
						if (clients_num == 1)
						{
							WSACleanup();
							//return 1;
						}
						continue;
					}
					//отправляем цифры отдельно
					if (send(newConnection, nums_res, sizeof(int) * nums_res_len, NULL) == -1)
					{
						//std::cerr << "Error, client num " << i + 1 << " seems to be closed\n";
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
						if (clients_num == 1)
						{
							WSACleanup();
							//return 1;
						}
						continue;
					}
					//delete[] nums;
					//delete[] symbols;
				}
				connections[i] = newConnection;
				++counter;
			}
			if (closesocket(sListen) == SOCKET_ERROR)
				//std::cerr << "Failed to terminate connection.\n Error code: " << WSAGetLastError();
			WSACleanup();
		}

		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->header_background = (gcnew System::Windows::Forms::PictureBox());
			this->header_server = (gcnew System::Windows::Forms::Label());
			this->button_close = (gcnew System::Windows::Forms::Button());
			this->input_client_num = (gcnew System::Windows::Forms::TextBox());
			this->client_num_background = (gcnew System::Windows::Forms::PictureBox());
			this->text_input_client_num = (gcnew System::Windows::Forms::Label());
			this->current_client_result = (gcnew System::Windows::Forms::PictureBox());
			this->text_current_client = (gcnew System::Windows::Forms::Label());
			this->current_client_output = (gcnew System::Windows::Forms::TextBox());
			this->log_background = (gcnew System::Windows::Forms::PictureBox());
			this->text_log = (gcnew System::Windows::Forms::Label());
			this->log_output = (gcnew System::Windows::Forms::TextBox());
			this->text_clients_max_amount = (gcnew System::Windows::Forms::Label());
			this->clients_amount_input_button = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->client_num_background))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->current_client_result))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->log_background))->BeginInit();
			this->SuspendLayout();
			// 
			// header_background
			// 
			this->header_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->header_background->Location = System::Drawing::Point(0, 0);
			this->header_background->Name = L"header_background";
			this->header_background->Size = System::Drawing::Size(750, 60);
			this->header_background->TabIndex = 0;
			this->header_background->TabStop = false;
			this->header_background->Click += gcnew System::EventHandler(this, &MainForm::header_background_Click);
			// 
			// header_server
			// 
			this->header_server->AutoSize = true;
			this->header_server->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->header_server->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->header_server->Font = (gcnew System::Drawing::Font(L"Corbel", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->header_server->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(31)), static_cast<System::Int32>(static_cast<System::Byte>(92)),
				static_cast<System::Int32>(static_cast<System::Byte>(182)));
			this->header_server->Location = System::Drawing::Point(22, 15);
			this->header_server->Name = L"header_server";
			this->header_server->Size = System::Drawing::Size(89, 29);
			this->header_server->TabIndex = 1;
			this->header_server->Text = L"Сервер";
			// 
			// button_close
			// 
			this->button_close->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->button_close->FlatAppearance->BorderSize = 0;
			this->button_close->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_close->Font = (gcnew System::Drawing::Font(L"Corbel", 18, System::Drawing::FontStyle::Bold));
			this->button_close->Location = System::Drawing::Point(614, 7);
			this->button_close->Name = L"button_close";
			this->button_close->Size = System::Drawing::Size(124, 45);
			this->button_close->TabIndex = 2;
			this->button_close->Text = L"Выход";
			this->button_close->UseVisualStyleBackColor = false;
			this->button_close->Click += gcnew System::EventHandler(this, &MainForm::button_close_Click);
			// 
			// input_client_num
			// 
			this->input_client_num->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->input_client_num->Font = (gcnew System::Drawing::Font(L"Corbel", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->input_client_num->Location = System::Drawing::Point(39, 133);
			this->input_client_num->Name = L"input_client_num";
			this->input_client_num->Size = System::Drawing::Size(164, 23);
			this->input_client_num->TabIndex = 3;
			// 
			// client_num_background
			// 
			this->client_num_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->client_num_background->Location = System::Drawing::Point(27, 82);
			this->client_num_background->Name = L"client_num_background";
			this->client_num_background->Size = System::Drawing::Size(241, 107);
			this->client_num_background->TabIndex = 4;
			this->client_num_background->TabStop = false;
			this->client_num_background->Click += gcnew System::EventHandler(this, &MainForm::client_num_background_Click);
			// 
			// text_input_client_num
			// 
			this->text_input_client_num->AutoSize = true;
			this->text_input_client_num->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->text_input_client_num->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->text_input_client_num->Font = (gcnew System::Drawing::Font(L"Corbel", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->text_input_client_num->ForeColor = System::Drawing::SystemColors::ControlText;
			this->text_input_client_num->Location = System::Drawing::Point(55, 96);
			this->text_input_client_num->Name = L"text_input_client_num";
			this->text_input_client_num->Size = System::Drawing::Size(194, 23);
			this->text_input_client_num->TabIndex = 5;
			this->text_input_client_num->Text = L"Количество клиентов";
			// 
			// current_client_result
			// 
			this->current_client_result->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->current_client_result->Location = System::Drawing::Point(27, 206);
			this->current_client_result->Name = L"current_client_result";
			this->current_client_result->Size = System::Drawing::Size(241, 329);
			this->current_client_result->TabIndex = 6;
			this->current_client_result->TabStop = false;
			// 
			// text_current_client
			// 
			this->text_current_client->AutoSize = true;
			this->text_current_client->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->text_current_client->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->text_current_client->Font = (gcnew System::Drawing::Font(L"Corbel", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->text_current_client->ForeColor = System::Drawing::SystemColors::ControlText;
			this->text_current_client->Location = System::Drawing::Point(50, 220);
			this->text_current_client->Name = L"text_current_client";
			this->text_current_client->Size = System::Drawing::Size(173, 23);
			this->text_current_client->TabIndex = 7;
			this->text_current_client->Text = L"Текущий клиент №";
			// 
			// current_client_output
			// 
			this->current_client_output->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->current_client_output->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->current_client_output->Location = System::Drawing::Point(39, 257);
			this->current_client_output->Multiline = true;
			this->current_client_output->Name = L"current_client_output";
			this->current_client_output->ReadOnly = true;
			this->current_client_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->current_client_output->Size = System::Drawing::Size(219, 265);
			this->current_client_output->TabIndex = 9;
			// 
			// log_background
			// 
			this->log_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->log_background->Location = System::Drawing::Point(301, 82);
			this->log_background->Name = L"log_background";
			this->log_background->Size = System::Drawing::Size(422, 453);
			this->log_background->TabIndex = 10;
			this->log_background->TabStop = false;
			// 
			// text_log
			// 
			this->text_log->AutoSize = true;
			this->text_log->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->text_log->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->text_log->Font = (gcnew System::Drawing::Font(L"Corbel", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->text_log->ForeColor = System::Drawing::SystemColors::ControlText;
			this->text_log->Location = System::Drawing::Point(496, 96);
			this->text_log->Name = L"text_log";
			this->text_log->Size = System::Drawing::Size(42, 23);
			this->text_log->TabIndex = 11;
			this->text_log->Text = L"Лог";
			// 
			// log_output
			// 
			this->log_output->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->log_output->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->log_output->Location = System::Drawing::Point(314, 131);
			this->log_output->Multiline = true;
			this->log_output->Name = L"log_output";
			this->log_output->ReadOnly = true;
			this->log_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->log_output->Size = System::Drawing::Size(396, 391);
			this->log_output->TabIndex = 12;
			this->log_output->TextChanged += gcnew System::EventHandler(this, &MainForm::log_output_TextChanged);
			// 
			// text_clients_max_amount
			// 
			this->text_clients_max_amount->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->text_clients_max_amount->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->text_clients_max_amount->Font = (gcnew System::Drawing::Font(L"Corbel", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->text_clients_max_amount->ForeColor = System::Drawing::SystemColors::ControlText;
			this->text_clients_max_amount->ImageAlign = System::Drawing::ContentAlignment::TopLeft;
			this->text_clients_max_amount->Location = System::Drawing::Point(39, 159);
			this->text_clients_max_amount->Name = L"text_clients_max_amount";
			this->text_clients_max_amount->Size = System::Drawing::Size(219, 23);
			this->text_clients_max_amount->TabIndex = 13;
			this->text_clients_max_amount->Text = L"1 <= n <= 6";
			this->text_clients_max_amount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->text_clients_max_amount->UseMnemonic = false;
			// 
			// clients_amount_input_button
			// 
			this->clients_amount_input_button->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->clients_amount_input_button->FlatAppearance->BorderSize = 0;
			this->clients_amount_input_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->clients_amount_input_button->Font = (gcnew System::Drawing::Font(L"Corbel", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->clients_amount_input_button->Location = System::Drawing::Point(209, 133);
			this->clients_amount_input_button->Name = L"clients_amount_input_button";
			this->clients_amount_input_button->Size = System::Drawing::Size(49, 25);
			this->clients_amount_input_button->TabIndex = 14;
			this->clients_amount_input_button->Text = L"Ввод";
			this->clients_amount_input_button->UseVisualStyleBackColor = false;
			this->clients_amount_input_button->Click += gcnew System::EventHandler(this, &MainForm::clients_amount_input_button_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->ClientSize = System::Drawing::Size(750, 560);
			this->Controls->Add(this->clients_amount_input_button);
			this->Controls->Add(this->text_clients_max_amount);
			this->Controls->Add(this->log_output);
			this->Controls->Add(this->text_log);
			this->Controls->Add(this->log_background);
			this->Controls->Add(this->current_client_output);
			this->Controls->Add(this->text_current_client);
			this->Controls->Add(this->current_client_result);
			this->Controls->Add(this->text_input_client_num);
			this->Controls->Add(this->input_client_num);
			this->Controls->Add(this->button_close);
			this->Controls->Add(this->header_server);
			this->Controls->Add(this->header_background);
			this->Controls->Add(this->client_num_background);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->client_num_background))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->current_client_result))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->log_background))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}


#pragma endregion
		private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
		}


		private: System::Void button_close_Click(System::Object^ sender, System::EventArgs^ e) {
			this->Close();
		}
		private: System::Void header_background_Click(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void client_num_background_Click(System::Object^ sender, System::EventArgs^ e) {
		}

		private: System::Void log_output_TextChanged(System::Object^ sender, System::EventArgs^ e) {
		}
	
		private: System::Void input_client_num_Click(System::Object^ sender, System::EventArgs^ e) {
			this->input_client_num->ResetText();

		}
		private: System::Void clients_amount_input_button_Click(System::Object^ sender, System::EventArgs^ e) {
			String^ inp;
			if (!check_input(inp))
			{
				MessageBox::Show(this, "Input int 1 <= n <= 6", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				this->input_client_num->ResetText();
			}
			else
			{
				clients_num = System::Convert::ToInt32(inp);
				this->input_client_num->ReadOnly = true;
				text_clients_max_amount->Text = "Количество клиентов: " + System::Convert::ToString(clients_num);
				main_function();
			}
		}
	};
}
