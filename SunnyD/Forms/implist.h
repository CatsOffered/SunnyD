#pragma once
#include "baselist.h"

class CImpList : public CBaseControl
{
public:
	CImpList(CBaseForm* Parent, int X, int Y, int W, int H);

	HTREEITEM AddModule(LPCSTR Name) { MTXCALLR(AddModule(Name)); }
	void AddProc(LPCSTR Module, LPCSTR Proc) { MTXCALL(AddProc(Module, Proc)); }
	HTREEITEM Find(LPCSTR Name, HTREEITEM Parent = TVI_ROOT) { MTXCALLR(Find(Name, Parent)); }
	int Count(HTREEITEM Item) { MTXCALLR(Count(Item)); }

private:
	HTREEITEM _AddModule(LPCSTR Name);
	void _AddProc(LPCSTR Module, LPCSTR Proc);
	HTREEITEM _Find(LPCSTR Name, HTREEITEM Parent = TVI_ROOT);
	int _Count(HTREEITEM Item);

	std::mutex m_mtx;
};