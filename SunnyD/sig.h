#pragma once
#include <Windows.h>

namespace Sig
{
	UINT_PTR FindPattern(UINT_PTR Start, UINT_PTR End, const char* Pattern);
	UINT_PTR FindCave(const char* Module, size_t Len);
	UINT_PTR GetBaseOfCode(HMODULE Module);
	DWORD GetSizeOfCode(HMODULE Module);
	const char* GetExportedName(UINT_PTR Func, DWORD pId = GetCurrentProcessId());

	// - Searches for 'Count' duplicate bytes from given start within 'Len' bytes
	UINT_PTR FindDup(UINT_PTR Start, size_t Len, BYTE Val, size_t Count);

	// - Returns the module being pointed inside by 'Ptr'
	HMODULE FindModule(UINT_PTR Ptr, DWORD pId = GetCurrentProcessId());

	// - Searches for a pattern within Module's code region
	UINT_PTR FromModule(HMODULE Handle, const char* Pattern);

	// - Returns the start of Module's code region and optionally returns the length in 'Len'
	UINT_PTR GetCodeRegion(const char* Module, size_t* Len = nullptr);

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

	unsigned Dude(const char* Pattern);

	inline UINT_PTR FromModule(const char* Module, const char* Pattern) {
		return FromModule(GetModuleHandle(Module), Pattern);
	}

	template <class T = UINT_PTR, class TStart>
	inline T FindDup(TStart Start, size_t Len, BYTE Val, size_t Count) {
		return (T)FindDup((UINT_PTR)Start, Len, Val, Count);
	}
	template <class T = UINT_PTR, class TStart>
	inline T FindPattern(TStart Start, size_t Len, const char* Pattern) {
		return (T)FindPattern((UINT_PTR)Start, Start + Len, Pattern);
	}
}