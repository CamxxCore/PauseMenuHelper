#include "stdafx.h"
#include "MenuFunctions.h"

CPauseMenuItem * CMenuFunctions::AppendMenuItem(CPauseMenuInstance * pMenu, const char * label, int menuIndex, int type, int actionType, int settingIdx, int stateFlags)
{
	CPauseMenuItem newItem;

	memset(&newItem, 0x0, sizeof(CPauseMenuItem));

	newItem.type = type;

	newItem.textHash = setGxtEntry(label, label);

	newItem.menuIndex = menuIndex;

	newItem.settingId = settingIdx;

	newItem.actionType = actionType;

	newItem.stateFlags = stateFlags;

	SetMenuSize(pMenu, pMenu->itemCount + 1);

	pMenu->items[pMenu->itemCount - 1] = newItem;

	return &pMenu->items[pMenu->itemCount - 1];
}

void CMenuFunctions::SetMenuSize(CPauseMenuInstance * pMenu, int newItemCount)
{
	auto newSize = newItemCount * sizeof(CPauseMenuItem);

	CPauseMenuItem * pOriginalItems = pMenu->items;

	CPauseMenuItem * newItemArray = 
		(CPauseMenuItem*) rage::GetAllocator()->allocate(newSize, 16, 0);

	auto sourceSize = pMenu->itemCount > newItemCount ? 
		newSize : pMenu->itemCount * sizeof(CPauseMenuItem);

	memcpy_s(newItemArray, newSize, pMenu->items, sourceSize);

	pMenu->itemCount = newItemCount;

	pMenu->maxItems = newItemCount;

	pMenu->items = newItemArray;
}

void CMenuFunctions::RemoveMenuItem(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem)
{
	for (int i = 0; i < pMenu->itemCount; i++)
	{
		if (pItem == &pMenu->items[i])
		{
			for (int x = i; x < pMenu->itemCount - 1; x++)
				pMenu->items[x] = pMenu->items[x + 1];

			SetMenuSize(pMenu, pMenu->itemCount - 1);

			return;
		}
	}
}
