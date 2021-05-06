#include "sigscan.h"

uintptr_t ScanBasic(const char* pattern, const char* mask, const char* begin, intptr_t size)
{
	//if (!size)
		//return nullptr;

	bool found;

	for (int i = 0; i < size; i++)
	{
		found = true;

		for (int j = 0; j < strlen(mask); j++)
		{
			if (mask[j] != '?' && pattern[j] != *(char*)(begin + i + j))
			{
				found = false;
				break;
			}
		}
		if (found) // necessary as you don't break to top you continue loop.
		{
			return (uintptr_t)(begin + i);
		}
	}
	return 0;
}

uintptr_t ScanInternal(const char* pattern, const char* mask, const char* begin, intptr_t size)
{
	uintptr_t match{ 0 };

	MEMORY_BASIC_INFORMATION mbi{}; // I bet this must be initialized to access regionsize in for loop param 1st run

	for (char* curr = (char*)begin; curr < begin + size; curr += mbi.RegionSize)
	{

		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
			continue;

		match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

		if (match)
			break;
	}


	return match;
}

char* to_char(const wchar_t* string)
{
	size_t length = wcslen(string) + 1;
	char* c_string = new char[length]; // will we need to free this later, or is this a smart pointer?
	size_t numCharsRead;
	wcstombs_s(&numCharsRead, c_string, length, string, _TRUNCATE);

	return c_string;
}

PEB* GetPEB()
{
#ifdef _WIN64
	PEB* peb = (PEB*)__readgsword(0x60);
#else
	PEB* peb = (PEB*)__readfsdword(0x30);
#endif

	return peb;
}

LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name)
{
	LDR_DATA_TABLE_ENTRY* ldr = nullptr;

	PEB* peb = GetPEB();

	LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;

	LIST_ENTRY curr = head;
	// Flink points to the first entry in the list so long as it's the head.
	// I'm pretty sure CONTAINING_RECORD works due to the the table entries and data table entries being treated as the same somehow. That's what I vaguely remember but never wrote down.
	// The Second entry in a LDR_DATA_TABLE_ENTRY structure is a LIST_ENTRY called InMemoryOrderLinks. Our LIST_ENTRY's are in memory order, so that is obviously the
	// field we are pointing to with curr.Flink.

	while (curr.Flink != head.Blink) // Blink? No. This points to the last entry in the list since the list is not empty. We are skipping a LIST_ENTRY structure doing this.
	{
		LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks); // why not use curr over curr.Flink? curr doesn't contain anything as it's the head as we've set it.

		// we're using the list entries to navigate between the data table entries. Containing_record gives us the address of the larger structure that our LIST_ENTRY is a part of.
		// This allows us to access the dllName field.

		if (mod->FullDllName.Buffer)
		{

			char* cName = to_char(mod->BaseDllName.Buffer);

			if (!_stricmp(cName, name.c_str()))
			{
				ldr = mod;
				break;
			}

			delete[] cName; // calls destructor as well compared to free
		}
		curr = *curr.Flink;
	}

	return ldr;
}

uintptr_t ScanModIn(const char* pattern, const char* mask, const char* dllName, int offset, int extra)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(dllName);

	uintptr_t match = ScanInternal(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

	if (!match)
		return 0;

	match = *(uintptr_t*)(match + offset);

	if (extra)
	{
		match = *(uintptr_t*)(match + extra); // We want an address here, but dereferencing will just give us a character....duh.
	}

	return match; // returns dereferenced pointer. not for clientstate tho. I have to assume the game's code uses the two structs differently.
}

uintptr_t ptrScanModIn(char const* pattern, char const* mask, char const* dllName, int offset, int extra)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(dllName);

	uintptr_t match = ScanInternal(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

	match = *(uintptr_t*)(match + offset);

	if (extra)
	{
		match += extra;
	}

	return match; // returns pointer
}

uintptr_t altScanBasic(const char* pattern, const char* begin, intptr_t size)
{
	//if (!size)
		//return nullptr;

	bool found;
	size_t length = strlen(pattern);


	for (int i = 0; i < size; i++)
	{
		found = true;

		for (size_t j = 0; j < length; j++)
		{

			if (!((pattern[j] == '?') || pattern[j] == *(char*)(begin + i + j)))
			{
				found = false;
				break;
			}
		}


		if (found) // necessary as you don't break to top you continue loop.
		{
			return (uintptr_t)(begin + i);
		}
	}
	return 0;
}

uintptr_t altScanInternal(const char* pattern, const char* begin, intptr_t size)
{
	uintptr_t match{ 0 };

	MEMORY_BASIC_INFORMATION mbi{}; // I bet this must be initialized to access regionsize in for loop param 1st run

	for (char* curr = (char*)begin; curr < begin + size; curr += mbi.RegionSize)
	{

		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
			continue;

		match = altScanBasic(pattern, curr, mbi.RegionSize);

		if (match)
			break;
	}


	return match;
}

uintptr_t altScanModIn(const char* pattern, const char* dllName, int offset, int extra)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(dllName);

	uintptr_t match = altScanInternal(pattern, (char*)ldr->DllBase, ldr->SizeOfImage);

	if (!match)
		return 0;

	match = *(uintptr_t*)(match + offset);

	if (extra)
	{
		match = *(uintptr_t*)(match + extra); // We want an address here, but dereferencing will just give us a character....duh.
	}

	return match; // returns dereferenced pointer. not for clientState tho
}

uintptr_t altScanModIn2(const char* pattern, const char* dllName, int offset, int extra)
{
	LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(dllName);

	uintptr_t match = altScanInternal(pattern, (char*)ldr->DllBase, ldr->SizeOfImage);

	if (!match)
		return 0;

	match = (uintptr_t)(match + offset);

	if (extra)
	{
		match = *(uintptr_t*)(match + extra); // We want an address here, but dereferencing will just give us a character....duh.
	}

	return match;
}