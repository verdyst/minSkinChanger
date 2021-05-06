#pragma once
#include <Windows.h>
#include "sigscan.h"

struct Offsets
{

	uintptr_t m_fFlags;
	uintptr_t m_iHealth;
	uintptr_t m_vecOrigin;
	uintptr_t m_vecViewOffset;
	uintptr_t dwClientState = altScanModIn("\xA1????\x33\xD2\x6A\x00\x6A\x00\x33\xC9\x89\xB0", "engine.dll", 1, 0);
	uintptr_t dwClientState_ViewAngles = altScanModIn("\xF3\x0F\x11\x80????\xD9\x46\x04\xD9\x05", "engine.dll", 4, 0);



	uintptr_t m_hMyWeapons;
	uintptr_t m_hActiveWeapon;
	uintptr_t dwEntityList = altScanModIn("\xBB????\x83\xFF\x01\x0F\x8C????\x3B\xF8", "client.dll", 1, 0);


	uintptr_t m_iItemDefinitionIndex;
	uintptr_t clientstate_delta_ticks = 0x174;
	uintptr_t m_AttributeManager = 0x2D80;
	uintptr_t m_Item = 0x40;
	uintptr_t m_iEntityQuality;
	uintptr_t m_nModelIndex = 0x258;
	uintptr_t m_hViewModel = 0x32F8;
	uintptr_t m_hWeapon = 0x29C8;
	uintptr_t m_iViewModelIndex = 0x3240;
	uintptr_t m_iWorldModelIndex = 0x3244;

	uintptr_t m_nSequenceOffset;






	uintptr_t localPlayer = (uintptr_t)altScanModIn("\x8D\x34\x85????\x89\x15????\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "client.dll", 3, 4);



};

extern Offsets offset;