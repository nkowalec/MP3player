#include "MainForm.h"
#include "PlaylistNameForm.h"
#include "InfoItem.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Media;
using namespace System::IO;

bool timerFlag = false;

//Otwiera okno wyboru plik�w
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

//Dodaje nazwy utwor�w do listy odtwarzania
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
	listView->Refresh();	//Od�wie�amy list� po dodaniu obiekt�w
}

//Obs�uga metody Drag&Drop
System::Void MP3player::MainForm::listView_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	array<String^>^ paths = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);		//Dane przeci�gni�te
	System::Collections::Generic::List<String^>^ lista = gcnew System::Collections::Generic::List<String^>();
	
	for each (String^ path in paths)
	{
		FileAttributes^ attr = File::GetAttributes(path);
		
		if (!attr->HasFlag(FileAttributes::Directory))
			lista->Add(path);	//Je�li plik to dodajemy do bufora

		if (attr->HasFlag(FileAttributes::Directory))		//A je�li nie to pliki z folderu wrzucamy do bufora
		{
			for each (String^ item in plikiZKatalogu(path))
			{
				attr = File::GetAttributes(item);
				if (!attr->HasFlag(FileAttributes::Directory) && item->EndsWith(".mp3"))	//Je�li s� to pliki MP3
					lista->Add(item);
			}
		}
	}

	nazwyDoListy(lista->ToArray());

	delete lista;
}

//Je�li przeci�gniemy na form� (nie na list�) to zr�b to samo co dla listy
System::Void MP3player::MainForm::MainForm_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	listView_DragDrop(sender, e);
}

//Ustawienia aktywuj�ce metod� D&D
System::Void MP3player::MainForm::listView_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop))
		e->Effect = DragDropEffects::All;
	else
		e->Effect = DragDropEffects::None;
}

//Obs�uga przycisk�w dla zaznaczonego utworu na li�cie
System::Void MP3player::MainForm::listView_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e)
{
	if (e->KeyCode == Keys::Delete)		//Usu� z listy
	{
		for each (ListViewItem^ item in listView->SelectedItems)
		{
			listView->Items->Remove(item);
		}

		listView->Refresh();
	}
	else if(e->KeyCode == Keys::Enter)		//Odtw�rz utw�r
	{
		listView_DoubleClick(sender, e);
	}
}

//Opcja z menu plik (zamyka aplikacj�)
System::Void MP3player::MainForm::wyj�cieToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	Application::Exit();
}

//Obs�uguje rozpocz�cie odtworzenia utworu przy podw�jnym klikni�ciu
System::Void MP3player::MainForm::listView_DoubleClick(System::Object ^ sender, System::EventArgs ^ e)
{
	nameBox->Text = Path::GetFileName(listView->SelectedItems[0]->Text);	//Pobierz �cie�k� utworu
	String^ filePath = listView->SelectedItems[0]->Name;
	CurrentItem = listView->SelectedItems[0];
	
	player->URL = filePath;		//Wskazanie pliku dla silnika muzycznego
	if(!timerFlag) { timer->Start(); timerFlag = true; }		//Obs�uga Timera
	player->play();		//I odtwarzamy
	playPauseBtn->Text = "Pause";
}

//Akcja wykonywana dla cyklu timera
System::Void MP3player::MainForm::Timer_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	double percent = 0;
	
	if (playFlag)		//Obs�uga odtwarzania kolejnego utworu (po zako�czeniu poprzedniego z listy)
	{		
		player->controls->play();
		playFlag = false;
	}

	if (player->controls->currentPosition > 0 && player->controls->currentItem->duration > 0)
	{		//Ustawienie suwaka post�pu odtwarzania oraz ustawienie p�l informacyjnych o czasie odtwarzania
		currTime->Text = player->controls->currentPositionString;
		fullTime->Text = player->currentItem->durationString;
		percent = ((double)player->controls->currentPosition / player->controls->currentItem->duration);

		trackBar1->Value = (int)(percent * trackBar1->Maximum);
	}
}

//Obs�uga przycisku Play/Pause
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

//Obs�uga przycisku Stop
System::Void MP3player::MainForm::stopBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	playPauseBtn->Text = "Play";
	timer->Stop();
	timerFlag = false;
	player->controls->stop();
	trackBar1->Value = 0;
	currTime->Text = "00:00";
}

//Obs�uga przesuwania suwaka post�pu odtwarzania (przy przesuwaniu)
System::Void MP3player::MainForm::trackBar1_MouseDown(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	timer->Stop();	
	timerFlag = false;
}

//Po zako�czeniu przesuwania suwaka, (przestaw utw�r, uruchom ponownie Timer)
System::Void MP3player::MainForm::trackBar1_MouseUp(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	double percent = (double)trackBar1->Value / trackBar1->Maximum;
	player->controls->currentPosition = percent * player->controls->currentItem->duration;

	if(!timerFlag) { timer->Start(); timerFlag = true; }
}

//Pobieranie �cie�ek plik�w z katalogu
array<String^>^ MP3player::MainForm::plikiZKatalogu(String ^ path)
{
	array<String^>^ files = Directory::GetFiles(path);
	return files;
}

//Obs�uga zdarzenia zako�czenia utworu
void MP3player::MainForm::OnPlayStateChange(int NewState)
{
	if (NewState == (int)WMPLib::WMPPlayState::wmppsMediaEnded)
	{
		//Nast�pny utw�r..
		NextSong();
		playFlag = true;
	}
}

//Obs�uga przej�cia do nast�pnego utworu
void MP3player::MainForm::NextSong()
{
	int nextIndex = CurrentItem->Index + 1;
	if (listView->Items->Count - 1 >= nextIndex)		//Je�li jest nast�pny
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
		stopBtn_Click(this, EventArgs::Empty);		//A je�li jest ju� koniec listy
	}
}

//Obs�uga przej�cia do poprzedniego utworu
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

//Przycisk nast�pnego utworu
System::Void MP3player::MainForm::nextBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	NextSong();
}

//Przycisk poprzedniego utworu
System::Void MP3player::MainForm::prevBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	PrevSong();
}

//Pobierz �cie�k� katalogu z playlistami
String^ GetPlaylistsDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ playlistsDir = Path::Combine(rootDir, "Playlists");

	if (!Directory::Exists(playlistsDir))
		Directory::CreateDirectory(playlistsDir);

	return playlistsDir;
}

//Pobierz �cie�k� katalogu z danymi analizy odtwarzania
String^ GetAnalizerDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ analyzerDir = Path::Combine(rootDir, "Analyzer");

	if (!Directory::Exists(analyzerDir))
		Directory::CreateDirectory(analyzerDir);

	return analyzerDir;
}

//Udost�pnia serializer dla tworzenia plik�w XML
Xml::Serialization::XmlSerializer^ GetSerializer(Type^ _type)
{
	return gcnew Xml::Serialization::XmlSerializer(_type);
}

//Pobiera dost�pne playlisty i dodaje do interfejsu programu
System::Void MP3player::MainForm::PreparePlaylists()
{
	for each (ToolStripMenuItem^ item in PlaylistItems)		//Wyczy�� list�
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

//Obs�uga zapisu nowej playlisty
// -- Zapisuje playlist� z aktualnej listy
System::Void MP3player::MainForm::dodajList�ToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	GetValueForm^ dialog = gcnew GetValueForm();

	if (dialog->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	String^ plName = dialog->GetResult();
	String^ path = Path::Combine(GetPlaylistsDirectory(), plName);

	if (File::Exists(path))
	{
		if(MessageBox::Show("Playlista o nazwie \"" + plName + "\" ju� istnieje! \nCzy nadpisa�?", "Konflikt", MessageBoxButtons::YesNo) != System::Windows::Forms::DialogResult::Yes)
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

//�aduje wskazan� (po nazwie) playlist� do list teraz odtwarzanych
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

//Akcja dla cyklu zegarowego timera zbieraj�cego dane
void MP3player::MainForm::OnTick(System::Object ^ sender, System::EventArgs ^ e)
{
	//Zbieranie danych o preferencjach..
	
	if (player->playState == WMPLib::WMPPlayState::wmppsPlaying)	//Zbiera je�li co� jest odtwarzane
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

//Akcja dla klikni�cia playlisty w menu
void MP3player::MainForm::OnClick(System::Object ^sender, System::EventArgs ^e)
{
	ToolStripMenuItem^ item = (ToolStripMenuItem^)sender;
	LoadPlaylist(item->Text);
}

//�cie�ka do aktualnego pliku analizy odtwarzania
String^ GetAnalizerPath()
{
	String^ dir = GetAnalizerDirectory();
	return Path::Combine(dir, DateTime::Today.ToString("yyyyMM"));
}

//Tutaj zapisujemy dane, przy niszczeniu obiektu (na koniec dzia�ania programu)
System::Void MP3player::MainForm::SaveCollectedData()
{
	
	String^ path = GetAnalizerPath();
	StreamWriter^ writer = gcnew StreamWriter(path, false);

	Xml::Serialization::XmlSerializer^ serializer = GetSerializer(InfoItemsList->GetType());

	serializer->Serialize(writer, InfoItemsList);
	
	writer->Close();
}

//Odczytujemy plik, je�li takowy istnieje na ten miesi�c
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

//Metoda generuj�ca playlist� na podstawie zebranych danych
//Ogranicza ilo�� utwor�w do przekazanej parametrem (je�li jest dost�pnych wi�cej)
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

//Obs�uga opcji wybranej w menu
//Pobranie informacji o maksymalnej d�ugo�ci playlisty oraz uruchomienie generatora
System::Void MP3player::MainForm::generujList�UlubionychToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	SaveCollectedData();
	GetValueForm^ form = gcnew GetValueForm("Maksymalna d�ugo�� playlisty");
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
				MessageBox::Show("Wprowadzono b��dn� warto��, maksymalna d�ugo�� playlisty musi by� liczb� ca�kowit�!");
			}

		}
		else
		{
			flag = false;
		}
	}

}