#include "dumpform.h"
#include "zydisview.h"

CDumpForm::CDumpForm(CBaseForm* Parent, int X, int Y)
	: CBaseForm("CDumpForm", Parent)
{
	CreateWnd("Dump form", 300, 300, WS_SIZEBOX | WS_VISIBLE);
	SetPos(X, Y);
	m_mem = new CMemList(this, 0, 0, 200, 300);
	m_dis = new CZydisView(this, 200, 0, 200, 300);
}

void CDumpForm::AddAlloc(MemInfo* Info)
{
	m_mtx.lock();
	m_allocs.push_back(Info);
	m_mem->AddAlloc(Info);
	m_mtx.unlock();
}

void CDumpForm::AddWrite(MemInfo* Info)
{
	m_mtx.lock();
	m_writes.push_back(Info);
	m_mem->AddWrite(Info);
	m_mtx.unlock();
}

void CDumpForm::AddThread(MemInfo* Info)
{
	m_mtx.lock();

	if (!Info->dwSize)
	{
		for (auto alloc : m_allocs)
		{
			if (Info->pThreadArgs == alloc->pLoc)
			{
				Info->dwSize = alloc->dwSize;
				break;
			}
		}
	}

	m_threads.push_back(Info);
	m_mem->AddThread(Info);
	m_mtx.unlock();
}

LRESULT CDumpForm::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		LPNMHDR what = (LPNMHDR)lParam;
		switch (what->code)
		{
		case TVN_SELCHANGED:
			m_mtx.lock();
			LPNMTREEVIEW tv = (LPNMTREEVIEW)what;
			if (what->hwndFrom == m_mem->Handle())
			{
				HTREEITEM item = tv->itemNew.hItem;
				if (TreeView_GetParent(m_mem->Handle(), item)) // Item has parent, meaning it holds a MemInfo struct
				{
					MemInfo* info = (MemInfo*)m_mem->Param(item);
					if (info->type == Mem_Write && info->pWriteCpy)
						m_dis->SetLocation(info->pWriteCpy, info->dwSize);
				}
			}
			m_mtx.unlock();
			break;
		}
		break;
	}
	}
	return CBaseForm::WindowProc(hWnd, uMsg, wParam, lParam);
}

const MemInfo* CDumpForm::_FindAlloc(void* Loc)
{
	for (auto alloc : m_allocs)
		if ((UINT_PTR)Loc >= (UINT_PTR)alloc->pLoc && (UINT_PTR)Loc <= (UINT_PTR)alloc->pLoc + alloc->dwSize)
			return alloc;
	return nullptr;
}
