#include "CThread_Manager.h"

#include <process.h>

CThread_Manager::CThread_Manager()
{
}

// 멤버 함수 아님. 전역 함수임. 멤버 변수 직접 접근 불가.
unsigned int __stdcall ThreadMain(void* pArg)
{
    CThread_Manager* pThread_Manager = static_cast<CThread_Manager*>(pArg);

    pThread_Manager->DoWork();

    // return으로 끝내면 내부적으로 호출
    // 즉시 종료라 지역 객체들 소멸자 안 불린다고 함.
    // _endthreadex(0);

    return 0;
}

HRESULT CThread_Manager::Initialize()
{
    m_hEvent = CreateEvent(nullptr, false, false, nullptr);
    if (m_hEvent == 0)
    {
        MSG_BOX("CThread_Manager.cpp - Failed to Created: Event");
        return E_FAIL;
    }

    size_t iThreadCount = 4;
    m_vecThreads.reserve(iThreadCount);

    for (size_t i = 0; i < iThreadCount; i++)
    {
        HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,    // 보안속성(부모 프로세스 핸들의 상속 여부, nullptr인 경우 상속에서 제외)
                                                                 0,          // 스택 사이즈(디폴트 0:1MB)
                                                                 ThreadMain, // 쓰레드가 시작될 때 실행할 함수
                                                                 this,       // 쓰레드 함수에 전달할 매개변수 
                                                                 0,          // 생성 즉시 실행 여부(0이면 바로 실행)
                                                                 nullptr     // 생성된 쓰레드의 ID를 받을 변수 주소  
                                                                 ));
        if (hThread == 0)
        {
            MSG_BOX("CThread_Manager.cpp - Failed to Created: Thread");
            return E_FAIL;
        }

        m_vecThreads.emplace_back(hThread);    
    }

    return S_OK;
}

void CThread_Manager::DoWork()
{
    while (true) {

        WaitForSingleObject(m_hEvent, INFINITE);

        if (m_bExit.load(memory_order_relaxed) == true) {
            SetEvent(m_hEvent);
            break;
        }

        JOB tJob = {};
        while (m_JobQueue.try_pop(tJob)) {
            if (tJob.work != nullptr) {
                tJob.work();
            }
        }
    }
}

void CThread_Manager::Add_Job(function<void()> func)
{
    if (func == nullptr)
    {
        return;
    }

    JOB tJob = {};
    tJob.work = func;

    m_JobQueue.push(tJob);

    SetEvent(m_hEvent);
}

CThread_Manager* CThread_Manager::Create()
{
    CThread_Manager* pInstance = new CThread_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CThread_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThread_Manager::Free()
{
    __super::Free();

    m_bExit.store(true, std::memory_order_relaxed);

    SetEvent(m_hEvent);
    
    if (!m_vecThreads.empty())
    {
        WaitForMultipleObjects((DWORD)m_vecThreads.size(), m_vecThreads.data(), TRUE, INFINITE);
    }

    for (auto& hThread : m_vecThreads) {
        if (hThread) CloseHandle(hThread);
    }
    m_vecThreads.clear();

    if (m_hEvent) 
    {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }
}