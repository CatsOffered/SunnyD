#include "sig.h"
#include <TlHelp32.h>

#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))

UINT_PTR Sig::FindPattern(UINT_PTR Start, UINT_PTR End, const char* Pattern)
{
	const char* pat = Pattern;
	UINT_PTR firstMatch = NULL;
	for (UINT_PTR pCur = Start; pCur < End; pCur++)
	{
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;
		}
		else {
			pat = Pattern;
			firstMatch = 0;
		}
	}
	return NULL;
}

UINT_PTR Sig::FindDup(UINT_PTR Start, size_t Len, BYTE Val, size_t Count)
{
	if (!Start || Len < Count)
		return 0;

	for (BYTE* pos = (BYTE*)Start; pos < (BYTE*)Start + Len - Count; pos++)
	{
		for (size_t i = 0; i < Len; i)
		{
			if (pos[i] != Val)
				break;
			if (i == Len - 1)
				return (UINT_PTR)pos;
		}
	}
	return 0;
}

UINT_PTR Sig::FindCave(const char* Module, size_t Len)
{
	UINT_PTR code = 0;
	size_t codelen = 0;
	if (!(code = GetCodeRegion(Module, &codelen)))
		return 0;

	UINT_PTR pos = code + codelen - 1;
	for (; pos >= code; pos--) // Loop from end of code until no more padding is found
		if (*(BYTE*)pos != 0 && *(BYTE*)pos != 0x90 && *(BYTE*)pos != 0xCC)
			break;

	UINT_PTR cavelen = (code + codelen - 1) - pos;
	return cavelen > 0xF + Len ? pos + 0x8 : 0; // Return 8 bytes ahead to prevent overwriting potentially valid code with a 0
}

UINT_PTR Sig::FromModule(HMODULE Handle, const char* Pattern) {
	return FindPattern(GetBaseOfCode(Handle), GetSizeOfCode(Handle), Pattern);
}

HMODULE Sig::FindModule(UINT_PTR Ptr, DWORD pId)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pId);
	MODULEENTRY32 mod;
	mod.dwSize = sizeof(mod);
	if (Module32First(snap, &mod))
	{
		do
		{
			UINT_PTR base = (UINT_PTR)mod.modBaseAddr;
			if (Ptr >= base && Ptr <= base + mod.modBaseSize)
				return mod.hModule;
		} while (Module32Next(snap, &mod));
	}
	return 0;
}

unsigned Sig::Dude(const char* Pattern)
{
	SYSTEM_INFO info;
	USHORT machine, nativemachine;
	if (!IsWow64Process2(GetCurrentProcess(), &machine, &nativemachine))
		return 0;
	else if (machine == IMAGE_FILE_MACHINE_UNKNOWN)
		GetSystemInfo(&info);
	else
		GetNativeSystemInfo(&info);

	// all readable pages: adjust this as required
	const DWORD pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE |
		PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	UINT_PTR start = (UINT_PTR)info.lpMinimumApplicationAddress;
	while (start < (UINT_PTR)info.lpMaximumApplicationAddress)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (!VirtualQuery((void*)start, &mbi, sizeof(mbi)))
			return 0;

		if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && (mbi.Protect & pmask))
		{
			DWORD result = 0;
			if (result = FindPattern((UINT_PTR)mbi.BaseAddress, mbi.RegionSize, Pattern))
				return result;
		}
		start += mbi.RegionSize;
	}
	return 0;
}