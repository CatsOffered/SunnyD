#pragma once
#include "framework.h"
#include <intrin.h>

struct ProcAddr
{
	__forceinline ProcAddr(LPCSTR Module, LPCSTR Proc = 0)
	{
		size_t len = strlen(Module) + 1;
		szModule = new char[len];
		strcpy_s(szModule, len, Module);

		if (Proc)
		{
			len = strlen(Proc) + 1;
			szProc = new char[len];
			strcpy_s(szProc, len, Proc);
		}

		pCaller = _AddressOfReturnAddress();
	}

	~ProcAddr();
	LPSTR szModule, szProc = 0;
	void* pCaller;
};

enum EMemInfo
{
	Mem_Null = 0,
	Mem_Alloc,
	Mem_Write,
	Mem_Thread
};

struct MemInfo
{
	__forceinline MemInfo(EMemInfo Type, void* Loc, DWORD Size, HANDLE Proc)
	{
		type = Type;
		pLoc = Loc, dwSize = Size, hProc = Proc;
		pCaller = _AddressOfReturnAddress();
	}

	~MemInfo() { if (pWriteCpy) delete[] pWriteCpy; }

	EMemInfo type = Mem_Null;
	HANDLE hProc;
	DWORD dwSize;
	void* pLoc, * pCaller, * pThreadArgs = 0;
	BYTE* pWriteCpy = 0;
};

__forceinline MemInfo* MemInfo_Alloc(void* Loc, DWORD Size, HANDLE Proc) {
	return new MemInfo(Mem_Alloc, Loc, Size, Proc);
}
__forceinline MemInfo* MemInfo_Write(void* Loc, void* Buf, DWORD Size, HANDLE Proc) {
	MemInfo* info = new MemInfo(Mem_Write, Loc, Size, Proc);
	if (Buf && Size <= 0x1000)
	{
		info->pWriteCpy = new BYTE[Size];
		memcpy(info->pWriteCpy, Buf, Size);
	}
	return info;
}
__forceinline MemInfo* MemInfo_Thread(void* Loc, HANDLE Proc, PVOID Args, DWORD ArgsLen = 0) {
	MemInfo* info = new MemInfo(Mem_Thread, Loc, ArgsLen, Proc);
	info->pThreadArgs = Args;
	return info;
}