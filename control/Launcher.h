#pragma once
#undef UNICODE
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>

namespace control {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Launcher
	/// </summary>
	public ref class Launcher : public System::Windows::Forms::Form
	{
	public:
		Launcher(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Launcher()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  gb_newServer;
	private: System::Windows::Forms::Button^  b_create;
	private: System::Windows::Forms::TextBox^  tb_password;

	private: System::Windows::Forms::Label^  l_password;
	private: System::Windows::Forms::NumericUpDown^  nud_port;
	private: System::Windows::Forms::Label^  l_port;
	private: System::Windows::Forms::Button^  b_administer;
	private: System::Windows::Forms::GroupBox^  gb_connect;
	private: System::Windows::Forms::Button^  b_connect;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::Label^  label1;
	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->gb_newServer = (gcnew System::Windows::Forms::GroupBox());
			this->b_administer = (gcnew System::Windows::Forms::Button());
			this->b_create = (gcnew System::Windows::Forms::Button());
			this->tb_password = (gcnew System::Windows::Forms::TextBox());
			this->l_password = (gcnew System::Windows::Forms::Label());
			this->nud_port = (gcnew System::Windows::Forms::NumericUpDown());
			this->l_port = (gcnew System::Windows::Forms::Label());
			this->gb_connect = (gcnew System::Windows::Forms::GroupBox());
			this->b_connect = (gcnew System::Windows::Forms::Button());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->gb_newServer->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nud_port))->BeginInit();
			this->gb_connect->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// gb_newServer
			// 
			this->gb_newServer->Controls->Add(this->b_administer);
			this->gb_newServer->Controls->Add(this->b_create);
			this->gb_newServer->Controls->Add(this->tb_password);
			this->gb_newServer->Controls->Add(this->l_password);
			this->gb_newServer->Controls->Add(this->nud_port);
			this->gb_newServer->Controls->Add(this->l_port);
			this->gb_newServer->Location = System::Drawing::Point(13, 13);
			this->gb_newServer->Name = L"gb_newServer";
			this->gb_newServer->Size = System::Drawing::Size(282, 79);
			this->gb_newServer->TabIndex = 0;
			this->gb_newServer->TabStop = false;
			this->gb_newServer->Text = L"Server";
			// 
			// b_administer
			// 
			this->b_administer->Location = System::Drawing::Point(178, 49);
			this->b_administer->Name = L"b_administer";
			this->b_administer->Size = System::Drawing::Size(75, 23);
			this->b_administer->TabIndex = 5;
			this->b_administer->Text = L"Administer";
			this->b_administer->UseVisualStyleBackColor = true;
			this->b_administer->Click += gcnew System::EventHandler(this, &Launcher::b_administer_Click);
			// 
			// b_create
			// 
			this->b_create->Location = System::Drawing::Point(178, 15);
			this->b_create->Name = L"b_create";
			this->b_create->Size = System::Drawing::Size(75, 23);
			this->b_create->TabIndex = 4;
			this->b_create->Text = L"Create";
			this->b_create->UseVisualStyleBackColor = true;
			this->b_create->Click += gcnew System::EventHandler(this, &Launcher::b_create_Click);
			// 
			// tb_password
			// 
			this->tb_password->Location = System::Drawing::Point(72, 51);
			this->tb_password->Name = L"tb_password";
			this->tb_password->Size = System::Drawing::Size(81, 20);
			this->tb_password->TabIndex = 3;
			// 
			// l_password
			// 
			this->l_password->AutoSize = true;
			this->l_password->Location = System::Drawing::Point(7, 54);
			this->l_password->Name = L"l_password";
			this->l_password->Size = System::Drawing::Size(53, 13);
			this->l_password->TabIndex = 2;
			this->l_password->Text = L"Password";
			// 
			// nud_port
			// 
			this->nud_port->Location = System::Drawing::Point(72, 18);
			this->nud_port->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 65535, 0, 0, 0 });
			this->nud_port->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->nud_port->Name = L"nud_port";
			this->nud_port->Size = System::Drawing::Size(81, 20);
			this->nud_port->TabIndex = 1;
			this->nud_port->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->nud_port->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3750, 0, 0, 0 });
			// 
			// l_port
			// 
			this->l_port->AutoSize = true;
			this->l_port->Location = System::Drawing::Point(7, 20);
			this->l_port->Name = L"l_port";
			this->l_port->Size = System::Drawing::Size(26, 13);
			this->l_port->TabIndex = 0;
			this->l_port->Text = L"Port";
			// 
			// gb_connect
			// 
			this->gb_connect->Controls->Add(this->b_connect);
			this->gb_connect->Controls->Add(this->numericUpDown1);
			this->gb_connect->Controls->Add(this->label1);
			this->gb_connect->Location = System::Drawing::Point(324, 13);
			this->gb_connect->Name = L"gb_connect";
			this->gb_connect->Size = System::Drawing::Size(175, 79);
			this->gb_connect->TabIndex = 1;
			this->gb_connect->TabStop = false;
			this->gb_connect->Text = L"Client";
			// 
			// b_connect
			// 
			this->b_connect->Location = System::Drawing::Point(41, 49);
			this->b_connect->Name = L"b_connect";
			this->b_connect->Size = System::Drawing::Size(75, 23);
			this->b_connect->TabIndex = 4;
			this->b_connect->Text = L"Connect";
			this->b_connect->UseVisualStyleBackColor = true;
			this->b_connect->Click += gcnew System::EventHandler(this, &Launcher::b_connect_Click);
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(71, 18);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 65535, 0, 0, 0 });
			this->numericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(81, 20);
			this->numericUpDown1->TabIndex = 3;
			this->numericUpDown1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3750, 0, 0, 0 });
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(26, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Port";
			// 
			// Launcher
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(517, 108);
			this->Controls->Add(this->gb_connect);
			this->Controls->Add(this->gb_newServer);
			this->Name = L"Launcher";
			this->Text = L"Launcher";
			this->gb_newServer->ResumeLayout(false);
			this->gb_newServer->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nud_port))->EndInit();
			this->gb_connect->ResumeLayout(false);
			this->gb_connect->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void b_create_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void b_administer_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void b_connect_Click(System::Object^  sender, System::EventArgs^  e);
};
}
