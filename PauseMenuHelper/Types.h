#pragma once

namespace rage
{
	template<typename T>
	struct pgCollection
	{
	public:
		T*			m_data;
		UINT16		m_count;
		UINT16		m_size;

	public:
		T first()
		{
			return m_data[0];
		}

		T* begin()
		{
			return m_data;
		}

		T* end()
		{
			return (m_data + m_count);
		}

		T* at(UINT16 index)
		{
			return &m_data[index];
		}
	};
}

enum eScaleformParameterType
{
	SPT_NONE = 0,
	SPT_FLOAT = 1,
	SPT_BOOLEAN = 2,
	SPT_STRING = 4,
};

union CScaleformParameterValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	unsigned int hash;
	const char * string;
	float fvalue;
	double dvalue;
};

struct CScaleformParameter
{
	CScaleformParameterValue m_value; //0x-0x8
	eScaleformParameterType m_type; // 0x8-0xC
	int m_pad; //0xC-0x10
	bool m_unk; // true if value is a string.
	char pad0[0xF];
}; //sizeof=0x20

struct CPauseMenuItem
{
	int menuIndex; // 0x0-0x4
	unsigned int textHash; //0x4-0x8
	void * unkPtr; //0x8-0x10
	short unkCount; //0x10-0x12
	short unkCount1; //0x12-0x14
	char pad0[0x4]; //0x14-0x18
	unsigned char settingId; // 0x18-0x19 Index of the setting that is changed when this item is invoked
	char actionType; //0x19-0x1A i.e. slider, on_off etc.
	char type; //0x1A-0x1B 1 = default button, 2 = dynamic 8 = immutable text block
	char stateFlags; //0x1B-0x1C // 1 = disabled
	char pad1[0x4];

	CPauseMenuItem() {
		memset(this, 0x0, sizeof(CPauseMenuItem));
		menuIndex = 0;
		textHash = -1;
		settingId = 0;
		type = 0;
		stateFlags = 0;
	}
}; //sizeof=0x20

struct CPauseMenuInstance
{
	void * cmenu; //0x0-0x8
	CPauseMenuItem * items; //0x8-0x10
	short itemCount; //0x10-0x12
	short maxItems; //0x12-0x14
	char pad0[0x4]; //0x14-0x18
	void * unk; //0x18-0x20
	char pad1[0x10]; //0x20-0x30
	const char * szMenuName; //0x30-0x38
	char pad2[0x8]; //0x38-0x40
	int menuId; //0x40-0x44
	int unk1; //0x44-0x48
	short unkFlag;
	short unkShort1;
	char pad[0x4];

	CPauseMenuInstance() {
		memset(this, 0x0, sizeof(CPauseMenuInstance));
		menuId = 0;
		unk1 = 2;
		unkFlag = 128;
		unkShort1 = 50;
	}
}; //sizeof=0x50
