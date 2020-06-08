#include "maintabs.h"

CMainTabs::CMainTabs(CBaseForm* Parent)
	: CBaseControl(WC_TABCONTROL, Parent)
{
	CreateWnd("Tabs", 0, 0, 420, 420);
	CreateTab("Imports");
	CreateTab("Alloc/Write");
}

void CMainTabs::CreateTab(LPCSTR Name)
{
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPSTR)Name;
	TabCtrl_InsertItem(m_hwnd, Count(), &item);
}

int CMainTabs::Count() {
	return TabCtrl_GetItemCount(m_hwnd);
}
