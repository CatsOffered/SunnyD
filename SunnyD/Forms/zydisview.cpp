#include "zydisview.h"
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

void CZydisView::SetLocation(void* Location, size_t Len)
{
	m_code = (PBYTE)Location;
    // Initialize decoder context
    ZydisDecoder decoder;

#ifdef _WIN64
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
#else
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
#endif

    std::string text;

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = (ZyanU64)m_code;
    ZyanUSize offset = 0;
    ZydisDecodedInstruction instruction;
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, m_code + offset, Len - offset,
        &instruction)))
    {
        // Print current instruction pointer.
        char buffer[256];
        sprintf_s(buffer, "%016" PRIX64 "  ", runtime_address);
        text += buffer;

        // Format & print the binary instruction structure to human readable format
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
            runtime_address);
        text += buffer;
        text += "\r\n";

        offset += instruction.length;
        runtime_address += instruction.length;
    }

    SetWindowText(m_hwnd, text.c_str());
}
