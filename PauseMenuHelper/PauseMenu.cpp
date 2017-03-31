#include "stdafx.h"

PauseMenu::PauseMenu(MenuType menuType)
{
	cmenu = lookupMenuForIndex((int)menuType);

	if (!cmenu)
	{
		cmenu = addMenuInstance();
	}
}

PauseMenu::PauseMenu()
{
	cmenu = addMenuInstance();
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType subMenuType, int settingIndex)
{
	const char * cstr = static_cast<const char*>(static_cast<void*>(Marshal::StringToHGlobalAnsi(text)));

	auto * pMenuItem = CMenuFunctions::AppendItem(
		cmenu, cstr, (int)subMenuType, (int)type, subtype, settingIndex, 0, m_addedItemCount > 0);

	PauseMenuItem ^ item = gcnew PauseMenuItem(pMenuItem);

	item->Initialize();

	m_addedItemCount++;

	return item;
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType subMenuType)
{
	int settingIndex = -1;

	while (getMenuPreference(settingIndex) != -1)
	{
		settingIndex++;
		if (settingIndex > 255)
			throw gcnew System::IndexOutOfRangeException("No valid setting index found.");
	}

	return AddItem(text, type, subtype, subMenuType, settingIndex);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype)
{
	return AddItem(text, type, subtype, MenuType::Settings_List);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type)
{
	return AddItem(text, type, 0);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text)
{
	return AddItem(text, MenuItemType::Default);
}

int PauseMenu::IndexOf(PauseMenuItem ^ item)
{
	return CMenuFunctions::GetItemIndex(cmenu, item->m_item);
}

void PauseMenu::RemoveAt(int index)
{
	if (index < 0 || index > cmenu->itemCount)
		throw gcnew System::ArgumentOutOfRangeException("itemIndex: out of range: " + index);
	CMenuFunctions::RemoveItem(cmenu, this[index]->m_item);
}

void PauseMenu::Remove(PauseMenuItem ^ item)
{
	CMenuFunctions::RemoveItem(cmenu, item->m_item);
}

PauseMenu::~PauseMenu()
{
	this->!PauseMenu();
}

PauseMenu::!PauseMenu()
{
	CMenuFunctions::SetSize(cmenu, cmenu->itemCount - m_addedItemCount);
}
