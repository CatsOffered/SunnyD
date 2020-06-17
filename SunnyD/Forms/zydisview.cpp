#include "zydisview.h"
#include "sig.h"
#include <string>

#define Zydis_EXPORTS
#include <Zydis/Zydis.h>
#include <inttypes.h>

CZydisView::CZydisView(CBaseForm* Parent, int X, int Y, int W, int H)
	: CBaseControl(WC_EDIT, Parent)
{
	CreateWnd("Zydis view", X, Y, W, H, WS_THICKFRAME | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL);
	SetFont(NULL);
}

void CZydisView::SetLocation(const MemInfo* Info)
{
	m_code = (PBYTE)Info->pWriteCpy;
    // Initialize decoder context
    ZydisDecoder decoder;

	if constexpr (SunnyD::Win64)
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	else
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);

    std::string text;

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = (ZyanU64)Info->pLoc;
    ZyanUSize offset = 0;
    ZydisDecodedInstruction ins;
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, m_code + offset, Info->dwSize - offset,
        &ins)))
    {
        // Print current instruction pointer.
        char buffer[256];
        sprintf_s(buffer, "%016" PRIX64 "  ", runtime_address);
        text += buffer;

        // Format & print the binary instruction structure to human readable format
        ZydisFormatterFormatInstruction(&formatter, &ins, buffer, sizeof(buffer),
            runtime_address);
        text += buffer;

		
		// Scrape any information possible from pointers.
		// Their location in a module, or possibly if it's an import

		for (size_t i = 0; i < 2; i++)
		{
			auto& op = ins.operands[i];
			ZyanU64 absptr = 0;
			switch (ins.mnemonic)
			{
			case ZYDIS_MNEMONIC_JMP:
			case ZYDIS_MNEMONIC_PUSH:
			case ZYDIS_MNEMONIC_CALL:
			case ZYDIS_MNEMONIC_MOV:
			{
				ZydisCalcAbsoluteAddress(&ins, &op, (ZyanU64)Info->pLoc + offset, &absptr);

				if (!absptr) // Usually Zydis doesn't recognize shellcode pushing / moving imm values as pointers
				{
					switch (op.type)
					{
					case ZYDIS_OPERAND_TYPE_POINTER:
						absptr = (ZyanU64)Info->pLoc + offset + ins.length + op.ptr.offset;
						break;

					case ZYDIS_OPERAND_TYPE_IMMEDIATE:
						if constexpr (SunnyD::Win64)
						{
							if (op.size == sizeof(__int32) * 8) // If x64, maybe this is a relative pointer
								absptr = (ZyanU64)((ZyanI64)runtime_address + ins.length + op.imm.value.s);
						}
						else
							absptr = op.imm.value.u;
						break;
					}
				}

				if (absptr)
				{
					printf("Absptr: %p\n", (UINT_PTR)absptr);
					DWORD pId = GetProcessId(Info->hProc);
					if (const char* name = Sig::GetExportedName(absptr, pId))
						text += name;
					else if (HMODULE mod = Sig::FindModule(absptr, pId))
					{
						char name[MAX_PATH];
						if (GetModuleFileNameA(mod, name, sizeof(name)))
						{
							char* base = strrchr(name, '\\') + 1;
							text += base;
						}
					}
				}
			}
			}
		}

        text += "\r\n";

        offset += ins.length;
        runtime_address += ins.length;
    }

    SetWindowText(m_hwnd, text.c_str());
}
