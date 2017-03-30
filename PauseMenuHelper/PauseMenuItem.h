#pragma once
public delegate void MenuValueChangedEvent(int, int);

public ref class PauseMenuItem
{
internal:
	MenuValueChangedEvent ^ m_nativeCallback;
	static MenuValueChangedEvent ^ m_managedCallback;
	void Initialize();
	~PauseMenuItem();
	!PauseMenuItem();

public:
	property int Value {
		int get() {
			return getMenuPreference(m_item->settingId);
		}

		void set(int value) {
			setMenuPreference(m_item->settingId, value);
		}
	}

	property System::String ^ Text {
		System::String ^ get() {
			return gcnew System::String(getGxtEntry(m_item->textHash));
		}

		void set(System::String ^ text) {
			const char* cstr = (const char*)(void*)Marshal::StringToHGlobalAnsi(text);
			m_item->textHash = setGxtEntry(cstr, cstr);
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
	}

	event MenuValueChangedEvent ^ ValueChanged {
		void add(MenuValueChangedEvent ^p) {
			m_managedCallback += p;
		}
		void remove(MenuValueChangedEvent ^p) {
			m_managedCallback -= p;
		}
	}

	static void valueChanged(int oldIndex, int newIndex) {
		if (m_managedCallback != nullptr)
		{
			OutputDebugStringA("Firing event from PauseMenuItem::valueChanged()");
			return m_managedCallback(oldIndex, newIndex);
		}
	}

internal:
	PauseMenuItem(CPauseMenuItem * ref);
	CPauseMenuItem * m_item;
};

