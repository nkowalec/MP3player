#include "InfoItem.h"
using namespace System::IO;


InfoItem::InfoItem()
{
	
}

InfoItem::~InfoItem()
{
	
}

InfoItem::InfoItem(String^ fname)	//Inicjuje now� instancj� obiektu ze wskazan� nazw�
{
	Name = fname;
	paths = gcnew List<String^>();
	playTime = 0;
}

String ^ InfoItem::GetName()		//Zwraca nazw� obiektu
{
	return Name;
}

void InfoItem::Tick()	//Je�li wyst�pi rz�danie, zwi�ksz zapami�tany czas odtwarzania
{
	playTime++;
}

int InfoItem::GetPlayTime()		//Czas odtwarzania w ramach obiektu
{
	return playTime;
}

void InfoItem::AddPath(String ^ _path)		//Dodaje �cie�k� do pliku utworu, je�li takiej jeszcze nie ma
{
	if(!paths->Contains(_path))
		paths->Add(_path);
}

String^ InfoItem::GetPath()		//Pobierz jedn� �cie�k� z kolekcji, wyczy�� je�li pliki zosta�y usuni�te
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
