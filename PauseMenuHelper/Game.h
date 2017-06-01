#pragma once

#include <windows.h>

#include "Hooking.h"

#include "Types.h"

typedef unsigned int(*GetHashKey_t)(const char * text, unsigned int hashName);

typedef const char *(*GetGlobalTextEntry_t)(void * gtxArray, unsigned int hashName);

typedef void(*SetPauseMenuPreference_t)(long long settingIndex, int value, unsigned int unk);

typedef bool(*SetMenuSlot_t)(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bIsSlotUpdate);

typedef void(*CallFunctionOnMovie_t)(void * sfMovie, const char * functionName, CScaleformParameter * arg, CScaleformParameter * arg1, CScaleformParameter * arg2, CScaleformParameter * arg3);

typedef void(*CMenuPreferenceCallback)(UIMenu * parent, int itemIndex, int value);

UIMenu * lookupMenuForIndex(int menuIndex);

UIMenu * addMenuInstance(int menuId);

void removeMenuInstance(UIMenu * menu);

int getFreeMenuIndex();

int getFreeSettingIndex();

int getMenuPref(int settingIndex);

void setMenuPref(int settingIndex, int value, bool ignoreCallback = false);

void registerMenuPref(int prefId, int menuId, int itemIndex, CMenuPreferenceCallback cb);

void unregisterMenuPref(int prefId);

unsigned int getHashKey(const char * text);

unsigned int setGxtEntry(const char * key, const char * text);

const char * getGxtEntry(unsigned int key);

void initializeGame();

void unload();
