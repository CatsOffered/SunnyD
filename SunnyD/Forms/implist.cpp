#include "implist.h"

CImpList::CImpList(CBaseForm* Parent, int X, int Y, int W, int H)
	: CBaseControl(WC_TREEVIEW, Parent) {
	CreateWnd("Imports", X, Y, W, H, TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES | WS_THICKFRAME);
}

HTREEITEM CImpList::_AddModule(LPCSTR Name)
{
	if (HTREEITEM next = _Find(Name))
		return next;

	TVITEM item;
	item.mask = TVIF_TEXT;
	item.pszText = (LPSTR)Name;

	TVINSERTSTRUCT ins;
	ins.item = item;
	ins.hInsertAfter = TVI_LAST;
	ins.hParent = TVI_ROOT;

	return TreeView_InsertItem(m_hwnd, &ins);
}

void CImpList::_AddProc(LPCSTR Module, LPCSTR Proc)
{
	HTREEITEM next;
	next = TreeView_GetChild(m_hwnd, TVI_ROOT);

	size_t len = strlen(Module);
	char buf[MAX_PATH];
	while (next)
	{
		TVITEM info;
		info.mask = TVIF_TEXT | TVIF_HANDLE;
		info.hItem = next;
		info.pszText = buf;
		info.cchTextMax = sizeof(buf);
		TreeView_GetItem(m_hwnd, &info);

		if (!strcasecmp(info.pszText, Module, len))
		{
			if (_Find(Proc, next))
				break;
			TVITEM item;
			item.mask = TVIF_TEXT;
			item.pszText = (LPSTR)Proc;

			TVINSERTSTRUCT ins;
			ins.item = item;
			ins.hInsertAfter = 0;
			ins.hParent = next;
			TreeView_InsertItem(m_hwnd, &ins);

			sprintf_s(buf, "%s (%i)", Module, _Count(next));
			item.mask = TVIF_TEXT;
			item.pszText = buf;
			item.hItem = next;
			TreeView_SetItem(m_hwnd, &item);
			break;
		}
		next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT);
	}

	if (!next) // Module not found
	{
		_AddModule(Module);
		_AddProc(Module, Proc);
	}
}

HTREEITEM CImpList::_Find(LPCSTR Name, HTREEITEM Parent)
{
	HTREEITEM next;
	next = TreeView_GetChild(m_hwnd, Parent);

	size_t len = strlen(Name);
	char buf[MAX_PATH];

	while (next)
	{
		TVITEM info;
		info.mask = TVIF_TEXT | TVIF_HANDLE;
		info.hItem = next;
		info.pszText = buf;
		info.cchTextMax = sizeof(buf);
		TreeView_GetItem(m_hwnd, &info);

		if (!strcasecmp(info.pszText, Name, len))
			break;
		next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT);
	}
	return next;
}

int CImpList::_Count(HTREEITEM Item)
{
	int count = 0;
	for (HTREEITEM next = TreeView_GetChild(m_hwnd, Item); next; next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT), count++);
	return count;
}
