#include "memlist.h"
#include <Psapi.h>
#include <inttypes.h>
#include "sig.h"

CMemList::CMemList(CDumpForm* Parent, int X, int Y, int W, int H)
	: CBaseList(WC_TREEVIEW, (CBaseForm*)Parent) {
	CreateWnd("Mem list", X, Y, W, H, TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES | WS_SIZEBOX);
}

HTREEITEM CMemList::_AddAlloc(MemInfo* Info)
{
	HTREEITEM proc = _AddProc(Info->hProc);

	char buf[128];
	sprintf_s(buf, "Alloc: %" PRIXPTR " - Size: %X", Info->pLoc, Info->dwSize);

	TVINSERTSTRUCT ins;
	ins.hInsertAfter = TVI_LAST;
	ins.hParent = proc;

	ins.item.mask = TVIF_PARAM | TVIF_TEXT;
	ins.item.pszText = buf;
	ins.item.cchTextMax = sizeof(buf);
	ins.item.lParam = (LPARAM)Info;

	return TreeView_InsertItem(m_hwnd, &ins);
}

HTREEITEM CMemList::_AddWrite(MemInfo* Info)
{
	HTREEITEM alloc = _Find(*Info);
	if (!alloc)
		return 0;

	TVITEM info;
	info.mask = TVIF_PARAM;
	info.hItem = alloc;
	info.lParam = 0;
	TreeView_GetItem(m_hwnd, &info);
	MemInfo* alloc_info = (MemInfo*)info.lParam;

	char buf[128];
	sprintf_s(buf, "Off: %" PRIXPTR " - Len: %X", (UINT_PTR)Info->pLoc - (UINT_PTR)alloc_info->pLoc, Info->dwSize);

	TVINSERTSTRUCT ins;
	ins.hInsertAfter = TVI_LAST;
	ins.hParent = alloc;

	ins.item.mask = TVIF_PARAM | TVIF_TEXT;
	ins.item.pszText = buf;
	ins.item.cchTextMax = sizeof(buf);
	ins.item.lParam = (LPARAM)Info;

	return TreeView_InsertItem(m_hwnd, &ins);
}

HTREEITEM CMemList::_AddThread(MemInfo* Info)
{
	// Find alloc, and if it exists then add thread to list
	// else, stick it in process

	char buf[128];

	TVINSERTSTRUCT ins;
	ins.hInsertAfter = TVI_LAST;
	ins.item.mask = TVIF_PARAM | TVIF_TEXT;
	ins.item.lParam = (LPARAM)Info;
	ins.item.pszText = buf;

	if (HTREEITEM alloc = _Find(*Info))
	{
		ins.hParent = alloc;

		TVITEM info;
		info.mask = TVIF_PARAM;
		info.hItem = alloc;
		info.lParam = 0;
		TreeView_GetItem(m_hwnd, &info);
		MemInfo* alloc_info = (MemInfo*)info.lParam;

		sprintf_s(buf, "(THREAD) Off: %" PRIXPTR " - Arg size: %X", (UINT_PTR)Info->pLoc - (UINT_PTR)alloc_info->pLoc, Info->dwSize);

		return TreeView_InsertItem(m_hwnd, &ins);
	}

	ins.hParent = _AddProc(Info->hProc);

	HMODULE hmod = Sig::FindModule((UINT_PTR)Info->pLoc, GetProcessId(Info->hProc));
	char name[MAX_PATH];
	if (hmod && hmod != Info->hProc && GetModuleFileNameA(hmod, name, sizeof(name)))
	{
		char* base = strrchr(name, '\\');
		sprintf_s(buf, "THREAD in %s at %" PRIXPTR " - Arg size: %X\n", base ? base + 1 : name, Info->pLoc, Info->dwSize);
	}
	else
		sprintf_s(buf, "THREAD at %" PRIXPTR " - Arg size: %X\n", Info->pLoc, Info->dwSize);

	return TreeView_InsertItem(m_hwnd, &ins);
}

HTREEITEM CMemList::_AddProc(HANDLE Proc)
{
	if (HTREEITEM next = _Find(Proc))
		return next;

	char path[MAX_PATH];
	char buf[MAX_PATH + 32];
	if (GetModuleFileNameExA(Proc, 0, path, sizeof(path)))
	{
		char* name = strrchr(path, '\\');
		sprintf_s(buf, "%s %X", name ? name + 1 : path, GetProcessId(Proc));
	}
	else
		sprintf_s(buf, "??? %X", GetProcessId(Proc));

	TVINSERTSTRUCT ins;
	ins.hInsertAfter = TVI_LAST;
	ins.hParent = TVI_ROOT;

	ins.item.mask = TVIF_PARAM | TVIF_TEXT;
	ins.item.pszText = buf;
	ins.item.lParam = (LPARAM)GetProcessId(Proc);

	return TreeView_InsertItem(m_hwnd, &ins);
}

HTREEITEM CMemList::_Find(HANDLE Proc)
{
	DWORD pId = GetProcessId(Proc);
	HTREEITEM next;
	next = TreeView_GetChild(m_hwnd, TVI_ROOT);
	while (next)
	{
		TVITEM info;
		info.mask = TVIF_PARAM | TVIF_HANDLE;
		info.hItem = next;
		info.lParam = 0;
		TreeView_GetItem(m_hwnd, &info);

		if (info.lParam == (LPARAM)pId)
			return next;

		next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT);
	}
	return next;
}

HTREEITEM CMemList::_Find(const MemInfo& Info)
{
	HTREEITEM next = _Find(Info.hProc);
	if (next)
		next = TreeView_GetChild(m_hwnd, next);

	while (next)
	{
		TVITEM info;
		info.mask = TVIF_PARAM | TVIF_HANDLE;
		info.hItem = next;
		info.lParam = 0;
		TreeView_GetItem(m_hwnd, &info);

		if (const MemInfo* alloc = (MemInfo*)info.lParam)
		{
			if (alloc->type == Mem_Alloc && Info.pLoc >= alloc->pLoc &&
				(UINT_PTR)Info.pLoc <= ((UINT_PTR)alloc->pLoc + alloc->dwSize))
				return next;
		}
		next = TreeView_GetNextItem(m_hwnd, next, TVGN_NEXT);
	}
	if (next)
		MessageBox(0, "Failed to find it", "Not cool", MB_ICONERROR);
	return next;
}
