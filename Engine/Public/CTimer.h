#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	_float Get_TimeDelta() const { return m_fTimeDelta; }

public:
	HRESULT			Ready_Timer();
	// 이전 호출과 현재 호출 사이 Delta Time을 계산
	void			Compute_Timer();

private:
	LARGE_INTEGER		m_CntPerSec			= {};
	LARGE_INTEGER		m_LastSyncCnt		= {};
	LARGE_INTEGER		m_PreCnt			= {};
	LARGE_INTEGER		m_CurCnt			= {};

	_float				m_fTimeDelta		= {};

public:
	static CTimer* Create();
protected:
	virtual void Free() override;
};

NS_END
