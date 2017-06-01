#pragma once

class CMenuFunctions sealed
{
public:
	static uiWidget * AppendItem(UIMenu * pMenu, const char * label, int menuId, int type, int actionType, int settingIdx, int stateFlags, bool bFreeOldItems);
	
	static int GetItemIndex(UIMenu * pMenu, uiWidget * pItem);

	static void RemoveItem(UIMenu * pMenu, uiWidget * pItem);

	static void SetSize(UIMenu * pMenu, int newItemCount, bool bFreeOldItems = false);
};
