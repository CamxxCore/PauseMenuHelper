#pragma once

class CMenuFunctions sealed
{
public:
	static CPauseMenuItem * AppendItem(CPauseMenuInstance * pMenu, const char * label, int menuId, int type, int actionType, int settingIdx, int stateFlags, bool bFreeOldItems);
	static void SetSize(CPauseMenuInstance * pMenu, int newItemCount, bool bFreeOldItems = false);
	static void RemoveItem(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem);
	static int GetItemIndex(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem);
};

