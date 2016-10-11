#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Media;

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
		ListViewItem^ listItem = gcnew ListViewItem(item);
		listView->Items->Add(listItem);
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
}

System::Void MP3player::MainForm::wyjœcieToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	Application::Exit();
}


