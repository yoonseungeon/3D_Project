#include "CLoader.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

// 멤버 함수 아님. 전역 함수임. 멤버 변수 직접 접근 불가.
unsigned int __stdcall ThreadMain(void* pArg)
{
    CLoader* pLoader = static_cast<CLoader*>(pArg);

    if (FAILED(pLoader->Loading()))
        return -1;

    // return으로 끝내면 내부적으로 호출
    // 즉시 종료라 지역 객체들 소멸자 안 불린다고 함.
    // _endthreadex(0);

    return 0;

}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
    /* eNextLevelID에 필요한 자원을 로딩하는 작업을 수행한다. 누가? 스레드가 */
    // int -> void*라 reinterpret_cast 써야 함.
    m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,    // 보안속성(부모 프로세스 핸들의 상속 여부, nullptr인 경우 상속에서 제외)
                                                        0,          // 스택 사이즈(디폴트 0:1MB)
                                                        ThreadMain, // 쓰레드가 시작될 때 실행할 함수
                                                        this,       // 쓰레드 함수에 전달할 매개변수 
                                                        0,          // 생성 즉시 실행 여부(0이면 바로 실행)
                                                        nullptr     // 생성된 쓰레드의 ID를 받을 변수 주소  
                                                    ));
    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Loading()
{
    return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
    CLoader* pInstance = new CLoader(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created: CLoader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoader::Free()
{
    __super::Free();

    // 메인 쓰레드가 m_hThread 끝날 때까지 대기
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
