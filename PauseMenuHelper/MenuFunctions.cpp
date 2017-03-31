#include "stdafx.h"

CPauseMenuItem * CMenuFunctions::AppendItem(CPauseMenuInstance * pMenu, const char * label, int menuIndex, int type, int actionType, int settingIdx, int stateFlags, bool bFreeOldItems)
{
	SetSize(pMenu, pMenu->itemCount + 1, bFreeOldItems);

	CPauseMenuItem newItem;

	newItem.type = type;
	newItem.menuIndex = menuIndex;
	newItem.textHash = setGxtEntry(label, label);
	newItem.settingId = settingIdx;
	newItem.actionType = actionType;
	newItem.stateFlags = stateFlags;

	pMenu->items[pMenu->itemCount - 1] = newItem;

	return &pMenu->items[pMenu->itemCount - 1];
}

void CMenuFunctions::SetSize(CPauseMenuInstance * pMenu, int newItemCount, bool bFreeOldItems)
{
	auto allocator = rage::GetAllocator();

	size_t newSize = newItemCount * sizeof(CPauseMenuItem);

	CPauseMenuItem * newItemArray = (CPauseMenuItem*)allocator->allocate(newSize, 16, 0);

	CPauseMenuItem * pOriginalItems = pMenu->items;

	memcpy_s(newItemArray, newSize, pMenu->items, min(newSize, pMenu->itemCount * sizeof(CPauseMenuItem)));

	pMenu->itemCount = newItemCount;
	pMenu->maxItems = newItemCount;
	pMenu->items = newItemArray;

	if (bFreeOldItems)
		allocator->free(pOriginalItems);
}

void CMenuFunctions::RemoveItem(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem)
{
	for (int i = 0; i < pMenu->itemCount; i++)
	{
		if (pItem == &pMenu->items[i])
		{
			for (int x = i; x < pMenu->itemCount - 1; x++)
				pMenu->items[x] = pMenu->items[x + 1];
			SetSize(pMenu, pMenu->itemCount - 1);
			return;
		}
	}
}

int CMenuFunctions::GetItemIndex(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem)
{
	for (int i = 0; i < pMenu->itemCount; i++)
	{
		if (pItem == &pMenu->items[i])
			return i;
	}

	return -1;
}