#include "CLevel_GamePlay.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CSharedUI_Manager.h"

#include "CCamera_Free.h"
#include "CLumia_NavMesh.h"
#include "CSplitGround.h"
#include "CLumia_Structure.h"
#include "CRoof.h"
#include "CItemBox.h"
#include "CItemBox_Collectible.h"
#include "CChicken.h"
#include "CWolf.h"
#include "CDog.h"
#include "CBear.h"
#include "CBat.h"
#include "CBoar.h"

#include "CMap_Grass.h"
#include "CMap_Environment.h"

#include "CAIFiora.h"

#include "CUI_Image.h"

#include "CUI_GameResult.h"
#include "CUI_Timer.h"

#include "CUI_MiniMap.h"

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

    if (FAILED(Ready_Shadow()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map_Lumia(TEXT("Layer_Map_Lumia"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map_Grass(TEXT("Layer_Map_Grass"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map_Environment(TEXT("Layer_Map_Environment"))))
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

    if (FAILED(Ready_Layer_Wolf(TEXT("Layer_Wolf"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Dog(TEXT("Layer_Dog"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Bear(TEXT("Layer_Bear"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Bat(TEXT("Layer_Bat"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Boar(TEXT("Layer_Boar"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_AIFiora(TEXT("Layer_AIFiora"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_GameResult(TEXT("Layer_UI_GameResult"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Timer(TEXT("Layer_UI_Timer"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MiniMap(TEXT("Layer_MiniMap"))))
        return E_FAIL;

    m_pSharedUI_Manager = CSharedUI_Manager::GetInstance();
    Safe_AddRef(m_pSharedUI_Manager);

    m_fDaySoundLoopGap = m_fAccDaySoundLoopTime = 10.f;
    m_pGameInstance->PlaySound_Loop(ETOUI(SOUND_KEY::SUNNY), ETOUI(SOUND_CHANNEL_GAMEPLAY::ENVIRONMENT_1));

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{    
    m_pInGame_Manager->Update_InGameManager(fTimeDelta);

    Update_EnvironmentSound(fTimeDelta);

    if (m_pInGame_Manager->Get_GameEnd() == true)
    {
        m_pGameInstance->Clear_ResourcesPrevious();

        CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::ENDING);

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
            return;
    }
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
    //SetWindowText(g_hWnd, TEXT("GamePlay 레벨입니다."));
#endif

    return S_OK;
}

void CLevel_GamePlay::Update_EnvironmentSound(_float fTimeDelta)
{
    m_fAccDaySoundLoopTime += fTimeDelta;

    _bool bCurDay = m_pInGame_Manager->IsDay();

    if (m_fAccDaySoundLoopTime >= m_fDaySoundLoopGap || m_bDay != bCurDay)
    {
        m_bDay = bCurDay;
        m_fAccDaySoundLoopTime = 0.f;

        _uint iFirstSoundKey{};
        _uint iLastSoundKey{};

        if (bCurDay == true)
        {
            iFirstSoundKey = ETOUI(SOUND_KEY::DAY_BIRD1);
            iLastSoundKey = ETOUI(SOUND_KEY::DAY_BIRD6);
        }
        else
        {
            iFirstSoundKey = ETOUI(SOUND_KEY::NIGHT_BIRD1);
            iLastSoundKey = ETOUI(SOUND_KEY::NIGHT_BIRD5);
        }

        _uint iSoundCount = iLastSoundKey - iFirstSoundKey + 1;

        _uint iRandomSound = iFirstSoundKey + (rand() % iSoundCount);

        m_pGameInstance->PlaySound_OnceFixed(iRandomSound, ETOUI(SOUND_CHANNEL_GAMEPLAY::ENVIRONMENT_2));
    }
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
    LIGHT_DESC LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(1.00f, 1.00f, 1.00f, 1.f);
    LightDesc.vAmbient = _float4(0.90f, 0.90f, 0.90f, 1.f);   

    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vDirection = _float4(0.08f, -0.89f, 0.44f, 0.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    //LightDesc.eType = LIGHT::POINT;
    //LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    //LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
    //LightDesc.vSpecular = _float4(1.f, 0.1f, 0.1f, 1.f);
    //LightDesc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
    //LightDesc.fRange = 15.f;

    //if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
    //    return E_FAIL;

    //LightDesc.eType = LIGHT::POINT;
    //LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    //LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
    //LightDesc.vSpecular = _float4(0.1f, 1.f, 0.1f, 1.f);
    //LightDesc.vPosition = _float4(25.f, 5.f, 10.f, 1.f);
    //LightDesc.fRange = 15.f;

    //if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Shadow()
{
    SHADOW_LIGHT_DESC ShadowDesc{};
    ShadowDesc.vEye = _float4(-6.5f, 20.2f, -5.5f, 1.f);
    ShadowDesc.vAt = _float4(-4.5f, 0.2f, 4.5f, 1.f);
    ShadowDesc.fWidth = 53.33f;
    ShadowDesc.fHeight = 30.f;
    ShadowDesc.fNear = 0.1f;
    ShadowDesc.fFar = 1000.f;

    if (FAILED(m_pGameInstance->Add_ShadowLight(ETOUI(LEVEL::GAMEPLAY), ShadowDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC CameraDesc{};

    //CameraDesc.vEye = _float3(0.f, 450.f, 0.f);
    //CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
    CameraDesc.vEye = _float3(0.f, 10.f, 0.f);
    CameraDesc.vAt = _float3(-4.5f, 0.2f, 4.5f);
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
 /*   for (size_t i = 0; i < 10; ++i) {

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
            return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map_Lumia(const _wstring& strLayerTag)
{
    // Ground
    CLumia_NavMesh::LUMIA_NAVMESH_DESC GroundDesc{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_NavMesh"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &GroundDesc)))
        return E_FAIL;

    // SplitGround
    for (_uint i = 0; i < sizeof(GROUNDS) / sizeof(GROUNDS[0]); ++i)
    {
        CSplitGround::SPLIT_GROUND_DESC SplitGroundDesc{};

        SplitGroundDesc.wstrPrototype_Component_Tag = GROUNDS[i].PROTYPE_TAG;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lumia_SplitGround"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &SplitGroundDesc)))
        {
            return E_FAIL;
        }
    }

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

HRESULT CLevel_GamePlay::Ready_Layer_Map_Grass(const _wstring& strLayerTag)
{
    // Grass
    for (_uint i = 0; i < sizeof(GRASSES) / sizeof(GRASSES[0]); ++i)
    {
        CMap_Grass::MAP_GRASS_DESC GrassDesc{};

        GrassDesc.wstrPrototype_Component_Tag = GRASSES[i].PROTYPE_TAG;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Grass"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &GrassDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map_Environment(const _wstring& strLayerTag)
{
    // Map Objects
    for (_uint i = 0; i < sizeof(MAP_ENVIRONMENTS) / sizeof(MAP_ENVIRONMENTS[0]); ++i)
    {
        CMap_Environment::MAP_ENVIRONMENT_DESC EnvironmentDesc{};

        EnvironmentDesc.wstrPrototype_Component_Tag = MAP_ENVIRONMENTS[i].PROTYPE_TAG;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Environment"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &EnvironmentDesc)))
        {
            return E_FAIL;
        }
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
    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
    //    ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
    //    ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
    //    ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
    //    return E_FAIL;

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
        
        SOUND_KEY eSoundKey = Get_ItemBoxOpenSoundKey(strBoxType);
        tItemBoxDesc.iSoundKey = ETOUI(eSoundKey);

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

SOUND_KEY CLevel_GamePlay::Get_ItemBoxOpenSoundKey(const string& strBoxType)
{
    if (strBoxType == "ItemBox_ATM_01")
        return SOUND_KEY::OPENSOUND_ATM_01;
    else if (strBoxType == "ItemBox_AltarBig_Re" || strBoxType == "ItemBox_AltarTable")
        return SOUND_KEY::OPENSOUND_ALTARTABLE_01;
    else if (strBoxType == "ItemBox_Bag_01" || strBoxType == "ItemBox_Bag_02")
        return SOUND_KEY::OPENSOUND_BAG_01;
    else if (strBoxType == "ItemBox_BarbequeGrill_01")
        return SOUND_KEY::OPENSOUND_BARBEQUEGRILL_01;
    else if (strBoxType == "ItemBox_BeachChair")
        return SOUND_KEY::OPENSOUND_BEACHCHAIR_01;
    else if (strBoxType == "ItemBox_Biotoilet_01")
        return SOUND_KEY::OPENSOUND_BIOTOILET_01;
    else if (strBoxType == "ItemBox_Boat_01")
        return SOUND_KEY::OPENSOUND_BOAT_01;
    else if (strBoxType == "ItemBox_Box_01")
        return SOUND_KEY::OPENSOUND_BOX_01;
    else if (strBoxType == "ItemBox_Box_02")
        return SOUND_KEY::OPENSOUND_BOX_02;
    else if (strBoxType == "ItemBox_CartonBox_01")
        return SOUND_KEY::OPENSOUND_CARTONBOX_01;
    else if (strBoxType == "ItemBox_CementMixer")
        return SOUND_KEY::OPENSOUND_CEMENTMIXER;
    else if (strBoxType == "ItemBox_CoffeeMachine_01")
        return SOUND_KEY::OPENSOUND_COFFEEMACHINE_01;
    else if (strBoxType == "ItemBox_Coffin_Large" || strBoxType == "ItemBox_Coffin_Middle")
        return SOUND_KEY::OPENSOUND_COFFIN_01;
    else if (strBoxType == "ItemBox_ConcreteBag_Set_01" || strBoxType == "ItemBox_ConcreteBag_Set_02")
        return SOUND_KEY::OPENSOUND_CONCRETEBAG_SET_01;
    else if (strBoxType == "ItemBox_Confessional_01")
        return SOUND_KEY::OPENSOUND_CONFESSIONAL_01;
    else if (strBoxType == "ItemBox_Drum_W")
        return SOUND_KEY::OPENSOUND_DRUM_01;
    else if (strBoxType == "ItemBox_Dumpster_01")
        return SOUND_KEY::OPENSOUND_DUMPSTER_01;
    else if (strBoxType == "ItemBox_EraserCleaner_01")
        return SOUND_KEY::OPENSOUND_ERASERCLEANER_01;
    else if (strBoxType == "ItemBox_FilingCabinets_01" || strBoxType == "ItemBox_FilingCabinets_02" || strBoxType == "ItemBox_Quiver_01")
        return SOUND_KEY::OPENSOUND_FILINGCABINETS_01;
    else if (strBoxType == "ItemBox_GarbageBag_Set")
        return SOUND_KEY::OPENSOUND_GARBAGEBAG_SET_01;
    else if (strBoxType == "ItemBox_Hospital_Cabinet_01")
        return SOUND_KEY::OPENSOUND_HOSPITAL_CABINET_01;
    else if (strBoxType == "ItemBox_Hospital_Cart_01")
        return SOUND_KEY::OPENSOUND_HOSPITAL_CART_01;
    else if (strBoxType == "ItemBox_IceBox_B" || strBoxType == "ItemBox_IceBox_R")
        return SOUND_KEY::OPENSOUND_ICEBOX_01;
    else if (strBoxType == "ItemBox_Jar_01")
        return SOUND_KEY::OPENSOUND_JAR_01;
    else if (strBoxType == "ItemBox_Jar_Big_01")
        return SOUND_KEY::OPENSOUND_JAR_BIG_01;
    else if (strBoxType == "ItemBox_Locker_01")
        return SOUND_KEY::OPENSOUND_LOCKER_01;
    else if (strBoxType == "ItemBox_Organ_01")
        return SOUND_KEY::OPENSOUND_ORGAN_01;
    else if (strBoxType == "ItemBox_Pallet_Loaded_01")
        return SOUND_KEY::OPENSOUND_PALLET_LOADED_01;
    else if (strBoxType == "ItemBox_Sedan_Brown_01")
        return SOUND_KEY::OPENSOUND_SEDAN_BROWN_01;
    else if (strBoxType == "ItemBox_Sedan_Police_01")
        return SOUND_KEY::OPENSOUND_SEDAN_POLICE_01;
    else if (strBoxType == "ItemBox_Sedan_Taxi_01")
        return SOUND_KEY::OPENSOUND_SEDAN_TAXI_01;
    else if (strBoxType == "ItemBox_SteelBox_02")
        return SOUND_KEY::OPENSOUND_STEELBOX_02;
    else if (strBoxType == "ItemBox_SteelBox_03")
        return SOUND_KEY::OPENSOUND_STEELBOX_03;
    else if (strBoxType == "ItemBox_Suitcase_01" || strBoxType == "ItemBox_Suitcase_02" || strBoxType == "ItemBox_Suitcase_03" || strBoxType == "ItemBox_Suitcase_04")
        return SOUND_KEY::OPENSOUND_SUITCASE_01;
    else if (strBoxType == "ItemBox_Switchboard_01")
        return SOUND_KEY::OPENSOUND_SWITCHBOARD_01;
    else if (strBoxType == "ItemBox_Temple_Box_01" || strBoxType == "ItemBox_Temple_Box_02")
        return SOUND_KEY::OPENSOUND_TEMPLE_BOX_01;
    else if (strBoxType == "ItemBox_Temple_Thurible_01")
        return SOUND_KEY::OPENSOUND_THURIBLE_01;
    else if (strBoxType == "ItemBox_Temple_Lamp_01" || strBoxType == "ItemBox_Tomb")
        return SOUND_KEY::OPENSOUND_TOMB_01;
    else if (strBoxType == "ItemBox_TrashCan_01")
        return SOUND_KEY::OPENSOUND_TRASHCAN_01;
    else if (strBoxType == "ItemBox_TrashCan_02" || strBoxType == "ItemBox_TrashCan_03")
        return SOUND_KEY::OPENSOUND_TRASHCAN2_01;
    else if (strBoxType == "ItemBox_TreeStump_01")
        return SOUND_KEY::OPENSOUND_TREESTUMP_01;
    else if (strBoxType == "ItemBox_TreeStump_02")
        return SOUND_KEY::OPENSOUND_TREESTUMP_02;
    else if (strBoxType == "ItemBox_Uptown_BookCase_01")
        return SOUND_KEY::OPENSOUND_UPTOWN_BOOKCASE_01;
    else if (strBoxType == "ItemBox_Uptown_Drawer_01")
        return SOUND_KEY::OPENSOUND_UPTOWN_DRAWER_01;
    else if (strBoxType == "ItemBox_Uptown_FirePlace_01")
        return SOUND_KEY::OPENSOUND_UPTOWN_FIREPLACE_01;
    else if (strBoxType == "ItemBox_VendingMachine_01" || strBoxType == "ItemBox_VendingMachine_02")
        return SOUND_KEY::OPENSOUND_VENDINGMACHINE_01;
    else if (strBoxType == "ItemBox_WaterTank_01" || strBoxType == "ItemBox_Water_Dispenser_01")
        return SOUND_KEY::OPENSOUND_WATERTANK_01;
    else if (strBoxType == "ItemBox_Wheelbarrow")
        return SOUND_KEY::OPENSOUND_WHEELBARROW_01;
    else if (strBoxType == "ItemBox_WreckCar_Blue_01" || strBoxType == "ItemBox_WreckCar_White_01" || strBoxType == "ItemBox_WreckCar_Yellow_01")
        return SOUND_KEY::OPENSOUND_WRECKCAR_01;

    return SOUND_KEY::OPENSOUND_BOX_01;
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

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_WaterPlan"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Chicken(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/ChickenSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CChicken::CHICKEN_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Chicken"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Wolf(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/WolfSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CWolf::WOLF_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Wolf"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Dog(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/DogSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CDog::DOG_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Dog"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Bear(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/BearSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CBear::BEAR_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bear"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Bat(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/BatSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CBat::BAT_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bat"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Boar(const _wstring& strLayerTag)
{
    std::ifstream ifs("../Bin/Data/BoarSpawnPoints.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return E_FAIL;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return E_FAIL;
    }

    for (const auto& pos : root)
    {
        if (!pos.is_array() || pos.size() != 3)
        {
            MSG_BOX("Json Format Error");
            return E_FAIL;
        }

        CBoar::BOAR_DESC Desc{};

        Desc.tTransformDesc.vStartPos = _float3(
            pos[0].get<_float>(),
            pos[1].get<_float>(),
            pos[2].get<_float>()
        );

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Boar"),
            ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_AIFiora(const _wstring& strLayerTag)
{
    CAIFiora::AIFIORA_DESC Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_AIFiora"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_GameResult(const _wstring& strLayerTag)
{
    CUI_GameResult::CUI_GAMERESULT_DESC Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_GameResult"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Timer(const _wstring& strLayerTag)
{
    CUI_Timer::CUI_TIMTER_DESC Desc{};

    Desc.fScaleRatioX = 0.15f;
    Desc.fScaleRatioY = 0.05f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.5f - Desc.fScaleRatioY * 0.5f;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Timer"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MiniMap(const _wstring& strLayerTag)
{
    CUI_MiniMap::CUI_MINIMAP_DESC MiniMapDesc{};

    MiniMapDesc.fScaleRatioX = 0.169f;
    MiniMapDesc.fScaleRatioY = MiniMapDesc.fScaleRatioX / static_cast<_float>(g_iWinSizeY) * static_cast<_float>(g_iWinSizeX);
    MiniMapDesc.fPosRatioX = 0.415469f;
    MiniMapDesc.fPosRatioY = -0.35f;

    MiniMapDesc.iUILayer = ETOUI(UILAYER::PANEL);

    MiniMapDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    MiniMapDesc.wstrTexturePrototypeTag = L"Prototype_Texture_MiniMap";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_MiniMap"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &MiniMapDesc)))
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
    m_pInGame_Manager->Release_Player();
    m_pInGame_Manager->Release_GameResultUI();
    m_pInGame_Manager->Release_DayTimer();

    Safe_Release(m_pSharedUI_Manager);
    CSharedUI_Manager::DestroyInstance();

    Safe_Release(m_pInGame_Manager);
    
    __super::Free();
}
