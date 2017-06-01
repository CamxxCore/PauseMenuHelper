#include "stdafx.h"

void PauseMenuItem::Initialize(UIMenu * parent, uiWidget * item)
{
	m_parentId = parent->menuId;

	m_index = CMenuFunctions::GetItemIndex(parent, item);

	if (item->settingId > 0) {
		m_nativeCallback = gcnew NativeMenuValueChangedEvent(this, &PauseMenuItem::valueChanged);

		registerMenuPref(item->settingId, parent->menuId, m_index, CMenuPreferenceCallback(
			Marshal::GetFunctionPointerForDelegate(m_nativeCallback).ToPointer()));
	}
}

PauseMenuItem::~PauseMenuItem()
{
	this->!PauseMenuItem();
}

PauseMenuItem::!PauseMenuItem()
{
	auto cmenuitem = getBaseRef();

	if (cmenuitem) {
		unregisterMenuPref(cmenuitem->settingId);
	}
}
