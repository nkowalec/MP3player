#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Media;
using namespace System::IO;

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
		ListViewItem^ listItem = gcnew ListViewItem(Path::GetFileName(item));
		listView->Items->Add(listItem);
		listItem->Name = item;
	}
	listView->Refresh();
}

System::Void MP3player::MainForm::listView_DragDrop(System::Object ^ sender, System::Windows::Forms::DragEventArgs ^ e)
{
	array<String^>^ files = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
	nazwyDoListy(files);

	delete files;
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

	player->URL = filePath;
	timer->Start();
	player->controls->play();
	playPauseBtn->Text = "Pause";
}

System::Void MP3player::MainForm::Timer_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	double percent = 0;
	
	if (player->controls->currentPosition > 0 && player->controls->currentItem->duration > 0)
	{
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
	player->controls->stop();
	trackBar1->Value = 0;
}


