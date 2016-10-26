#include "MainForm.h"
#include "PlaylistNameForm.h"
#include "InfoItem.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Media;
using namespace System::IO;

bool timerFlag = false;

System::Void MP3player::MainForm::otworzToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	OpenFileDialog^ opendialog = gcnew OpenFileDialog;
	opendialog->Multiselect = true;
	opendialog->Filter = "MP3 (*.mp3)|*.mp3|All files (*.*)|*.*";
	opendialog->ShowDialog();

	nazwyDoListy(opendialog->FileNames);

	delete opendialog;
}

System::Void MP3player::MainForm::nazwyDoListy(array<String^>^ elementy)
{
	for each (String^ item in elementy)
	{
		TagLib::File^ tag = TagLib::File::Create(item);
		TimeSpan^ ts = tag->Properties->Duration;

		ListViewItem^ listItem = gcnew ListViewItem(Path::GetFileName(item));
		
		listItem->SubItems->Add(ts->Minutes.ToString()->PadLeft(2, '0') + ":" + ts->Seconds.ToString()->PadLeft(2, '0'));

		listView->Items->Add(listItem);
		listItem->Name = item;
		
		
	}
	listView->Refresh();
}

System::Void MP3player::MainForm::listView_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	array<String^>^ paths = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
	System::Collections::Generic::List<String^>^ lista = gcnew System::Collections::Generic::List<String^>();
	
	for each (String^ path in paths)
	{
		FileAttributes^ attr = File::GetAttributes(path);
		
		if (!attr->HasFlag(FileAttributes::Directory))
			lista->Add(path);

		if (attr->HasFlag(FileAttributes::Directory))
		{
			for each (String^ item in plikiZKatalogu(path))
			{
				attr = File::GetAttributes(item);
				if (!attr->HasFlag(FileAttributes::Directory) && item->EndsWith(".mp3"))
					lista->Add(item);
			}
		}
	}

	nazwyDoListy(lista->ToArray());

	delete lista;
}

System::Void MP3player::MainForm::MainForm_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	listView_DragDrop(sender, e);
}

System::Void MP3player::MainForm::listView_DragEnter(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	if (e->Data->GetDataPresent(DataFormats::FileDrop))
		e->Effect = DragDropEffects::All;
	else
		e->Effect = DragDropEffects::None;
}

System::Void MP3player::MainForm::listView_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e)
{
	if (e->KeyCode == Keys::Delete)
	{
		for each (ListViewItem^ item in listView->SelectedItems)
		{
			listView->Items->Remove(item);
		}

		listView->Refresh();
	}
	else if(e->KeyCode == Keys::Enter)
	{
		listView_DoubleClick(sender, e);
	}
}

System::Void MP3player::MainForm::wyjœcieToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	Application::Exit();
}

System::Void MP3player::MainForm::listView_DoubleClick(System::Object ^ sender, System::EventArgs ^ e)
{
	nameBox->Text = Path::GetFileName(listView->SelectedItems[0]->Text);
	String^ filePath = listView->SelectedItems[0]->Name;
	CurrentItem = listView->SelectedItems[0];
	
	player->URL = filePath;
	if(!timerFlag) { timer->Start(); timerFlag = true; }
	player->play();
	playPauseBtn->Text = "Pause";
}

System::Void MP3player::MainForm::Timer_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	double percent = 0;
	
	if (playFlag)
	{		
		player->controls->play();
		playFlag = false;
	}

	if (player->controls->currentPosition > 0 && player->controls->currentItem->duration > 0)
	{
		currTime->Text = player->controls->currentPositionString;
		fullTime->Text = player->currentItem->durationString;
		percent = ((double)player->controls->currentPosition / player->controls->currentItem->duration);

		trackBar1->Value = (int)(percent * trackBar1->Maximum);
	}
}

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

System::Void MP3player::MainForm::stopBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	playPauseBtn->Text = "Play";
	timer->Stop();
	timerFlag = false;
	player->controls->stop();
	trackBar1->Value = 0;
	currTime->Text = "00:00";
}

System::Void MP3player::MainForm::trackBar1_MouseDown(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	timer->Stop();
	timerFlag = false;
}

System::Void MP3player::MainForm::trackBar1_MouseUp(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e)
{
	double percent = (double)trackBar1->Value / trackBar1->Maximum;
	player->controls->currentPosition = percent * player->controls->currentItem->duration;

	if(!timerFlag) { timer->Start(); timerFlag = true; }
}

array<String^>^ MP3player::MainForm::plikiZKatalogu(String ^ path)
{
	array<String^>^ files = Directory::GetFiles(path);
	return files;
}

void MP3player::MainForm::OnPlayStateChange(int NewState)
{
	if (NewState == (int)WMPLib::WMPPlayState::wmppsMediaEnded)
	{
		//Nastêpny utwór..
		NextSong();
		playFlag = true;
	}
}

void MP3player::MainForm::NextSong()
{
	int nextIndex = CurrentItem->Index + 1;
	if (listView->Items->Count - 1 >= nextIndex)
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
		stopBtn_Click(this, EventArgs::Empty);
	}
}

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

System::Void MP3player::MainForm::nextBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	NextSong();
}

System::Void MP3player::MainForm::prevBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	PrevSong();
}

String^ GetPlaylistsDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ playlistsDir = Path::Combine(rootDir, "Playlists");

	if (!Directory::Exists(playlistsDir))
		Directory::CreateDirectory(playlistsDir);

	return playlistsDir;
}

String^ GetAnalizerDirectory()
{
	System::Reflection::Assembly^ ea = System::Reflection::Assembly::GetExecutingAssembly();
	String^ rootDir = Path::GetDirectoryName(ea->Location);
	String^ analyzerDir = Path::Combine(rootDir, "Analyzer");

	if (!Directory::Exists(analyzerDir))
		Directory::CreateDirectory(analyzerDir);

	return analyzerDir;
}


Xml::Serialization::XmlSerializer^ GetSerializer(Type^ _type)
{
	return gcnew Xml::Serialization::XmlSerializer(_type);
}

System::Void MP3player::MainForm::PreparePlaylists()
{
	for each (ToolStripMenuItem^ item in PlaylistItems)
	{
		playlistyMenuItem->DropDownItems->Remove(item);
	}
	PlaylistItems->Clear();

	String^ playlistsDir = GetPlaylistsDirectory();

	array<String^>^ files = Directory::GetFiles(playlistsDir);
	
	for each (String^ file in files)
	{
		ToolStripMenuItem^ item = gcnew ToolStripMenuItem(Path::GetFileName(file));
		item->Click += gcnew System::EventHandler(this, &MP3player::MainForm::OnClick);

		playlistyMenuItem->DropDownItems->Add(item);
		PlaylistItems->Add(item);
		//playlistyMenuItem->DropDownItems->Add()
	}
}

System::Void MP3player::MainForm::dodajListêToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	PlaylistNameForm^ dialog = gcnew PlaylistNameForm();

	if (dialog->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		return;

	String^ plName = dialog->GetResult();
	String^ path = Path::Combine(GetPlaylistsDirectory(), plName);

	if (File::Exists(path))
	{
		MessageBox::Show("Playlista o nazwie \"" + plName + "\" ju¿ istnieje!");
		return;
	}

	StreamWriter^ sw = gcnew StreamWriter(path);

	for each (ListViewItem^ item in listView->Items)
	{
		sw->WriteLine(item->Name);
	}
	sw->Flush();
	sw->Close();

	PreparePlaylists();
}

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

void MP3player::MainForm::OnTick(System::Object ^ sender, System::EventArgs ^ e)
{
	//Zbieranie danych o preferencjach..
	
	if (player->playState == WMPLib::WMPPlayState::wmppsPlaying)
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

System::Void MP3player::MainForm::generujListêUlubionychToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	SaveCollectedData();
	PlaylistNameForm^ form = gcnew PlaylistNameForm("Maksymalna d³ugoœæ playlisty");
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