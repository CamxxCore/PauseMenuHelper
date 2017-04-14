#pragma once

public delegate void MenuValueChangedEvent(int parentId, int itemIndex, int value);

public delegate void NativeMenuValueChangedEvent(CPauseMenuInstance*, int, int);

public ref class PauseMenuItem
{
internal:
	NativeMenuValueChangedEvent ^ m_nativeCallback;
	MenuValueChangedEvent ^ m_managedCallback;
	void Initialize(CPauseMenuInstance * parent, CPauseMenuItem * item);
	~PauseMenuItem();
	!PauseMenuItem();

public:
	property int MenuID {
		int get() {
			return baseRef()->menuIndex;
		}

		void set(int value) {
			baseRef()->menuIndex = value;
		}
	}

	property int Value {
		int get() {
			return getMenuPreference(baseRef()->settingId);
		}
		void set(int value) {
			setMenuPreference(baseRef()->settingId, value);
		}
	}

	property int SettingID {
		int get() {
			return baseRef()->settingId;
		}
	}

	property System::String ^ Text {
		System::String ^ get() {
			return gcnew System::String(getGxtEntry(baseRef()->textHash));
		}

		void set(System::String ^ text) {
			const char* cstr = (const char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);
			baseRef()->textHash = setGxtEntry(cstr, cstr);
		}
	}

	property System::IntPtr MemoryAddress {
		System::IntPtr get() {
			CPauseMenuItem * pItem = baseRef();
			return System::IntPtr(pItem);
		}
	}

	property bool IsSelectable {
		bool get() {
			return (baseRef()->stateFlags & 1) == 0;
		}

		void set(bool value) {
			baseRef()->stateFlags = !value;
		}
	}

	event MenuValueChangedEvent ^ ValueChanged {
		void add(MenuValueChangedEvent ^p) {
			m_managedCallback += p;
		}
		void remove(MenuValueChangedEvent ^p) {
			m_managedCallback -= p;
		}
	}

	void valueChanged(CPauseMenuInstance * parent, int itemIndex, int newValue) {
		if (m_managedCallback != nullptr)
		{
			m_managedCallback(parent->menuId, itemIndex, newValue);
		}
	}

internal:
	PauseMenuItem();

	inline CPauseMenuItem * baseRef()
	{
		auto cmenu = lookupMenuForIndex(m_parentId);
		if (!cmenu) 
			throw gcnew System::NullReferenceException("Internal menu reference was null.");
		auto pItem = &cmenu->items[m_index];
		if (!pItem) 
			throw gcnew System::NullReferenceException("Internal item reference was null.");
		return pItem;
	}

private:
	int m_index;
	int m_parentId;
};

public enum class MenuItemType : int
{
	Basic = 0,
	Default = 1,
	Setting = 2,
	TextBlock = 8
};

public enum class MenuSettingType : int
{
	Slider = 0,
	Toggle = 1,
	AimMode = 3,
	GamepadLayout = 4,
	LowMedHi = 5,
	AudioOutput = 8,
	FadeRadio = 10,
	SelfRadioMode = 11,
	OffOnBlips = 13,
	SimpleComplex = 14,
	Language = 15,
	LowHi = 16
};
