#include "pch.h"
#include "dx.h"

BOOL CALLBACK enumWind(HWND handle, LPARAM lp)
{
	DWORD procId;

	GetWindowThreadProcessId(handle, &procId);

	if (GetCurrentProcessId() != procId)
		return TRUE;

	window = handle;

	return FALSE;
}

HWND GetProcessWindow() // gets window of our process
{
	window = NULL;

	EnumWindows(enumWind, NULL); // applies function we write to every window

	RECT size;

	GetWindowRect(window, &size);
	g_vars.windowWidth = size.right - size.left;
	g_vars.windowHeight = size.bottom - size.top;

	g_vars.windowWidth -= 5;
	g_vars.windowHeight -= 29;

	return window;
}

bool initD3D(void** pTable, size_t size)
{

	if (!pTable)
		return false;

	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	LPDIRECT3DDEVICE9 d3ddev = nullptr; // why do we need to initialize to nullptr? Zero out? I bet we don't need to.

	D3DPRESENT_PARAMETERS d3dpp = {}; // this should do the same as ZeroMemory

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDeviceResult = d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3dpp.hDeviceWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	if (dummyDeviceResult != S_OK)
	{
		d3dpp.Windowed = !d3dpp.Windowed;

		dummyDeviceResult = d3d->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3dpp.hDeviceWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&d3ddev);

		if (dummyDeviceResult != S_OK)
		{
			d3d->Release();
			return false;
		}
	}

	memcpy(pTable, *(void***)d3ddev, size); // dereferencing gets us the vTable
	d3ddev->Release();
	d3d->Release();

	return true;
}

IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
DWORD dwOld_D3DRS_COLORWRITEENABLE;

void SaveState(IDirect3DDevice9* pDevice)
{
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	//	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state); // This seam not to be needed anymore because valve fixed their problem
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
}

void RestoreState(IDirect3DDevice9* pDevice) // not restoring everything. Because its not needed.
{
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	//pixel_state->Apply(); 
	//pixel_state->Release();
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
}