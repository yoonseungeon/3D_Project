#include "CLoader.h"
#include <process.h>

#include "CGameInstance.h"
#include "CCharData_Manager.h"
#include "CCamera_Free.h"
#include "CUI_AniImage.h"
#include "CLobbyTabBtn.h"
#include "CUI_PickPanel.h"
#include "CPickSlot.h"
#include "CUI_SkinPanel.h"
#include "CPickSkin.h"
#include "CLobbySelectBtn.h"
#include "CUI_MapPanel.h"
#include "CMapSelectBtn.h"

#include "CMap_Lumia.h"

#include "CMonster.h"
#include "CForkLift.h"
#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"

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
    m_pCharData_Manager = CCharData_Manager::GetInstance();
    Safe_AddRef(m_pCharData_Manager);

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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Component_VIBuffer_Rect");
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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Component_Shader_VtxTex");
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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Texture_Simbol");
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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Texture_Loading_Line");
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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Texture_Img_Loading");
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
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_GameObject_CUI_Image");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_AniImage */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_AniImage"),
                CUI_AniImage::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_GameObject_CUI_AniImage");
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

#pragma region 텍스처
    /* Prototype_Texture_LobbyAni */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_LobbyAni"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/FHD_DDS/LobbyAni%d.dds"), 961))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LobbyAni");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LobbyTabBtnOrange */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_LobbyTabBtnOrange"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyTabBtnOrange.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LobbyTabBtnOrange");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LobbyTabBtnBlue */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_LobbyTabBtnBlue"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyTabBtnBlue.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LobbyTabBtnBlue");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LobbyShadow_Deco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_LobbyShadow_Deco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyShadow_Deco.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LobbyShadow_Deco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Lobby_GameStartBtn_Deco1 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_Lobby_GameStartBtn_Deco1"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyGameStartBtn_Deco1.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Lobby_GameStartBtn_Deco1");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Lobby_GameStartBtn_Deco2 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_Lobby_GameStartBtn_Deco2"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyGameStartBtn_Deco2.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Lobby_GameStartBtn_Deco2");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Lobby_GameStartBtn_Deco3 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_Lobby_GameStartBtn_Deco3"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyGameStartBtn_Deco3.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Lobby_GameStartBtn_Deco3");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Lobby_GameStartBtn_Deco4 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_Lobby_GameStartBtn_Deco4"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Lobby/LobbyGameStartBtn_Deco4.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Lobby_GameStartBtn_Deco4");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Select_BG */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_Select_BG"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/SelectBg.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Select_BG");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_PickPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_PickPanel"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/PickPanel.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_PickPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_CharPickSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_CharPickSlot"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/CharPickSlot%d.png"), 2))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_CharPickSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_StartegyFrameDeco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_StartegyFrameDeco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/StartegyFrameDeco.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_StartegyFrameDeco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_StartegyTitleDeco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_StartegyTitleDeco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/StartegyTitleDeco.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_StartegyTitleDeco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_SkinSlotFrame */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_SkinSlotFrame"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/SkinSlotFrame%d.png"), 2))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_SkinSlotFrame");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );


#pragma region CharLobbyPick
    /* CharLobbyPick */
    _tchar szPickTexPathDefault[MAX_PATH] = TEXT("../Bin/Resources/Lobby/Select/CharPick/CharLobbyPickImg%d.png");
    _tchar szPickTexPath[MAX_PATH] = TEXT("");

    for (_uint i = 0; i < ETOUI(CHAR_NAME::CHARNAME_END); ++i)
    {
        wsprintf(szPickTexPath, szPickTexPathDefault, i);

        const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(static_cast<CHAR_NAME>(i));
        const wstring wstrPickTag = pCharInfo->wstrPickTag;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, i, szPickTexPath, wstrPickTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), wstrPickTag,
                    CTexture::Create(m_pDevice, m_pContext, szPickTexPath, 1))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: CharLobby");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }
#pragma endregion

#pragma region PickSkin
    wstring wstrPickSkinTexPathDefault = L"../Bin/Resources/Lobby/Select/CharPickSkin/";

    for (_uint i = 0; i < ETOUI(CHAR_NAME::CHARNAME_END); ++i)
    {
        const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(static_cast<CHAR_NAME>(i));
        const wstring SkinTexTag = pCharInfo->wstrSkinTag;        
        const _uint iSkinCnt = static_cast<_uint>(pCharInfo->Skins.size());

        const wstring wstrFinalPath = wstrPickSkinTexPathDefault + pCharInfo->wstrSkinPath;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, SkinTexTag, iSkinCnt]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), SkinTexTag,
                    CTexture::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), iSkinCnt))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_CharPickSkin");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );          
    }
#pragma endregion

#pragma region FullSkin
    wstring wstrFullSkinTexPathDefault = L"../Bin/Resources/Lobby/Select/FullSkin/";

    for (_uint i = 0; i < ETOUI(CHAR_NAME::CHARNAME_END); ++i)
    {
        const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(static_cast<CHAR_NAME>(i));
        const wstring FullTexTag = pCharInfo->wstrFullSkinTag;
        const _uint iSkinCnt = static_cast<_uint>(pCharInfo->Skins.size());

        const wstring wstrFinalPath = wstrFullSkinTexPathDefault + pCharInfo->wstrFullSkinPath;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, FullTexTag, iSkinCnt]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), FullTexTag,
                    CTexture::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), iSkinCnt))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_FullSkin");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }
#pragma endregion

#pragma region Map Over
    wstring wstrMapOverPathDefault = L"../Bin/Resources/Lobby/Map/Over/";

    for (_uint i = 0; i < ETOUI(MAP_NAME::MAP_END); ++i)
    {
        const wstring wstrFinalPath = wstrMapOverPathDefault + MAPS[i].TEX_PATH_OVER;
        const wstring wstrMapOverTexTag = MAPS[i].TEX_OVER_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, wstrMapOverTexTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), wstrMapOverTexTag,
                    CTexture::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), 1))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Map_Over");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }
#pragma endregion

#pragma region Map Select
    wstring wstrMapSelectPathDefault = L"../Bin/Resources/Lobby/Map/Select/";

    for (_uint i = 0; i < ETOUI(MAP_NAME::MAP_END); ++i)
    {
        const wstring wstrFinalPath = wstrMapSelectPathDefault + MAPS[i].TEX_PATH_SELECT;
        const wstring wstrMapSelectTexTag = MAPS[i].TEX_SELECT_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, wstrMapSelectTexTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), wstrMapSelectTexTag,
                    CTexture::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), 1))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_Map_Select");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }
#pragma endregion


    /* Prototype_Texture_NonFullSkin */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_NonFullSkin"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/FullSkin/NonFullSkin.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_NonFullSkin");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_SkinSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_SkinSlot"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/SkinSlot%d.png"), 4))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_SkinSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LobbySelectBtn */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_LobbySelectBtn"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/LobbySelectBtn%d.png"), 2))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LobbySelectBtn");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_MapDeco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_MapDeco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Map/MapDeco.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_MapDeco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_MapGrid */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_MapGrid"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Map/MapGrid.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_MapGrid");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_MapPatternBg */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_MapPatternBg"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Map/MapPatternBg.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_MapPatternBg");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_MapPanel*/
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_MapPanel"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Map/MapPanel.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_MapPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 객체 원형
    /* Prototype_GameObject_CLobbyTabBtn */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
                CLobbyTabBtn::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_CLobbyTabBtn");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_PickPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickPanel"),
                CUI_PickPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_PickPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_PickSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickSlot"),
                CPickSlot::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_PickSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_SkinPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_SkinPanel"),
                CUI_SkinPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_SkinPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_PickSkin */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickSkin"),
                CPickSkin::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_PickSkin");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_LobbySelectBtn */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_LobbySelectBtn"),
                CLobbySelectBtn::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_LobbySelectBtn");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_MapPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_MapPanel"),
                CUI_MapPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_MapPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_MapSelectBtn */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_MapSelectBtn"),
                CMapSelectBtn::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_GameObject_MapSelectBtn");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 이미지
    /* Prototype_Image_MapOver */

    wstring wstrMapOverImagePath = L"../Bin/Resources/Lobby/Map/Over/";

    for (_uint i = 0; i < ETOUI(MAP_NAME::MAP_END); ++i)
    {
        const wstring wstrFinalPath = wstrMapOverImagePath + MAPS[i].TEX_PATH_OVER;
        const wstring wstrMapOverImageTag = MAPS[i].IMAGE_OVER_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, wstrMapOverImageTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), wstrMapOverImageTag,
                    CImage::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), 1))))
                {
                    MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Image_MapOver");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }
#pragma endregion

    m_bIsAllJobsQueued.store(true, memory_order_release);
    return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{
#pragma region 수업 코드
    /* Prototype_Component_Shader_VtxMesh */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Shader_VtxMesh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Shader_VtxAnimMesh */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Shader_VtxAnimMesh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Monster */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
                CMonster::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Monster");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );


    _matrix PreTransformMatrix = {};

    /* Prototype_Component_Model_Fiona */
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, PreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.mymodel", PreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Fiona");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_ForkLift */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
                CForkLift::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_ForkLift");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );


    /* Prototype_Component_Model_ForkLift */
    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, PreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.mymodel", PreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_ForkLift");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Player */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
                CPlayer::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Player");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Player */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
                CBody_Player::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Player");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Weapon */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
                CWeapon::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Weapon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 모델
    /* Prototype_Component_Model_Map_Lumia */
    _matrix MapPreTransformMatrix = XMMatrixIdentity();

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, MapPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Map_Lumia"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Map_Lumia_PNG/Map_Lumia_Final.mymodel", MapPreTransformMatrix, true))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Map_Lumia");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 객체 원형
    /* Prototype_GameObject_Camera_Free */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
                CCamera_Free::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Camera_Free");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Map_Lumia */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Lumia"),
                CMap_Lumia::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Map_Lumia");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

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

    Safe_Release(m_pCharData_Manager);

    __super::Free();
}
