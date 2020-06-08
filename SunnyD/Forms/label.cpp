#include "label.h"

CLabel::CLabel(CBaseForm* Parent, int X, int Y, LPCSTR Text)
	: CBaseControl(WC_STATIC, Parent)
{
	CreateWnd("", X, Y, 100, 12);
	SetWindowText(m_hwnd, Text);
}