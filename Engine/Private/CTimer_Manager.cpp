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
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	CTimer* pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.emplace(strTimerTag, pTimer);

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
	auto iter = m_mapTimers.find(strTimerTag);

	if (m_mapTimers.end() == iter)
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_mapTimers)
		Safe_Release(Pair.second);
	m_mapTimers.clear();
}
