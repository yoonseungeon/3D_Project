#include "CLevel_GamePlay.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CSharedUI_Manager.h"

#include "CCamera_Free.h"
#include "CLumia_Ground.h"
#include "CLumia_Structure.h"
#include "CRoof.h"
#include "CItemBox.h"
#include "CItemBox_Collectible.h"
#include "CChicken.h"

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

    if (FAILED(Ready_Layer_ItemBox(TEXT("Layer_ItemBox"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_ItemBox_Collectible(TEXT("Layer_ItemBox_Collectible"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_River(TEXT("Layer_River"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Chicken(TEXT("Layer_Chicken"))))
        return E_FAIL;

    m_pSharedUI_Manager = CSharedUI_Manager::GetInstance();
    Safe_AddRef(m_pSharedUI_Manager);

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
    //SetWindowText(g_hWnd, TEXT("GamePlay 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
    LIGHT_DESC LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 0.8f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
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

HRESULT CLevel_GamePlay::Ready_Layer_ItemBox(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/ItemBox.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.contains("spawnBoxes") || !root["spawnBoxes"].is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    wstring wstrPrefix = { L"Prototype_Component_Model_" };

    for (const auto& jsonObj : root["spawnBoxes"])
    {
        CItemBox::ITEMBOX_DESC tItemBoxDesc{};

        string strArea = jsonObj["area"].get<string>();
        Set_ItemBoxSpwanArea(strArea, tItemBoxDesc.eSpawnArea);

        string strBoxType = jsonObj["boxType"].get<string>();

        auto pos = jsonObj["position"];
        auto rot = jsonObj["rotationQuat"];
        auto scale = jsonObj["scale"];

        tItemBoxDesc.wstrModelPrototypeTag = wstrPrefix + wstring(strBoxType.begin(), strBoxType.end());

        tItemBoxDesc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        tItemBoxDesc.vQuaternion = _float4(
            rot[0].get<_float>(),
            rot[1].get<_float>(),
            rot[2].get<_float>(),
            rot[3].get<_float>()
        );

        tItemBoxDesc.vScale = _float3(
            scale[0].get<_float>(),
            scale[1].get<_float>(),
            scale[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CItemBox"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &tItemBoxDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_ItemBox_Collectible(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/ItemBoxCollectible.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.contains("spawnBoxes") || !root["spawnBoxes"].is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    wstring wstrPrefix = { L"Prototype_Component_Model_" };

    for (const auto& jsonObj : root["spawnBoxes"])
    {
        CItemBox_Collectible::ITEMBOX_COLLECTIBLE_DESC tCollectibleDesc{};

        string strArea = jsonObj["area"].get<string>();
        Set_ItemBoxSpwanArea(strArea, tCollectibleDesc.eSpawnArea);

        string strBoxType = jsonObj["boxType"].get<string>();

        auto pos = jsonObj["position"];
        auto rot = jsonObj["rotationQuat"];
        auto scale = jsonObj["scale"];

        tCollectibleDesc.wstrModelPrototypeTag = wstrPrefix + wstring(strBoxType.begin(), strBoxType.end());

        if (tCollectibleDesc.wstrModelPrototypeTag == L"Prototype_Component_Model_ItemBox_Branch_01")
            tCollectibleDesc.iItemId = 3;
        else if (tCollectibleDesc.wstrModelPrototypeTag == L"Prototype_Component_Model_ItemBox_Pebbles_01")
            tCollectibleDesc.iItemId = 5;
        else if (tCollectibleDesc.wstrModelPrototypeTag == L"Prototype_Component_Model_ItemBox_Flower_01_active")
            tCollectibleDesc.iItemId = 27;
        else
            return E_FAIL;

        tCollectibleDesc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        tCollectibleDesc.vQuaternion = _float4(
            rot[0].get<_float>(),
            rot[1].get<_float>(),
            rot[2].get<_float>(),
            rot[3].get<_float>()
        );

        tCollectibleDesc.vScale = _float3(
            scale[0].get<_float>(),
            scale[1].get<_float>(),
            scale[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CItemBox_Collectible"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &tCollectibleDesc)))
            return E_FAIL;
    }

    return S_OK;
}

void CLevel_GamePlay::Set_ItemBoxSpwanArea(const string& strArea, SPAWN_MAP& eSpawnArea)
{    
    if (strArea == "ALLEY")                 eSpawnArea = SPAWN_MAP::ALLEY;
    else if (strArea == "ARCHERY")          eSpawnArea = SPAWN_MAP::ARCHERY;
    else if (strArea == "CEMETERY")         eSpawnArea = SPAWN_MAP::CEMETERY;
    else if (strArea == "CHURCH")           eSpawnArea = SPAWN_MAP::CHURCH;
    else if (strArea == "FACTORY")          eSpawnArea = SPAWN_MAP::FACTORY;
    else if (strArea == "FIRESTATION")      eSpawnArea = SPAWN_MAP::FIRE_STATION;
    else if (strArea == "FOREST")           eSpawnArea = SPAWN_MAP::FOREST;
    else if (strArea == "GASSTATION")       eSpawnArea = SPAWN_MAP::GAS_STATION;
    else if (strArea == "HARBOR")           eSpawnArea = SPAWN_MAP::HARBOR;
    else if (strArea == "HOSPITAL")         eSpawnArea = SPAWN_MAP::HOSPITAL;
    else if (strArea == "HOTEL")            eSpawnArea = SPAWN_MAP::HOTEL;
    else if (strArea == "POLICESTATION")    eSpawnArea = SPAWN_MAP::POLICE_STATION;
    else if (strArea == "POND")             eSpawnArea = SPAWN_MAP::POND;
    else if (strArea == "SANDYBEACH")       eSpawnArea = SPAWN_MAP::SANDY_BEACH;
    else if (strArea == "SCHOOL")           eSpawnArea = SPAWN_MAP::SCHOOL;
    else if (strArea == "STREAM")           eSpawnArea = SPAWN_MAP::STREAM;
    else if (strArea == "TEMPLE")           eSpawnArea = SPAWN_MAP::TEMPLE;
    else if (strArea == "UPTOWN")           eSpawnArea = SPAWN_MAP::UPTOWN;
    else if (strArea == "WAREHOUSE")        eSpawnArea = SPAWN_MAP::WAREHOUSE;
    else
    {
        MSG_BOX("SPAWN_MAP Error: CLevel_GamePlay.cpp");
    }
}

HRESULT CLevel_GamePlay::Ready_Layer_River(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_River"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Chicken(const _wstring& strLayerTag)
{
    CChicken::CHICKEN_DESC Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Chicken"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
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

    Safe_Release(m_pSharedUI_Manager);
    CSharedUI_Manager::DestroyInstance();

    Safe_Release(m_pInGame_Manager);
    
    __super::Free();
}
