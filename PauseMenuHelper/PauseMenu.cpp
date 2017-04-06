#include "stdafx.h"

PauseMenu::PauseMenu()
{
	this->PauseMenu::PauseMenu(static_cast<MenuType>(getFreeMenuIndex()));
}

PauseMenu::PauseMenu(MenuType menuType)
{
	m_menuId = (int)menuType;

	m_addedItems = gcnew System::Collections::Generic::List<PauseMenuItem^>();

	if (!lookupMenuForIndex(m_menuId))
	{
		addMenuInstance(m_menuId);

		bIsAddonMenu = true;
	}
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu, int settingIndex)
{
	auto cmenu = lookupMenuForIndex(m_menuId);

	if (!cmenu) throw gcnew System::NullReferenceException("Internal menu reference not found.");

	const char * cstr = static_cast<const char*>(static_cast<void*>(Marshal::StringToHGlobalAnsi(text)));

	auto * pMenuItem = CMenuFunctions::AppendItem(cmenu, cstr, (int)childMenu, (int)type, subtype, settingIndex, 0, m_addedItemCount > 0);

	PauseMenuItem ^ item = gcnew PauseMenuItem();

	item->Initialize(cmenu, pMenuItem);

	m_addedItemCount++;

	m_addedItems->Add(item);

	return item;
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu)
{
	int settingIndex;

	if (type == MenuItemType::Setting)
	{
		settingIndex = 200;

		while (getMenuPreference(settingIndex) != -1)
		{
			settingIndex++;

			if (settingIndex > 255)
				throw gcnew System::IndexOutOfRangeException("No valid setting index found.");
		}
	}

	else settingIndex = 0;

	return AddItem(text, type, subtype, childMenu, settingIndex);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text)
{
	return AddItem(text, MenuItemType::Default);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type)
{
	return AddItem(text, type, 0);
}

PauseMenuItem^ PauseMenu::AddItem(System::String ^ text, MenuItemType type, int subtype)
{
	return AddItem(text, type, subtype, MenuType::Settings_List);
}

int PauseMenu::IndexOf(PauseMenuItem ^ item)
{
	auto cmenu = lookupMenuForIndex(m_menuId);

	if (!cmenu) throw gcnew System::NullReferenceException("Internal menu reference not found.");

	return CMenuFunctions::GetItemIndex(cmenu, item->getMenuRef());
}

void PauseMenu::Remove(PauseMenuItem ^ item)
{
	auto cmenu = lookupMenuForIndex(m_menuId);

	if (!cmenu) throw gcnew System::NullReferenceException("Internal menu reference not found.");

	auto pItem = item->getMenuRef();

	unregisterMenuPref(pItem->settingId);

	CMenuFunctions::RemoveItem(cmenu, pItem);

	if (m_addedItems->Contains(item))
		m_addedItems->Remove(item);
}

void PauseMenu::RemoveAt(int index)
{
	auto cmenu = lookupMenuForIndex(m_menuId);

	if (!cmenu) throw gcnew System::NullReferenceException("Internal menu reference not found.");

	if (index < 0 || index > cmenu->itemCount)
		throw gcnew System::ArgumentOutOfRangeException("itemIndex: out of range: " + index);

	auto pItem = this[index]->getMenuRef();

	unregisterMenuPref(pItem->settingId);

	CMenuFunctions::RemoveItem(cmenu, pItem);

	int origCount = cmenu->itemCount - m_addedItems->Count;

	if (index >= origCount)
		m_addedItems->RemoveAt(index - origCount);
}

void PauseMenu::Clear()
{
	OutputDebugString(TEXT("PauseMenu::Clear(): clearing"));

	auto cmenu = lookupMenuForIndex(m_menuId);

	if (!cmenu) throw gcnew System::NullReferenceException("Internal menu reference not found.");

	if (bIsAddonMenu)
	{
		for (int i = 0; i < cmenu->itemCount; i++)
		{
			this[i]->~PauseMenuItem();
		}

		CMenuFunctions::SetSize(cmenu, 0, true);
	}

	else
	{
		OutputDebugString(TEXT("PauseMenu::Clear(): Restoring original menu..."));

		for each(PauseMenuItem ^ item in m_addedItems)
		{
			item->~PauseMenuItem();
		}

		int addedItemCount = m_addedItems->Count;

		CMenuFunctions::SetSize(cmenu, cmenu->itemCount - addedItemCount, addedItemCount > 0);
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
	if (bIsAddonMenu)
	{
		auto cmenu = lookupMenuForIndex(m_menuId);

		if (cmenu)
			removeMenuInstance(cmenu);
	}
}
