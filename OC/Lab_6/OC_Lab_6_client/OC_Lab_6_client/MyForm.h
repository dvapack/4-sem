#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

namespace OCLab6client {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
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
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:

		String^ inp;


	private: System::Windows::Forms::PictureBox^ header_background;
	private: System::Windows::Forms::Label^ header_client;


	private: System::Windows::Forms::PictureBox^ string_input_background;
	private: System::Windows::Forms::Label^ text_string_input_num;
	private: System::Windows::Forms::TextBox^ input_client_num;
	private: System::Windows::Forms::Button^ string_input_button;

	private: System::Windows::Forms::PictureBox^ current_client_result;
	private: System::Windows::Forms::Label^ text_current_client;
	private: System::Windows::Forms::TextBox^ current_client_output;


		   /// <summary>
		   /// Обязательная переменная конструктора.
		   /// </summary>
		   System::ComponentModel::Container^ components;

		   void output(int res_len, char* symbols, int* nums)
		   {
			   current_client_output->Text = "String: ";
			   for (int i = 0; i < inp->Length; ++i)
			   {
				   current_client_output->Text += inp[i];
			   }
			   current_client_output->Text += System::Environment::NewLine;
			   for (int i = 0; i < res_len / sizeof(int); ++i)
			   {
				   current_client_output->Text += "Symbol: " + System::Convert::ToChar(symbols[i]) +
					   " - " + System::Convert::ToString(nums[i]) + System::Environment::NewLine;
			   }
		   }


		   void main_function()
		   {
			   //Загрузка библиотеки 
			   WSAData wsaData; //создаем структуру для загрузки
			   WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock
			   if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения
				   MessageBox::Show(this, "Error: failed to link library.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
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
				   MessageBox::Show(this, "Error: failed connect to the server.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			   }
			   int client_num = 0;
			   if (recv(Connection, (char*)&client_num, sizeof(int), NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
					   WSACleanup();
			   }
			   header_client->Text = "Клиент №" + System::Convert::ToString(client_num + 1);
			   // сначала отправляем длину строки
			   int len = inp->Length;
			   if (send(Connection, (char*)&len, sizeof(int), NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   WSACleanup();
			   }
			   // потом саму строку, но преобразованную в массив чар
			   char* msg;
			   msg = new char[len + 1];
			   for (int i = 0; i < len; ++i)
				   msg[i] = inp[i];
			   msg[len] = '\0';

			   if (send(Connection, msg, len, NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   WSACleanup();
			   }
			   // получаем длину результата
			   int res_len;
			   if (recv(Connection, (char*)&res_len, sizeof(int), NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   WSACleanup();
			   }
			   // теперь символы
			   char* symbols = new char[res_len];
			   if (recv(Connection, symbols, res_len, NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   WSACleanup();
			   }
			   res_len *= sizeof(int);
			   char* res = new char[res_len];
			   // теперь цифры, но сначала как массив чаров
			   if (recv(Connection, res, res_len, NULL) == -1)
			   {
				   MessageBox::Show(this, "Server is closed", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   if (closesocket(Connection) == SOCKET_ERROR)
					   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				   WSACleanup();
			   }
			   // преобразовываем в массив интов
			   int* nums = (int*)res;
			   output(res_len, symbols, nums);
			   if (closesocket(Connection) == SOCKET_ERROR)
				   MessageBox::Show(this, "Failed to terminate connection", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			   WSACleanup();
			   
			   delete[] msg;
			   delete[] res;
			   delete[] symbols;
		   }

#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Требуемый метод для поддержки конструктора — не изменяйте 
		   /// содержимое этого метода с помощью редактора кода.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   this->header_background = (gcnew System::Windows::Forms::PictureBox());
			   this->header_client = (gcnew System::Windows::Forms::Label());
			   this->string_input_background = (gcnew System::Windows::Forms::PictureBox());
			   this->text_string_input_num = (gcnew System::Windows::Forms::Label());
			   this->input_client_num = (gcnew System::Windows::Forms::TextBox());
			   this->string_input_button = (gcnew System::Windows::Forms::Button());
			   this->current_client_result = (gcnew System::Windows::Forms::PictureBox());
			   this->text_current_client = (gcnew System::Windows::Forms::Label());
			   this->current_client_output = (gcnew System::Windows::Forms::TextBox());
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->string_input_background))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->current_client_result))->BeginInit();
			   this->SuspendLayout();
			   // 
			   // header_background
			   // 
			   this->header_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->header_background->Location = System::Drawing::Point(0, 0);
			   this->header_background->Name = L"header_background";
			   this->header_background->Size = System::Drawing::Size(286, 60);
			   this->header_background->TabIndex = 1;
			   this->header_background->TabStop = false;
			   // 
			   // header_client
			   // 
			   this->header_client->AutoSize = true;
			   this->header_client->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->header_client->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->header_client->Font = (gcnew System::Drawing::Font(L"Corbel", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->header_client->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(31)), static_cast<System::Int32>(static_cast<System::Byte>(92)),
				   static_cast<System::Int32>(static_cast<System::Byte>(182)));
			   this->header_client->Location = System::Drawing::Point(20, 15);
			   this->header_client->Name = L"header_client";
			   this->header_client->Size = System::Drawing::Size(90, 29);
			   this->header_client->TabIndex = 2;
			   this->header_client->Text = L"Клиент";
			   // 
			   // string_input_background
			   // 
			   this->string_input_background->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->string_input_background->Location = System::Drawing::Point(23, 82);
			   this->string_input_background->Name = L"string_input_background";
			   this->string_input_background->Size = System::Drawing::Size(241, 107);
			   this->string_input_background->TabIndex = 5;
			   this->string_input_background->TabStop = false;
			   // 
			   // text_string_input_num
			   // 
			   this->text_string_input_num->AutoSize = true;
			   this->text_string_input_num->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->text_string_input_num->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->text_string_input_num->Font = (gcnew System::Drawing::Font(L"Corbel", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->text_string_input_num->ForeColor = System::Drawing::SystemColors::ControlText;
			   this->text_string_input_num->Location = System::Drawing::Point(71, 93);
			   this->text_string_input_num->Name = L"text_string_input_num";
			   this->text_string_input_num->Size = System::Drawing::Size(142, 23);
			   this->text_string_input_num->TabIndex = 6;
			   this->text_string_input_num->Text = L"Введите строку";
			   this->text_string_input_num->Click += gcnew System::EventHandler(this, &MyForm::text_string_input_num_Click);
			   // 
			   // input_client_num
			   // 
			   this->input_client_num->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)),
				   static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)));
			   this->input_client_num->Font = (gcnew System::Drawing::Font(L"Corbel", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->input_client_num->Location = System::Drawing::Point(35, 129);
			   this->input_client_num->Name = L"input_client_num";
			   this->input_client_num->Size = System::Drawing::Size(164, 23);
			   this->input_client_num->TabIndex = 7;
			   this->input_client_num->TextChanged += gcnew System::EventHandler(this, &MyForm::input_client_num_TextChanged);
			   // 
			   // string_input_button
			   // 
			   this->string_input_button->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->string_input_button->FlatAppearance->BorderSize = 0;
			   this->string_input_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->string_input_button->Font = (gcnew System::Drawing::Font(L"Corbel", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(204)));
			   this->string_input_button->Location = System::Drawing::Point(205, 129);
			   this->string_input_button->Name = L"string_input_button";
			   this->string_input_button->Size = System::Drawing::Size(49, 25);
			   this->string_input_button->TabIndex = 15;
			   this->string_input_button->Text = L"Ввод";
			   this->string_input_button->UseVisualStyleBackColor = false;
			   this->string_input_button->Click += gcnew System::EventHandler(this, &MyForm::string_input_button_Click);
			   // 
			   // current_client_result
			   // 
			   this->current_client_result->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)),
				   static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			   this->current_client_result->Location = System::Drawing::Point(23, 210);
			   this->current_client_result->Name = L"current_client_result";
			   this->current_client_result->Size = System::Drawing::Size(241, 329);
			   this->current_client_result->TabIndex = 16;
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
			   this->text_current_client->Location = System::Drawing::Point(85, 226);
			   this->text_current_client->Name = L"text_current_client";
			   this->text_current_client->Size = System::Drawing::Size(114, 23);
			   this->text_current_client->TabIndex = 17;
			   this->text_current_client->Text = L"Лог клиента";
			   // 
			   // current_client_output
			   // 
			   this->current_client_output->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)),
				   static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)));
			   this->current_client_output->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->current_client_output->Location = System::Drawing::Point(35, 261);
			   this->current_client_output->Multiline = true;
			   this->current_client_output->Name = L"current_client_output";
			   this->current_client_output->ReadOnly = true;
			   this->current_client_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			   this->current_client_output->Size = System::Drawing::Size(219, 265);
			   this->current_client_output->TabIndex = 18;
			   // 
			   // MyForm
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				   static_cast<System::Int32>(static_cast<System::Byte>(242)));
			   this->ClientSize = System::Drawing::Size(286, 560);
			   this->Controls->Add(this->current_client_output);
			   this->Controls->Add(this->text_current_client);
			   this->Controls->Add(this->current_client_result);
			   this->Controls->Add(this->string_input_button);
			   this->Controls->Add(this->input_client_num);
			   this->Controls->Add(this->text_string_input_num);
			   this->Controls->Add(this->string_input_background);
			   this->Controls->Add(this->header_client);
			   this->Controls->Add(this->header_background);
			   this->Name = L"MyForm";
			   this->Text = L"Client";
			   this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->header_background))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->string_input_background))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->current_client_result))->EndInit();
			   this->ResumeLayout(false);
			   this->PerformLayout();

		   }
#pragma endregion
		private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {

		}
		private: System::Void string_input_button_Click(System::Object^ sender, System::EventArgs^ e) {
			inp = input_client_num->Text;
			main_function();
		}

private: System::Void text_string_input_num_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button_close_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void input_client_num_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
};
}