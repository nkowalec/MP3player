#pragma once
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

ref class InfoItem
{
	
public:
	InfoItem(String^ fname);
	String^ GetName();
	void Tick();
	int GetPlayTime();
	void AddPath(String^ _path);
	String^ GetPath();

private:
	String^ Name;
	List<String^>^ paths;
	int playTime;
};

