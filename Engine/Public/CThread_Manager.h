#pragma once

#include "CBase.h"

// 1. 디버그용 new 매크로가 있다면 잠시 해제
#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

// 2. TBB 헤더 포함 (이제 충돌 안 남)
#include <tbb/tbb.h>
#include <tbb/concurrent_unordered_map.h>

// 3. 다시 디버그용 new 매크로 복구
#ifdef _DEBUG
#ifndef new           // DBG_NEW 대신 'new' 매크로가 없는지 확인합니다.
#define new DBG_NEW   // 이미 정의된 DBG_NEW를 다시 new에 연결합니다.
#endif
#endif

NS_BEGIN(Engine)

class CThread_Manager final : public CBase
{
private:
	CThread_Manager();
	virtual ~CThread_Manager() = default;

public:
	HRESULT Initialize();
	HANDLE Get_Event() { return m_hEvent; }
	_bool IsExit() { return m_bExit.load(memory_order_relaxed); }

private:
	vector<HANDLE>	m_vecThreads;
	HANDLE			m_hEvent = { };
	//_bool			m_bExit = { false };
	atomic_bool		m_bExit = { false };

public:
	static CThread_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END