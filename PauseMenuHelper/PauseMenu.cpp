#include "stdafx.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu(EMenuType menuType)
{
	cmenu = lookupMenuForIndex( (int) menuType );
	m_origMenuItemCount = cmenu->itemCount;
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, int type)
{
	return AddItem(text, EMenuType::SETTINGS_LIST, type);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, EMenuType baseType, int type)
{
	int settingIndex = -1;

	while (getMenuPreference(settingIndex) != -1)
	{
		settingIndex++;

		if (settingIndex > 255)
		{
			throw gcnew System::IndexOutOfRangeException("No valid setting index found.");
		}
	}

	return AddItem(text, baseType, type, settingIndex);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, EMenuType layoutType, int type, int settingIndex)
{
	const char * cstr = static_cast<const char*>(static_cast<void*>(Marshal::StringToHGlobalAnsi(text)));

	auto * pMenuItem = CMenuFunctions::AppendMenuItem(cmenu, cstr, (int)layoutType, 2, type, settingIndex, 0);

	PauseMenuItem ^ item = gcnew PauseMenuItem(pMenuItem);

	item->Initialize();

	return item;
}

PauseMenu::~PauseMenu()
{
	this->!PauseMenu();
}

PauseMenu::!PauseMenu()
{
	if (cmenu->itemCount > m_origMenuItemCount)
	{
		for (int i = m_origMenuItemCount; i < cmenu->itemCount; i++)
		{
			CMenuFunctions::RemoveMenuItem(cmenu, this[i]->m_item);
		}
	}
}
