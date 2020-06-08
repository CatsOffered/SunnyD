#pragma once
#include "baselist.h"

class CDumpForm;

class CMemList : public CBaseList
{
public:
	CMemList(CDumpForm* Parent, int X, int Y, int W, int H);

	void AddAlloc(MemInfo* Info) { MTXCALL(AddAlloc(Info)); }
	void AddWrite(MemInfo* Info) { MTXCALL(AddWrite(Info)); }
	void AddThread(MemInfo* Info) { MTXCALL(AddThread(Info)); }

private:
	CDumpForm* Dumps() { return (CDumpForm*)m_parent; }

	HTREEITEM _AddAlloc(MemInfo* Info);
	HTREEITEM _AddWrite(MemInfo* Info);
	HTREEITEM _AddThread(MemInfo* Info);
	HTREEITEM _AddProc(HANDLE Proc);
	HTREEITEM _Find(HANDLE Proc);
	HTREEITEM _Find(const MemInfo& Info);
};