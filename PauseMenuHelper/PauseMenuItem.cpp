#include "stdafx.h"

PauseMenuItem::PauseMenuItem(CPauseMenuItem* item)
{
	m_item = item;
}

void PauseMenuItem::Initialize()
{
	m_nativeCallback = gcnew NativeMenuValueChangedEvent(&valueChanged);

	registerMenuPrefCallback(m_item,
		CMenuPreferenceCallback(
			Marshal::GetFunctionPointerForDelegate(m_nativeCallback).ToPointer()));
}

PauseMenuItem::~PauseMenuItem()
{
	this->!PauseMenuItem();
}

PauseMenuItem::!PauseMenuItem()
{
	registerMenuPrefCallback(m_item, nullptr);
}