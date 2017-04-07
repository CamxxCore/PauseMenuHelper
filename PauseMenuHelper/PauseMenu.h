#pragma once

#include "MenuFunctions.h"
#include "PauseMenuItem.h"

enum class MenuType;

using namespace	System::Collections::Generic;

public ref class PauseMenu
{
public:
	property MenuType MenuID {
		MenuType get() {
			return (MenuType)getMenuRef()->menuId;
		}
	}

	property int ItemCount {
		int get() {
			return getMenuRef()->itemCount;
		}
	}

	property System::IntPtr MemoryAddress {
		System::IntPtr get() {
			auto pMenu = getMenuRef();
			return System::IntPtr(pMenu);
		}
	}

	property PauseMenuItem ^ default[int]
	{
		PauseMenuItem ^ get(int index) {
		CPauseMenuInstance* cmenu = getMenuRef();

		if (index < 0 || index >= cmenu->itemCount)
			throw gcnew System::ArgumentOutOfRangeException("index: out of range.");

			int origCount = cmenu->itemCount - m_addedItemCount;

			if (index >= origCount)
			{
				return m_addedItems[index - origCount];
			}

			else
			{
				auto item = gcnew PauseMenuItem();

				item->Initialize(cmenu, &cmenu->items[index]);

				return item;
			}
	}

	void set(int index, PauseMenuItem ^ value) {
		CPauseMenuInstance*  cmenu = getMenuRef();
		if (index < 0 || index >= cmenu->itemCount)
			throw gcnew System::ArgumentOutOfRangeException("index: out of range.");
		cmenu->items[index] = *value->baseRef();
	}
	}
	
	PauseMenuItem ^ AddItem(System::String ^ text);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuItemType type);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuItemType type, int subtype);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuItemType type, int subtype, MenuType childMenu, int settingIndex);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuSettingType settingType);
	PauseMenuItem ^ AddItem(System::String ^ text, MenuSettingType settingType, int initialValue);
	int IndexOf(PauseMenuItem ^ item);
	void Remove(PauseMenuItem ^ item);
	PauseMenuItem ^ ItemAt(int index);
	void RemoveAt(int index);
	void Clear();
	PauseMenu(MenuType menuType);
	PauseMenu();

internal:

	inline CPauseMenuInstance * getMenuRef()
	{
		CPauseMenuInstance* cmenu = lookupMenuForIndex(m_menuId);
		if (!cmenu)
			throw gcnew System::NullReferenceException("Internal menu reference was null.");
		return cmenu;
	}

private:
	~PauseMenu();
	!PauseMenu();
	List<PauseMenuItem^> ^ m_addedItems;
	bool bIsAddonMenu = false;
	int m_menuId = 0;
	int m_addedItemCount = 0;
};

public enum class MenuType : int
{
	Map = 0,
	Info = 1,
	Friends = 2,
	Gallery = 3,
	Socialclub = 4,
	Game = 5,
	Settings = 6,
	Players = 7,
	Weapons = 8,
	Medals = 9,
	Stats = 10,
	Available = 11,
	Vagos = 12,
	Cops = 13,
	Lost = 14,
	Home_Mission = 15,
	Corona_Settings = 16,
	Corona_Invite = 17,
	Store = 18,
	Home_Help = 19,
	Home_Brief = 20,
	Home_Feed = 21,
	Settings_Audio = 22,
	Settings_Display = 23,
	Settings_Controls = 24,
	New_Game = 25,
	Load_Game = 26,
	Save_Game = 27,
	Header = 28,
	Header_Save_game = 29,
	Home = 30,
	Crews = 31,
	Settings_Savegame = 32,
	Gallery_Item = 33,
	Freemode = 34,
	Mp_Char_1 = 35,
	Mp_Char_2 = 36,
	Mp_Char_3 = 37,
	Mp_Char_4 = 38,
	Mp_Char_5 = 39,
	Header_Multiplayer = 40,
	Header_My_mp = 41,
	Mission_Creator = 42,
	Game_Mp = 43,
	Leave_Game = 44,
	Header_Pre_lobby = 45,
	Header_Lobby = 46,
	Party = 47,
	Lobby = 48,
	Placeholder = 49,
	Stats_Category = 50,
	Settings_List = 51,
	Save_Game_list = 52,
	Map_Legend = 53,
	Crews_Category = 54,
	Crews_Filter = 55,
	Crews_Card = 56,
	Spectator = 57,
	Stats_Listitem = 58,
	Crew_Mine = 59,
	Crew_Rockstar = 60,
	Crew_Friends = 61,
	Crew_Invites = 62,
	Crew_List = 63,
	Mission_Creator_category = 64,
	Mission_Creator_listitem = 65,
	Mission_Creator_stat = 66,
	Friends_List = 67,
	Friends_Options = 68,
	Header_Mp_character_select = 69,
	Header_Mp_character_creation = 70,
	Creation_Heritage = 71,
	Creation_Lifestyle = 72,
	Creation_You = 73,
	Party_List = 74,
	Replay_Mission = 75,
	Replay_Mission_list = 76,
	Replay_Mission_activity = 77,
	Crew = 78,
	Creation_Heritage_list = 79,
	Creation_Lifestyle_list = 80,
	Players_List = 81,
	Players_Options = 82,
	Players_Options_list = 83,
	Party_Options = 84,
	Party_Options_list = 85,
	Crew_Options = 86,
	Crew_Options_list = 87,
	Friends_Options_list = 88,
	Friends_Mp = 89,
	Team_Select = 90,
	Home_Dialog = 91,
	Header_Empty = 92,
	Settings_Feed = 93,
	Gallery_Options = 94,
	Gallery_Options_list = 95,
	Brightness_Calibration = 96,
	Header_Text_selection = 97,
	Lobby_List = 98,
	Lobby_List_item = 99,
	Header_Corona = 100,
	Header_Corona_lobby = 101,
	Header_Corona_joined_players = 102,
	Header_Corona_invite_players = 103,
	Header_Corona_invite_friends = 104,
	Header_Corona_invite_crews = 105,
	Corona_Joined_players = 106,
	Corona_Invite_players = 107,
	Corona_Invite_friends = 108,
	Corona_Invite_crews = 109,
	Settings_Facebook = 110,
	Header_Joining_screen = 111,
	Corona_Settings_list = 112,
	Corona_Details_list = 113,
	Corona_Invite_list = 114,
	Corona_Joined_list = 115,
	Header_Corona_invite_matched_players = 116,
	Header_Corona_invite_last_job_players = 117,
	Corona_Invite_matched_players = 118,
	Corona_Invite_last_job_players = 119,
	Crew_Leaderboards = 120,
	Home_Open_jobs = 121,
	Crew_Request = 122,
	Header_Race = 123,
	Race_Info = 124,
	Race_Infolist = 125,
	Race_Lobbylist = 126,
	Header_Betting = 127,
	Betting = 128,
	Betting_Infolist = 129,
	Betting_Lobbylist = 130,
	Incept_Trigger = 131,
	Settings_Sixaxis = 132,
	Replay_Random = 133,
	Cutscene_Empty = 134,
	Home_Newswire = 135,
	Settings_Camera = 136,
	Settings_Video = 137,
	Settings_Graphics = 138,
	Settings_Voice_chat = 139,
	Settings_Misc_controls = 140,
	Help = 141,
	Movie_Editor = 142,
	Exit_To_windows = 143,
	Header_Landing_page = 144,
	Show_Account_picker = 145,
	Settings_Replay = 146,
	Replay_Editor = 147,
	Menu_Unique_id_settings_fps = 148
};
