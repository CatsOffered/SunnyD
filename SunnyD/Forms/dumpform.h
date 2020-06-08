#pragma once
#include "baseform.h"
#include "memlist.h"

class CZydisView;

class CDumpForm : public CBaseForm
{
public:
	CDumpForm(CBaseForm* Parent, int X, int Y);

	void AddAlloc(MemInfo* Info);
	void AddWrite(MemInfo* Info);
	void AddThread(MemInfo* Info);

protected:
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	friend class CMemList;
	inline std::list<MemInfo*>& Allocs() { return m_allocs; }
	inline std::list<MemInfo*>& Writes() { return m_writes; }

private:
	const MemInfo* _FindAlloc(void* Loc);

	CMemList* m_mem;
	CZydisView* m_dis;
	std::list<MemInfo*> m_allocs, m_writes, m_threads;
	std::mutex m_mtx;
};