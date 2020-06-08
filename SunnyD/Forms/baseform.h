#pragma once
#include "sunnyd.h"
#include <list>

class CBaseControl;

class CBaseForm
{
public:
	inline HWND Handle() { return m_hwnd; }
	inline void Show(bool Yes) { ShowWindow(m_hwnd, Yes ? SW_SHOW : SW_HIDE); }
	inline const std::list<CBaseControl*>& Children() { return m_children; }
	inline LPCSTR Class() { return m_class; }
	void SetPos(int X, int Y);
	void SetSize(int Width, int Height);

protected:
	CBaseForm(LPCSTR ClassName, CBaseForm* Parent = 0)
		: m_class(ClassName), m_parent(Parent) { }
	virtual ~CBaseForm() { DestroyWnd(); }

	ATOM Register();
	virtual void MakeClass(WNDCLASS& Class) { }
	void CreateWnd(LPCSTR Title, int Width, int Height, DWORD Style = WS_OVERLAPPEDWINDOW, HWND hParent = 0, HMENU hMenu = 0);
	virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	inline void DestroyWnd()
	{
		if (m_hwnd)
			DestroyWindow(m_hwnd);
		m_hwnd = 0;
	}

	friend class CBaseControl;
	inline void Add(CBaseControl* Control) { m_children.push_back(Control); }
	inline void Remove(CBaseControl* Control) { m_children.remove(Control); }

private:
	static LRESULT CALLBACK _WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	HWND m_hwnd = 0;
	LPCSTR m_class = 0;
	std::list<CBaseControl*> m_children;
	CBaseForm* m_parent = 0;
};