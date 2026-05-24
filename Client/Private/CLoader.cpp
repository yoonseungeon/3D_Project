#include "CLoader.h"
#include <process.h>

#include "CGameInstance.h"
#include "CCharData_Manager.h"
#include "CCamera_Free.h"
#include "CUI_AniImage.h"
#include "CLobbyTabBtn.h"
#include "CUI_MaskImage.h"
#include "CUI_PickPanel.h"
#include "CPickSlot.h"
#include "CUI_SkinPanel.h"
#include "CPickSkin.h"
#include "CLobbySelectBtn.h"
#include "CUI_MapPanel.h"
#include "CMapSelectBtn.h"
#include "CUI_TextBox.h"
#include "CInGameHPBar.h"

#include "CLumia_NavMesh.h"
#include "CSplitGround.h"
#include "CLumia_Structure.h"
#include "CRoof.h"
#include "CRiver.h"

#include "CMonster.h"
#include "CForkLift.h"
#include "CSnow.h"
#include "CExplosion.h"
#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CBottle.h"
#include "CCraftTool.h"
#include "CCraftHammer.h"
#include "CBurner.h"
#include "CFryingPan.h"

#include "CInGameCharHUD_Empty.h"
// Level
#include "CInGameCharLevel_Empty.h"
#include "CUI_EXPGauge.h"
#include "CUI_LevelPanel.h"
#include "CUI_MainGaugePanel.h"
#include "CUI_MainGaugeBar.h"
#include "CUI_InGameCharProfile.h"
// Skill
#include "CUI_CharSkillPanel.h"
#include "CUI_StackSkillIcon.h"
#include "CUI_NormalSkillIcon.h"

#include "CSkillLevelUpBtn.h"
#include "CSkillCoolDisplay.h"
// Stat
#include "CUI_StatPanel.h"
#include "CUI_StatBox.h"
// Inven
#include "CUI_Inventory.h"
#include "CUI_InventorySlot.h"
#include "CUI_ItemImage.h"
#include "CUI_Craft.h"
#include "CUI_CraftSlot.h"
#include "CUI_InvenItemBg.h"
// Equipment
#include "CUI_Equipment.h"
#include "CUI_EquipmentSlot.h"
// ItemBox
#include "CItemBox.h"
#include "CItemBox_Collectible.h"
// ItemBox UI
#include "CUI_ItemBoxPanel.h"
#include "CUI_ItemBoxSlot.h"
// Monster
#include "CChicken.h"
#include "CBody_Chicken.h"
#include "CWolf.h"
#include "CBody_Wolf.h"
#include "CDog.h"
#include "CBody_Dog.h"
#include "CBear.h"
#include "CBody_Bear.h"
#include "CBat.h"
#include "CBody_Bat.h"
#include "CBoar.h"
#include "CBody_Boar.h"

#include "CAIFiora.h"
#include "CBody_Fiora.h"
#include "CRapier.h"

//Effect
#include "CTrailEffect.h"
#include "CNunchaku_AfterImage.h"
#include "CSlashEffect.h"
#include "CSpinEffect.h"
#include "CLiDailinSlash2.h"
#include "CShockWave_Q.h"
#include "CLava_Q.h"
#include "CSpinWind.h"
#include "CLiDailin_E_Range.h"
#include "CDragon_R.h"
#include "CLiDailin_W_Effect.h"

#include "CFiora_Q_1.h"
#include "CFiora_W_1.h"

// GameResult
#include "CUI_GameResult.h"
#include "CGameResultTextBox.h"
#include "CUI_Timer.h"
#include "CUI_Day.h"
#include "CUI_DayIcon.h"

// Vision Mask
#include "CVisionMask.h"

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

            wstring wstrLoading = L"Loading ... (" + to_wstring(static_cast<_uint>(fProgress)) + L"%)";
            m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
                wstrLoading.data(),
                _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.9f), XMVectorSet(1.f, 1.f, 1.f, 1.f),
                _float2(fDefaultFontSize, fDefaultFontSize)
            );
        }
        else
        {
            wstring wstrLoading = L"Loading ... (100%)";
            m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
                wstrLoading.data(),
                _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.9f), XMVectorSet(1.f, 1.f, 1.f, 1.f),
                _float2(fDefaultFontSize, fDefaultFontSize)
                );
        }
    }
    else
    {
        //m_pGameInstance->Draw_Text(TEXT("Font_Pretendard"), TEXT("로딩 준비 중"), _float2(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.9f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
    }

    //SetWindowText(g_hWnd, szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_Static()
{
#pragma region Font
    // .\MakeSpriteFont.exe "Pretendard Medium" / fontSize:10 / Sharp / fastPack / characterRegion : 0x0020 - 0x00FF / characterRegion : 0x3131 - 0x3163 / characterRegion : 0xAC00 - 0xD7A3 / defaultCharacter : 0xAC00 Pretendard_Middle_10.spritefont

    // Font_Pretendard_Middle
    if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Pretendard_Middle"), TEXT("../Bin/Resources/Fonts/Pretendard_Middle.spritefont"))))
    {
        MSG_BOX("CLoader.cpp(Static) - Failed to Created: Font_Pretendard_Middle");
    }

    // Font_Pretendard_SemiBold
    if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Pretendard_SemiBold"), TEXT("../Bin/Resources/Fonts/Pretendard_SemiBold.spritefont"))))
    {
        MSG_BOX("CLoader.cpp(Static) - Failed to Created: Font_Pretendard_SemiBold");
    }

    // Pretendard_Middle_100
    if (FAILED(m_pGameInstance->Add_Font(TEXT("Pretendard_Middle_100"), TEXT("../Bin/Resources/Fonts/Pretendard_Middle_100.spritefont"))))
    {
        MSG_BOX("CLoader.cpp(Static) - Failed to Created: Pretendard_Middle_100");
    }
#pragma endregion

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

    /* Prototype_GameObject_CUI_MaskImage */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_MaskImage"),
                CUI_MaskImage::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_GameObject_CUI_MaskImage");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_TextBox */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_TextBox"),
                CUI_TextBox::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_GameObject_CUI_TextBox");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region Move
    /* Prototype_Component_Move */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Move"),
                CMove::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Component_Move");
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
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Fifteen.dds"), 1))))
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
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Sexuality.dds"), 1))))
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
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Logo/Violence.dds"), 1))))
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
#pragma region 사운드
    // BGM_Lobby
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BGM_Lobby.wav", ETOUI(SOUND_KEY::BGM_LOBBY))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: BGM_Lobby");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // BGM_Select
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BGM_Select.wav", ETOUI(SOUND_KEY::BGM_SELECT))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: BGM_Select");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LobbyTab_Hover
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LobbyTab_Hover.wav", ETOUI(SOUND_KEY::LOBBY_TAB_HOVER))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: LobbyTab_Hover");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LobbyTab_Click
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LobbyTab_Click.wav", ETOUI(SOUND_KEY::LOBBY_TAB_CLICK))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: LobbyTab_Click");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Char_Hover
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Char_Hover.wav", ETOUI(SOUND_KEY::CHAR_HOVER))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: Char_Hover");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Char_Click
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Char_Click.wav", ETOUI(SOUND_KEY::CHAR_CLICK))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: Char_Click");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Select_Click
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Select_Click.wav", ETOUI(SOUND_KEY::SELECT_CLICK))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: Select_Click");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // GameStart_Click
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/GameStart_Click.wav", ETOUI(SOUND_KEY::GAMESTART_CLICK))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: GameStart_Click");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_selected
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_selected.wav", ETOUI(SOUND_KEY::LIDAILIN_SELECT))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: LiDailin_selected");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Hyunwoo_selected
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Hyunwoo_selected.wav", ETOUI(SOUND_KEY::HYUNWOO_SELECT))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Added: Hyunwoo_selected");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

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

    /* Prototype_Texture_BlackBlock */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_BlackBlock"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Map/Blackblock.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_BlackBlock");
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

    /* Prototype_Texture_SkinSlotMask*/
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOBBY), TEXT("Prototype_Texture_SkinSlotMask"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Lobby/Select/SkinSlotMask.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_SkinSlotMask");
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
#pragma region 사운드
    // Dailin_Normal_Nunchaku_Hit
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Dailin_Normal_Nunchaku_Hit.wav", ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Dailin_Normal_Nunchaku_Hit");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Dailin_Normal_Nunchaku_Hit_P
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Dailin_Normal_Nunchaku_Hit_P.wav", ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_P))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Dailin_Normal_Nunchaku_Hit_P");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/attackNunchaku_r1.wav", ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_WIND_1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: attackNunchaku_r1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/attackNunchaku_r3.wav", ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_WIND_2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: attackNunchaku_r3");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Lidailin_Skill01_Attack_Nunchaku
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill01_Attack0_Nunchaku.wav", ETOUI(SOUND_KEY::LIDAILIN_Q1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill01_Attack0_Nunchaku");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill01_Attack1_Nunchaku.wav", ETOUI(SOUND_KEY::LIDAILIN_Q2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill01_Attack1_Nunchaku");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill01_Attack2_Nunchaku.wav", ETOUI(SOUND_KEY::LIDAILIN_Q3))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill01_Attack2_Nunchaku");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill01_Hit.wav", ETOUI(SOUND_KEY::LIDAILIN_Q_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill01_Hit");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Lidailin_Skill02_Active
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill02_Active.wav", ETOUI(SOUND_KEY::LIDAILIN_W))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill02_Active");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Lidailin_Skill03
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill03_Attack.wav", ETOUI(SOUND_KEY::LIDAILIN_E))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill03_Attack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill03_Attack_p.wav", ETOUI(SOUND_KEY::LIDAILIN_E_P))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill03_Attack_p");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_Skill03_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_E_VOICE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: LiDailin_Skill03_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill03_Hit.wav", ETOUI(SOUND_KEY::LIDAILIN_E_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill03_Hit");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Lidailin_Skill04
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill04_Dash.wav", ETOUI(SOUND_KEY::LIDAILIN_R_DASH))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill04_Dash");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Lidailin_Skill04_Hit_Base.wav", ETOUI(SOUND_KEY::LIDAILIN_R_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: Lidailin_Skill04_Hit_Base");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // AreaBGM
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Alley.wav", ETOUI(SOUND_KEY::BGM_ALLEY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Alley");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Archery.wav", ETOUI(SOUND_KEY::BGM_ARCHERY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Archery");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Cemetery.wav", ETOUI(SOUND_KEY::BGM_CEMETERY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Cemetery");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Church.wav", ETOUI(SOUND_KEY::BGM_CHURCH))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Church");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Downtown.wav", ETOUI(SOUND_KEY::BGM_DOWNTOWN))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Downtown");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Factory.wav", ETOUI(SOUND_KEY::BGM_FACTORY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Factory");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Forest.wav", ETOUI(SOUND_KEY::BGM_FOREST))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Forest");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Harbor.wav", ETOUI(SOUND_KEY::BGM_HARBOR))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Harbor");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Hospital.wav", ETOUI(SOUND_KEY::BGM_HOSPITAL))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Hospital");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Hotel.wav", ETOUI(SOUND_KEY::BGM_HOTEL))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Hotel");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Laboratory.wav", ETOUI(SOUND_KEY::BGM_LABORATORY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Laboratory");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Pond.wav", ETOUI(SOUND_KEY::BGM_POND))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Pond");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_SandyBeach.wav", ETOUI(SOUND_KEY::BGM_SANDY_BEACH))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_SandyBeach");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_School.wav", ETOUI(SOUND_KEY::BGM_SCHOOL))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_School");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Temple.wav", ETOUI(SOUND_KEY::BGM_TEMPLE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Temple");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/BSER_AreaBGM_Uptown.wav", ETOUI(SOUND_KEY::BGM_UPTOWN))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Added: BSER_AreaBGM_Uptown");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Bat_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/batAttack.wav", ETOUI(SOUND_KEY::BAT_ATTACK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: batAttack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/batDie.wav", ETOUI(SOUND_KEY::BAT_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: batDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/batHit.wav", ETOUI(SOUND_KEY::BAT_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: batHit");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Bear_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/bearAttack1.wav", ETOUI(SOUND_KEY::BEAR_ATTACK1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: bearAttack1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/bearAttack2.wav", ETOUI(SOUND_KEY::BEAR_ATTACK2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: bearAttack2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/bearDie.wav", ETOUI(SOUND_KEY::BEAR_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: bearDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/bearHit.wav", ETOUI(SOUND_KEY::BEAR_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: bearHit");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/bearWakeUp.wav", ETOUI(SOUND_KEY::BEAR_WAKEUP))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: bearWakeUp");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Boar_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/boar_attack.wav", ETOUI(SOUND_KEY::BOAR_ATTACK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: boar_attack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/boarDie.wav", ETOUI(SOUND_KEY::BOAR_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: boarDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/boarHit.wav", ETOUI(SOUND_KEY::BOAR_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: boarHit");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Chicken_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/chickenAttack.wav", ETOUI(SOUND_KEY::CHICKEN_ATTACK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: chickenAttack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/chickenDie.wav", ETOUI(SOUND_KEY::CHICKEN_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: chickenDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/chickenHit.wav", ETOUI(SOUND_KEY::CHICKEN_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: chickenHit");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/chickenWakeUp.wav", ETOUI(SOUND_KEY::CHICKEN_WAKEUP))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: chickenWakeUp");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // WildDog_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wildDogAttack.wav", ETOUI(SOUND_KEY::WILDDOG_ATTACK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wildDogAttack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wildDogDie.wav", ETOUI(SOUND_KEY::WILDDOG_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wildDogDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wildDogHit.wav", ETOUI(SOUND_KEY::WILDDOG_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wildDogHit");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wildDogWakeUp.wav", ETOUI(SOUND_KEY::WILDDOG_WAKEUP))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wildDogWakeUp");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    ); 

    // Wolf_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wolfAttack.wav", ETOUI(SOUND_KEY::WOLF_ATTACK))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wolfAttack");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wolfDie.wav", ETOUI(SOUND_KEY::WOLF_DIE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wolfDie");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wolfHit.wav", ETOUI(SOUND_KEY::WOLF_HIT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wolfHit");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Monster/wolfWakeUp.wav", ETOUI(SOUND_KEY::WOLF_WAKEUP))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: wolfWakeUp");
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Weather
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird1.wav", ETOUI(SOUND_KEY::DAY_BIRD1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird2.wav", ETOUI(SOUND_KEY::DAY_BIRD2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird3.wav", ETOUI(SOUND_KEY::DAY_BIRD3))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird3");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird4.wav", ETOUI(SOUND_KEY::DAY_BIRD4))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird4");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird5.wav", ETOUI(SOUND_KEY::DAY_BIRD5))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird5");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Day_Bird6.wav", ETOUI(SOUND_KEY::DAY_BIRD6))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Day_Bird6");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Night_Bird1.wav", ETOUI(SOUND_KEY::NIGHT_BIRD1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Night_Bird1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Night_Bird2.wav", ETOUI(SOUND_KEY::NIGHT_BIRD2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Night_Bird2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Night_Bird3.wav", ETOUI(SOUND_KEY::NIGHT_BIRD3))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Night_Bird3");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Night_Bird4.wav", ETOUI(SOUND_KEY::NIGHT_BIRD4))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Night_Bird4");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Night_Bird5.wav", ETOUI(SOUND_KEY::NIGHT_BIRD5))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Night_Bird5");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Weather/Sunny.wav", ETOUI(SOUND_KEY::SUNNY))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Sunny");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Action_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Action/Collect.wav", ETOUI(SOUND_KEY::COLLECT))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Collect");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Action/Collect_branch.wav", ETOUI(SOUND_KEY::COLLECT_BRANCH))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Collect_branch");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Action/Collect_stone.wav", ETOUI(SOUND_KEY::COLLECT_STONE))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Collect_stone");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Action/Craft_Food.wav", ETOUI(SOUND_KEY::CRAFT_FOOD))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Craft_Food");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Action/Craft_Tool.wav", ETOUI(SOUND_KEY::CRAFT_TOOL))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: Craft_Tool");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_MoveIn_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            // Alley
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInAlley_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINALLEY_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInAlley_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInAlley_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINALLEY_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInAlley_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInAlley_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINALLEY_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInAlley_3");

            // Archery
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInArchery_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINARCHERY_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInArchery_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInArchery_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINARCHERY_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInArchery_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInArchery_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINARCHERY_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInArchery_3");

            // Cemetery
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInCemetery_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCEMETERY_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInCemetery_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInCemetery_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCEMETERY_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInCemetery_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInCemetery_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCEMETERY_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInCemetery_3");

            // Church
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInChurch_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCHURCH_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInChurch_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInChurch_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCHURCH_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInChurch_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInChurch_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINCHURCH_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInChurch_3");

            // Factory
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFactory_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFACTORY_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFactory_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFactory_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFACTORY_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFactory_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFactory_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFACTORY_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFactory_3");

            // FireStation
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFireStation_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFIRESTATION_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFireStation_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFireStation_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFIRESTATION_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFireStation_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInFireStation_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFIRESTATION_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInFireStation_3");

            // Forest
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInForest_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFOREST_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInForest_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInForest_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFOREST_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInForest_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInForest_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINFOREST_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInForest_3");

            // GasStation
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInGasStation_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINGASSTATION_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInGasStation_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInGasStation_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINGASSTATION_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInGasStation_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInGasStation_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINGASSTATION_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInGasStation_3");

            // Harbor
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHarbor_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHARBOR_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHarbor_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHarbor_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHARBOR_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHarbor_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHarbor_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHARBOR_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHarbor_3");

            // Hospital
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHospital_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOSPITAL_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHospital_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHospital_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOSPITAL_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHospital_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHospital_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOSPITAL_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHospital_3");

            // Hotel
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHotel_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOTEL_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHotel_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHotel_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOTEL_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHotel_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInHotel_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINHOTEL_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInHotel_3");

            // Laboratory
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInLaboratoryOutside_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINLABORATORY_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInLaboratory_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInLaboratoryOutside_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINLABORATORY_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInLaboratory_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInLaboratoryOutside_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINLABORATORY_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInLaboratory_3");

            // PoliceStation
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPoliceStation_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOLICESTATION_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPoliceStation_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPoliceStation_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOLICESTATION_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPoliceStation_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPoliceStation_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOLICESTATION_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPoliceStation_3");

            // Pond
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPond_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOND_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPond_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPond_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOND_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPond_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInPond_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINPOND_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInPond_3");

            // SandyBeach
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSandyBeach_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSANDYBEACH_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSandyBeach_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSandyBeach_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSANDYBEACH_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSandyBeach_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSandyBeach_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSANDYBEACH_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSandyBeach_3");

            // School
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSchool_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSCHOOL_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSchool_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSchool_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSCHOOL_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSchool_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInSchool_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSCHOOL_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInSchool_3");

            // Stream
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInStream_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSTREAM_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInStream_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInStream_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSTREAM_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInStream_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInStream_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINSTREAM_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInStream_3");

            // Temple
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInTemple_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINTEMPLE_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInTemple_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInTemple_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINTEMPLE_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInTemple_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInTemple_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINTEMPLE_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInTemple_3");

            // Warehouse
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInWarehouse_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINWAREHOUSE_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInWarehouse_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInWarehouse_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINWAREHOUSE_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInWarehouse_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInWarehouse_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINWAREHOUSE_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInWarehouse_3");

            // Uptown
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInUptown_1_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINUPTOWN_1)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInUptown_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInUptown_2_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINUPTOWN_2)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInUptown_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_MoveArea/LiDailin_moveInUptown_3_ko.wav", ETOUI(SOUND_KEY::LIDAILIN_MOVEINUPTOWN_3)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: LiDailin_moveInUptown_3");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_SkillVoice_W_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_PlaySkill_Voice_W_1.wav", ETOUI(SOUND_KEY::LIDAILIN_W_VOICE_1))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_PlaySkill_Voice_W_1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_PlaySkill_Voice_W_2.wav", ETOUI(SOUND_KEY::LIDAILIN_W_VOICE_2))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_PlaySkill_Voice_W_2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_PlaySkill_Voice_W_3.wav", ETOUI(SOUND_KEY::LIDAILIN_W_VOICE_3))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_PlaySkill_Voice_W_3");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_CollectVoice_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_CollectBranch_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_COLLECTBRANCH_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_CollectBranch_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_CollectFlower_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_COLLECTFLOWER_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_CollectFlower_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_CollectStone_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_COLLECTSTONE_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_CollectStone_Voice");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_CraftVoice_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            // Uncommon
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftUncommon_1_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTUNCOMMON_1_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftUncommon_1_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftUncommon_2_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTUNCOMMON_2_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftUncommon_2_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftUncommon_3_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTUNCOMMON_3_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftUncommon_3_Voice");

            // Rare
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftRare_3_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTRARE_3_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftRare_3_Voice");

            // Epic
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftEpic_1_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTEPIC_1_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftEpic_1_Voice");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftEpic_2_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTEPIC_2_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftEpic_2_Voice");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_craftLegend_2_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_CRAFTLEGENDARY_2_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_craftLegend_2_Voice");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // LiDailin_MakeFoodVoice_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_makeFood_1_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_MAKEFOOD_1_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_makeFood_1_Voice");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/LiDailin_makeFood_3_Voice.wav", ETOUI(SOUND_KEY::LIDAILIN_MAKEFOOD_3_VOICE))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: LiDailin_makeFood_3_Voice");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // ItemSystem_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/ItemPickUp.wav", ETOUI(SOUND_KEY::ITEM_PICKUP))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: ItemPickUp");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/ItemFail.wav", ETOUI(SOUND_KEY::ITEM_FAIL))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: ItemFail");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/EquipmentIteml.wav", ETOUI(SOUND_KEY::EQUIP_ITEM))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: EquipmentIteml");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Effect_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/effect_levelup.wav", ETOUI(SOUND_KEY::EFFECT_LEVELUP))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: effect_levelup");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Skill_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/SkillUp.wav", ETOUI(SOUND_KEY::SKILL_UP))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: SkillUp");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Consume_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/Consume_Eat_3.wav", ETOUI(SOUND_KEY::CONSUME_EAT_3))))
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: Consume_Eat_3");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // OpenObject_Sound
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_AltarTable_01.wav", ETOUI(SOUND_KEY::OPENSOUND_ALTARTABLE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_AltarTable_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_ATM_01.wav", ETOUI(SOUND_KEY::OPENSOUND_ATM_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_ATM_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Bag_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BAG_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Bag_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_BarbequeGrill_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BARBEQUEGRILL_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_BarbequeGrill_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_BeachChair_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BEACHCHAIR_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_BeachChair_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Biotoilet_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BIOTOILET_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Biotoilet_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Boat_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BOAT_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Boat_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Box_01.wav", ETOUI(SOUND_KEY::OPENSOUND_BOX_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Box_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Box_02.wav", ETOUI(SOUND_KEY::OPENSOUND_BOX_02)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Box_02");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_CartonBox_01.wav", ETOUI(SOUND_KEY::OPENSOUND_CARTONBOX_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_CartonBox_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_CementMixer.wav", ETOUI(SOUND_KEY::OPENSOUND_CEMENTMIXER)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_CementMixer");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_CoffeeMachine_01.wav", ETOUI(SOUND_KEY::OPENSOUND_COFFEEMACHINE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_CoffeeMachine_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Coffin_01.wav", ETOUI(SOUND_KEY::OPENSOUND_COFFIN_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Coffin_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_ConcreteBag_Set_01.wav", ETOUI(SOUND_KEY::OPENSOUND_CONCRETEBAG_SET_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_ConcreteBag_Set_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Confessional_01.wav", ETOUI(SOUND_KEY::OPENSOUND_CONFESSIONAL_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Confessional_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Drum_01.wav", ETOUI(SOUND_KEY::OPENSOUND_DRUM_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Drum_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Dumpster_01.wav", ETOUI(SOUND_KEY::OPENSOUND_DUMPSTER_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Dumpster_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_EraserCleaner_01.wav", ETOUI(SOUND_KEY::OPENSOUND_ERASERCLEANER_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_EraserCleaner_01");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_FilingCabinets_01.wav", ETOUI(SOUND_KEY::OPENSOUND_FILINGCABINETS_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_FilingCabinets_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_GarbageBag_Set_01.wav", ETOUI(SOUND_KEY::OPENSOUND_GARBAGEBAG_SET_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_GarbageBag_Set_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Hospital_Cabinet_01.wav", ETOUI(SOUND_KEY::OPENSOUND_HOSPITAL_CABINET_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Hospital_Cabinet_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Hospital_Cart_01.wav", ETOUI(SOUND_KEY::OPENSOUND_HOSPITAL_CART_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Hospital_Cart_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_IceBox_01.wav", ETOUI(SOUND_KEY::OPENSOUND_ICEBOX_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_IceBox_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Jar_01.wav", ETOUI(SOUND_KEY::OPENSOUND_JAR_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Jar_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Jar_Big_01.wav", ETOUI(SOUND_KEY::OPENSOUND_JAR_BIG_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Jar_Big_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Locker_01.wav", ETOUI(SOUND_KEY::OPENSOUND_LOCKER_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Locker_01");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Organ_01.wav", ETOUI(SOUND_KEY::OPENSOUND_ORGAN_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Organ_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Pallet_Loaded_01.wav", ETOUI(SOUND_KEY::OPENSOUND_PALLET_LOADED_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Pallet_Loaded_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Sedan_Brown_01.wav", ETOUI(SOUND_KEY::OPENSOUND_SEDAN_BROWN_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Sedan_Brown_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Sedan_Police_01.wav", ETOUI(SOUND_KEY::OPENSOUND_SEDAN_POLICE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Sedan_Police_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Sedan_Taxi_01.wav", ETOUI(SOUND_KEY::OPENSOUND_SEDAN_TAXI_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Sedan_Taxi_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_SteelBox_02.wav", ETOUI(SOUND_KEY::OPENSOUND_STEELBOX_02)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_SteelBox_02");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_SteelBox_03.wav", ETOUI(SOUND_KEY::OPENSOUND_STEELBOX_03)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_SteelBox_03");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Suitcase_01.wav", ETOUI(SOUND_KEY::OPENSOUND_SUITCASE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Suitcase_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Switchboard_01.wav", ETOUI(SOUND_KEY::OPENSOUND_SWITCHBOARD_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Switchboard_01");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Temple_Box_01.wav", ETOUI(SOUND_KEY::OPENSOUND_TEMPLE_BOX_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Temple_Box_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Thurible_01.wav", ETOUI(SOUND_KEY::OPENSOUND_THURIBLE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Thurible_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Tomb_01.wav", ETOUI(SOUND_KEY::OPENSOUND_TOMB_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Tomb_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_TrashCan_01.wav", ETOUI(SOUND_KEY::OPENSOUND_TRASHCAN_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_TrashCan_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_TrashCan2_01.wav", ETOUI(SOUND_KEY::OPENSOUND_TRASHCAN2_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_TrashCan2_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_TreeStump_01.wav", ETOUI(SOUND_KEY::OPENSOUND_TREESTUMP_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_TreeStump_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_TreeStump_02.wav", ETOUI(SOUND_KEY::OPENSOUND_TREESTUMP_02)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_TreeStump_02");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Uptown_BookCase_01.wav", ETOUI(SOUND_KEY::OPENSOUND_UPTOWN_BOOKCASE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Uptown_BookCase_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Uptown_Drawer_01.wav", ETOUI(SOUND_KEY::OPENSOUND_UPTOWN_DRAWER_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Uptown_Drawer_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Uptown_FirePlace_01.wav", ETOUI(SOUND_KEY::OPENSOUND_UPTOWN_FIREPLACE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Uptown_FirePlace_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_VendingMachine_01.wav", ETOUI(SOUND_KEY::OPENSOUND_VENDINGMACHINE_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_VendingMachine_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_WaterTank_01.wav", ETOUI(SOUND_KEY::OPENSOUND_WATERTANK_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_WaterTank_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_Wheelbarrow_01.wav", ETOUI(SOUND_KEY::OPENSOUND_WHEELBARROW_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_Wheelbarrow_01");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/OpenSound_WreckCar_01.wav", ETOUI(SOUND_KEY::OPENSOUND_WRECKCAR_01)))) MSG_BOX("CLoader.cpp(Lobby) - Failed to Add: OpenSound_WreckCar_01");

            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/OpenSound/DeadOpen.wav", ETOUI(SOUND_KEY::DEAD_OPEN)))) MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: DeadOpen");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Footstep
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/FootStep/footstepAsphalt_w1.wav", ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W1))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: footstepAsphalt_w1");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/FootStep/footstepAsphalt_w2.wav", ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W2))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: footstepAsphalt_w2");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/FootStep/footstepAsphalt_w3.wav", ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W3))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: footstepAsphalt_w3");
            if (FAILED(m_pGameInstance->Add_Sound("../Bin/Resources/Sound/FootStep/footstepAsphalt_w4.wav", ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W4))))
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Add: footstepAsphalt_w4");

            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 콜라이더
    // 생성할 때 context 씀
    if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
        return E_FAIL;

    /* Prototype_Component_Collider_OBB */
    if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
        return E_FAIL;

    /* Prototype_Component_Collider_Sphere */
    if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
        return E_FAIL;

    /* Prototype_Component_Collider_Frustum */
    if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Frustum"),
        CCollider::Create(m_pDevice, m_pContext, COLLIDER::FRUSTUM))))
        return E_FAIL;
#pragma endregion

#pragma region 버퍼
    ///* Prototype_Component_VIBuffer_Trail */
    //m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    //m_pGameInstance->Add_Job(
    //    [this]()->void {
    //        if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Trail"),
    //            CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
    //        {
    //            MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_VIBuffer_Trail");
    //        }
    //        m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
    //    }
    //);
#pragma endregion

#pragma region 셰이더
    ///* Prototype_Component_Shader_VtxTexEffect */
    //m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    //m_pGameInstance->Add_Job(
    //    [this]()->void {
    //        if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxTexEffect"),
    //            CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexEffect.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
    //        {
    //            MSG_BOX("CLoader.cpp(Static) - Failed to Created: Prototype_Component_Shader_VtxTexEffect");
    //        }
    //        m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
    //    }
    //);

    /* Prototype_Component_Shader_VtxMeshEffect */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Shader_VtxMeshEffect");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 수업 코드
    /* Prototype_Component_Shader_VtxRectInstance */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxRectInstance"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECT_INSTANCE_DESC::Elements, VTXRECT_INSTANCE_DESC::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Shader_VtxMesh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Shader_VtxPointInstance */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxPointInstance"),
                CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_INSTANCE_DESC::Elements, VTXPOINT_INSTANCE_DESC::iNumElements))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Shader_VtxMesh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

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

    /* Prototype_Component_VIBuffer_Instance_Snow */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC SnowDesc{};
            SnowDesc.iNumInstance = 10;
            SnowDesc.vCenter = _float3(0.f, 0.f, 0.f);
            SnowDesc.vRange = _float3(10.f, 0.3f, 10.f);
            SnowDesc.vSize = _float2(1.0f, 1.f);
            SnowDesc.vSpeed = _float2(1.f, 3.f);
            SnowDesc.vLifeTime = _float2(4.f, 8.f);
            SnowDesc.isLoop = true;

            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Instance_Snow"),
                CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &SnowDesc))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_VIBuffer_Instance_Snow");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_VIBuffer_Instance_Explosion */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            CVIBuffer_Point_Instance::POINT_INSTANCE_DESC     ExploDesc{};
            ExploDesc.iNumInstance = 600;
            ExploDesc.vCenter = _float3(0.f, 0.f, 0.f);
            ExploDesc.vRange = _float3(0.3f, 0.3f, 0.3f);
            ExploDesc.vSize = _float2(0.1f, 0.2f);
            ExploDesc.vSpeed = _float2(1.f, 3.f);
            ExploDesc.vLifeTime = _float2(0.5f, 0.8f);
            ExploDesc.vPivot = _float3(0.f, -0.5f, 0.f);
            ExploDesc.isLoop = true;

            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Instance_Explosion"),
                CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &ExploDesc))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_VIBuffer_Instance_Snow");
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
                CLiDailin::Create(m_pDevice, m_pContext))))
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

    /* Prototype_GameObject_Snow */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
                CSnow::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Weapon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Snow */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
                CExplosion::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Weapon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 모델
    ///* Prototype_Component_Model_Lumia_Ground */
    _matrix MapPreTransformMatrix = XMMatrixIdentity();

    //m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    //m_pGameInstance->Add_Job(
    //    [this, MapPreTransformMatrix]()->void {
    //        if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Lumia_Ground"),
    //            CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Map_Lumia_PNG/Lumia_Ground2.mymodel", MapPreTransformMatrix, true))))
    //        {
    //            MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Lumia_Ground");
    //        }
    //        m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
    //    }
    //);

    /* Prototype_Component_Model_Lumia_Structure */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Lumia_Structure"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Map_Lumia_PNG/Lumia_Structure_NoRoof3_NoRiver.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Lumia_Structure");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    // Roof Model
    string strRoofPath = "../Bin/Resources/GamePlay/Map_Lumia_PNG/";

    for (_uint i = 0; i < iRoofCnt; ++i)
    {
        const string strFinalPath = strRoofPath + ROOFS[i].MODEL_PATH;
        const wstring wstrPrototypeTag = ROOFS[i].PROTYPE_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, strFinalPath, wstrPrototypeTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), wstrPrototypeTag,
                    CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, strFinalPath.c_str()))))
                {
                    MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Roof");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }

    // Ground Model
    string strGroundPath = "../Bin/Resources/GamePlay/Map_Lumia_PNG/";

    for (_uint i = 0; i < sizeof(GROUNDS) / sizeof(GROUNDS[0]); ++i)
    {
        const string strFinalPath = strGroundPath + GROUNDS[i].MODEL_PATH;
        const wstring wstrPrototypeTag = GROUNDS[i].PROTYPE_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);

        m_pGameInstance->Add_Job(
            [this, MapPreTransformMatrix, strFinalPath, wstrPrototypeTag]()->void
            {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), wstrPrototypeTag,
                    CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, strFinalPath.c_str(), MapPreTransformMatrix, true))))
                {
                    MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Ground");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }

    /* Prototype_Component_Model_River */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_River"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Map_Lumia_PNG/River.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_River");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_LiDailin */
    _matrix PlayerPreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.f));;

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, PlayerPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LiDailin"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/LiDailin/LiDailin_DDS.mymodel", PlayerPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_LiDailin");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Nunchaku */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Nunchaku"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Nunchaku/Weapon_Nunchaku_DDS.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Nunchaku");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Bottle */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bottle"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Bottle/Bottle003_DDS.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Bottle");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Hammer */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Hammer"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Craft/Craft_Hammer_DDS.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Hammer");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Craft_Tool */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Craft_Tool"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Craft/Craft_Tool_DDS.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Craft_Tool");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FryingPan */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FryingPan"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Craft_Food/FryingPan.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FryingPan");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Burner */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Burner"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Craft_Food/Craft_Burner.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Burner");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_ItemBox */
    string strItemBoxPath = "../Bin/Resources/GamePlay/ItemBox/";

    for (_uint i = 0; i < sizeof(ItemBoxMeta) / sizeof(ItemBoxMeta[0]); ++i)
    {
        const string strFinalPath = strItemBoxPath + ItemBoxMeta[i].MODEL_PATH;
        const wstring wstrPrototypeTag = ItemBoxMeta[i].PROTYPE_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, strFinalPath, wstrPrototypeTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), wstrPrototypeTag,
                    CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, strFinalPath.c_str()))))
                {
                    MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_ItemBox");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }

    /* Prototype_Component_Model_ItemBox_Collectible */
    string strItemBoxCollectiblePath = "../Bin/Resources/GamePlay/ItemBox_Collectible/";

    for (_uint i = 0; i < sizeof(ItemBoxCollectibleMeta) / sizeof(ItemBoxCollectibleMeta[0]); ++i)
    {
        const string strFinalPath = strItemBoxCollectiblePath + ItemBoxCollectibleMeta[i].MODEL_PATH;
        const wstring wstrPrototypeTag = ItemBoxCollectibleMeta[i].PROTYPE_TAG;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, strFinalPath, wstrPrototypeTag]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), wstrPrototypeTag,
                    CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, strFinalPath.c_str()))))
                {
                    MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_ItemBox_Collectible");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }

    /* Prototype_Component_Model_Chicken */
     _matrix ChickenPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, ChickenPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Chicken"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Chicken/Chicken.mymodel", ChickenPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Chicken");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Wolf */
    _matrix WolfPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, WolfPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Wolf"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Wolf/Wolf.mymodel", WolfPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Wolf");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Dog */
    _matrix DogPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(90.f));
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, DogPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Dog"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Dog/Dog.mymodel", DogPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Dog");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Bear */
    _matrix BearPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, BearPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bear"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Bear/Bear.mymodel", BearPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Bear");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Bat */
    _matrix BatPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationX(XMConvertToRadians(-90.f));
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, BatPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bat"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Bat/Bat.mymodel", BatPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Bat");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Boar */
    _matrix BoarPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, BoarPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Boar"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Monster/Boar/Boar.mymodel", BoarPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Boar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_Fiora */
    _matrix FioraPreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));;

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiora"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/GamePlay/Fiora/Fiora.mymodel", FioraPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Fiora");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FioraRapier */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FioraRapier"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/Rapier/FioraRapier.mymodel"))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FioraRapier");
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

    /* Prototype_GameObject_Lumia_NavMesh */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_NavMesh"),
                CLumia_NavMesh::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Lumia_NavMesh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Lumia_SplitGround */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_SplitGround"),
                CSplitGround::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Lumia_SplitGround");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Lumia_Structure */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_Structure"),
                CLumia_Structure::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Lumia_Structure");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Roof */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Roof"),
                CRoof::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Roof");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_River */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_River"),
                CRiver::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_River");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Bottle */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bottle"),
                CBottle::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Bottle");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CraftHammer */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CraftHammer"),
                CCraftHammer::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CraftHammer");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CraftTool */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CraftTool"),
                CCraftTool::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CraftTool");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Burner */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Burner"),
                CBurner::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Burner");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_FryingPan */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_FryingPan"),
                CFryingPan::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_FryingPan");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Chicken */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Chicken"),
                CChicken::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Chickenh");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Chicken */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Chicken"),
                CBody_Chicken::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Chicken");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Wolf */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Wolf"),
               CWolf::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Wolf");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Wolf */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Wolf"),
                CBody_Wolf::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Wolf");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Dog */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Dog"),
                CDog::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Dog");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Dog */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Dog"),
                CBody_Dog::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Dog");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Bear */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bear"),
                CBear::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Bear");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Bear */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Bear"),
                CBody_Bear::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Bear");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Bat */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bat"),
                CBat::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Bat");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Bat */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Bat"),
                CBody_Bat::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Bat");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Boar */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Boar"),
                CBoar::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Boar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Boar */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Boar"),
                CBody_Boar::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Boar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CInGameCharHUD_Empty */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CInGameCharHUD_Empty"),
                CInGameCharHUD_Empty::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CInGameCharHUD_Empty");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_StatPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StatPanel"),
                CUI_StatPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_StatPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_StatBox */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StatBox"),
                CUI_StatBox::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_StatBox");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_CharSkillPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_CharSkillPanel"),
                CUI_CharSkillPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_CharSkillPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CInGameCharLevel_Empty */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CInGameCharLevel_Empty"),
                CInGameCharLevel_Empty::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CInGameCharLevel_Empty");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_EXPGauge */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_EXPGauge"),
                CUI_EXPGauge::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_EXPGauge");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_LevelPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_LevelPanel"),
                CUI_LevelPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_LevelPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_InGameCharProfile */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InGameCharProfile"),
               CUI_InGameCharProfile::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_InGameCharProfile");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_StackSkillIcon */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StackSkillIcon"),
                CUI_StackSkillIcon::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_StackSkillIcon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_NormalSkillIcon */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_NormalSkillIcon"),
                CUI_NormalSkillIcon::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_NormalSkillIcon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_MainGaugePanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_MainGaugePanel"),
                CUI_MainGaugePanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_MainGaugePanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_MainGaugeBar */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_MainGaugeBar"),
                CUI_MainGaugeBar::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_MainGaugeBar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_Inventory */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Inventory"),
                CUI_Inventory::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_Inventory");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_InventorySlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InventorySlot"),
                CUI_InventorySlot::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_InventorySlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_InvenItemBg */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InvenItemBg"),
                CUI_InvenItemBg::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_InvenItemBg");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_ItemImage */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemImage"),
                CUI_ItemImage::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_ItemImage");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_Craft */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Craft"),
                CUI_Craft::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_Craft");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_CraftSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_CraftSlot"),
                CUI_CraftSlot::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_CraftSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_Equipment */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Equipment"),
                CUI_Equipment::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_Equipment");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_EquipmentSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_EquipmentSlot"),
                CUI_EquipmentSlot::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_EquipmentSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CItemBox */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CItemBox"),
               CItemBox::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CItemBox");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CItemBox_Collectible */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CItemBox_Collectible"),
                CItemBox_Collectible::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CItemBox_Collectible");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_ItemBoxPanel */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemBoxPanel"),
                CUI_ItemBoxPanel::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_ItemBoxPanel");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_ItemBoxSlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemBoxSlot"),
                CUI_ItemBoxSlot::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_ItemBoxSlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_AIFiora */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_AIFiora"),
                CAIFiora::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_AIFiora");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Body_Fiora */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Fiora"),
                CBody_Fiora::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Body_Fiora");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Rapier */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Rapier"),
                CRapier::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Rapier");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_InGameHPBar */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_InGameHPBar"),
                CInGameHPBar::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_InGameHPBar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_SkillLevelUpBtn */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SkillLevelUpBtn"),
                CSkillLevelUpBtn::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_SkillLevelUpBtn");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_SkillCoolDisplay */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SkillCoolDisplay"),
                CSkillCoolDisplay::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_SkillCoolDisplay");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_GameResult */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_GameResult"),
                CUI_GameResult::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_GameResult");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_GameResultTextBox */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_GameResultTextBox"),
                CGameResultTextBox::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_GameResultTextBox");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_Timer */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Timer"),
                CUI_Timer::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_Timer");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_Day */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Day"),
                CUI_Day::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_Day");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CUI_DayIcon */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_DayIcon"),
                CUI_DayIcon::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CUI_DayIcon");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_VisionMask */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_VisionMask"),
               CVisionMask::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_VisionMask");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region Navigation
    /* Prototype_Component_Navigation */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
                CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation3.dat"), TEXT("../Bin/DataFiles/Neighbors3.dat")))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Navigation");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 텍스처
    /* Prototype_Texture_InGameCharBg */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_InGameCharBg"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/InGameCharBg.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_InGameCharBg");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LiDailin_Q */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LiDailin_Q"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/LiDailin_Q.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LiDailin_Q");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LiDailin_W */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LiDailin_W"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/LiDailin_W.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LiDailin_W");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LiDailin_E */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LiDailin_E"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/LiDailin_E.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LiDailin_E");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LiDailin_R */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LiDailin_R"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/LiDailin_R.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_LiDailin_R");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_WhiteBlock */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_WhiteBlock"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/WhiteBlock.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_WhiteBlock");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_ProfileBg */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_ProfileBg"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/ProfileBg.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(Lobby) - Failed to Created: Prototype_Texture_ProfileBg");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_CharProfile */
    wstring wstrProFileTexPathDefault = L"../Bin/Resources/GamePlay/2D/";

    for (_uint i = 0; i < ETOUI(CHAR_NAME::CHARNAME_END); ++i)
    {
        const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(static_cast<CHAR_NAME>(i));
        const wstring ProfileTag = pCharInfo->wstrProfileTag;
        const _uint iSkinCnt = static_cast<_uint>(pCharInfo->Skins.size());

        const wstring wstrFinalPath = wstrProFileTexPathDefault + pCharInfo->wstrProfilePath;

        m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
        m_pGameInstance->Add_Job(
            [this, wstrFinalPath, ProfileTag, iSkinCnt]()->void {
                if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), ProfileTag,
                    CTexture::Create(m_pDevice, m_pContext, wstrFinalPath.c_str(), iSkinCnt))))
                {
                    MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_CharProfile");
                }
                m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
            }
        );
    }

    /* Prototype_Texture_CircleMask */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_CircleMask"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/CircleMask.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_CircleMask");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_IcoStat */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_IcoStat"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Ico_ChaStat%d.dds"), 8))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_IcoStat");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_Img_HudDecoRight */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_Img_HudDecoRight"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_HudDecoRight.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_Img_HudDecoRight");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_Img_HudDecoLeft */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_Img_HudDecoLeft"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_HudDecoLeft.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_Img_HudDecoLeft");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_LevelGauge */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_LevelGauge"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_LevelGage.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_LevelGauge");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LevelPanelDeco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LevelPanelDeco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/LevelPanelDeco.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_LevelPanelDeco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_MainGauge */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_MainGauge"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_MainGauge%d.dds"), 3))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_MainGauge");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Ico_Crafting */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Ico_Crafting"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Ico_Crafting.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Ico_Crafting");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Ico_ItemGradeBg */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Ico_ItemGradeBg"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Ico_ItemGradeBg%d.png"), 6))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Ico_ItemGradeBg");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Item */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Item"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Item/Item%d.png"), 89))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Item");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Ico_Equipment */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Ico_Equipment"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Ico_Equipment%d.dds"), 5))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Ico_Equipment");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_IItemEmptySlot */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_IItemEmptySlot"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/ItemEmptySlot.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_IItemEmptySlot");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_HealthBar */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_HealthBar"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/HealthBar.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_HealthBar");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_SkillLevelUp */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_SkillLevelUp"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Btn_LevelUp%d.dds"), 3))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_SkillLevelUp");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_GameResult_Deco */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_GameResult_Deco"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Result/Img_GameResult_Deco%d.png"), 4))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_GameResult_Deco");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
    
    /* Prototype_Texture_Img_GameResult_Bg_Color0 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_GameResult_Bg_Color0"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Result/Img_GameResult_Bg_Color0.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_GameResult_Bg_Color0");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_HUD_TimerFrame_Big */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_HUD_TimerFrame_Big"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_HUD_TimerFrame_Big.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_HUD_TimerFrame_Big");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Img_HUD_SideFrame */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_HUD_SideFrame"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Img_HUD_SideFrame.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Img_HUD_SideFrame");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Ico_Day */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Ico_Day"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/2D/Ico_Day%d.png"), 2))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Ico_Day");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 이미지
    /* Prototype_Image_Item */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Image_Item"),
                CImage::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Item/Item%d.png"), 89))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Image_Item");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 이펙트 객체 원형
    ///* Prototype_GameObject_TrailEffect */
    //m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    //m_pGameInstance->Add_Job(
    //    [this]()->void {
    //        if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_TrailEffect"),
    //            CTrailEffect::Create(m_pDevice, m_pContext))))
    //        {
    //            MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_TrailEffect");
    //        }
    //        m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
    //    }
    //);

    /* Prototype_GameObject_Nunchaku_AfterImage */

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nunchaku_AfterImage"),
                CNunchaku_AfterImage::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Nunchaku_AfterImage");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Nunchaku_SlashEffect */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nunchaku_SlashEffect"),
                CSlashEffect::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Nunchaku_SlashEffect");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Nunchaku_SpinEffect */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nunchaku_SpinEffect"),
                CSpinEffect::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Nunchaku_SpinEffect");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_LiDailinSlash2 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_LiDailinSlash2"),
                CLiDailinSlash2::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_LiDailinSlash2");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
    
    /* Prototype_GameObject_ShockWave_Q */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ShockWave_Q"),
                CShockWave_Q::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_ShockWave_Q");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Lava_Q */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lava_Q"),
                CLava_Q::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Lava_Q");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
       }
    );

    /* Prototype_GameObject_SpinWind */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SpinWind"),
                CSpinWind::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_SpinWind");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_LiDailin_E_Range */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_LiDailin_E_Range"),
                CLiDailin_E_Range::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_LiDailin_E_Range");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_Dragon_R */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Dragon_R"),
                CDragon_R::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_Dragon_R");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
    
    /* Prototype_GameObject_LiDailin_W_Effect */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_LiDailin_W_Effect"),
                CLiDailin_W_Effect::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_LiDailin_W_Effect");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_GameObject_CFiora_Q_1 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CFiora_Q_1"),
                CFiora_Q_1::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CFiora_Q_1");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );


    /* Prototype_GameObject_CFiora_W_1 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CFiora_W_1"),
                CFiora_W_1::Create(m_pDevice, m_pContext))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_GameObject_CFiora_W_1");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 이펙트 텍스처
    /* Prototype_Texture_Fx_Nunchaku */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Fx_Nunchaku"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/Fx_Nunchaku%d.dds"), 3))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Fx_Nunchaku");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FX_Nunchaku_SlashLine */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FX_Nunchaku_SlashLine"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FX_Nunchaku_SlashLine.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FX_Nunchaku_SlashLine");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FX_LiDailin_Q_Spin */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FX_LiDailin_Q_Spin"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FX_BI_Mirka_Skill03_Slash.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FX_LiDailin_Q_Spin");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FX_BI_Dragon01 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FX_BI_Dragon01"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FX_BI_Dragon01.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FX_BI_Dragon01");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FX_DragonNoise */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FX_DragonNoise"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/Dragon_Noise.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FX_DragonNoise");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_ShockWave */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_ShockWave"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/ShockWave4.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_ShockWave");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Lava */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Lava"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/Lava.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Lava");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_Nuncahku_Wind */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Nuncahku_Wind"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/Nuncahku_Wind.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_Nuncahku_Wind");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_LiDailin_E_Tex */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_LiDailin_E_Tex"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/LiDailin_E_Tex.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_LiDailin_E_Tex");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FXt_Noise_03 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FXt_Noise_03"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FXt_Noise_03.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FXt_Noise_03");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FXt_Noise_11 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FXt_Noise_11"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FXt_Noise_11.png"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FXt_Noise_11");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Texture_FX_BI_Dailin_W */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_FX_BI_Dailin_W"),
                CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GamePlay/Effect/FX_BI_Dailin_W.dds"), 1))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Texture_FX_BI_Dailin_W");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );
#pragma endregion

#pragma region 이펙트 모델
    _matrix AfterImagePreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

    /* Prototype_Component_Model_Nunchaku_AfterImage0 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, AfterImagePreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Nunchaku_AfterImage0"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/Nunchaku_AfterImage0.mymodel", AfterImagePreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_Nunchaku_AfterImage0");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_LiDailin_Circle_3_4 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, AfterImagePreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LiDailin_Circle_3_4"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/LiDailin_Circle_3_4.mymodel", AfterImagePreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_LiDailin_Circle_3_4");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_FBX_LiDailin_Spin */
    _matrix SpinPreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, SpinPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_FBX_LiDailin_Spin"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/FX_FBX_LiDailin_Spin.mymodel", SpinPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_FBX_LiDailin_Spin");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_LiDailin_Slash_02 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, SpinPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LiDailin_Slash_02"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/LiDailin_Slash_02.mymodel", SpinPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_LiDailin_Slash_02");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_LiDailinE_Wind4 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, SpinPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LiDailinE_Wind4"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/LiDailinE_Wind4.mymodel", SpinPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_LiDailinE_Wind4");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_LiDailin_E_Range */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, SpinPreTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LiDailin_E_Range"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/EffectModel/LiDailin_E_Range.mymodel", SpinPreTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_LiDailin_E_Range");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    _matrix FioraEffectTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

    /* Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_1 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_1"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill01_Range1_1.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_1");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_2 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_2"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill01_Range1_2.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill01_Range1_2");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_1 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_1"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill01_Range2_1.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_1");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_2 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_2"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill01_Range2_2.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill01_Range2_2");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_BI_Fiora_Skill02_Range1_S004 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill02_Range1_S004"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill02_Range1_S004.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill02_Range1_S004");
            }
            m_iFinishedJobCnt.fetch_add(1, memory_order_relaxed);
        }
    );

    /* Prototype_Component_Model_FX_BI_Fiora_Skill02_Range2_S004 */
    m_iTotalJobCnt.fetch_add(1, memory_order_relaxed);
    m_pGameInstance->Add_Job(
        [this, FioraEffectTransformMatrix]()->void {
            if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FX_BI_Fiora_Skill02_Range2_S004"),
                CMyModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/GamePlay/FioraEffect/FX_BI_Fiora_Skill02_Range2_S004.mymodel", FioraEffectTransformMatrix))))
            {
                MSG_BOX("CLoader.cpp(GamePlay) - Failed to Created: Prototype_Component_Model_FX_BI_Fiora_Skill02_Range2_S004");
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
#pragma region 사운드

#pragma endregion

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
