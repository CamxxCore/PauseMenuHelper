#include "stdafx.h"

PauseMenu::PauseMenu()
{
	this->PauseMenu::PauseMenu(static_cast<MenuType>(getFreeMenuIndex()));
}

PauseMenu::PauseMenu(MenuType menuType)
{
	m_menuId = (int)menuType;

	m_addedItems = gcnew System::Collections::Generic::List<PauseMenuItem^>();

	if (!lookupMenuForIndex(m_menuId)) {
		addMenuInstance(m_menuId);
		bAddonMenu = true;
	}
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu, int settingIndex)
{
	marshal_context cxt;
	
	auto * cmenuitem = AddWidgetInternal(cxt.marshal_as<const char*>(text), 
		(int)childMenu, (int)type, subtype, settingIndex);

	PauseMenuItem ^ pMenuItem = gcnew PauseMenuItem();

	pMenuItem->Initialize(getMenuReference(), cmenuitem);

	m_addedItems->Add(pMenuItem);

	return pMenuItem;
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu)
{
	return AddItem(text, type, subtype, childMenu, 0);
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

PauseMenuItem ^ PauseMenu::AddItem(System::String ^ text, MenuSettingType settingType, int initialValue)
{
	auto item = AddItem(text, MenuItemType::Setting, 
		(int)settingType, MenuType::Settings_List, getFreeSettingIndex());

	auto baseitem = item->getBaseRef();

	setMenuPref(baseitem->settingId, initialValue, true);

	return item;
}

PauseMenuItem ^ PauseMenu::AddItem(System::String ^ text, MenuSettingType settingType)
{
	return AddItem(text, settingType, 0);
}

PauseMenuItem ^ PauseMenu::ItemAt(int index)
{
	return this[index];
}

int PauseMenu::IndexOf(PauseMenuItem ^ item)
{
	return CMenuFunctions::GetItemIndex(getMenuReference(), item->getBaseRef());
}

void PauseMenu::Remove(PauseMenuItem ^ item)
{
	item->~PauseMenuItem();

	CMenuFunctions::RemoveItem(getMenuReference(), item->getBaseRef());

	if (m_addedItems->Contains(item)) {
		m_addedItems->Remove(item);
	}
}

void PauseMenu::RemoveAt(int index)
{
	Remove(ItemAt(index));
}

void PauseMenu::Clear()
{
	auto cmenu = getMenuReference();

	if (bAddonMenu)
	{
		for (int i = 0; i < cmenu->itemCount; i++)
		{
			this[i]->~PauseMenuItem();
		}

		CMenuFunctions::SetSize(cmenu, 0, true);
	}

	else
	{
		for each(PauseMenuItem ^ item in m_addedItems)
		{
			item->~PauseMenuItem();
		}

		CMenuFunctions::SetSize(cmenu, 
			cmenu->itemCount - m_addedItems->Count, 
			m_addedItems->Count > 0);
	}

	m_addedItems->Clear();
}

PauseMenu::~PauseMenu()
{
	Clear();
	this->!PauseMenu();
}

PauseMenu::!PauseMenu()
{
	if (bAddonMenu) {
		auto cmenu = getMenuReference();
		removeMenuInstance(cmenu);
	}
}
