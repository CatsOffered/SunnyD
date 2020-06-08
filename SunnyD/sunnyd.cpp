#include "sunnyd.h"
#include "Forms/mainform.h"

namespace SunnyD
{
	HINSTANCE hInst;
	HANDLE hThread;
	CMainForm* form;
	HANDLE hWin = 0;
	bool reD = false;

	DWORD WINAPI UIThread(LPVOID Args)
	{
		form = new CMainForm;
		form->Show(true);

		reD = true;

		MSG msg;
		while (GetMessage(&msg, 0, 0, UINT_MAX))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		FreeLibraryAndExitThread(hInst, EXIT_SUCCESS);
		return 0;
	}

	DWORD WINAPI UpdateThread(LPVOID Args)
	{
		while (!reD)
			Sleep(100);

		auto p = (ProcAddr*)Args;
		if (p->szProc)
			form->Imports()->AddProc(p->szModule, p->szProc);
		else
			form->Imports()->AddModule(p->szModule);

		return 0;
	}
	void AddProcInfo(ProcAddr* Info) {
		CreateThread(0, 0, SunnyD::UpdateThread, Info, 0, 0);
	}

	std::list<HANDLE> thredz;
	std::mutex mtx_thredz;

	void Update(std::function<void()> Func) {
		auto func = new std::function<void()>(Func);
		mtx_thredz.lock();
		thredz.push_back(CreateThread(0, 0, WaitThread, func, 0, 0));
		mtx_thredz.unlock();
	}

	DWORD WINAPI WaitThread(LPVOID Args)
	{
		while (!reD)
			Sleep(100);

		HANDLE nextup = 0;
		mtx_thredz.lock();
		if (!thredz.empty() && GetThreadId(thredz.front()) != GetCurrentThreadId())
		{
			for (auto it = thredz.rbegin(); it != thredz.rend(); it++)
			{
				if (GetThreadId(*it) == GetCurrentThreadId())
				{
					nextup = *++it;
					break;
				}
			}
		}
		mtx_thredz.unlock();

		if (nextup)
			WaitForSingleObject(nextup, INFINITE);
		auto func = (std::function<void()>*)Args;
		(*func)();
		delete func;
		mtx_thredz.lock();
		thredz.pop_front();
		mtx_thredz.unlock();
		return 0;
	}
}

ProcAddr::~ProcAddr()
{
	delete[] szModule;
	if (szProc)
		delete[] szProc;
}
