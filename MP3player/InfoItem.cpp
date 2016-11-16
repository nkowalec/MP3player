#include "InfoItem.h"
using namespace System::IO;


InfoItem::InfoItem()
{
	
}

InfoItem::~InfoItem()
{
	
}

InfoItem::InfoItem(String^ fname)	//Inicjuje now¹ instancjê obiektu ze wskazan¹ nazw¹
{
	Name = fname;
	paths = gcnew List<String^>();
	playTime = 0;
}

String ^ InfoItem::GetName()		//Zwraca nazwê obiektu
{
	return Name;
}

void InfoItem::Tick()	//Jeœli wyst¹pi rz¹danie, zwiêksz zapamiêtany czas odtwarzania
{
	playTime++;
}

int InfoItem::GetPlayTime()		//Czas odtwarzania w ramach obiektu
{
	return playTime;
}

void InfoItem::AddPath(String ^ _path)		//Dodaje œcie¿kê do pliku utworu, jeœli takiej jeszcze nie ma
{
	if(!paths->Contains(_path))
		paths->Add(_path);
}

String^ InfoItem::GetPath()		//Pobierz jedn¹ œcie¿kê z kolekcji, wyczyœæ jeœli pliki zosta³y usuniête
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
