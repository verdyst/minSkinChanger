#pragma once
#include "csgosdk.h"
#include "frameStageNotify.h"

class Entity;

struct global_vars
{
	// offsets

	Entity* localEnt;

	IClientEntityList* ClientEntityList;
	IVModelInfo* modelInfo;

	int knifeAddr;
	int modelIndex;




	BYTE createMoveBytes[9] = { 0 };
	tCreateMove oCreateMove;

	tFrameStageNotify oFrameStageNotify;
	BYTE frameStageNotifyBytes[9] = { 0 };

	// SkinChanger netvars

	uintptr_t m_iItemIDHigh;
	uintptr_t m_nFallbackPaintKit;
	uintptr_t m_flFallbackWear;
	uintptr_t m_nFallbackStatTrak;
	uintptr_t m_nFallbackSeed;
	uintptr_t m_iEntityQuality;

	int econId;
	int wID;

	int paintKits[100] = { 0 };
	int knifeSkins[100] = { 0 };
	int knife_skin;

	int knife = 507; // doesn't get initialized until menu opens, causing assert crash

	void* d3d9Device[119];
	HWND window;
	int windowHeight, windowWidth;

	BYTE EndSceneBytes[7]{};

	LPDIRECT3DDEVICE9 pDevice = nullptr;

	uintptr_t EndSceneRetAddr;
	uintptr_t CMRetAddr;
	uintptr_t FSMRetAddr;
};

extern global_vars g_vars;