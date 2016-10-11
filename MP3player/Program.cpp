#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace MP3player;

[STAThread]

void main(array<String^>^ args) 
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	MainForm form;

	Application::Run(%form);
}