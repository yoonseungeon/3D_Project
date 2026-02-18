#include "CTimer.h"

CTimer::CTimer()
{
}

HRESULT CTimer::Ready_Timer()
{
	// 고해상도 성능 카운터의 현재 누적 카운트(타임스탬프) 읽기
	QueryPerformanceCounter(&m_CurCnt);
	QueryPerformanceCounter(&m_PreCnt);
	QueryPerformanceCounter(&m_LastSyncCnt);

	// QPC(고해상도 성능 카운터)가 1초에 몇 번 증가하는지(= 주파수)
	QueryPerformanceFrequency(&m_CntPerSec);

	return S_OK;
}

void CTimer::Compute_Timer()
{
	// 고해상도 성능 카운터의 현재 누적 카운트(타임스탬프) 읽기
	QueryPerformanceCounter(&m_CurCnt);

	// 1초 이상 경과했으면 QPC 주파수(초당 카운트)를 다시 조회
	if (m_CurCnt.QuadPart - m_LastSyncCnt.QuadPart >= m_CntPerSec.QuadPart)
	{
		QueryPerformanceFrequency(&m_CntPerSec);
		m_LastSyncCnt = m_CurCnt;
	}

	// 시간 계산
	m_fTimeDelta = (m_CurCnt.QuadPart - m_PreCnt.QuadPart) / static_cast<_float>(m_CntPerSec.QuadPart);

	m_PreCnt = m_CurCnt;
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX("CTimer.cpp Failed to Created : CTimer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
	__super::Free();
}

