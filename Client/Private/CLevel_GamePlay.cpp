#include "CLevel_GamePlay.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CCamera_Free.h"
#include "CLumia_Ground.h"
#include "CLumia_Structure.h"
#include "CRoof.h"

#include "CUI_Image.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
    , m_pInGame_Manager{ CInGame_Manager::GetInstance() }
{
    Safe_AddRef(m_pInGame_Manager);
}

HRESULT CLevel_GamePlay::Initialize()
{
    m_pGameInstance->Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE::PARALLEL);

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map_Lumia(TEXT("Layer_Map_Lumia"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
    //if (m_pGameInstance->Key_Down(DIK_RETURN))
    //{
    //    CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::ENDING);

    //    // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
    //    // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
    //    if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
    //        return;
    //}
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("GamePlay 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
    LIGHT_DESC LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT::POINT;
    LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 0.1f, 0.1f, 1.f);
    LightDesc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
    LightDesc.fRange = 15.f;

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT::POINT;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vSpecular = _float4(0.1f, 1.f, 0.1f, 1.f);
    LightDesc.vPosition = _float4(25.f, 5.f, 10.f, 1.f);
    LightDesc.fRange = 15.f;

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC CameraDesc{};

    CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
    CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
    CameraDesc.fFovy = XMConvertToRadians(60.f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 500.f;    
    CameraDesc.tTransformDesc.fSpeedPerSec = 20.f;
    CameraDesc.tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);
    CameraDesc.fMouseSensor = 0.05f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Free"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    for (size_t i = 0; i < 10; ++i) {

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map_Lumia(const _wstring& strLayerTag)
{
    // Ground
    CLumia_Ground::LUMIA_GROUND_DESC GroundDesc{};

    CLumia_Ground* pMap{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_Ground"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &GroundDesc, reinterpret_cast<CGameObject**>(&pMap))))
        return E_FAIL;

    m_pInGame_Manager->Set_Map(pMap);

    Safe_Release(pMap);

    // Structure
    CLumia_Structure::LUMIA_STRUCTURE_DESC StructureDesc{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_Structure"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &StructureDesc)))
        return E_FAIL;

    // Roof
    for (_uint i = 0; i < iRoofCnt; ++i)
    {
        CRoof::ROOF_DESC RoofDesc{};

        RoofDesc.wstrModelTag = ROOFS[i].PROTYPE_TAG;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Roof"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &RoofDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
    

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CInGameCharHUD_Empty"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_GamePlay");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_GamePlay::Free()
{
    m_pInGame_Manager->Release_Map();   
    m_pInGame_Manager->Release_Player();

    Safe_Release(m_pInGame_Manager);

    __super::Free();
}
