#include "MainForm.h"
#include "PlaylistNameForm.h"
#include "InfoItem.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Media;
using namespace System::IO;

bool timerFlag = false;

//Otwiera okno wyboru plików
//po zatwierdzeniu dodaje wybrane utwory do listy
System::Void MP3player::MainForm::otworzToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	OpenFileDialog^ opendialog = gcnew OpenFileDialog;
	opendialog->Multiselect = true;
	opendialog->Filter = "MP3 (*.mp3)|*.mp3|All files (*.*)|*.*";		//Filtrujemy pliki po rozszerzeniu
	opendialog->ShowDialog();

	nazwyDoListy(opendialog->FileNames);

	delete opendialog;
}

//Dodaje nazwy utworów do listy odtwarzania
System::Void MP3player::MainForm::nazwyDoListy(array<String^>^ elementy)
{
	for each (String^ item in elementy)
	{
		TagLib::File^ tag = TagLib::File::Create(item);
		TimeSpan^ ts = tag->Properties->Duration;

		ListViewItem^ listItem = gcnew ListViewItem(Path::GetFileName(item));	//Tworzymy obiekt dla listy
		
		//Dodajemy do obiektu listy dane z czasem trwania utworu
		listItem->SubItems->Add(ts->Minutes.ToString()->PadLeft(2, '0') + ":" + ts->Seconds.ToString()->PadLeft(2, '0'));

		//Dodajemy obiekt do naszej listy
		listView->Items->Add(listItem);
		listItem->Name = item;
		
		
	}
	listView->Refresh();	//Odœwie¿amy listê po dodaniu obiektów
}

//Obs³uga metody Drag&Drop
System::Void MP3player::MainForm::listView_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	array<String^>^ paths = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);		//Dane przeci¹gniête
	System::Collections::Generic::List<String^>^ lista = gcnew System::Collections::Generic::List<String^>();
	
	for each (String^ path in paths)
	{
		FileAttributes^ attr = File::GetAttributes(path);
		
		if (!attr->HasFlag(FileAttributes::Directory))
			lista->Add(path);	//Jeœli plik to dodajemy do bufora

		if (attr->HasFlag(FileAttributes::Directory))		//A jeœli nie to pliki z folderu wrzucamy do bufora
		{
			for each (String^ item in plikiZKatalogu(path))
			{
				attr = File::GetAttributes(item);
				if (!attr->HasFlag(FileAttributes::Directory) && item->EndsWith(".mp3"))	//Jeœli s¹ to pliki MP3
					lista->Add(item);
			}
		}
	}

	nazwyDoListy(lista->ToArray());

	delete lista;
}

//Jeœli przeci¹gniemy na formê (nie na listê) to zrób to samo co dla listy
System::Void MP3player::MainForm::MainForm_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	listView_DragDrop(sender, e);
}

//Ustawienia aktywuj¹ce metodê D&D
System::Void MP3player::MainForm::listView_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop))
		e->Effect = DragDropEffects::All;
	else
		e->Effect = DragDropEffects::None;
}

//Obs³uga przycisków dla zaznaczonego utworu na liœcie
System::Void MP3player::MainForm::listView_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e)
{
	if (e->KeyCode == Keys::Delete)		//Usuñ z listy
	{
		for each (ListViewItem^ item in listView->SelectedItems)
		{
			listView->Items->Remove(item);
		}

		listView->Refresh();
	}
	else if(e->KeyCode == Keys::Enter)		//Odtwórz utwór
	{
		listView_DoubleClick(sender, e);
	}
}

//Opcja z menu plik (zamyka aplikacjê)
System::Void MP3player::MainForm::wyjœcieToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	Application::Exit();
}

//Obs³uguje rozpoczêcie odtworzenia utworu przy podwójnym klikniêciu
System::Void MP3player::MainForm::listView_DoubleClick(System::Object ^ sender, System::EventArgs ^ e)
{
	nameBox->Text = Path::GetFileName(listView->SelectedItems[0]->Text);	//Pobierz œcie¿kê utworu
	String^ filePath = listView->SelectedItems[0]->Name;
	CurrentItem = listView->SelectedItems[0];
	
	player->URL = filePath;		//Wskazanie pliku dla silnika muzycznego
	if(!timerFlag) { timer->Start(); timerFlag = true; }		//Obs³uga Timera
	player->play();		//I odtwarzamy
	playPauseBtn->Text = "Pause";
}

//Akcja wykonywana dla cyklu timera
System::Void MP3player::MainForm::Timer_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	double percent = 0;
	
	if (playFlag)		//Obs³uga odtwarzania kolejnego utworu (po zakoñczeniu poprzedniego z listy)
	{		
		player->controls->play();
		playFlag = false;
	}

	if (player->controls->currentPosition > 0 && player->controls->currentItem->duration > 0)
	{		//Ustawienie suwaka postêpu odtwarzania oraz ustawienie pól informacyjnych o czasie odtwarzania
		currTime->Text = player->controls->currentPositionString;
		fullTime->Text = player->currentItem->durationString;
		percent = ((double)player->controls->currentPosition / player->controls->currentItem->duration);

		trackBar1->Value = (int)(percent * trackBar1->Maximum);
	}
}

//Obs³uga przycisku Play/Pause
System::Void MP3player::MainForm::playPauseBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	if (playPauseBtn->Text == "Play")
	{
		playPauseBtn->Text = "Pause";
		player->controls->play();
		if(!timerFlag) { timer->Start(); timerFlag = true; }
	}
	else
	{
		playPauseBtn->Text = "Play";
		player->controls->pause();
	}
}

//Obs³uga przycisku Stop
System::Void MP3player::MainForm::stopBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	playPauseBtn->Text = "Play";
	timer->Stop();
	timerFlag = false;
	player->controls->stop();
	trackBar1->Value = 0;
	currTime->Text = "00:00";
}

//Obs³uga przesuwania suwaka postêpu odtwarzania (przy przesuwaniu)
System::Void MP3player::MainForm::trackBar1_MouseDown(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	timer->Stop();	
	timerFlag = false;
}

//Po zakoñczeniu przesuwania suwaka, (przestaw utwór, uruchom ponownie Timer)
System::Void MP3player::MainForm::trackBar1_MouseUp(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	double percent = (double)trackBar1->Value / trackBar1->Maximum;
	player->controls->currentPosition = percent * player->controls->currentItem->duration;

	if(!timerFlag) { timer->Start(); timerFlag = true; }
}

//Pobieranie œcie¿ek plików z katalogu
array<String^>^ MP3player::MainForm::plikiZKatalogu(String ^ path)
{
	array<String^>^ files = Directory::GetFiles(path);
	return files;
}

//Obs³uga zdarzenia zakoñczenia utworu
void MP3player::MainForm::OnPlayStateChange(int NewState)
{
	if (NewState == (int)WMPLib::WMPPlayState::wmppsMediaEnded)
	{
		//Nastêpny utwór..
		NextSong();
		playFlag = true;
	}
}

//Obs³uga przejœcia do nastêpnego utworu
void MP3player::MainForm::NextSong()
{
	int nextIndex = CurrentItem->Index + 1;
	if (listView->Items->Count - 1 >= nextIndex)		//Jeœli jest nastêpny
	{
		for each (ListViewItem^ item in listView->SelectedItems)
		{
			item->Selected = false;
		}

		listView->Items[nextIndex]->Selected = true;
		listView->Focus();
		listView_KeyUp(this, %KeyEventArgs::KeyEventArgs(Keys::Enter));
	}
	else
	{
		stopBtn_Click(this, EventArgs::Empty);		//A jeœli jest ju¿ koniec listy
	}
}

//Obs³uga przejœcia do poprzedniego utworu
void MP3player::MainForm::PrevSong()
{
	int prevIndex = CurrentItem->Index - 1;
	if (prevIndex >= 0 && listView->Items->Count >= prevIndex)
	{
		for each (ListViewItem^ item in listView->SelectedItems)
		{
			item->Selected = false;
		}

		listView->Items[prevIndex]->Selected = true;
		
		listView->Focus();
		listView_KeyUp(this, %KeyEventArgs::KeyEventArgs(Keys::Enter));
	}
	else
	{
		stopBtn_Click(this, EventArgs::Empty);
	}
}

//Przycisk nastêpnego utworu
System::Void MP3player::MainForm::nextBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	NextSong();
}

//Przycisk poprzedniego utworu
System::Void MP3player::MainForm::prevBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	PrevSong();
}

//Pobierz œcie¿kê katalogu z playlistami
String^ GetPlaylistsDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ playlistsDir = Path::Combine(rootDir, "Playlists");

	if (!Directory::Exists(playlistsDir))
		Directory::CreateDirectory(playlistsDir);

	return playlistsDir;
}

//Pobierz œcie¿kê katalogu z danymi analizy odtwarzania
String^ GetAnalizerDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ analyzerDir = Path::Combine(rootDir, "Analyzer");

	if (!Directory::Exists(analyzerDir))
		Directory::CreateDirectory(analyzerDir);

	return analyzerDir;
}

//Udostêpnia serializer dla tworzenia plików XML
Xml::Serialization::XmlSerializer^ GetSerializer(Type^ _type)
{
	return gcnew Xml::Serialization::XmlSerializer(_type);
}

//Pobiera dostêpne playlisty i dodaje do interfejsu programu
System::Void MP3player::MainForm::PreparePlaylists()
{
	for each (ToolStripMenuItem^ item in PlaylistItems)		//Wyczyœæ listê
	{
		playlistyMenuItem->DropDownItems->Remove(item);
	}
	PlaylistItems->Clear();

	String^ playlistsDir = GetPlaylistsDirectory();		
	//Pobierz zapisane playlisty
	array<String^>^ files = Directory::GetFiles(playlistsDir);
	
	for each (String^ file in files)
	{
		//Dodaj elementy i akcje
		ToolStripMenuItem^ item = gcnew ToolStripMenuItem(Path::GetFileName(file));
		item->Click += gcnew System::EventHandler(this, &MP3player::MainForm::OnClick);

		playlistyMenuItem->DropDownItems->Add(item);
		PlaylistItems->Add(item);
	}
}

//Obs³uga zapisu nowej playlisty
// -- Zapisuje playlistê z aktualnej listy
System::Void MP3player::MainForm::dodajListêToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	GetValueForm^ dialog = gcnew GetValueForm();

	if (dialog->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	String^ plName = dialog->GetResult();
	String^ path = Path::Combine(GetPlaylistsDirectory(), plName);

	if (File::Exists(path))
	{
		if(MessageBox::Show("Playlista o nazwie \"" + plName + "\" ju¿ istnieje! \nCzy nadpisaæ?", "Konflikt", MessageBoxButtons::YesNo) != System::Windows::Forms::DialogResult::Yes)
		return;
	}

	StreamWriter^ sw = gcnew StreamWriter(path, false);

	for each (ListViewItem^ item in listView->Items)
	{
		sw->WriteLine(item->Name);
	}
	sw->Flush();
	sw->Close();

	PreparePlaylists();
}

//£aduje wskazan¹ (po nazwie) playlistê do list teraz odtwarzanych
System::Void MP3player::MainForm::LoadPlaylist(String^ name)
{
	listView->Items->Clear();
	String^ dir = GetPlaylistsDirectory();
	String^ path = Path::Combine(dir, name);

	StreamReader^ sr = gcnew StreamReader(path);
	System::Collections::Generic::List<String^>^ lista = gcnew System::Collections::Generic::List<String^>();
	while (sr->Peek() != -1)
	{
		String^ linia = sr->ReadLine();
		lista->Add(linia);
	}
	sr->Close();
	nazwyDoListy(lista->ToArray());
}

//Akcja dla cyklu zegarowego timera zbieraj¹cego dane
void MP3player::MainForm::OnTick(System::Object ^ sender, System::EventArgs ^ e)
{
	//Zbieranie danych o preferencjach..
	
	if (player->playState == WMPLib::WMPPlayState::wmppsPlaying)	//Zbiera jeœli coœ jest odtwarzane
	{
		String^ path = player->URL;
		String^ name = Path::GetFileName(path);

		InfoItem^ item = nullptr;
		for each (InfoItem^ var in InfoItemsList)
		{
			if (var->GetName() == name)
			{
				item = var;
				break;
			}
		}

		if (item == nullptr)
			InfoItemsList->Add(item = gcnew InfoItem(name));

		item->AddPath(path);
		item->Tick();
	}

}

//Akcja dla klikniêcia playlisty w menu
void MP3player::MainForm::OnClick(System::Object ^sender, System::EventArgs ^e)
{
	ToolStripMenuItem^ item = (ToolStripMenuItem^)sender;
	LoadPlaylist(item->Text);
}

//Œcie¿ka do aktualnego pliku analizy odtwarzania
String^ GetAnalizerPath()
{
	String^ dir = GetAnalizerDirectory();
	return Path::Combine(dir, DateTime::Today.ToString("yyyyMM"));
}

//Tutaj zapisujemy dane, przy niszczeniu obiektu (na koniec dzia³ania programu)
System::Void MP3player::MainForm::SaveCollectedData()
{
	
	String^ path = GetAnalizerPath();
	StreamWriter^ writer = gcnew StreamWriter(path, false);

	Xml::Serialization::XmlSerializer^ serializer = GetSerializer(InfoItemsList->GetType());

	serializer->Serialize(writer, InfoItemsList);
	
	writer->Close();
}

//Odczytujemy plik, jeœli takowy istnieje na ten miesi¹c
System::Void MP3player::MainForm::LoadInfoItemsList()
{
	String^ path = GetAnalizerPath();

	if (File::Exists(path))
	{
		StreamReader^ reader = gcnew StreamReader(path);
		
		Xml::Serialization::XmlSerializer^ serializer = GetSerializer(InfoItemsList->GetType());

		InfoItemsList = (List<InfoItem^>^)serializer->Deserialize(reader);

		reader->Close();
	}
}

//Metoda generuj¹ca playlistê na podstawie zebranych danych
//Ogranicza iloœæ utworów do przekazanej parametrem (jeœli jest dostêpnych wiêcej)
array<String^>^ MP3player::MainForm::GenerujPlayliste(int _max)
{
	List<InfoItem^>^ tmp = gcnew List<InfoItem^>();

	for each (InfoItem^ item in InfoItemsList)
	{
		int index = -1;
		for (int i = 0; i < tmp->Count; i++)
		{
			if (tmp[i]->GetPlayTime() < item->GetPlayTime())
			{
				index = i;
				break;
			}
		}
		if (index != -1)
			tmp->Insert(index, item);
		else
			tmp->Add(item);
	}

	if (tmp->Count < _max)
	{
		array<String^>^ analizerFiles = Directory::GetFiles(GetAnalizerDirectory());
		Array::Sort(analizerFiles);
		Xml::Serialization::XmlSerializer^ serializer = GetSerializer(tmp->GetType());

		for (int i = analizerFiles->Length - 2; i >= 0; i--)
		{
			StreamReader^ sr = gcnew StreamReader(analizerFiles[i]);
			List<InfoItem^>^ listaFile = (List<InfoItem^>^)serializer->Deserialize(sr);

			for each (InfoItem^ item in listaFile)
			{
				bool breakFlag = false;
				for each (InfoItem^ findItem in tmp)
				{
					if (findItem->GetName() == item->GetName())
					{
						breakFlag = true;
						break;
					}
				}
				if (breakFlag) continue;

				int index = -1;
				for (int i = 0; i < tmp->Count; i++)
				{
					if (tmp[i]->GetPlayTime() < item->GetPlayTime())
					{
						index = i;
						break;
					}
				}
				if (index != -1)
					tmp->Insert(index, item);
				else
					tmp->Add(item);
			}

			sr->Close();
			if (tmp->Count >= _max)
				break;
		}
	}

	List<String^>^ genPaths = gcnew List<String^>();
	DateTime^ currFile = DateTime::Today;

	for each (InfoItem^ item in tmp)
	{
		genPaths->Add(item->GetPath());

		if (genPaths->Count == _max) break;
	}
	return genPaths->ToArray();
}

//Obs³uga opcji wybranej w menu
//Pobranie informacji o maksymalnej d³ugoœci playlisty oraz uruchomienie generatora
System::Void MP3player::MainForm::generujListêUlubionychToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	SaveCollectedData();
	GetValueForm^ form = gcnew GetValueForm("Maksymalna d³ugoœæ playlisty");
	bool flag = true;
	while (flag)
	{
		if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			int max;
			if (int::TryParse(form->GetResult(), max))
			{
				flag = false;

				//Generowanie playlisty
				array<String^>^ lista = GenerujPlayliste(max);
				listView->Items->Clear();
				nazwyDoListy(lista);
			}
			else
			{
				MessageBox::Show("Wprowadzono b³êdn¹ wartoœæ, maksymalna d³ugoœæ playlisty musi byæ liczb¹ ca³kowit¹!");
			}

		}
		else
		{
			flag = false;
		}
	}

}