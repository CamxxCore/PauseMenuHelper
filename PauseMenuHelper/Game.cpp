#include "Game.h"
#include <mutex>
#include <map>

#ifdef _DEBUG
#define DEBUGLOG(str) OutputDebugStringA(str);
#else
#define DEBUGLOG(str)
#endif

GetHashKey_t g_getHashKey;

std::map<unsigned int, std::string> g_textEntries;

static std::mutex g_textMutex;

CallHook<GetGlobalTextEntry_t> * g_getGxtEntryFn;

CallHook<SetMenuSlot_t> * g_createSliderItemFn;

CallHook<SetMenuSlot_t> * g_createToggleItemFn;

CallHook<SetPauseMenuPreference_t> * g_setPauseMenuPreferenceFn;

CallHook<CallFunctionOnMovie> * g_callInteractionResponseFn;

rage::pgCollection<CPauseMenuInstance> * g_activeMenuArray;

uintptr_t g_globalTextManager = 0;

int g_nativeMenuPrefCount = 0;

struct CustomMenuPref
{
	int m_menuId;
	int m_itemIndex;
	int m_value;
	CMenuPreferenceCallback m_callback;
};


std::map<int, CustomMenuPref> g_customPrefs;

int g_origMenuCount;

bool bIgnoreNextMenuEvent = false;

void callFunctionOnMovie_Stub(void * sfMovie, const char * functionName, CScaleformParameter * arg, CScaleformParameter * arg1, CScaleformParameter * arg2, CScaleformParameter * arg3)
{
	int menuState = static_cast<int>(arg->m_value.dvalue) - 1000;
	// we need to explicity override the menu state here so the game loads
	// a valid layout for custom sub menus.
	// This is necessary or the game won't set any layout at all.
	int idx = menuState - 148;

	if (idx > 0 && idx < g_customPrefs.size())
	{
		arg->m_value.dvalue = 1136.0;
	}

	return g_callInteractionResponseFn->fn(sfMovie, functionName, arg, arg1, arg2, arg3);
}

const char * getGxtEntry_Stub(void * gtxArray, unsigned int hashName)
{
	std::unique_lock<std::mutex> lock(g_textMutex);

	auto it = g_textEntries.find(hashName);

	if (it != g_textEntries.end())
	{
		return it->second.c_str();
	}

	return g_getGxtEntryFn->fn(gtxArray, hashName);
}

bool SetMenuSlot_Stub(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bSlotUpdate)
{
	if (settingIndex >= 175) // custom?
	{
		auto it = g_customPrefs.find(settingIndex);

		if (it != g_customPrefs.end())
		{
			value = it->second.m_value;
		}
	}

	return g_createToggleItemFn->fn(columnId, slotIndex, menuState, settingIndex, unk, value, text, bPopScaleform, bSlotUpdate);
}

void SetPauseMenuPreference_Stub(long long settingIndex, int value, unsigned int unk)
{
	if (settingIndex >= 175)
	{
		DEBUGLOG("SetPauseMenuPreference_Stub(): Found a custom pref.. doing lookup.");

		auto it = g_customPrefs.find((int)settingIndex);

		if (it != g_customPrefs.end())
		{
			DEBUGLOG("SetPauseMenuPreference_Stub(): Found custom pref instance.");

			it->second.m_value = value;

			if (!bIgnoreNextMenuEvent)
			{
				if (it->second.m_callback)
				{
					DEBUGLOG("SetPauseMenuPreference_Stub(): Found callback..");

					auto cmenu = lookupMenuForIndex(it->second.m_menuId);

					if (cmenu)
					{
						DEBUGLOG("SetPauseMenuPreference_Stub(): Invoking callback..");

						it->second.m_callback(cmenu, it->second.m_itemIndex, value);
					}
				}
			}

			else bIgnoreNextMenuEvent = false;
		}
	}

	return g_setPauseMenuPreferenceFn->fn(settingIndex, value, unk);
}

int getMenuPreference(int settingIndex)
{
	if (settingIndex >= 175)
	{
		auto it = g_customPrefs.find((int)settingIndex);

		if (it != g_customPrefs.end())
		{
			return it->second.m_value;
		}
	}

	return -1;
}

void setMenuPreference(int settingIndex, int value, bool ignoreCallback)
{
	DEBUGLOG("setMenuPreference(): setting menu pref..");

	bIgnoreNextMenuEvent = ignoreCallback;

	SetPauseMenuPreference_Stub(settingIndex, value, 3u);
}

unsigned int getHashKey(const char * text)
{
	return g_getHashKey(text, 0);
}

const char * getGxtEntry(unsigned int key)
{
	auto it = g_textEntries.find(key);

	if (it != g_textEntries.end())
	{
		return it->second.c_str();
	}

	return getGxtEntry_Stub((void*)g_globalTextManager, key);
}

unsigned int setGxtEntry(const char * key, const char * text)
{
	auto hashKey = getHashKey(key);

	std::unique_lock<std::mutex> lock(g_textMutex);

	g_textEntries[hashKey] = text;

	return hashKey;
}

CPauseMenuInstance * lookupMenuForIndex(int menuIndex)
{
	for (auto it = g_activeMenuArray->begin(); it != g_activeMenuArray->end(); it++)
	{
		if (it && it->menuId == menuIndex)
			return it;
	}

	return NULL;
}

int getFreeMenuIndex()
{
	int menuId = 148;

	for (auto it = g_activeMenuArray->begin(); it != g_activeMenuArray->end(); it++)
	{
		if (it && it->menuId > menuId)
			menuId = it->menuId;
	}

	menuId++;

	return menuId;
}

int getFreeSettingIndex()
{
	int settingIndex = 175;

	while (getMenuPreference(settingIndex) != -1)
	{
		settingIndex++;

		if (settingIndex > 255)
			return 0;
	}

	return settingIndex;
}

CPauseMenuInstance * addMenuInstance(int menuId)
{
	CPauseMenuInstance menu;

	menu.menuId = menuId;

	int const newItemCount = g_activeMenuArray->m_count + 1;

	int const newSize = newItemCount * g_activeMenuArray->m_size;

	CPauseMenuInstance * newMenuArray = (CPauseMenuInstance*)malloc(newSize);

	CPauseMenuInstance * pOriginalItems = g_activeMenuArray->m_data;

	memset(newMenuArray, 0x0, newSize);

	memcpy_s(
		newMenuArray, 
		newSize, 
		g_activeMenuArray->m_data, 
		g_activeMenuArray->m_count * sizeof(CPauseMenuInstance));

	newMenuArray[newItemCount - 1] = menu;

	g_activeMenuArray->m_data = newMenuArray;

	g_activeMenuArray->m_count = newItemCount;

	g_activeMenuArray->m_size = newItemCount;

	return &newMenuArray[newItemCount - 1];
}

void removeMenuInstance(CPauseMenuInstance * menu)
{
	int const newItemCount = g_activeMenuArray->m_count - 1;

	int const newSize = newItemCount * g_activeMenuArray->m_size;

	bool bSearchError = true;

	for (auto it = g_activeMenuArray->begin(); it != g_activeMenuArray->end(); it++)
	{
		if (it == menu)
		{
			for (auto x = it; x < g_activeMenuArray->end() - 1; x++)
				*x = *(x + 1);

			bSearchError = false;
			break;
		}
	}

	if (bSearchError) return;

	CPauseMenuInstance * newMenuArray = (CPauseMenuInstance*)malloc(newSize);

	CPauseMenuInstance * pOriginalItems = g_activeMenuArray->m_data;

	memset(newMenuArray, 0x0, newSize);

	memcpy_s(newMenuArray, newSize, g_activeMenuArray->m_data, newSize);

	g_activeMenuArray->m_data = newMenuArray;

	if (g_activeMenuArray->m_count > g_origMenuCount)
		delete pOriginalItems;

	g_activeMenuArray->m_count = newItemCount;

	g_activeMenuArray->m_size = newItemCount;
}

void registerMenuPref(int prefId, int menuId, int itemIndex, CMenuPreferenceCallback callback)
{
	auto it = g_customPrefs.find(prefId);

	if (it == g_customPrefs.end())
	{
		CustomMenuPref p;
		p.m_menuId = menuId;
		p.m_itemIndex = itemIndex;
		p.m_callback = callback;
		p.m_value = 0;

		g_customPrefs.insert(std::make_pair(prefId, p));
	}
}

void unregisterMenuPref(int prefId)
{
	auto it = g_customPrefs.find(prefId);

	if (it != g_customPrefs.end())
	{
		g_customPrefs.erase(prefId);
	}
}

void initializeGame()
{
	auto result = BytePattern((BYTE*)"\x3B\xC7\x74\x18", "xxxx").get(-0x5);

	if (result)
	{
		g_getHashKey = (GetHashKey_t)(*reinterpret_cast<int *>(result - 4) + result);
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_getHashKey");
		return;
	}
		
	result = BytePattern((BYTE*)"\x48\x85\xC0\x75\x34\x8B\x0D", "xxxxxxx").get(-0x5);

	if (result)
	{
		g_getGxtEntryFn = HookManager::SetCall<GetGlobalTextEntry_t>(result, getGxtEntry_Stub);
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_getGxtEntryFn");
		return;
	}

	result = BytePattern((BYTE*)"\x0F\xB7\x54\x51\x00", "xxxx?").get();

	if (result)
	{
		g_activeMenuArray = (rage::pgCollection<CPauseMenuInstance>*)(*reinterpret_cast<int *>(result - 4) + result);

		g_origMenuCount = g_activeMenuArray->m_count;
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_activeMenuArray");
		return;
	}

	BytePattern pattern = BytePattern((BYTE*)"\x83\xFF\x05\x74\x15", "xxxxx");

	result = pattern.get(-0x1A);
	
	if (result)
	{
		g_createSliderItemFn = HookManager::SetCall<SetMenuSlot_t>(result, SetMenuSlot_Stub); //-0x1A
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find SetMenuSlot #1");
		return;
	}

	result = pattern.get(0xA8);

	if (result)
	{
		g_createToggleItemFn = HookManager::SetCall<SetMenuSlot_t>(result, SetMenuSlot_Stub); // +0xA8
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find SetMenuSlot #2");
		return;
	}
	
	result = pattern.get(-0x58);

	if (result)
	{
		g_globalTextManager = (uintptr_t) *reinterpret_cast<int*>(result - 4) + result;
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_globalTextManager #2");
		return;
	}

	pattern = BytePattern((BYTE*)"\xF2\x0F\x2C\x56\x00", "xxxx?");

	result = pattern.get(0x20);

	if (result)
	{
		g_setPauseMenuPreferenceFn = HookManager::SetCall<SetPauseMenuPreference_t>(result, SetPauseMenuPreference_Stub);
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_setPauseMenuPreferenceFn");
		return;
	}

	result = pattern.get(0x12);

	if (result)
	{
		// always toggle preferences
		memset((void*)result, 0x90, 6);
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find toggle prefs patch");
		return;
	}

	pattern = BytePattern((BYTE*)"\x44\x8B\x4C\x24\x00\x45\x8B\xC5", "xxxx?xxx");

	result = pattern.get(-0x9);

	if (result)
	{
		g_callInteractionResponseFn = HookManager::SetCall<CallFunctionOnMovie>(result, callFunctionOnMovie_Stub);
	}

	else
	{
		DEBUGLOG("initializeGame(): Failed to find g_callInteractionResponseFn");
		return;
	}
}

void unload()
{
	if (g_callInteractionResponseFn)
	{
		delete g_callInteractionResponseFn;
		g_callInteractionResponseFn = nullptr;
	}

	if (g_getGxtEntryFn)
	{
		delete g_getGxtEntryFn;
		g_getGxtEntryFn = nullptr;
	}

	if (g_setPauseMenuPreferenceFn)
	{
		delete g_setPauseMenuPreferenceFn;
		g_setPauseMenuPreferenceFn = nullptr;
	}

	if (g_createSliderItemFn)
	{
		delete g_createSliderItemFn;
		g_createSliderItemFn = nullptr;
	}

	if (g_createToggleItemFn)
	{
		delete g_createToggleItemFn;
		g_createToggleItemFn = nullptr;
	}
}
