#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CTimer;

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_Timer(const _wstring& strTimerTag);

private:
	map<const _wstring, class CTimer*> m_mapTimers;

private:
	class CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END