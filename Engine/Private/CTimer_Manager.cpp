#include "CTimer_Manager.h"
#include "CTimer.h"

CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (pTimer == nullptr)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = CTimer::Create();
	if (pTimer == nullptr)
		return E_FAIL;

	auto iter = m_Timers.emplace(strTimerTag, pTimer);

	if (iter.second == false)
	{
		Safe_Release(pTimer);
		return E_FAIL;
	}

	return S_OK;
}

void CTimer_Manager::Compute_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (pTimer == nullptr)
		return;

	pTimer->Compute_Timer();
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	auto iter = m_Timers.find(strTimerTag);

	if (m_Timers.end() == iter)
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);
	m_Timers.clear();

	__super::Free();
}
