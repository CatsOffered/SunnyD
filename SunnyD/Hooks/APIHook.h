#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*
	Credits: Zer0Mem0ry
		thx for letting me b lazy
*/

struct hook_t
{
	bool isHooked;
	void* FunctionAddress;
	void* Hook;
	char  Jmp[6];
	char  APIBytes[6];
	void* APIFunction;
};

namespace hook
{
	bool InitializeHook(hook_t* Hook, const char* ModuleName, const char* FunctionName, void* HookFunction)
	{
		HMODULE hModule;
		UINT_PTR OrigFunc, FuncAddr;

		if (Hook->isHooked) {
			return false;
		}
		hModule = GetModuleHandle(ModuleName);
		if (hModule == NULL) {
			Hook->isHooked = false;
			return false;
		}
		Hook->FunctionAddress = GetProcAddress(hModule, FunctionName);
		if (Hook->FunctionAddress == NULL) {
			Hook->isHooked = false;
			return false;
		}
		Hook->Jmp[0] = (char)0xe9;
		*(PULONG)&Hook->Jmp[1] = (UINT_PTR)HookFunction - (UINT_PTR)Hook->FunctionAddress - 5;
		memcpy(Hook->APIBytes, Hook->FunctionAddress, 5);
		Hook->APIFunction = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (Hook->APIFunction == NULL) {
			return false;
		}
		memcpy(Hook->APIFunction, Hook->APIBytes, 5);
		OrigFunc = (UINT_PTR)Hook->APIFunction + 5;
		FuncAddr = (UINT_PTR)Hook->FunctionAddress + 5;
		*(LPBYTE)((LPBYTE)Hook->APIFunction + 5) = 0xe9;
		*(PULONG)((LPBYTE)Hook->APIFunction + 6) = FuncAddr - OrigFunc - 5;
		Hook->isHooked = true;
		return true;
	}

	bool InitializeByAddress(hook_t* Hook, void* Address, void* HookFunction)
	{
		UINT_PTR OrigFunction, FunctionAddress;
		if (Hook->isHooked) {
			return false;
		}
		Hook->FunctionAddress = Address;
		Hook->Jmp[0] = (char)0xe9;

		*(PULONG)&Hook->Jmp[1] = (ULONG)HookFunction - (ULONG)Hook->FunctionAddress - 5;
		memcpy(Hook->APIBytes, Hook->FunctionAddress, 5);

		Hook->APIFunction = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		memcpy(Hook->APIFunction, Hook->APIBytes, 5);

		OrigFunction = (UINT_PTR)Hook->APIFunction + 5;
		FunctionAddress = (UINT_PTR)Hook->FunctionAddress + 5;

		*(LPBYTE)((LPBYTE)Hook->APIFunction + 5) = 0xe9;
		*(PULONG)((LPBYTE)Hook->APIFunction + 6) = FunctionAddress - OrigFunction - 5;
		Hook->isHooked = true;
		return true;
	}

	bool InsertHook(hook_t* Hook)
	{
		DWORD op;
		if (!Hook->isHooked) {
			return false;
		}
		VirtualProtect(Hook->FunctionAddress, 5, PAGE_EXECUTE_READWRITE, &op);
		memcpy(Hook->FunctionAddress, Hook->Jmp, 5);
		VirtualProtect(Hook->FunctionAddress, 5, op, &op);
		return true;
	}

	bool Unhook(hook_t* Hook)
	{
		DWORD op;
		if (!Hook->isHooked) {
			return false;
		}
		VirtualProtect(Hook->FunctionAddress, 5, PAGE_EXECUTE_READWRITE, &op);
		memcpy(Hook->FunctionAddress, Hook->APIBytes, 5);
		VirtualProtect(Hook->FunctionAddress, 5, op, &op);

		Hook->isHooked = false;
		return true;
	}

	bool FreeHook(hook_t* Hook)
	{
		if (Hook->isHooked) {
			return false;
		}
		VirtualFree(Hook->APIFunction, 0, MEM_RELEASE);
		memset(Hook, 0, sizeof(hook_t*));
		return true;
	}
};