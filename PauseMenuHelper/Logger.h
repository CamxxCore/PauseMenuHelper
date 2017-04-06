#pragma once
ref class Logger
{
private:
	static System::String^ GetModuleFilename();
public:
	static void Log(System::String ^ text);
};

