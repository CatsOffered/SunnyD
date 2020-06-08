#pragma once
#include "sunnyd.h"
#include <CommCtrl.h>

class CBaseForm;

class CBaseControl
{
public:
	inline CBaseControl(HWND Handle) : m_hwnd(Handle) { }

	inline HWND Handle() { return m_hwnd; }
	void SetPos(int X, int Y);
	void SetSize(int Width, int Height);
	void SetFont(HFONT Font);

	static HFONT DefaultFont();

protected:
	CBaseControl(LPCSTR Class, CBaseForm* Parent) : m_class(Class), m_parent(Parent) { }
	virtual ~CBaseControl() { }

	void CreateWnd(LPCSTR Title, int X, int Y, int Width, int Height, DWORD Style = 0, HMENU hMenu = 0);
	inline void DestroyWnd()
	{
		if (m_hwnd)
			DestroyWindow(m_hwnd);
		m_hwnd = 0;
	}

	HWND m_hwnd = 0;
	CBaseForm* m_parent = 0;

private:
	LPCSTR m_class = 0;
};