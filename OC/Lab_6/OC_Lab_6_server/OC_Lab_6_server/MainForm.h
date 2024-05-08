#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>



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

	private: System::Windows::Forms::TextBox^ input_client_num;



	private: System::Windows::Forms::PictureBox^ client_num_background;
	private: System::Windows::Forms::Label^ text_input_client_num;



	private: System::Windows::Forms::PictureBox^ log_background;
	private: System::Windows::Forms::Label^ text_log;


	private: System::Windows::Forms::TextBox^ log_output;

	private: System::Windows::Forms::Label^ text_clients_max_amount;

	private: System::Windows::Forms::Button^ clients_amount_input_button;

		int clients_num;
		int counter = 0;
		array< SOCKET >^ connections = gcnew array< SOCKET >(6);

		List <int> nums;
	private: System::Windows::Forms::Button^ button1;
		   List <char> symbols;


		void log_output_file(std::string str)
		{
			auto time = std::chrono::system_clock::now();
			time_t cur_time = std::chrono::system_clock::to_time_t(time);
			std::ofstream file_output("output.txt", std::ios::app);
			file_output << "Client " << (counter + 1) << " | " << std::ctime(&cur_time) << std::endl;
			file_output << str << std::endl << std::endl;

		}

		void output(int len, char* message)
		{

			log_output->Text += "### Client num #" + System::Convert::ToString(counter + 1) + " ###" + System::Environment::NewLine;
			log_output->Text += "String: ";
			for (int i = 0; i < len; ++i)
			{
				log_output->Text += System::Convert::ToChar(message[i]);
			}
			log_output->Text += System::Environment::NewLine;
			for (int i = 0; i < nums.Count; ++i)
			{
				log_output->Text += "Symbol: " + System::Convert::ToChar(symbols[i]) +
					" - " + System::Convert::ToString(nums[i]) + System::Environment::NewLine;
			}
		}

		void start_client()
		{
			const char* navigation_command = "cd C:\\Users\\mserg\\source\\repos\\OC_Lab_6_client\\x64\\Debug";
			const char* run_command = "start C:\\Users\\mserg\\source\\repos\\OC_Lab_6_client\\x64\\Debug\\OC_Lab_6_client.exe";
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
			try {
				if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения
					throw gcnew Exception;
				}
			}
			catch (Exception^)
			{
				log_output_file("Error: failed to link library.");
			}
			//Заполняем информацию об адресе сокета
			SOCKADDR_IN addr;
			int sizeOfAddr = sizeof(addr);
			addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			addr.sin_port = htons(1111);
			addr.sin_family = AF_INET;
			SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //сокет для прослушивания порта
			try {
				if (bind(sListen, (SOCKADDR*)&addr, sizeOfAddr) == SOCKET_ERROR) { //привязка адреса сокету
					throw gcnew Exception;
				}
			}
			catch (Exception^)
			{
				log_output_file("ошибка привязки к сокету");
				closesocket(sListen);
				WSACleanup();
				MessageBox::Show(this, "ошибка привязки к сокету", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
			try {
				if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) { //подключение прослушивания с максимальной очередью
					throw gcnew Exception;
				}
			}
			catch (Exception^)
			{
				log_output_file("ошибка прослушивания");
				closesocket(sListen);
				WSACleanup();
				MessageBox::Show(this, "ошибка прослушивания", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
			SOCKET newConnection;
			for (int i = 0; i < clients_num; ++i)
			{
				start_client();
				newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
					if (!newConnection)
					{
						log_output_file("Error in connect!");
						MessageBox::Show(this, "Error in connect!", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
						if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
						{
							log_output_file("Failed to terminate connection");
							MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
						}
						if (clients_num == 1)
							WSACleanup();
						continue;
					}
					else
					{
						log_output_file("##################################################################");
						log_output_file("Client connected");
						try {
							if (send(newConnection, (char*)&i, sizeof(int), NULL) < 0)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("Client num is sent");
						// получаем сначала длину строки
						int len;
						try {
							if (recv(newConnection, (char*)&len, sizeof(int), NULL) == -1)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("String len is recieved");
						// затем саму строку
						char* message;
						message = new char[len + 1];
						message[len] = '\0';
						try {
							if (recv(newConnection, message, len, NULL) == -1)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("String is recieved");
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
						try {
							if (send(newConnection, (char*)&nums_res_len, sizeof(int), NULL) == -1)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("String len is sent");
						// отправляем символы отдельно
						try {
							if (send(newConnection, Symbols, nums_res_len, NULL) == -1)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("Symbols are sent");
						//отправляем цифры отдельно
						try {
							if (send(newConnection, nums_res, sizeof(int) * nums_res_len, NULL) == -1)
							{
								throw gcnew Exception;
							}
						}
						catch (Exception^)
						{
							log_output_file("Client is closed");
							MessageBox::Show(this, "Client is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							if (clients_num == 1 && (closesocket(sListen) == SOCKET_ERROR))
							{
								log_output_file("Failed to terminate connection");
								MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
							}
							if (clients_num == 1)
								WSACleanup();
							continue;
						}
						log_output_file("Nums are sent");
						delete[] numbers;
						delete[] Symbols;
						nums.Clear();
						symbols.Clear();
					}
					connections[i] = newConnection;
					++counter;
				}
				if (closesocket(sListen) == SOCKET_ERROR)
				{
					log_output_file("Failed to terminate connection");
					MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
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
			this->input_client_num = (gcnew System::Windows::Forms::TextBox());
			this->client_num_background = (gcnew System::Windows::Forms::PictureBox());
			this->text_input_client_num = (gcnew System::Windows::Forms::Label());
			this->log_background = (gcnew System::Windows::Forms::PictureBox());
			this->text_log = (gcnew System::Windows::Forms::Label());
			this->log_output = (gcnew System::Windows::Forms::TextBox());
			this->text_clients_max_amount = (gcnew System::Windows::Forms::Label());
			this->clients_amount_input_button = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->client_num_background))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->log_background))->BeginInit();
			this->SuspendLayout();
			// 
			// header_background
			// 
			this->header_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->header_background->Location = System::Drawing::Point(0, 0);
			this->header_background->Name = L"header_background";
			this->header_background->Size = System::Drawing::Size(296, 60);
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
			// log_background
			// 
			this->log_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->log_background->Location = System::Drawing::Point(27, 195);
			this->log_background->Name = L"log_background";
			this->log_background->Size = System::Drawing::Size(241, 353);
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
			this->text_log->Location = System::Drawing::Point(127, 202);
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
			this->log_output->Location = System::Drawing::Point(39, 232);
			this->log_output->Multiline = true;
			this->log_output->Name = L"log_output";
			this->log_output->ReadOnly = true;
			this->log_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->log_output->Size = System::Drawing::Size(219, 303);
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
			// button1
			// 
			this->button1->Location = System::Drawing::Point(193, 21);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 15;
			this->button1->Text = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->ClientSize = System::Drawing::Size(296, 560);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->clients_amount_input_button);
			this->Controls->Add(this->text_clients_max_amount);
			this->Controls->Add(this->log_output);
			this->Controls->Add(this->text_log);
			this->Controls->Add(this->log_background);
			this->Controls->Add(this->text_input_client_num);
			this->Controls->Add(this->input_client_num);
			this->Controls->Add(this->header_server);
			this->Controls->Add(this->header_background);
			this->Controls->Add(this->client_num_background);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MainForm";
			this->RightToLeftLayout = true;
			this->Text = L"Server";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->client_num_background))->EndInit();
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
				clients_amount_input_button->Enabled = false;
				text_clients_max_amount->Text = "Количество клиентов: " + System::Convert::ToString(clients_num);
				try {
					main_function();
				}
				catch (Exception^)
				{
					Application::Exit();
				}
			}
		}
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		MessageBox::Show(this, "Создатель этой работы Мещеряков Сергей", "Info", MessageBoxButtons::OK, MessageBoxIcon::Information);
	}
};
}
