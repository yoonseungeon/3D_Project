#include "CLoader.h"

#include "CGameInstance.h"
#include "CUI_Image.h"

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
    case LEVEL::LOBBY:
        hr = Ready_Resources_For_Lobby();
        break;
    case LEVEL::GAMEPLAY:
        hr = Ready_Resources_For_GamePlay();
        break;
    case LEVEL::ENDING:
        hr = Ready_Resources_For_Ending();
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

HRESULT CLoader::Ready_Resources_For_Static()
{
#pragma region 버퍼
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
#pragma endregion

#pragma region 셰이더
    /* Prototype_Component_Shader_VtxTex */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {


            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Component_Shader_VtxTex");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 텍스처
    /* Prototype_Texture_Simbol */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Texture_Simbol"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Loading/Simbol.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Simbol");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Loading_Line */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Texture_Loading_Line"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Loading/Loading_Line.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Loading_Line");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_Loading */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Texture_Img_Loading"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Loading/Img_Loading.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Img_Loading");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 객체 원형
    /* Prototype_GameObject_CUI_Image */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                CUI_Image::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_GameObject_CUI_Image");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Logo()
{
    if(FAILED(Ready_Resources_For_Static()))
        return E_FAIL;

#pragma region 텍스처
    /* Prototype_Texture_Logo */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Texture_Logo"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Logo.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Logo");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Fifteen */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Texture_Fifteen"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Fifteen.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Fifteen");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Sexuality */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Texture_Sexuality"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Sexuality.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Sexuality");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Violence */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Texture_Violence"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Violence.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_Violence");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_NimbleNeuron */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), TEXT("Prototype_Texture_NimbleNeuron"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/NimbleNeuron.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Logo) - Failed to Created: Prototype_Texture_NimbleNeuron");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

    // m_iTotalJobCnt 개수 보장
    m_bIsAllJobsQueued.store(true, memory_order_release);

    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Lobby()
{
    Sleep(1000);

    m_bIsAllJobsQueued.store(true, memory_order_release);
    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{
    Sleep(1000);

    m_bIsAllJobsQueued.store(true, memory_order_release);
    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Ending()
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
