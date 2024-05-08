#include "MainForm.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

using namespace System;
using namespace System::Windows::Forms;


[STAThreadAttribute]
int main()
{
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	OCLab6server::MainForm form;
	Application::Run(% form);
}