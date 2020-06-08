#pragma once
#include "basecontrol.h"

class CMainTabs : public CBaseControl
{
	CMainTabs(CBaseForm* Parent);

private:
	void CreateTab(LPCSTR Name);
	int Count();
};