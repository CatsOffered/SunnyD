#pragma once
#include "framework.h"
#include <string>
#define Zydis_EXPORTS
#include <Zydis/Zydis.h>

class CJumpHook
{
	// - Indicates if this hook is active
	bool hooked = false;
	// - Points to the original function's address
	void* original = nullptr;
	// - Points to overwritten bytes that got relocated
	BYTE* old_code = nullptr;
	// - Holds the length of the overwritten bytes
	size_t code_len = 0;
public:
	template<class T = void*>
	inline T GetOriginal() { return (T)original; }
	template<class T = void*>
	inline T GetOldCode() { return (T)old_code; }
	inline size_t GetLength() { return code_len; }

	~CJumpHook() { UnHook(); }

	// - Initializes the hook at 'From' and overwrites 'Length' bytes
	// - The hook will replace with a jump to 'To' and will relocate overwritten bytes
	template <class T = void*>
	void Hook(T From, void* To, size_t Length)
	{
		if (!Length)
		{
			ZydisDecoder de;
#ifdef _WIN64
			ZydisDecoderInit(&de, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
#define JMPFUNC AbsJmp
#define JMPSIZE 14
#else
			ZydisDecoderInit(&de, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
#define JMPFUNC RelJmp
#define JMPSIZE 5
#endif
			ZyanUSize offset = 0;
			const ZyanUSize length = 0xFF;
			ZydisDecodedInstruction instruction;
			while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(
				&de, (char*)From + offset, length - offset, &instruction)))
			{
				offset += instruction.length;
				if (offset >= JMPSIZE)
				{
					Length = offset;
					break;
				}
			}
		}
		DWORD dwOld;
		VirtualProtect((void*)From, Length, PAGE_EXECUTE_READWRITE, &dwOld);

		old_code = (PBYTE)VirtualAlloc(nullptr, Length + JMPSIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		memcpy(old_code, (void*)From, Length);

		JMPFUNC((UINT_PTR)old_code + Length, (UINT_PTR)From + Length);
		JMPFUNC((UINT_PTR)From, (UINT_PTR)To);

		VirtualProtect((void*)From, Length, dwOld, &dwOld);
		FlushInstructionCache(GetCurrentProcess(), 0, 0);

		original = (void*)From, code_len = Length;
		hooked = true;
	}

	// - Initializes the hook at start of the exported function in the module
	// - Will fail if the library or function is not found
	void Hook(const char* Module, const char* Function, void* To, size_t Length)
	{
		HMODULE hMod = GetModuleHandle(Module);
		FARPROC pFunc = GetProcAddress(hMod, Function);
		Hook(pFunc, To, Length);
	}

	// - Restores old code to the original function
	template <class T = unsigned long>
	void UnHook()
	{
		if (!hooked)
			return;

		DWORD dwOld;
		VirtualProtect(original, code_len, PAGE_EXECUTE_READWRITE, &dwOld);
		memcpy_s(original, code_len, old_code, code_len);
		VirtualProtect(original, code_len, dwOld, &dwOld);
		VirtualFree(old_code, 0, MEM_RELEASE);
		FlushInstructionCache(GetCurrentProcess(), 0, 0);

		original = nullptr, code_len = 0;
		hooked = false;
	}

private:
	// - Length: 5
	inline void RelJmp(UINT_PTR From, UINT_PTR To)
	{
		*(BYTE*)From = 0xE9;
		*(DWORD*)(From + 1) = To - From - 5;
	}

	// - Length: 14
	inline void AbsJmp(UINT_PTR From, UINT_PTR To)
	{
		PBYTE b = (PBYTE)From;

		*(WORD*)From = MAKEWORD(0xFF, 0x25);	// jmp [rip+imm32]
		*(DWORD*)&b[2] = 0;						// rip + 0
		*(UINT_PTR*)&b[6] = To;

		/**(WORD*)From = MAKEWORD(0x49, 0xBF);	// mov r15, qword
		*(DWORD64*)(From + sizeof(WORD)) = To;	// qword
		b[0xA] = 0x41;
		b[0xB] = 0xFF;
		b[0xC] = 0xE7;*/

		//*(WORD*)From = MAKEWORD(0x48, 0xB8);	// mov rax, qword
		//*(WORD*)(From + sizeof(WORD) + sizeof(DWORD64)) = MAKEWORD(0xFF, 0xE0);	// jmp rax
	}
};