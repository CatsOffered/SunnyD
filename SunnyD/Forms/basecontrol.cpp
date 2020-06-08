#include "basecontrol.h"
#include "baseform.h"

void CBaseControl::SetPos(int X, int Y) {
	SetWindowPos(m_hwnd, HWND_TOP, X, Y, 0, 0, SWP_NOSIZE);
}

void CBaseControl::SetSize(int Width, int Height) {
	SetWindowPos(m_hwnd, HWND_TOP, 0, 0, Width, Height, SWP_NOMOVE);
}

void CBaseControl::SetFont(HFONT Font) {
	SendMessage(m_hwnd, WM_SETFONT, (WPARAM)Font, MAKELPARAM(true, 0));
}

HFONT CBaseControl::DefaultFont()
{
	static HFONT hFont = CreateFontA(14, 0, 0, 0, FW_DONTCARE, false, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_MODERN, "Segoe UI");
	return hFont;
}

void CBaseControl::CreateWnd(LPCSTR Title, int X, int Y, int Width, int Height, DWORD Style, HMENU hMenu)
{
	static bool init = false;
	if (!init)
		InitCommonControls(), init = true;
	m_hwnd = CreateWindow(m_class, Title, Style | WS_CHILD | WS_VISIBLE, X, Y, Width, Height, m_parent->Handle(), hMenu, SunnyD::hInst, 0);
	SetFont(DefaultFont());
	m_parent->Add(this);
}