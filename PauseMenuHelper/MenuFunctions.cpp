#include "stdafx.h"

uiWidget * CMenuFunctions::AppendItem(UIMenu * pMenu, const char * label, int menuIndex, int type, int actionType, int settingIdx, int stateFlags, bool bFreeOldItems)
{
	uiWidget item;

	item.type = type;
	item.menuIndex = menuIndex;
	item.textHash = setGxtEntry(label, label);
	item.settingId = settingIdx;
	item.actionType = actionType;
	item.stateFlags = stateFlags;

	CMenuFunctions::SetSize(pMenu, pMenu->itemCount + 1, bFreeOldItems);

	pMenu->items[pMenu->itemCount - 1] = item;

	return &pMenu->items[pMenu->itemCount - 1];
}

void CMenuFunctions::SetSize(UIMenu * pMenu, int newItemCount, bool bFreeOldItems)
{
	auto allocator = rage::GetAllocator();

	uiWidget * pOriginalItems = pMenu->items;

	if (newItemCount > 0 && pMenu->itemCount != newItemCount)
	{
		size_t newSize = newItemCount * sizeof(uiWidget);

		uiWidget * newItemArray = (uiWidget*)allocator->allocate(newSize, 16, 0);

		memcpy_s(newItemArray, newSize, pMenu->items, min(newSize, pMenu->itemCount * sizeof(uiWidget)));

		pMenu->itemCount = newItemCount;

		pMenu->maxItems = newItemCount;

		pMenu->items = newItemArray;
	}

	if (bFreeOldItems)
		allocator->free(pOriginalItems);
}

void CMenuFunctions::RemoveItem(UIMenu * pMenu, uiWidget * pItem)
{
	auto index = CMenuFunctions::GetItemIndex(pMenu, pItem);

	if (index != -1)
	{
		for (int i = index; i < pMenu->itemCount - 1; i++)
			pMenu->items[i] = pMenu->items[i + 1];
		SetSize(pMenu, pMenu->itemCount - 1);
	}
}

int CMenuFunctions::GetItemIndex(UIMenu * pMenu, uiWidget * pItem)
{
	for (int i = 0; i < pMenu->itemCount; i++)
	{
		if (pItem == &pMenu->items[i])
			return i;
	}
	return -1;
}
