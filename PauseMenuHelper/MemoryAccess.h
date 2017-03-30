#pragma once

ref class MemoryAccess
{
public:
	MemoryAccess();
private:
	static System::UInt64 FindPattern(const char * pattern, const char * mask);

};



