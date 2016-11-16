
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

#pragma once
public ref class InfoItem
{
	/*
	 * Klasa obiekt�w reprezentuj�cych wyst�pienie zestawu informacji dotycz�cych 
	 * pojedynczego utworu. 
	 * Przechowuje informacje o wykorzystywanych �cie�kach 
	 * czasie odtwarzania ...
	 */
	
public:
	InfoItem();
	~InfoItem();
	InfoItem(String^ fname);
	String^ GetName();
	void Tick();
	int GetPlayTime();
	void AddPath(String^ _path);
	String^ GetPath();
	String^ Name;
	List<String^>^ paths;
	int playTime;
	
};

