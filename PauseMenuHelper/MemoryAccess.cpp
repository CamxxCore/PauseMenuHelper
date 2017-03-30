#include "stdafx.h"
#include "MemoryAccess.h"

#include <Psapi.h>

MemoryAccess::MemoryAccess()
{

}

uintptr_t MemoryAccess::FindPattern(const char * pattern, const char * mask)
{
	MODULEINFO module = {};
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &module, sizeof(MODULEINFO));

	auto *address = reinterpret_cast<const char *>(module.lpBaseOfDll), *address_end = address + module.SizeOfImage;
	const auto mask_length = static_cast<size_t>(strlen(mask) - 1);

	for (size_t i = 0; address < address_end; address++)
	{
		if (*address == pattern[i] || mask[i] == '?')
		{
			if (mask[i + 1] == '\0')
			{
				return reinterpret_cast<uintptr_t>(address) - mask_length;
			}

			i++;
		}
		else
		{
			i = 0;
		}
	}

	return 0;
}