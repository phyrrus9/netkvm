#include "Launcher.h"
#include <msclr/marshal_cppstd.h>

namespace control
{

	System::Void Launcher::b_create_Click(System::Object^  sender, System::EventArgs^  e)
	{
		char buffer[512], commandLine[768];
		char *cmd;
		PROCESS_INFORMATION     pi;
		STARTUPINFOA            si;
		GetCurrentDirectory(512, buffer);
		sprintf(
			commandLine,
			"\"%s\\netkvm.exe\" %u %s",
			buffer,
			Decimal::ToInt16(nud_port->Value),
			msclr::interop::marshal_as< std::string >(tb_password->Text).c_str());
		printf("%s\n", commandLine);
		cmd = strdup(commandLine);
		bool a = CreateProcessA(NULL,
			cmd,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_PROCESS_GROUP,// | DETACHED_PROCESS,
			NULL,
			NULL,
			&si,
			&pi);
		if (!a)
			printf("Error! %ld\n", GetLastError());
		if (pi.hThread)
			CloseHandle(pi.hThread);
		if (pi.hProcess)
			CloseHandle(pi.hProcess);
	}
	System::Void Launcher::b_administer_Click(System::Object^  sender, System::EventArgs^  e)
	{
	}
	System::Void Launcher::b_connect_Click(System::Object^  sender, System::EventArgs^  e)
	{
	}

}
