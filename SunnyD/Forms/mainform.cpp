#include "mainform.h"

CMainForm::CMainForm() : CBaseForm("CMainForm")
{
	CreateWnd("Main form", 700, 400);
	m_list = new CImpList(this, 5, 25, 250, 250);
	m_implabel = new CLabel(this, 5, 5, "Imports");
	m_dumps = new CDumpForm(this, 400, 25);
}

LRESULT CMainForm::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		RECT r;
		GetClientRect(Handle(), &r);
		int w = 0, h = r.bottom - r.top - 10;
		GetWindowRect(m_list->Handle(), &r);
		w = r.right - r.left;
		m_list->SetSize(w, h);
		break;
	}
	return CBaseForm::WindowProc(hwnd, uMsg, wParam, lParam);
}
