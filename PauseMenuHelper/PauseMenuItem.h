#pragma once

ref class PauseMenu;

public ref class PauseMenuItemChangedEventArgs : public System::EventArgs
{
public:
	PauseMenuItemChangedEventArgs(int oldValue, int newValue)
	{
		m_oldValue = oldValue;
		m_newValue = newValue;
	}

	property int OldValue {
		int get()
		{
			return m_oldValue;
		}
	}

	property int NewValue {
		int get()
		{
			return m_newValue;
		}
	}

private:
	int m_oldValue;
	int m_newValue;
};

ref class PauseMenuItem;

public delegate void MenuValueChangedEvent(PauseMenuItem^, PauseMenuItemChangedEventArgs^);
public delegate void NativeMenuValueChangedEvent(CPauseMenuItem*, int, int);

public ref class PauseMenuItem
{
internal:
	NativeMenuValueChangedEvent ^ m_nativeCallback;
	static MenuValueChangedEvent ^ m_managedCallback;
	void Initialize();
	~PauseMenuItem();
	!PauseMenuItem();

public:

	property int PrefID {
		int get() {
			return getMenuPreference(m_item->settingId);
		}
		void set(int value) {
			setMenuPreference(m_item->settingId, value);
		}
	}

	property System::String ^ Text {
		System::String ^ get() {
			return gcnew System::String(
				getGxtEntry(m_item->textHash));
		}

		void set(System::String ^ text) {
			const char* cstr = (const char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text);
			m_item->textHash = 
				setGxtEntry(cstr, cstr);
		}
	}

	property System::IntPtr MemoryAddress {
		System::IntPtr get() {
			return System::IntPtr(m_item);
		}
	}

	property int MenuID {
		int get() {
			return m_item->menuIndex;
		}

		void set(int value) {
			m_item->menuIndex = (int)value;
		}
	}

	property bool IsSelectable {
		bool get() {
			return (m_item->stateFlags & 1) == 0;
		}
		void set(bool value) {
			m_item->stateFlags = !value;
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

	 static void valueChanged(CPauseMenuItem * item, int oldValue, int newValue) {
		if (m_managedCallback != nullptr)
		{
			OutputDebugStringA("Firing event from PauseMenuItem::valueChanged()");
			return m_managedCallback(gcnew PauseMenuItem(item), gcnew PauseMenuItemChangedEventArgs(oldValue, newValue));
		}
	}

internal:
	PauseMenuItem(CPauseMenuItem * ref);
	CPauseMenuItem * m_item;
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
