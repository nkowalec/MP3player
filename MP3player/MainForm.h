#pragma once
#include "InfoItem.h"

namespace MP3player {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	
	public ref class MainForm : public System::Windows::Forms::Form
	{
		/*
		 * G³ówna klasa programu reprezentuj¹ca jego g³ówne okno.
		 * Zawiera kompletn¹ funkcjonalnoœæ aplikacji.
		 */
	public:
		MainForm(void)
		{
			InitializeComponent();

			player = gcnew WMPLib::WindowsMediaPlayerClass();	//Do odtwarzania utworów wykorzystujemy silnik Windows Media Player

			player->PlayStateChange += gcnew WMPLib::_WMPOCXEvents_PlayStateChangeEventHandler(this, &MP3player::MainForm::OnPlayStateChange);
			
			timer = gcnew Timer();		//Obiekt timera, odœwie¿aj¹cy dane podczas odtwarzania
			timer->Tick += gcnew EventHandler(this, &MainForm::Timer_Tick);
			PlaylistItems = gcnew System::Collections::Generic::List<ToolStripMenuItem^>();
			InfoItemsList = gcnew System::Collections::Generic::List<InfoItem^>();
			LoadInfoItemsList();		//Pobranie aktualnej listy odtwarzanych utworów z ich parametrami
			PreparePlaylists();		//Dodanie zapisanych playlist do programu

			//Timer zbieraj¹cy dane o odtwarzaniu
			collectData = gcnew Timer();
			collectData->Interval = 1000;
			collectData->Tick += gcnew System::EventHandler(this, &MP3player::MainForm::OnTick);
			collectData->Start();
		}
		System::Collections::Generic::List<ToolStripMenuItem^>^ PlaylistItems;
		System::Collections::Generic::List<InfoItem^>^ InfoItemsList;
	protected:
		System::Void LoadInfoItemsList();
		System::Void SaveCollectedData();

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			SaveCollectedData();
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ToolStripMenuItem^  generujListêUlubionychToolStripMenuItem;
	private: System::Windows::Forms::TextBox^  nameBox;
	private: Timer^ timer;
	private: Timer^ collectData;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  plikToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  otwórzToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  pomocToolStripMenuItem;
	private: System::Windows::Forms::ListView^  listView;
	private: System::Windows::Forms::ColumnHeader^  Nazwa;
	private: System::Windows::Forms::ColumnHeader^  Czas;
	private: System::Windows::Forms::ToolStripMenuItem^  wyjœcieToolStripMenuItem;
	private: System::Windows::Forms::TrackBar^  trackBar1;
	private: System::Windows::Forms::Button^  playPauseBtn;
	private: System::Windows::Forms::Button^  stopBtn;
	private: System::Windows::Forms::Label^  currTime;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  fullTime;
	private: System::Windows::Forms::Button^  nextBtn;
	private: System::Windows::Forms::Button^  prevBtn;
	private: System::Windows::Forms::ToolStripMenuItem^  playlistyMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  dodajListêToolStripMenuItem;

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->nameBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->plikToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->otwórzToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->wyjœcieToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->playlistyMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dodajListêToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pomocToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->listView = (gcnew System::Windows::Forms::ListView());
			this->Nazwa = (gcnew System::Windows::Forms::ColumnHeader());
			this->Czas = (gcnew System::Windows::Forms::ColumnHeader());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->playPauseBtn = (gcnew System::Windows::Forms::Button());
			this->stopBtn = (gcnew System::Windows::Forms::Button());
			this->currTime = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->fullTime = (gcnew System::Windows::Forms::Label());
			this->nextBtn = (gcnew System::Windows::Forms::Button());
			this->prevBtn = (gcnew System::Windows::Forms::Button());
			this->generujListêUlubionychToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			this->SuspendLayout();
			// 
			// nameBox
			// 
			this->nameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->nameBox->BackColor = System::Drawing::Color::White;
			this->nameBox->Cursor = System::Windows::Forms::Cursors::Default;
			this->nameBox->Enabled = false;
			this->nameBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(238)));
			this->nameBox->Location = System::Drawing::Point(0, 27);
			this->nameBox->Name = L"nameBox";
			this->nameBox->Size = System::Drawing::Size(339, 20);
			this->nameBox->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(1, 111);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(48, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Playlista:";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3)
			{
				this->plikToolStripMenuItem,
					this->playlistyMenuItem, this->pomocToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(339, 24);
			this->menuStrip1->TabIndex = 3;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// plikToolStripMenuItem
			// 
			this->plikToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2)
			{
				this->otwórzToolStripMenuItem,
					this->wyjœcieToolStripMenuItem
			});
			this->plikToolStripMenuItem->Name = L"plikToolStripMenuItem";
			this->plikToolStripMenuItem->Size = System::Drawing::Size(38, 20);
			this->plikToolStripMenuItem->Text = L"Plik";
			// 
			// otwórzToolStripMenuItem
			// 
			this->otwórzToolStripMenuItem->Name = L"otwórzToolStripMenuItem";
			this->otwórzToolStripMenuItem->Size = System::Drawing::Size(118, 22);
			this->otwórzToolStripMenuItem->Text = L"Otwórz..";
			this->otwórzToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::otworzToolStripMenuItem_Click);
			// 
			// wyjœcieToolStripMenuItem
			// 
			this->wyjœcieToolStripMenuItem->Name = L"wyjœcieToolStripMenuItem";
			this->wyjœcieToolStripMenuItem->Size = System::Drawing::Size(118, 22);
			this->wyjœcieToolStripMenuItem->Text = L"Wyjœcie";
			this->wyjœcieToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::wyjœcieToolStripMenuItem_Click);
			// 
			// playlistyMenuItem
			// 
			this->playlistyMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2)
			{
				this->dodajListêToolStripMenuItem,
					this->generujListêUlubionychToolStripMenuItem
			});
			this->playlistyMenuItem->Name = L"playlistyMenuItem";
			this->playlistyMenuItem->Size = System::Drawing::Size(62, 20);
			this->playlistyMenuItem->Text = L"Playlisty";
			// 
			// dodajListêToolStripMenuItem
			// 
			this->dodajListêToolStripMenuItem->Name = L"dodajListêToolStripMenuItem";
			this->dodajListêToolStripMenuItem->Size = System::Drawing::Size(202, 22);
			this->dodajListêToolStripMenuItem->Text = L"Zapisz listê";
			this->dodajListêToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::dodajListêToolStripMenuItem_Click);
			// 
			// pomocToolStripMenuItem
			// 
			this->pomocToolStripMenuItem->Name = L"pomocToolStripMenuItem";
			this->pomocToolStripMenuItem->Size = System::Drawing::Size(57, 20);
			this->pomocToolStripMenuItem->Text = L"Pomoc";
			// 
			// listView
			// 
			this->listView->AllowDrop = true;
			this->listView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) { this->Nazwa, this->Czas });
			this->listView->FullRowSelect = true;
			this->listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->listView->Location = System::Drawing::Point(0, 126);
			this->listView->Name = L"listView";
			this->listView->Size = System::Drawing::Size(339, 317);
			this->listView->TabIndex = 4;
			this->listView->UseCompatibleStateImageBehavior = false;
			this->listView->View = System::Windows::Forms::View::Details;
			this->listView->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::listView_DragDrop);
			this->listView->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::listView_DragEnter);
			this->listView->DoubleClick += gcnew System::EventHandler(this, &MainForm::listView_DoubleClick);
			this->listView->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::listView_KeyUp);
			// 
			// Nazwa
			// 
			this->Nazwa->Text = L"Nazwa";
			this->Nazwa->Width = 220;
			// 
			// Czas
			// 
			this->Czas->Text = L"Czas";
			this->Czas->Width = 99;
			// 
			// trackBar1
			// 
			this->trackBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->trackBar1->Location = System::Drawing::Point(0, 49);
			this->trackBar1->Maximum = 100;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(339, 45);
			this->trackBar1->TabIndex = 5;
			this->trackBar1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::trackBar1_MouseDown);
			this->trackBar1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::trackBar1_MouseUp);
			// 
			// playPauseBtn
			// 
			this->playPauseBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->playPauseBtn->Location = System::Drawing::Point(87, 83);
			this->playPauseBtn->Name = L"playPauseBtn";
			this->playPauseBtn->Size = System::Drawing::Size(70, 23);
			this->playPauseBtn->TabIndex = 6;
			this->playPauseBtn->Text = L"Play";
			this->playPauseBtn->UseVisualStyleBackColor = true;
			this->playPauseBtn->Click += gcnew System::EventHandler(this, &MainForm::playPauseBtn_Click);
			// 
			// stopBtn
			// 
			this->stopBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->stopBtn->Location = System::Drawing::Point(163, 83);
			this->stopBtn->Name = L"stopBtn";
			this->stopBtn->Size = System::Drawing::Size(70, 23);
			this->stopBtn->TabIndex = 7;
			this->stopBtn->Text = L"Stop";
			this->stopBtn->UseVisualStyleBackColor = true;
			this->stopBtn->Click += gcnew System::EventHandler(this, &MainForm::stopBtn_Click);
			// 
			// currTime
			// 
			this->currTime->AutoSize = true;
			this->currTime->Location = System::Drawing::Point(4, 83);
			this->currTime->Name = L"currTime";
			this->currTime->Size = System::Drawing::Size(34, 13);
			this->currTime->TabIndex = 8;
			this->currTime->Text = L"00:00";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(35, 83);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(12, 13);
			this->label3->TabIndex = 9;
			this->label3->Text = L"/";
			// 
			// fullTime
			// 
			this->fullTime->AutoSize = true;
			this->fullTime->Location = System::Drawing::Point(44, 83);
			this->fullTime->Name = L"fullTime";
			this->fullTime->Size = System::Drawing::Size(34, 13);
			this->fullTime->TabIndex = 10;
			this->fullTime->Text = L"00:00";
			// 
			// nextBtn
			// 
			this->nextBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->nextBtn->Location = System::Drawing::Point(279, 83);
			this->nextBtn->Name = L"nextBtn";
			this->nextBtn->Size = System::Drawing::Size(33, 23);
			this->nextBtn->TabIndex = 11;
			this->nextBtn->Text = L">|";
			this->nextBtn->UseVisualStyleBackColor = true;
			this->nextBtn->Click += gcnew System::EventHandler(this, &MainForm::nextBtn_Click);
			// 
			// prevBtn
			// 
			this->prevBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->prevBtn->Location = System::Drawing::Point(240, 83);
			this->prevBtn->Name = L"prevBtn";
			this->prevBtn->Size = System::Drawing::Size(33, 23);
			this->prevBtn->TabIndex = 12;
			this->prevBtn->Text = L"|<";
			this->prevBtn->UseVisualStyleBackColor = true;
			this->prevBtn->Click += gcnew System::EventHandler(this, &MainForm::prevBtn_Click);
			// 
			// generujListêUlubionychToolStripMenuItem
			// 
			this->generujListêUlubionychToolStripMenuItem->Name = L"generujListêUlubionychToolStripMenuItem";
			this->generujListêUlubionychToolStripMenuItem->Size = System::Drawing::Size(202, 22);
			this->generujListêUlubionychToolStripMenuItem->Text = L"Generuj listê ulubionych";
			this->generujListêUlubionychToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::generujListêUlubionychToolStripMenuItem_Click);
			// 
			// MainForm
			// 
			this->AllowDrop = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(339, 442);
			this->Controls->Add(this->prevBtn);
			this->Controls->Add(this->nextBtn);
			this->Controls->Add(this->fullTime);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->currTime);
			this->Controls->Add(this->stopBtn);
			this->Controls->Add(this->playPauseBtn);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->listView);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->nameBox);
			this->Controls->Add(this->menuStrip1);
			this->KeyPreview = true;
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"MP3 player";
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::MainForm_DragDrop);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void otworzToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void nazwyDoListy(array<String^>^ elementy);
	private: System::Void listView_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);

	private: System::Void MainForm_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void listView_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void listView_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void wyjœcieToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void listView_DoubleClick(System::Object^  sender, System::EventArgs^  e);

	private: WMPLib::WindowsMediaPlayerClass^ player;
	private: System::Void Timer_Tick(System::Object^ sender, System::EventArgs^ e);
	private: System::Void playPauseBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void stopBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void trackBar1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void trackBar1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: array<String^>^ plikiZKatalogu(String^ path);
	
	private: void OnPlayStateChange(int NewState);
	private: void NextSong();
	private: void PrevSong();
	private: ListViewItem^ CurrentItem;
	private: System::Void nextBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void prevBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: bool playFlag;
	private: System::Void dodajListêToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PreparePlaylists();
	private: void OnClick(System::Object ^sender, System::EventArgs ^e);
	private: System::Void LoadPlaylist(String^ name);
	private: void OnTick(System::Object ^sender, System::EventArgs ^e);
	private: System::Void generujListêUlubionychToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: array<String^>^ GenerujPlayliste(int _max);
};
}
