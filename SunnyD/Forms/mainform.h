#pragma once
#include "baseform.h"
#include "dumpform.h"
#include "implist.h"
#include "label.h"

class CMainForm : public CBaseForm
{
public:
	CMainForm();
	~CMainForm() { }

	inline CImpList* Imports() { return m_list; }
	inline CDumpForm* Dumps() { return m_dumps; }

protected:
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	CDumpForm* m_dumps;
	CImpList* m_list;
	CLabel* m_implabel;
};