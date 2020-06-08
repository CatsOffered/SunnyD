#include "baseform.h"

void CBaseForm::SetPos(int X, int Y) {
	SetWindowPos(m_hwnd, HWND_TOP, X, Y, 0, 0, SWP_NOSIZE);
}

void CBaseForm::SetSize(int Width, int Height) {
	SetWindowPos(m_hwnd, HWND_TOP, 0, 0, Width, Height, SWP_NOMOVE);
}

ATOM CBaseForm::Register()
{
	WNDCLASS cl;
	ZeroMemory(&cl, sizeof(cl));
	cl.style = CS_DBLCLKS;
	cl.hInstance = SunnyD::hInst;
	cl.hbrBackground = (HBRUSH)COLOR_WINDOW;
	cl.lpszClassName = m_class;
	cl.lpfnWndProc = &CBaseForm::_WindowProc;
	cl.cbWndExtra = sizeof(CBaseForm*);
	cl.hCursor = LoadCursor(0, IDC_ARROW);
	MakeClass(cl);
	ATOM result = RegisterClass(&cl);
	if (!result)
	{
		char buf[256];
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, buf, sizeof(buf), 0))
			MessageBoxA(0, buf, "Error", MB_ICONERROR);
	}
	return result;
}

void CBaseForm::CreateWnd(LPCSTR Title, int Width, int Height, DWORD Style, HWND hParent, HMENU hMenu)
{
	static WNDCLASSEX wc;
	if (!GetClassInfoEx(SunnyD::hInst, (LPCSTR)m_class, &wc))
		Register();

	DWORD extra = m_parent ? WS_CHILD : 0;
	m_hwnd = CreateWindow(m_class, Title, Style | extra, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, m_parent ? m_parent->Handle() : hParent, hMenu, SunnyD::hInst, 0);
	if (!m_hwnd)
		MessageBoxA(0, "Failed to create window", "Info", MB_ICONINFORMATION);
	else
		SetWindowLongPtr(m_hwnd, 0, (LONG_PTR)this);
}

LRESULT CBaseForm::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CBaseForm::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBaseForm* inst = (CBaseForm*)GetWindowLongPtr(hwnd, 0);
	if (inst)
		return inst->WindowProc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
