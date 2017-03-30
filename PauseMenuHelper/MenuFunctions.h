#pragma once

class CMenuFunctions sealed
{
public:
	static CPauseMenuItem * AppendMenuItem(CPauseMenuInstance * pMenu, const char * label, int menuIdx, int type, int actionType, int settingIdx, int stateFlags);
	static void SetMenuSize(CPauseMenuInstance * pMenu, int newItemCount);
	static void RemoveMenuItem(CPauseMenuInstance * pMenu, CPauseMenuItem * pItem);
};

