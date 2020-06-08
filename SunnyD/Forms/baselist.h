#pragma once
#include "basecontrol.h"
#include <mutex>

#define MTXCALL(x) m_mtx.lock(); _##x; m_mtx.unlock()
#define MTXCALLR(x) m_mtx.lock(); auto ret = _##x; m_mtx.unlock(); return ret

class CBaseList : public CBaseControl
{
public:
	int Count(HTREEITEM Item) { MTXCALLR(Count(Item)); }
	LPARAM Param(HTREEITEM Item) { MTXCALLR(Param(Item)); }

protected:
	CBaseList(LPCSTR Class, CBaseForm* Parent) : CBaseControl(Class, Parent) { }
	int _Count(HTREEITEM Item);
	LPARAM _Param(HTREEITEM Item);

	std::mutex m_mtx;
};