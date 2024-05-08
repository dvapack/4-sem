#include "MyForm.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>

using namespace System;
using namespace System::Windows::Forms;


[STAThreadAttribute]
int main()
{
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	OCLab6client::MyForm form;
	Application::Run(% form);
}