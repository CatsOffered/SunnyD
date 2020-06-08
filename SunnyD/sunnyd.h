#pragma once
#include "sunnydata.h"
#include <functional>

class CBaseForm;
class CMainForm;

namespace SunnyD
{
	extern HINSTANCE hInst;
	extern HANDLE hThread;
	extern CMainForm* form;
	extern HANDLE hWin;
	extern bool reD;

	DWORD WINAPI UIThread(LPVOID Args);
	DWORD WINAPI UpdateThread(LPVOID Args);
	DWORD WINAPI WaitThread(LPVOID Args);

	void AddProcInfo(ProcAddr* Info);
	void Update(std::function<void()> Func);
}