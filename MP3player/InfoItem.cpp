#include "InfoItem.h"
using namespace System::IO;


InfoItem::InfoItem()
{
	
}

InfoItem::~InfoItem()
{
	
}

InfoItem::InfoItem(String^ fname)
{
	Name = fname;
	paths = gcnew List<String^>();
	playTime = 0;
}

String ^ InfoItem::GetName()
{
	return Name;
}

void InfoItem::Tick()
{
	playTime++;
}

int InfoItem::GetPlayTime()
{
	return playTime;
}

void InfoItem::AddPath(String ^ _path)
{
	if(!paths->Contains(_path))
		paths->Add(_path);
}

String^ InfoItem::GetPath()
{
	List<int>^ toRemove = gcnew List<int>;
	String^ ret = "";
	int i = 0;
	for each (String^ path in paths)
	{
		if (!File::Exists(path))
		{
			toRemove->Add(i);
		}
		else
		{
			ret = path;
			break;
		}
		i++;
	}

	for (int j = 0; j < toRemove->Count; j++)
	{
		paths->RemoveAt(toRemove[j]);
	}
	
	return ret;
}
