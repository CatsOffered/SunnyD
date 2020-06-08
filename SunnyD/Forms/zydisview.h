#pragma once
#include "basecontrol.h"

class CZydisView : public CBaseControl
{
public:
	CZydisView(CBaseForm* Parent, int X, int Y, int W, int H);

	inline PBYTE Location() const { return m_code; }
	void SetLocation(void* Location, size_t Len = 0x100);

private:
	PBYTE m_code;
};