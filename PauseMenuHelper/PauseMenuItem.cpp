#include "stdafx.h"

PauseMenuItem::PauseMenuItem()
{ }

void PauseMenuItem::Initialize(CPauseMenuInstance * parent, CPauseMenuItem * item)
{
	m_parentId = parent->menuId;

	m_menuIndex = CMenuFunctions::GetItemIndex(parent, item);

	m_nativeCallback = gcnew NativeMenuValueChangedEvent(&valueChanged);

	registerMenuPref(item->settingId, parent->menuId, m_menuIndex, CMenuPreferenceCallback(
			Marshal::GetFunctionPointerForDelegate(m_nativeCallback).ToPointer()));
}

PauseMenuItem::~PauseMenuItem()
{
	this->!PauseMenuItem();
}

PauseMenuItem::!PauseMenuItem()
{
	auto cmenu = lookupMenuForIndex(m_parentId);

	if (cmenu)
	{
		auto item = &cmenu->items[m_menuIndex];
		if (item)
		{
			unregisterMenuPref(item->settingId);
		}
	}
}