#pragma once
#include <Windows.h>

namespace Sig
{
	// - Searches for a pattern within 'Start' and 'End'
	UINT_PTR FindPattern(UINT_PTR Start, UINT_PTR End, const char* Pattern);
	template <class T = UINT_PTR, class TStart>
	inline T FindPattern(TStart Start, size_t Len, const char* Pattern) {
		return (T)FindPattern((UINT_PTR)Start, Start + Len, Pattern);
	}

	// - Searches for 'Count' duplicate bytes from given start within 'Len' bytes
	UINT_PTR FindDup(UINT_PTR Start, size_t Len, BYTE Val, size_t Count);
	template <class T = UINT_PTR, class TStart>
	inline T FindDup(TStart Start, size_t Len, BYTE Val, size_t Count) {
		return (T)FindDup((UINT_PTR)Start, Len, Val, Count);
	}

	// - Searches for a code cave in 'Module' that has at least 'Len' bytes + max opcode length
	UINT_PTR FindCave(const char* Module, size_t Len);
	// - Searches for 'Count' padding bytes (0xCC or 0x00) from given start within 'Len' bytes
	// ** UNSAFE: If count is too low, it may return important code and memory
	template <class T = UINT_PTR, class TStart>
	inline T FindPad(TStart Start, size_t Len, size_t Count)
	{
		UINT_PTR result = FindDup((UINT_PTR)Start, Len, 0xCC, Count);
		if (!result)
			result = FindDup((UINT_PTR)Start, Len, 0x00, Count);
		return (T)result;
	}

	// - Searches for a pattern within Module's code region
	UINT_PTR FromModule(HMODULE Handle, const char* Pattern);
	inline UINT_PTR FromModule(const char* Module, const char* Pattern) {
		return FromModule(GetModuleHandle(Module), Pattern);
	}

	inline UINT_PTR GetBaseOfCode(HMODULE Module)
	{
		PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)Module;
		PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((UINT_PTR)Module) + pDOSHeader->e_lfanew);
		return (UINT_PTR)Module + pNTHeaders->OptionalHeader.BaseOfCode;
	}
	inline DWORD GetSizeOfCode(HMODULE Module)
	{
		PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)Module;
		PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((UINT_PTR)Module) + pDOSHeader->e_lfanew);
		return pNTHeaders->OptionalHeader.SizeOfCode;
	}
	// - Returns the start of Module's code region and optionally returns the length in 'Len'
	inline UINT_PTR GetCodeRegion(const char* Module, size_t* Len = nullptr)
	{
		HMODULE hmod = GetModuleHandle(Module);
		if (!hmod)
			return 0;
		if (Len) *Len = GetSizeOfCode(hmod);
		return GetBaseOfCode(hmod);
	}

	// - Returns the module being pointed inside by 'Ptr'
	HMODULE FindModule(UINT_PTR Ptr, DWORD pId = GetCurrentProcessId());

	unsigned Dude(const char* Pattern);
}