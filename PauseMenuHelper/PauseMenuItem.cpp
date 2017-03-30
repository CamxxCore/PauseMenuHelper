#include "stdafx.h"
#include "PauseMenuItem.h"

PauseMenuItem::PauseMenuItem(CPauseMenuItem* item)
{
	m_item = item;
	m_nativeCallback = 
		gcnew MenuValueChangedEvent(&valueChanged);
}

void PauseMenuItem::Initialize()
{
	registerNativeMenuCallback(
		m_item,
		CMenuItemEventCallback(
			Marshal::GetFunctionPointerForDelegate(m_nativeCallback).ToPointer()));
}

PauseMenuItem::~PauseMenuItem()
{
	this->!PauseMenuItem();
}

PauseMenuItem::!PauseMenuItem()
{
	registerNativeMenuCallback(m_item, nullptr);
}