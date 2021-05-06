#include "pch.h"
#include "hook.h"

class WithVirtualProtect
{
public:
	// Modify protection flags on construction
	WithVirtualProtect(void* pAddress, uint32_t dwSize, DWORD &dwNewProtect)
		: pAddress(pAddress), dwSize(dwSize)
	{
		VirtualProtect(pAddress, dwSize, dwNewProtect, &dwOldProtect);
	}
	// Revert protection flags on destruction
	~WithVirtualProtect()
	{
		VirtualProtect(pAddress, dwSize, dwOldProtect, &dwOldProtect);
	}
private:
	void* pAddress;
	uint32_t    dwSize;
	DWORD    dwOldProtect;
};

template<typename T>
T GetNtFunction(const char* szFunction)
{
	// Initialise static handle to NtDll
	static HMODULE hNtDll = nullptr;

	// Get handle if still nullptr
	if (!hNtDll) hNtDll = GetModuleHandleA("ntdll.dll");

	// Return pointer to function
	if (hNtDll) return reinterpret_cast<T>(GetProcAddress(hNtDll, szFunction));
	else return nullptr;
}

LONG NtProtectVirtualMemory(HANDLE hProcess, void* pAddress, unsigned int dwSize, int dwNewProtect, DWORD* dwOldProtect)
{
	
	typedef LONG(WINAPI* tNtFunction)(
		IN      HANDLE  ProcessHandle,
		IN OUT  PVOID* BaseAddress,
		IN OUT  PULONG  NumberOfBytesToProtect,
		IN      ULONG   NewAccessProtection,
		OUT     PULONG  OldAccessProtection
		);

	// Get function pointer for NtProtectVirtualMemory
	static tNtFunction NtFunction = nullptr;
	if (!NtFunction) NtFunction = GetNtFunction<tNtFunction>("NtProtectVirtualMemory");

	// Align address and size pointers
	void* _pAddress = reinterpret_cast<void*>(pAddress);
	unsigned int _dwSize = dwSize;
	auto    pAddressAligned = _pAddress;
	auto    dwSizeAligned = _dwSize;

	// Cast variables to correct types to call function
	HANDLE  ProcessHandle = hProcess;
	PVOID* BaseAddress = reinterpret_cast<PVOID*>(&pAddressAligned);
	PULONG  NumberOfBytesToProtect = reinterpret_cast<PULONG>(&dwSizeAligned);
	ULONG   NewAccessProtection = static_cast<ULONG>(dwNewProtect);
	PULONG  OldAccessProtection = reinterpret_cast<PULONG>(dwOldProtect);

	// Return function call
	return NtFunction(ProcessHandle, BaseAddress, NumberOfBytesToProtect, NewAccessProtection, OldAccessProtection);
}

LONG NtProtectVirtualMemory(void* pAddress, unsigned int dwSize, int dwNewProtect, DWORD* dwOldProtect)
{
	// Set process handle to -1 (use if internal)
	HANDLE hProcess = reinterpret_cast<HANDLE>(-1);

	// Return function call from main function
	return NtProtectVirtualMemory(hProcess, pAddress, dwSize, dwNewProtect, dwOldProtect);
}


void Patch(BYTE* src, BYTE* dst, unsigned int len)
{
	DWORD oldProtect = 0;

	//VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	NtProtectVirtualMemory((void*)dst, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(dst, src, len);

	NtProtectVirtualMemory((void*)dst, len, oldProtect, &oldProtect);

	//VirtualProtect(dst, len, oldProtect, &oldProtect);
}

bool Detour(BYTE* src, BYTE* dst, int len)
{
	if (len < 5)
		return false;

	DWORD oldProtect;

	//VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	NtProtectVirtualMemory((void*)src, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	memset(src, 0x90, len);

	uintptr_t relativeAddr = dst - src - 5;

	*src = 0xE9; // for jump

	*(uintptr_t*)(src + 1) = relativeAddr;

	//VirtualProtect(src, len, oldProtect, &oldProtect);
	NtProtectVirtualMemory(src, len, oldProtect, &oldProtect);

	return true;
}

BYTE* TrampHook(BYTE* src, BYTE* dst, int len)
{
	if (len < 5)
		return nullptr;

	uintptr_t oldProtect;

	BYTE* gateway = (BYTE*)VirtualAlloc(0, len + 5, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(gateway, src, len); // copy src bytes

	*(gateway + len) = 0xE9;

	uintptr_t jmpBackAddy = src - 5 - gateway;

	*(uintptr_t*)(gateway + len + 1) = jmpBackAddy;

	if (Detour(src, dst, len))
		return gateway;

	return nullptr;
}