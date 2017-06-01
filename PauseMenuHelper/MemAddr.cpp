#include "stdafx.h"

MemAddr::operator uintptr_t()
{
	return addr;
}

MemAddr::operator void*()
{
	return (void*)addr;
}
