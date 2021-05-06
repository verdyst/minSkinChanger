#pragma once
#include "globals.h"

static HWND window;

typedef HRESULT(APIENTRY* tEndScene) (LPDIRECT3DDEVICE9 pDevice);

bool initD3D(void** pTable, size_t size);

void SaveState(IDirect3DDevice9* pDevice);

void RestoreState(IDirect3DDevice9* pDevice); // not restoring everything. Because its not needed.