#include "Game.h"
#include <mutex>
#include <map>

#include "pattern.h"

#ifdef _DEBUG
#define LOG(x) OutputDebugStringA(x);
#else
#define LOG(x)
#endif

GetHashKey_t g_getHashKey;

std::map<unsigned int, std::string> g_textEntries;

static std::mutex g_textMutex;

CallHook<GetGlobalTextEntry_t> * g_getGxtEntry;

CallHook<SetMenuSlot_t> * g_createSliderItem;

CallHook<SetMenuSlot_t> * g_createToggleItem;

CallHook<SetPauseMenuPreference_t> * g_setPauseMenuPreference;

CallHook<CallFunctionOnMovie_t> * g_callInteractionResponse;

rage::pgCollection<UIMenu> * g_activeMenuArray;

MemAddr g_globalTextManager;

int g_origMenuCount;

bool bIgnoreNextMenuEvent = false;

struct CustomMenuPref
{
	CustomMenuPref()
	{
		memset(this, 0x0, sizeof(CustomMenuPref));
	}

	int m_menuId;
	int m_itemIndex;
	int m_value;

	CMenuPreferenceCallback m_callback;
};

std::map<int, CustomMenuPref> g_customPrefs;

#pragma region Trampoline Funcs

const char * getGxtEntry_Hook(void * gtxArray, unsigned int hashName)
{
	std::unique_lock<std::mutex> lock(g_textMutex);

	auto it = g_textEntries.find(hashName);

	if (it != g_textEntries.end())
	{
		return it->second.c_str();
	}

	return g_getGxtEntry->fn(gtxArray, hashName);
}

void SetPauseMenuPreference_Hook(long long settingIndex, int value, unsigned int unk)
{
	if (settingIndex >= 175)
	{
		LOG("SetPauseMenuPreference_Hook(): Found a custom pref.. doing lookup.");

		auto it = g_customPrefs.find(static_cast<int>(settingIndex));

		if (it != g_customPrefs.end())
		{
			LOG("SetPauseMenuPreference_Hook(): Found custom pref instance.");

			it->second.m_value = value;

			if (!bIgnoreNextMenuEvent)
			{
				if (it->second.m_callback)
				{
					LOG("SetPauseMenuPreference_Hook(): Found callback..");

					auto cmenu = lookupMenuForIndex(it->second.m_menuId);

					if (cmenu)
					{
						LOG("SetPauseMenuPreference_Hook(): Invoking callback..");

						it->second.m_callback(cmenu, it->second.m_itemIndex, value);
					}
				}
			}

			else bIgnoreNextMenuEvent = false;
		}
	}

	return g_setPauseMenuPreference->fn(settingIndex, value, unk);
}

bool SetMenuSlot_Hook(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bSlotUpdate)
{
	if (settingIndex >= 175) // custom?
	{
		auto it = g_customPrefs.find(settingIndex);

		if (it != g_customPrefs.end())
		{
			value = it->second.m_value;
		}
	}

	return g_createToggleItem->fn(columnId, slotIndex, menuState, settingIndex, unk, value, text, bPopScaleform, bSlotUpdate);
}

void callFunctionOnMovie_Hook(void * sfMovie, const char * functionName, CScaleformParameter * arg, CScaleformParameter * arg1, CScaleformParameter * arg2, CScaleformParameter * arg3)
{
	auto menuState = static_cast<int>(arg->m_value.dvalue) - 1000;

	// we need to explicity override the menu state here so the game loads
	// a valid layout for custom sub menus.
	// This is necessary or the game won't set any layout at all.

	for (auto it = g_customPrefs.begin(); it != g_customPrefs.end(); ++it)
	{
		if (it->second.m_menuId == menuState)
		{
			arg->m_value.dvalue = 1138.0;
			break;
		}
	}

	return g_callInteractionResponse->fn(sfMovie, functionName, arg, arg1, arg2, arg3);
}

#pragma endregion

UIMenu * lookupMenuForIndex(int menuIndex)
{
	for (auto it = g_activeMenuArray->begin(); it != g_activeMenuArray->end(); it++)
	{
		if (it && it->menuId == menuIndex)
			return it;
	}

	return nullptr;
}

UIMenu * addMenuInstance(int menuId)
{
	UIMenu menu;

	menu.menuId = menuId;

	auto const newItemCount = g_activeMenuArray->m_count + 1;

	auto const newSize = newItemCount * g_activeMenuArray->m_size;

	UIMenu* newMenuArray = static_cast<UIMenu*>(malloc(newSize));

	memset(newMenuArray, 0x0, newSize);

	memcpy_s(
		newMenuArray,
		newSize,
		g_activeMenuArray->m_data,
		g_activeMenuArray->m_count * sizeof(UIMenu));

	newMenuArray[newItemCount - 1] = menu;

	g_activeMenuArray->m_data = newMenuArray;

	g_activeMenuArray->m_count = newItemCount;

	g_activeMenuArray->m_size = newItemCount;

	return &newMenuArray[newItemCount - 1];
}

void removeMenuInstance(UIMenu * menu)
{
	auto const newItemCount = g_activeMenuArray->m_count - 1;

	auto const newSize = newItemCount * g_activeMenuArray->m_size;

	auto bSearchError = true;

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

	auto newMenuArray = static_cast<UIMenu*>(malloc(newSize));

	auto pOriginalItems = g_activeMenuArray->m_data;

	memset(newMenuArray, 0x0, newSize);

	memcpy_s(newMenuArray, newSize, g_activeMenuArray->m_data, newSize);

	g_activeMenuArray->m_data = newMenuArray;

	if (g_activeMenuArray->m_count > g_origMenuCount)
		delete pOriginalItems;

	g_activeMenuArray->m_count = newItemCount;

	g_activeMenuArray->m_size = newItemCount;
}

int getFreeMenuIndex()
{
	auto menuId = 148;

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
	auto settingIndex = 175;

	while (getMenuPref(settingIndex) != -1)
	{
		settingIndex++;

		if (settingIndex > 255)
			return 0;
	}

	return settingIndex;
}

int getMenuPref(int settingIndex)
{
	if (settingIndex >= 175)
	{
		auto it = g_customPrefs.find(static_cast<int>(settingIndex));

		if (it != g_customPrefs.end())
		{
			return it->second.m_value;
		}
	}

	return -1;
}

void setMenuPref(int settingIndex, int value, bool ignoreCallback)
{
	LOG("setMenuPref(): setting menu pref..");

	bIgnoreNextMenuEvent = ignoreCallback;

	SetPauseMenuPreference_Hook(settingIndex, value, 3u);
}

void registerMenuPref(int prefId, int menuId, int itemIndex, CMenuPreferenceCallback cb)
{
	auto it = g_customPrefs.find(prefId);

	if (it == g_customPrefs.end())
	{
		CustomMenuPref p;

		p.m_menuId = menuId;
		p.m_itemIndex = itemIndex;
		p.m_callback = cb;
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

	return getGxtEntry_Hook(g_globalTextManager, key);
}

unsigned int setGxtEntry(const char * key, const char * text)
{
	auto hashKey = getHashKey(key);

	std::unique_lock<std::mutex> lock(g_textMutex);

	g_textEntries[hashKey] = text;

	return hashKey;
}

void initializeGame()
{
	auto result = BytePattern((BYTE*)"\x3B\xC7\x74\x18", "xxxx").get(-0x5);

	if (result)
	{
		g_getHashKey = reinterpret_cast<GetHashKey_t>(*reinterpret_cast<int *>(result - 4) + result);
	}

	else
	{
		LOG("initializeGame(): Failed to find g_getHashKey");
		return;
	}
		
	result = BytePattern((BYTE*)"\x48\x85\xC0\x75\x34\x8B\x0D", "xxxxxxx").get(-0x5);

	if (result)
	{
		g_getGxtEntry = HookManager::SetCall<GetGlobalTextEntry_t>(result, getGxtEntry_Hook);
	}

	else
	{
		LOG("initializeGame(): Failed to find g_getGxtEntryFn");
		return;
	}

	result = BytePattern((BYTE*)"\x0F\xB7\x54\x51\x00", "xxxx?").get();

	if (result)
	{
		g_activeMenuArray = reinterpret_cast<rage::pgCollection<UIMenu>*>(
			*reinterpret_cast<int *>(result - 4) + result);

		g_origMenuCount = g_activeMenuArray->m_count;
	}

	else
	{
		LOG("initializeGame(): Failed to find g_activeMenuArray");
		return;
	}

	BytePattern pattern = BytePattern((BYTE*)"\x83\xFF\x05\x74\x15", "xxxxx");

	result = pattern.get(-0x1A);
	
	if (result)
	{
		g_createSliderItem = HookManager::SetCall<SetMenuSlot_t>(result, SetMenuSlot_Hook); //-0x1A
	}

	else
	{
		LOG("initializeGame(): Failed to find SetMenuSlot #1");
		return;
	}

	result = pattern.get(0xA9);

	if (result)
	{
		g_createToggleItem = HookManager::SetCall<SetMenuSlot_t>(result, SetMenuSlot_Hook); // +0xA9
	}

	else
	{
		LOG("initializeGame(): Failed to find SetMenuSlot #2");
		return;
	}
	
	result = pattern.get(-0x58);

	if (result)
	{
		g_globalTextManager = static_cast<uintptr_t>(*reinterpret_cast<int*>(result - 4)) + result;
	}

	else
	{
		LOG("initializeGame(): Failed to find g_globalTextManager #2");
		return;
	}

	pattern = BytePattern((BYTE*)"\xF2\x0F\x2C\x56\x00", "xxxx?");

	result = pattern.get(0x20);

	if (result)
	{
		g_setPauseMenuPreference = HookManager::SetCall<SetPauseMenuPreference_t>(result, SetPauseMenuPreference_Hook);
	}

	else
	{
		LOG("initializeGame(): Failed to find g_setPauseMenuPreferenceFn");
		return;
	}

	result = pattern.get(0x12);

	if (result)
	{
		// always toggle preferences
		memset(reinterpret_cast<void*>(result), 0x90, 6);
	}

	else
	{
		LOG("initializeGame(): Failed to find toggle prefs patch");
		return;
	}

	pattern = BytePattern((BYTE*)"\x44\x8B\x4C\x24\x00\x45\x8B\xC5", "xxxx?xxx");

	result = pattern.get(-0x9);

	if (result)
	{
		g_callInteractionResponse = HookManager::SetCall<CallFunctionOnMovie_t>(result, callFunctionOnMovie_Hook);
	}

	else
	{
		LOG("initializeGame(): Failed to find g_callInteractionResponseFn");
	}
}

void unload()
{
	if (g_callInteractionResponse)
	{
		delete g_callInteractionResponse;
		g_callInteractionResponse = nullptr;
	}

	if (g_getGxtEntry)
	{
		delete g_getGxtEntry;
		g_getGxtEntry = nullptr;
	}

	if (g_setPauseMenuPreference)
	{
		delete g_setPauseMenuPreference;
		g_setPauseMenuPreference = nullptr;
	}

	if (g_createSliderItem)
	{
		delete g_createSliderItem;
		g_createSliderItem = nullptr;
	}

	if (g_createToggleItem)
	{
		delete g_createToggleItem;
		g_createToggleItem = nullptr;
	}
}
