#pragma once

#include <windows.h>

#include "pattern.h"

#include "Hooking.h"

#include "Types.h"

typedef unsigned int(*GetHashKey_t)(const char * text, unsigned int hashName);

typedef const char *(*GetGlobalTextEntry_t)(void * gtxArray, unsigned int hashName);

typedef void(*SetPauseMenuPreference_t)(long long settingIndex, int value, unsigned int unk);

typedef bool(*SetMenuSlot_t)(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bIsSlotUpdate);

typedef void(*CallFunctionOnMovie)(void * sfMovie, const char * functionName, CScaleformParameter * arg, CScaleformParameter * arg1, CScaleformParameter * arg2, CScaleformParameter * arg3);

typedef void(*CMenuPreferenceCallback)(CPauseMenuInstance * parent, int itemIndex, int value);

CPauseMenuInstance * lookupMenuForIndex(int menuIndex);

int getFreeMenuIndex();

int getFreeSettingIndex();

CPauseMenuInstance * addMenuInstance(int menuId);

void removeMenuInstance(CPauseMenuInstance * menu);

void callMenuMovieFunction(const char * functionName, CScaleformParameter * arg, CScaleformParameter * arg1, CScaleformParameter * arg2, CScaleformParameter * arg3);

void setMenuPreference(int settingIndex, int value, bool ignoreCallback = false);

int getMenuPreference(int settingIndex);

unsigned int getHashKey(const char * text);

unsigned int setGxtEntry(const char * key, const char * text);

const char * getGxtEntry(unsigned int key);

void registerMenuPref(int prefId, int menuId, int itemIndex, CMenuPreferenceCallback callback);

void unregisterMenuPref(int prefId);

void initializeGame();

void unload();
