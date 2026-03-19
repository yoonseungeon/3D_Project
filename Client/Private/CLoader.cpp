#include "CLoader.h"

#include "CGameInstance.h"
#include "CBackGround.h"

#include <process.h>

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
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
    m_eNextLevelID = eNextLevelID;

    /* eNextLevelID에 필요한 자원을 로딩하는 작업을 수행한다. 누가? 스레드가 */
    // int -> void*라 reinterpret_cast 써야 함.
    m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,    // 보안속성(부모 프로세스 핸들의 상속 여부, nullptr인 경우 상속에서 제외)
                                                        0,          // 스택 사이즈(디폴트 0:1MB)
                                                        ThreadMain, // 쓰레드가 시작될 때 실행할 함수
                                                        this,       // 쓰레드 함수에 전달할 매개변수 
                                                        0,          // 생성 즉시 실행 여부(0이면 바로 실행)
                                                        nullptr     // 생성된 쓰레드의 ID를 받을 변수 주소  
                                                    ));
    if (m_hThread == 0)
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Loading()
{
    HRESULT hr = {};

    switch (m_eNextLevelID)
    {
    case LEVEL::LOGO:
        hr = Ready_Resources_For_Logo();
        break;
    case LEVEL::GAMEPLAY:
        hr = Ready_Resources_For_GamePlay();
        break;
    }

    if (FAILED(hr))
        return E_FAIL;

    return S_OK;
}

_bool CLoader::isFinished()
{
    if (m_bIsAllJobsQueued.load(memory_order_acquire) && (m_iTotalJobCnt.load(memory_order_relaxed) == m_iFinishedJobCnt.load(memory_order_relaxed))) {
        return true;
    }
    return false;
}

#ifdef _DEBUG
void CLoader::Show_Loading_Status()
{
    _tchar szLoadingText[MAX_PATH] = {};

    if(m_bIsAllJobsQueued.load(memory_order_acquire))
    {
        _int iTotalJobCnt = static_cast<_int>(m_iTotalJobCnt.load(memory_order_relaxed));
        _int iFinishedJobCnt = static_cast<_int>(m_iFinishedJobCnt.load(memory_order_relaxed));

        if(iTotalJobCnt != 0)
        {
            _float fProgress = static_cast<_float>(iFinishedJobCnt) / static_cast<_float>(iTotalJobCnt) * 100.f;
            swprintf_s(szLoadingText, L"%.f%%", fProgress);
        }
        else
        {
            swprintf_s(szLoadingText, L"%.f%%", 100.f);
        }
    }
    else
    {
        swprintf_s(szLoadingText, L"로딩 준비 중");
    }

    SetWindowText(g_hWnd, szLoadingText);
}
#endif

HRESULT CLoader::Ready_Resources_For_Logo()
{
#pragma region Static
    /* Prototype_Component_VIBuffer_Rect */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
                CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Component_VIBuffer_Rect");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Shader_VtxTex */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            // 셰이더에 넘길 정점 입력 형식 정보 배열(구조체 하나가 정점 구조체 안에 선언된 멤버 변수 하나 묘사)
            // 1. 문자열(시맨틱) 2. 시맨틱 idx(같은 시맨틱 여러 개면) 3. 포맷(float3 개, float 2개)
            // 4. IA에 바인딩된 정점 버퍼 중 몇 번째에서 꺼내와야 되는지 5. 구조체 멤버 변수 사작되는 byte offset
            // 6. VERTEX_DATA(일반적인 정점) or INSTANCE_DATA 7. INSTANCE_DATA인 StepRate 설정
            D3D11_INPUT_ELEMENT_DESC        Elements[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
            };

            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), Elements, 2))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Component_Shader_VtxTex");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 텍스처
    /* Prototype_Component_Texture_BackGround */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Component_Texture_BackGround Prototype");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 객체 원형
/* Prototype_GameObject_BackGround */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
                CBackGround::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_GameObject_BackGround");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

    // m_iTotalJobCnt 개수 보장
    m_bIsAllJobsQueued.store(true, memory_order_release);

    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{



    m_bIsAllJobsQueued.store(true, memory_order_release);
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
    // 메인 쓰레드가 m_hThread 끝날 때까지 대기
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);

    Safe_Release(m_pGameInstance);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
