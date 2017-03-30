#pragma once

#include <windows.h>

#include "pattern.h"

#include "Hooking.h"

#include "Types.h"

typedef unsigned int(*GetHashKey_t)(const char * text, unsigned int hashName);

typedef const char *(*GetGlobalTextEntry_t)(void * gtxArray, unsigned int hashName);

typedef void(*SetPauseMenuPreference_t)(long long settingIndex, int value, unsigned int unk);

typedef bool(*SetMenuSlot_t)(int columnId, int slotIndex, int menuState, int settingIndex, int unk, int value, const char * text, bool bPopScaleform, bool bIsSlotUpdate);

typedef void(__stdcall * CMenuItemEventCallback)(int oldValue, int value);

CPauseMenuInstance * lookupMenuForIndex(int menuIndex);

void setMenuPreference(int settingIndex, int value);

int getMenuPreference(int settingIndex);

unsigned int getHashKey(const char * text);

unsigned int setGxtEntry(const char * key, const char * text);

const char * getGxtEntry(unsigned int key);

void registerNativeMenuCallback(CPauseMenuItem * item, CMenuItemEventCallback callback);

void initializeGame();

void unload();