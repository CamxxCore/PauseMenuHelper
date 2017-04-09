#include "stdafx.h"

PauseMenuItem::PauseMenuItem()
{ }

void PauseMenuItem::Initialize(CPauseMenuInstance * parent, CPauseMenuItem * item)
{
	m_parentId = parent->menuId;

	m_index = CMenuFunctions::GetItemIndex(parent, item);

	m_nativeCallback = gcnew NativeMenuValueChangedEvent(&valueChanged);

	registerMenuPref(item->settingId, parent->menuId, m_index, CMenuPreferenceCallback(
			Marshal::GetFunctionPointerForDelegate(m_nativeCallback).ToPointer()));
}

PauseMenuItem::~PauseMenuItem()
{
	this->!PauseMenuItem();
}

PauseMenuItem::!PauseMenuItem()
{
	unregisterMenuPref(baseRef()->settingId);
}