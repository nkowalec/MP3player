#include "MainForm.h"

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
