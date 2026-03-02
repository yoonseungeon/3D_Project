#pragma once

#include "CBase.h"

#pragma region include_tbb/concurrent_queue.h

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

#include <tbb/concurrent_queue.h>

#ifdef _DEBUG
#ifndef new
#define new DBG_NEW
#endif
#endif

#pragma endregion

NS_BEGIN(Engine)

class CThread_Manager final : public CBase
{
private:
	struct JOB {
		function<void()> work = { nullptr };
	};

private:
	CThread_Manager();
	virtual ~CThread_Manager() = default;

public:
	HRESULT Initialize();
	void DoWork();
	void Add_Job(function<void()> func);

private:
	vector<HANDLE>				m_vecThreads;
	HANDLE						m_hEvent		= { };
	std::atomic<_bool>			m_bExit			= { false };
	tbb::concurrent_queue<JOB>	m_JobQueue;

public:
	static CThread_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END