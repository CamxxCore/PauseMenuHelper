#include "stdafx.h"
#include "Logger.h"

System::String ^ Logger::GetModuleFilename()
{
	auto assembly = System::Reflection::Assembly::GetExecutingAssembly();

	System::Reflection::AssemblyName ^ assemblyName = assembly->GetName();

	return assemblyName->Name;
}

void Logger::Log(System::String ^ text)
{
	auto filename = GetModuleFilename() + ".log";

	System::DateTime dtNow = System::DateTime::Now;

	auto formattedText = System::String::Format("[{0}] [LOG] {1}", dtNow.ToShortTimeString(), text);

	System::IO::File::AppendAllText(filename, formattedText);
}
