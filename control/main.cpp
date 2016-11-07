#include "Launcher.h"

using namespace System;
using namespace System::Windows::Forms;


[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	control::Launcher launcher;
	Application::Run(%launcher);
}

int main(void)
{
	array<String^>^ args;
	Main(args);
}
