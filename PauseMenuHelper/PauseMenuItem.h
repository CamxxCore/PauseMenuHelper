#pragma once

public delegate void MenuValueChangedEvent(int parentId, int itemIndex, int value);

public delegate void NativeMenuValueChangedEvent(UIMenu*, int, int);

public ref class PauseMenuItem
{
internal:
	NativeMenuValueChangedEvent ^ m_nativeCallback;
	MenuValueChangedEvent ^ m_managedCallback;
	void Initialize(UIMenu * parent, uiWidget * item);
	~PauseMenuItem();
	!PauseMenuItem();

public:
	property int MenuID {
		int get() {
			return getBaseRef()->menuIndex;
		}

		void set(int value) {
			getBaseRef()->menuIndex = value;
		}
	}

	property int Value {
		int get() {
			return getMenuPref(getBaseRef()->settingId);
		}
		void set(int value) {
			setMenuPref(getBaseRef()->settingId, value);
		}
	}

	property int SettingID {
		int get() {
			return getBaseRef()->settingId;
		}
	}

	property System::String ^ Text {
		System::String ^ get() {
			return gcnew System::String(getGxtEntry(getBaseRef()->textHash));
		}

		void set(System::String ^ text) {
			const char* cstr = (const char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);
			getBaseRef()->textHash = setGxtEntry(cstr, cstr);
		}
	}

	property System::IntPtr MemoryAddress {
		System::IntPtr get() {
			uiWidget * pItem = getBaseRef();
			return System::IntPtr(pItem);
		}
	}

	property bool IsSelectable {
		bool get() {
			return (getBaseRef()->stateFlags & 1) == 0;
		}

		void set(bool value) {
			getBaseRef()->stateFlags = !value;
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
internal:
	void valueChanged(UIMenu * parent, int itemIndex, int newValue) {
		if (m_managedCallback != nullptr)
		{
			m_managedCallback(parent->menuId, itemIndex, newValue);
		}
	}

	inline uiWidget * getBaseRef()
	{
		auto cmenu = lookupMenuForIndex(m_parentId);

		if (cmenu)
		{
			auto pItem = &cmenu->items[m_index];

			if (pItem)
			{
				return pItem;
			}
		}

		return NULL;
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
