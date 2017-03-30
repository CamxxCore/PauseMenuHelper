
#include "Game.h"
#include <mutex>
#include <map>

GetHashKey_t g_getHashKey;

std::map<unsigned int, std::string> g_textEntries;

static std::mutex g_textMutex;

CallHook<GetGlobalTextEntry_t> * g_getGxtEntryFn;

CallHook<SetMenuSlot_t> * g_createSliderItemFn;

CallHook<SetMenuSlot_t> * g_createToggleItemFn;

CallHook<SetPauseMenuPreference_t> * g_setPauseMenuPreferenceFn;

rage::pgCollection<CPauseMenuInstance> activeMenuArray;

uintptr_t g_globalTextManager = 0;

struct CustomMenuPref
{
	CMenuItemEventCallback m_callback;
	int m_value;
};

std::map<int, CustomMenuPref> g_customPrefs;

const char * getGxtEntry_Stub(void * gtxArray, unsigned int hashName)
{
	std::unique_lock<std::mutex> lock(g_textMutex);

	auto it = g_textEntries.find(hashName);

	if (it != g_textEntries.end())
	{
		return it->second.c_str();
	}

	return g_getGxtEntryFn->function(gtxArray, hashName);
}

bool SetMenuSlot_Stub(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bSlotUpdate)
{
	if (settingIndex >= 200)
	{
		auto it = g_customPrefs.find(settingIndex);

		if (it != g_customPrefs.end())
		{
			value = it->second.m_value;
		}
	}

	return g_createToggleItemFn->function(columnId, slotIndex, menuState, settingIndex, unk, value, text, bPopScaleform, bSlotUpdate);
}

void SetPauseMenuPreference_Stub(long long settingIndex, int value, unsigned int unk)
{
	if (settingIndex >= 200)
	{
		auto it = g_customPrefs.find((int)settingIndex);

		if (it != g_customPrefs.end())
		{
			int oldValue = it->second.m_value;

			it->second.m_value = value;

			if (it->second.m_callback)
			{
				OutputDebugStringA("Firing native event from SetPauseMenuPreference_Stub");

				it->second.m_callback(oldValue, value);		
			}
		}
	}

	else return g_setPauseMenuPreferenceFn->function(settingIndex, value, unk);
}

int getMenuPreference(int settingIndex)
{
	if (settingIndex >= 200)
	{
		auto it = g_customPrefs.find((int)settingIndex);

		if (it != g_customPrefs.end())
		{
			return it->second.m_value;
		}
	}

	return -1;
}

void setMenuPreference(int settingIndex, int value)
{
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
	for (auto it = activeMenuArray.begin(); it != activeMenuArray.end(); it++)
	{
		if (it && it->menuId == menuIndex)
			return it;
	}

	return NULL;
}

void registerNativeMenuCallback(CPauseMenuItem * item, CMenuItemEventCallback callback)
{
	int preferenceIdx = (int)item->settingId;

	auto it = g_customPrefs.find(preferenceIdx);

	if (it == g_customPrefs.end())
	{
		CustomMenuPref p;
		p.m_callback = callback;
		p.m_value = 0;

		g_customPrefs.insert(std::make_pair(preferenceIdx, p));
	}
}

void initializeGame()
{
	auto result = Pattern((BYTE*)"\x3B\xC7\x74\x18", "xxxx").get(-0x5);
		
	g_getHashKey = (GetHashKey_t)(*reinterpret_cast<int *>(result - 4) + result);
		
	result = Pattern((BYTE*)"\x48\x85\xC0\x75\x34\x8B\x0D", "xxxxxxx").get(-0x5);

	g_getGxtEntryFn = HookManager::SetCall<GetGlobalTextEntry_t>(result, getGxtEntry_Stub);

	result = Pattern((BYTE*)"\x0F\xB7\x54\x51\x00", "xxxx?").get();

	activeMenuArray = *(rage::pgCollection<CPauseMenuInstance>*)(*reinterpret_cast<int *>(result - 4) + result);

	Pattern pattern = Pattern((BYTE*)"\x83\xFF\x05\x74\x15", "xxxxx");

	g_createSliderItemFn = HookManager::SetCall<SetMenuSlot_t>(pattern.get(-0x1A), SetMenuSlot_Stub); //-0x1A

	g_createToggleItemFn = HookManager::SetCall<SetMenuSlot_t>(pattern.get(0xA8), SetMenuSlot_Stub); // +0xA8
	
	result = pattern.get(-0x58);

	g_globalTextManager = (uintptr_t) *reinterpret_cast<int*>(result - 4) + result;

	pattern = Pattern((BYTE*)"\xF2\x0F\x2C\x56\x00", "xxxx?");

	g_setPauseMenuPreferenceFn = HookManager::SetCall<SetPauseMenuPreference_t>(pattern.get(0x20), SetPauseMenuPreference_Stub);
}

void unload()
{
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
