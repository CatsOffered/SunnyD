#pragma once
#include "basecontrol.h"

struct MemInfo;

class CZydisView : public CBaseControl
{
public:
	CZydisView(CBaseForm* Parent, int X, int Y, int W, int H);

	inline PBYTE Location() const { return m_code; }
	void SetLocation(const MemInfo* Info);

private:
	PBYTE m_code;
};