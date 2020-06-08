#include "baselist.h"

int CBaseList::_Count(HTREEITEM Item)
{
	int count = 0;
	for (HTREEITEM next = TreeView_GetChild(m_hwnd, Item); next; next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT), count++);
	return count;
}

LPARAM CBaseList::_Param(HTREEITEM Item)
{
	TVITEM item;
	item.mask = TVIF_PARAM | TVIF_HANDLE;
	item.hItem = Item;
	TreeView_GetItem(m_hwnd, &item);
	return item.lParam;
}
