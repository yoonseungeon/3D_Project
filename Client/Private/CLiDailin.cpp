#include "CLiDailin.h"

#include "CGameInstance.h"

#include "CInventory.h"
#include "CEquipment.h"

#include "CBody_Player.h"
#include "CWeapon.h"
#include "CBottle.h"
#include "CCraftTool.h"
#include "CCraftHammer.h"
#include "CBurner.h"
#include "CFryingPan.h"

#include "CInGameHPBar.h"

#include "CInGame_Manager.h"

#include "CLiDailinIdle.h"
#include "CLiDailinMove.h"
#include "CLiDailinAttack.h"
#include "CLiDailin_Q.h"
#include "CLiDailin_W.h"
#include "CLiDailin_E.h"
#include "CLiDailin_R.h"

#include "CInteract_ItemBox.h"
#include "CAction_Craft.h"
#include "CAction_Collect.h"

#include "CUI_StackSkillIcon.h"
#include "CUI_NormalSkillIcon.h"

#include "CAbstractMonster.h"

CLiDailin::CLiDailin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractPlayer{ pDevice, pContext }
{
}

CLiDailin::CLiDailin(const CLiDailin& Prototype)
    : CAbstractPlayer{ Prototype }
{
}

HRESULT CLiDailin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLiDailin::Initialize(void* pArg)
{
    CABSTRACTPLAYER_DESC Desc{};
    Desc.eItemType = ITEM_TYPE::NUNCHAKU;
    Desc.tTransformDesc.fSpeedPerSec = 10.f;
    Desc.tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);
    Desc.tTransformDesc.vStartPos = { 4.f, 0.f, -2.8f };

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    // Initialize_State보다 위
    m_fAttackRange = 1.5f;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    
    if (FAILED(Initialize_State()))
        return E_FAIL;

    if (FAILED(Initialize_Skill()))
        return E_FAIL;

    CInGame_Manager::GetInstance()->Set_Player(this);
    
    if (FAILED(Initialize_Stat()))
        return E_FAIL;

    TryEquip_AddInven(4);
    TryEquip_AddInven(34);
    TryEquip_AddInven(55);
    TryEquip_AddInven(52);
    TryEquip_AddInven(34);
    TryEquip_AddInven(55);
    TryEquip_AddInven(52);
    TryEquip_AddInven(52);
    TryEquip_AddInven(52);
    TryEquip_AddInven(52);


    TryEquip_AddInven(86);

    
    return S_OK;
}

void CLiDailin::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CLiDailin::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

}

void CLiDailin::Update(_float fTimeDelta)
{    
    Key_Input();

    Apply_WaitActionState();
    Apply_WaitMovementState();

    if (m_pCurActionState != nullptr) {
        m_pCurActionState->Update(this, fTimeDelta);
    }
    if (m_pCurMovementState != nullptr) {
        m_pCurMovementState->Update(this, fTimeDelta);
    }

    Apply_WaitActionState();
    Apply_WaitMovementState();

    CoolTimer(fTimeDelta);

    m_pNavigationCom->Compute_OnNavigation();

    __super::Update(fTimeDelta);
}

void CLiDailin::Late_Update(_float fTimeDelta)
{
    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif   
}

HRESULT CLiDailin::Render()
{
    return S_OK;
}

void CLiDailin::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
    if (tCollision.pMyCollider == m_Colliders[LIDAILIN_Q])
    {
        CActionState* pLiDailinQ = Get_ActionState(L"CLiDailin_Q");
        if (pLiDailinQ != nullptr)
            pLiDailinQ->OnCollision_Enter(tCollision);
    }
 
    if (tCollision.pMyCollider == m_Colliders[LIDAILIN_E])
    {
        CActionState* pLiDailinE = Get_ActionState(L"CLiDailin_E");
        if (pLiDailinE != nullptr)
            pLiDailinE->OnCollision_Enter(tCollision);
    }

    if (tCollision.pMyCollider == m_Colliders[LIDAILIN_R])
    {
        CActionState* pLiDailinR = Get_ActionState(L"CLiDailin_R");
        if (pLiDailinR != nullptr)
            pLiDailinR->OnCollision_Enter(tCollision);
    }
}

void CLiDailin::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
}

void CLiDailin::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}

void CLiDailin::Set_WaitMovementState(const wstring& wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CLiDailin.cpp: No MovementState");
        m_pWaitMovementState = nullptr;
        return;
    }
    
    m_pWaitMovementState = dynamic_cast<CMovementState*>(iter->second);
}

void CLiDailin::Apply_WaitMovementState()
{
    if (m_pWaitMovementState == nullptr) {
        return;
    }

    if(m_pCurMovementState != nullptr)
    {
        m_pCurMovementState->Exit(this);
    }

    m_pCurMovementState = m_pWaitMovementState;
    m_pWaitMovementState = nullptr;

    m_pCurMovementState->Enter(this);
}

void CLiDailin::Process_MovementCommand(MOVEMENT_COMMAND& tMovement_Command)
{
    m_pCurMovementState->HandleMovementCommand(this, tMovement_Command);
}

void CLiDailin::Set_WaitActionState(const wstring& wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CLiDailin.cpp: No ActionState");
        m_pWaitActionState = nullptr;
        return;
    }

    m_pWaitActionState = dynamic_cast<CActionState*>(iter->second);
}

void CLiDailin::Apply_WaitActionState()
{
    if (m_pWaitActionState == nullptr) {
        if (m_bActionEnd == true)
        {
            if (m_pCurActionState != nullptr)
            {
                m_pCurActionState->Exit(this);
            }
            m_pCurActionState = nullptr;
            m_bActionEnd = false;
        }
        return;
    }

    if (m_pCurActionState != nullptr)
    {
        m_pCurActionState->Exit(this);
    }

    m_pCurActionState = m_pWaitActionState;
    m_pWaitActionState = nullptr;

    m_bActionEnd = false;
    m_pCurActionState->Enter(this);
}

void CLiDailin::Process_ActionCommand(ACTION_COMMAND& tAction_Command)
{
    if (m_pCurActionState != nullptr) {
        m_pCurActionState->HandleActionCommand(this, tAction_Command);
        return;
    }

    DefaultProcess_ActionCommand(tAction_Command);
}

void CLiDailin::DefaultProcess_ActionCommand(ACTION_COMMAND& tAction_Command)
{
    switch (tAction_Command.eCommandType)
    {
    case ACTION_COMMAND_TYPE::ATTACK:
    {
        Set_CurActionCommand(tAction_Command);
        Set_WaitActionState(L"CLiDailinAttack");

        break;
    }

    case ACTION_COMMAND_TYPE::ATTACK_Q:
    {
        if (CanUseSkill(SKILL_SLOT::Q) == true)
        {
            Set_CurActionCommand(tAction_Command);
            Set_WaitActionState(L"CLiDailin_Q");
        }
        break;
    }

    case ACTION_COMMAND_TYPE::ATTACK_W:
    {
        if (CanUseSkill(SKILL_SLOT::W) == true)
        {
            Set_CurActionCommand(tAction_Command);
            Set_WaitActionState(L"CLiDailin_W");
        }
        break;
    }

    case ACTION_COMMAND_TYPE::ATTACK_E:
    {
        if (CanUseSkill(SKILL_SLOT::E) == true)
        {
            Set_CurActionCommand(tAction_Command);
            Set_WaitActionState(L"CLiDailin_E");
        }
        break;
    }

    case ACTION_COMMAND_TYPE::ATTACK_R:
    {
        if (CanUseSkill(SKILL_SLOT::R) == true)
        {
            Set_CurActionCommand(tAction_Command);
            Set_WaitActionState(L"CLiDailin_R");
        }
        break;

    case ACTION_COMMAND_TYPE::INTERACT_ITEMBOX:
    {
        Set_CurActionCommand(tAction_Command);
        Set_WaitActionState(L"CInteract_ItemBox");
        break;
    }
    }

    case ACTION_COMMAND_TYPE::CRAFT:
    {
        Set_CurActionCommand(tAction_Command);
        Set_WaitActionState(L"CAction_Craft");
        break;
    }


    case ACTION_COMMAND_TYPE::COLLECT:
    {
        Set_CurActionCommand(tAction_Command);
        Set_WaitActionState(L"CAction_Collect");
        break;
    }
    }
}

COOL_INFO* CLiDailin::Get_CoolInfo(const SKILL_SLOT eType)
{
    switch (eType)
    {
        case SKILL_SLOT::Q:
        {
            return &tQCool;
            break;
        }
        case SKILL_SLOT::W:
        {
            return &tWCool;
            break;
        }
        case SKILL_SLOT::E:
        {
            return &tECool;
            break;
        }
        case SKILL_SLOT::R:
        {
            return &tRCool;
            break;
        }
    }

    return nullptr;
}

_bool CLiDailin::CanUseSkill(const SKILL_SLOT eType)
{
    switch (eType)
    {
        case SKILL_SLOT::Q:
        {
            if (tQCool.fAccCoolDown <= 0.f)
                return true;          
            break;
        }
        case SKILL_SLOT::W:
        {
            if (tWCool.fAccCoolDown <= 0.f)
                return true;       
            break;
        }
        case SKILL_SLOT::E:
        {
            if (tECool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
        case SKILL_SLOT::R:
        {
            if (tRCool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
    }   

    return false;
}

void CLiDailin::Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc)
{
    tDesc.eSkillSlot = eType;

    switch (eType)
    {
        case SKILL_SLOT::Q:
        {
            tDesc.eCoolDownType = COOLDOWN_TYPE::STACK;
            tDesc.TexturePrototypeTag = L"Prototype_Texture_LiDailin_Q";
            break;
        }
        case SKILL_SLOT::W:
        {
            tDesc.eCoolDownType = COOLDOWN_TYPE::NORMAL;
            tDesc.TexturePrototypeTag = L"Prototype_Texture_LiDailin_W";
            break;
        }
        case SKILL_SLOT::E:
        {
            tDesc.eCoolDownType = COOLDOWN_TYPE::NORMAL;
            tDesc.TexturePrototypeTag = L"Prototype_Texture_LiDailin_E";
            break;
        }
        case SKILL_SLOT::R:
        {
            tDesc.eCoolDownType = COOLDOWN_TYPE::NORMAL;
            tDesc.TexturePrototypeTag = L"Prototype_Texture_LiDailin_R";
            break;
        }
    }
}

_uint CLiDailin::Get_MaxSkillLevel(const SKILL_SLOT eType)
{  
    CSkillState* pSkillState = FindSkill(eType);

    if (pSkillState == nullptr)
        return 0;

    return pSkillState->Get_MaxLevel();
}

_uint CLiDailin::Get_CurSkillLevel(const SKILL_SLOT eType)
{
    CSkillState* pSkillState = FindSkill(eType);

    if (pSkillState == nullptr)
        return 0;

    return pSkillState->Get_CurLevel();
}

void CLiDailin::LevelUpSkill(const SKILL_SLOT eType)
{
    CSkillState* pSkillState = FindSkill(eType);

    if (pSkillState == nullptr)
        return;

    if (m_iSkillPoint >= 1 && pSkillState->SkillLevelUp() == true)
        --m_iSkillPoint;
}

_bool CLiDailin::CanLevelUpSkill(const SKILL_SLOT eType)
{
    CSkillState* pSkillState = FindSkill(eType);

    if (pSkillState == nullptr)
        return false;

    if (m_iSkillPoint <= 0)
        return false;

    _int iMaxLevel = pSkillState->Get_MaxLevel();
    _int iCurLevel = pSkillState->Get_CurLevel();

    _int iCurMaxLevel = {};

    switch (eType)
    {
        case SKILL_SLOT::Q:
        case SKILL_SLOT::W:
        case SKILL_SLOT::E:
        {
            iCurMaxLevel = (std::min)(iMaxLevel, (m_tCurStat.iLevel + 1) / 2);
            break;
        }
        case SKILL_SLOT::R:
        {
            iCurMaxLevel = (std::min)(iMaxLevel, (m_tCurStat.iLevel - 1) / 5);
            break;
        }
    }

    if (iCurLevel < iCurMaxLevel)
        return true;

    return false;
}

HRESULT CLiDailin::Ready_Components()
{
    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC NaviDesc;

    NaviDesc.bIsGround = false;
    NaviDesc.pTransformCom = m_pTransformCom;
    XMStoreFloat3(&NaviDesc.vObjectWorldPos, m_pTransformCom->Get_State(STATE::POSITION));

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;


    CMove::MOVE_DESC Desc{};
    Desc.pTransform = m_pTransformCom;
    Desc.pNavigationCom = m_pNavigationCom;

    Desc.fSpeed = 5.f;

    /* Com_Move */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Move"),
        TEXT("Com_Move"), reinterpret_cast<CComponent**>(&m_pMoveCom), &Desc)))
        return E_FAIL;


    CCollider* pColliderCom;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{ };
    AABBDesc.vSize = _float3(0.7f, 1.f, 0.7f);
    AABBDesc.vCenter = _float3(0.f, fColliderCenterY, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::ENEMY) | ETOUI(Collision_Layer::ENEMY_SKILL));

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);


    // Q
    /* For.Com_Collider_Sphere */
    CBounding_Sphere::BOUNDING_SPHERE_DESC  SphereDesc{ };
    SphereDesc.fRadius = 1.8f;
    SphereDesc.vCenter = _float3(0.f, fColliderCenterY, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&pColliderCom), &SphereDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);


    // E
    /* For.Com_Collider_Frustum */
    CBounding_Frustum::BOUNDING_FRUSTUM_DESC  FrustumDesc{ };
    FrustumDesc.vOrigin = _float3(0.f, fColliderCenterY, 0.f);
    FrustumDesc.vRadians = _float3(0.f, 0.f, 0.f);
    FrustumDesc.fNear = 0.f;
    FrustumDesc.fFar = 2.5f;

    // tan으로 넣어줘야 함.
    FrustumDesc.fRightSlope = tanf(XMConvertToRadians(30.f));  // 오른쪽 각도
    FrustumDesc.fLeftSlope = -FrustumDesc.fRightSlope;         // 왼쪽 각도

    FrustumDesc.fTopSlope = tanf(XMConvertToRadians(10.f)); // 위 각도
    FrustumDesc.fBottomSlope = -FrustumDesc.fTopSlope;      // 아래 각도

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Frustum"),
        TEXT("Com_Collider_Furstum"), reinterpret_cast<CComponent**>(&pColliderCom), &FrustumDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);


    // R
    /* For.Com_Collider_OBB */
    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{ };
    OBBDesc.vSize = _float3(0.7f, 1.f, 1.4f);
    OBBDesc.vCenter = _float3(0.f, fColliderCenterY, 0.35f);
    OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&pColliderCom), &OBBDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);

    return S_OK;
}

HRESULT CLiDailin::Ready_PartObjects()
{
    // Body
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBody);

    // Weapon
    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Equip_R");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
        TEXT("Weapon"), &WeaponDesc)))
        return E_FAIL;

    m_pWeapon = dynamic_cast<CWeapon*>(m_PartObjects[TEXT("Weapon")]);
    Safe_AddRef(m_pWeapon);


    // Bottle
    CBottle::BOTTLE_DESC BottleDesc{};
    BottleDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BottleDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Weapon_Special_1");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bottle"),
        TEXT("Bottle"), &BottleDesc)))
        return E_FAIL;
    

    // CraftTool
    CCraftTool::CRAFTTOOL_DESC CraftToolDesc{};
    CraftToolDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    CraftToolDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Craft_M_Tool");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CraftTool"),
        TEXT("CraftTool"), &CraftToolDesc)))
        return E_FAIL;

    m_pCraftTool = dynamic_cast<CCraftTool*>(m_PartObjects[TEXT("CraftTool")]);
    Safe_AddRef(m_pCraftTool);
    m_pCraftTool->Set_IsInactive(true);

    // CraftHammer
    CCraftHammer::CRAFTHAMMER_DESC CraftHammerDesc{};
    CraftHammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    CraftHammerDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Craft_R_Hammer");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CraftHammer"),
        TEXT("CraftHammer"), &CraftHammerDesc)))
        return E_FAIL;

    m_pCraftHammer = dynamic_cast<CCraftHammer*>(m_PartObjects[TEXT("CraftHammer")]);
    Safe_AddRef(m_pCraftHammer);
    m_pCraftHammer->Set_IsInactive(true);


    // CBurner
    CBurner::BURNER_DESC BurnerDesc{};
    BurnerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BurnerDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Craft_M_Burner");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Burner"),
        TEXT("Burner"), &BurnerDesc)))
        return E_FAIL;

    m_pBurner = dynamic_cast<CBurner*>(m_PartObjects[TEXT("Burner")]);
    Safe_AddRef(m_pBurner);
    m_pBurner->Set_IsInactive(true);

    // FryingPan
    CFryingPan::FRYINGPAN_DESC FryingPanDesc{};
    FryingPanDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    FryingPanDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Craft_R_Fryingpan");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_FryingPan"),
        TEXT("FryingPan"), &FryingPanDesc)))
        return E_FAIL;

    m_pFryingPan = dynamic_cast<CFryingPan*>(m_PartObjects[TEXT("FryingPan")]);
    Safe_AddRef(m_pFryingPan);
    m_pFryingPan->Set_IsInactive(true);
    

    CInGameHPBar::INGAMEHPBAR_DESC HPBarDesc{};
    HPBarDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    HPBarDesc.fHeight = 2.5f;

    HPBarDesc.pMaxHp = &m_tFinalStat.iHP;
    HPBarDesc.pCurHp = &m_tCurStat.iHP;

    HPBarDesc.pMaxMp = &m_tFinalStat.iMP;
    HPBarDesc.pCurMp = &m_tCurStat.iMP;

    HPBarDesc.vHPColor = _float3{ 0.196f, 0.843f, 0.f };
    HPBarDesc.vMPColor = COLOR_TO_FLOAT(243, 158, 28);

    HPBarDesc.eUnitType = CInGameHPBar::CHAR;

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_InGameHPBar"),
        TEXT("HPBar"), &HPBarDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLiDailin::Bind_ShaderResources()
{
    return S_OK;
} 

void CLiDailin::Key_Input()
{
    // debug
    if (m_pGameInstance->Key_Down(DIK_7)) {
        AddEXP(20);
    }
    if (m_pGameInstance->Key_Down(DIK_8)) {
        AddHP(110);
    }
    if (m_pGameInstance->Key_Down(DIK_9)) {
        AddMP(20);
    }
    if (m_pGameInstance->Key_Down(DIK_0)) {
        TryEquip_AddInven(33);
    }

    // Q
    if (m_pGameInstance->Key_Down(DIK_Q)) {
        ACTION_COMMAND tAction_Command{};
        tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK_Q;

        Process_ActionCommand(tAction_Command);
    }

    // W
    if (m_pGameInstance->Key_Down(DIK_W)) {
        ACTION_COMMAND tAction_Command{};
        tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK_W;

        Process_ActionCommand(tAction_Command);
    }

    // E
    if (m_pGameInstance->Key_Down(DIK_E)) {
        ACTION_COMMAND tAction_Command{};
        tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK_E;

        Process_ActionCommand(tAction_Command);
    }

    // R
    if (m_pGameInstance->Key_Down(DIK_R)) {
        ACTION_COMMAND tAction_Command{};
        tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK_R;

        Process_ActionCommand(tAction_Command);
    }


    if (m_pGameInstance->Mouse_Down(DIMB::RBUTTON))
    {
        COLLISION_RAY_INFO tRayInfo{};
        if(m_pGameInstance->Picking_Object(tRayInfo) == true)
        {
            if (tRayInfo.pColCollider->Get_Layer() == ETOUI(Collision_Layer::ITEMBOX))
            {
                ACTION_COMMAND tAction_Command{};
                tAction_Command.eCommandType = ACTION_COMMAND_TYPE::INTERACT_ITEMBOX;
                tAction_Command.pGameObject = tRayInfo.pColObject;
                tAction_Command.Data_UInt.iIsBox = true;
                Process_ActionCommand(tAction_Command);
                return;
            }
            else if (tRayInfo.pColCollider->Get_Layer() == ETOUI(Collision_Layer::ITEMBOX_COLLECTIBLE))
            {
                ACTION_COMMAND tAction_Command{};
                tAction_Command.eCommandType = ACTION_COMMAND_TYPE::COLLECT;
                tAction_Command.pGameObject = tRayInfo.pColObject;
                Process_ActionCommand(tAction_Command);
                return;
            }
            else if (tRayInfo.pColCollider->Get_Layer() == ETOUI(Collision_Layer::MONSTER))
            {
                ACTION_COMMAND tAction_Command{};
                tAction_Command.pGameObject = tRayInfo.pColObject;

                CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tRayInfo.pColObject);
                if (pMonster == nullptr)
                {
                    MSG_BOX("Bug Point 1: CLiDailin");
                    return;
                }
                
                if(pMonster->IsUnitDead())  // 죽었으면 인벤토리 오픈
                {
                    tAction_Command.Data_UInt.iIsBox = false;
                    tAction_Command.eCommandType = ACTION_COMMAND_TYPE::INTERACT_ITEMBOX;
                }
                else                        // 살았으면 공격
                    tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK;

                Process_ActionCommand(tAction_Command);
                return;
            }
        }
 
        if (m_bCanMoveCancle == true)
        {
            Set_ActionEnd();
        }

        MOVEMENT_COMMAND tMovement_Command{};
        tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
        tMovement_Command.vTargetPos = CInGame_Manager::GetInstance()->MapPIcking();

        Process_MovementCommand(tMovement_Command);
        m_bCanMoveCancle = false;

    }
}

void CLiDailin::CoolTimer(_float fTimeDelta)
{
    // Q
    tQCool.Update_Cool(fTimeDelta);
    tQCool.Update_SubCool(fTimeDelta);

    // W
    tWCool.Update_Cool(fTimeDelta);

    // E
    tECool.Update_Cool(fTimeDelta);

    // R
    tRCool.Update_Cool(fTimeDelta);
}

HRESULT CLiDailin::Initialize_Stat()
{
    SetStat(m_tBaseStat, 20, 40, 970, 100, 2.73f, 0.f, 40, 0, 0, 0, 66, 0.75f, 0, 0, 3.67f);
    SetStat(m_tCurStat, 1, 0, 970, 0, 2.73f, 0.f, 40, 0, 0, 0, 66, 0.75f, 0, 0, 3.67f);

    SetStat(m_tLevelUpStat, 0, 7, 188, 0, 0.63f, 0.f, 5, 0, 0, 0, 3, 0.03f, 0, 0, 0.01f);

    SetFinalStat();

    m_eMPType = MAINGAUGE_TYPE::INTOXICATION;

    return S_OK;
}

HRESULT CLiDailin::Initialize_Skill()
{
    // cool
    tQCool.fMaxCoolDown = tQCool.fCurCoolDown = 5.f;
    tQCool.fMaxSubCoolDown = tQCool.fCurSubCoolDown = 4.f;

    tWCool.fMaxCoolDown = tWCool.fCurCoolDown = 2.f;
    tECool.fMaxCoolDown = tECool.fCurCoolDown = 2.f;
    tRCool.fMaxCoolDown = tRCool.fCurCoolDown = 2.f;

    return S_OK;
}

HRESULT CLiDailin::Initialize_State()
{
    CState* pLiDailinIdle = CLiDailinIdle::Create();
    m_States.emplace(L"Idle", pLiDailinIdle);
    m_States.emplace(L"Move", CLiDailinMove::Create());
    m_States.emplace(L"CLiDailinAttack", CLiDailinAttack::Create(m_fAttackRange));
    m_States.emplace(L"CLiDailin_Q", CLiDailin_Q::Create());
    m_States.emplace(L"CLiDailin_W", CLiDailin_W::Create());
    m_States.emplace(L"CLiDailin_E", CLiDailin_E::Create());
    m_States.emplace(L"CLiDailin_R", CLiDailin_R::Create());

    m_States.emplace(L"CInteract_ItemBox", CInteract_ItemBox::Create());

    m_States.emplace(L"CAction_Craft", CAction_Craft::Create(static_cast<_uint>(LiDailin_Ani::Ani_Craft), static_cast<_uint>(LiDailin_Ani::Ani_Food)));
    m_States.emplace(L"CAction_Collect", CAction_Collect::Create(static_cast<_uint>(LiDailin_Ani::Ani_Collect)));


    m_pCurMovementState = dynamic_cast<CMovementState*>(pLiDailinIdle);
    if (m_pCurMovementState != nullptr)
    {
        m_pCurMovementState->Enter(this);
    }

    m_pCurActionState = nullptr;

    return S_OK;
}

CActionState* CLiDailin::Get_ActionState(const wstring& wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CLiDailin.cpp: No ActionState");
        return nullptr;
    }

    return dynamic_cast<CActionState*>(iter->second);
}

CSkillState* CLiDailin::FindSkill(const SKILL_SLOT eType)
{
    wstring wstrState;
    switch (eType)
    {
        case SKILL_SLOT::Q:
        {
            wstrState = L"CLiDailin_Q";
            break;
        }
        case SKILL_SLOT::W:
        {
            wstrState = L"CLiDailin_W";
            break;
        }
        case SKILL_SLOT::E:
        {
            wstrState = L"CLiDailin_E";
            break;
        }
        case SKILL_SLOT::R:
        {
            wstrState = L"CLiDailin_R";
            break;
        }
    }

    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CLiDailin.cpp: No ActionState");
        return nullptr;
    }

    CSkillState* pSkillState = dynamic_cast<CSkillState*>(iter->second);

    if (pSkillState == nullptr)
    {
        MSG_BOX("CLiDailin.cpp: pSkillState is nullptr");
        return nullptr;
    }

    return pSkillState;
}

CLiDailin* CLiDailin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLiDailin* pInstance = new CLiDailin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLiDailin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLiDailin::Clone(void* pArg)
{
    CLiDailin* pInstance = new CLiDailin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLiDailin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLiDailin::Free()
{
    for (auto& pair : m_States) {
        Safe_Release(pair.second);
    }
    m_States.clear();

    Safe_Release(m_pWeapon);
    Safe_Release(m_pBody);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}