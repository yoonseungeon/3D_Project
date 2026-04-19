#include "CPlayer.h"

#include "CGameInstance.h"

#include "CBody_Player.h"
#include "CWeapon.h"
#include "CBottle.h"

#include "CInGame_Manager.h"

#include "CLiDailinIdle.h"
#include "CLiDailinMove.h"
#include "CLiDailinAttack.h"
#include "CLiDailin_Q.h"
#include "CLiDailin_W.h"
#include "CLiDailin_E.h"
#include "CLiDailin_R.h"

#include "CUI_StackSkillIcon.h"
#include "CUI_NormalSkillIcon.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    if (pArg == nullptr) {
        GAMEOBJECT_DESC Desc{};

        Desc.tTransformDesc.fSpeedPerSec = 10.f;
        Desc.tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);

        if (FAILED(__super::Initialize(&Desc)))
            return E_FAIL;
    }
    else {
        GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

        pDesc->tTransformDesc.fSpeedPerSec = 10.f;
        pDesc->tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);

        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(4.f, 0.f, -2.8f, 1.f));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    
    CState* pLiDailinIdle = CLiDailinIdle::Create();
    m_States.emplace(L"Idle", pLiDailinIdle);
    m_States.emplace(L"Move", CLiDailinMove::Create());
    m_States.emplace(L"CLiDailinAttack", CLiDailinAttack::Create());
    m_States.emplace(L"CLiDailin_Q", CLiDailin_Q::Create());
    m_States.emplace(L"CLiDailin_W", CLiDailin_W::Create());
    m_States.emplace(L"CLiDailin_E", CLiDailin_E::Create());
    m_States.emplace(L"CLiDailin_R", CLiDailin_R::Create());

    
    m_pCurMovementState = dynamic_cast<CMovementState*>(pLiDailinIdle);
    if (m_pCurMovementState != nullptr)
    {
        m_pCurMovementState->Enter(this);
    }

    m_pCurActionState = nullptr;

    // cool
    tQCool.fMaxCoolDown = tQCool.fCurCoolDown = 2.f;
    tQCool.fMaxSubCoolDown = tQCool.fCurSubCoolDown = 4.f;

    tWCool.fMaxCoolDown = tWCool.fCurCoolDown = 2.f;
    tECool.fMaxCoolDown = tECool.fCurCoolDown = 2.f;
    tRCool.fMaxCoolDown = tRCool.fCurCoolDown = 2.f;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
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

    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CPlayer::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CPlayer::Update(_float fTimeDelta)
{    
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    m_pNavigationCom->Render();
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CPlayer::Set_WaitMovementState(const wstring& wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CPlayer.cpp: No MovementState");
        m_pWaitMovementState = nullptr;
        return;
    }
    
    m_pWaitMovementState = dynamic_cast<CMovementState*>(iter->second);
}

void CPlayer::Apply_WaitMovementState()
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

void CPlayer::Process_MovementCommand(MOVEMENT_COMMAND& tMovement_Command)
{
    m_pCurMovementState->HandleMovementCommand(this, tMovement_Command);
}

void CPlayer::Set_WaitActionState(const wstring& wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CPlayer.cpp: No ActionState");
        m_pWaitActionState = nullptr;
        return;
    }

    m_pWaitActionState = dynamic_cast<CActionState*>(iter->second);
}

void CPlayer::Apply_WaitActionState()
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

void CPlayer::Process_ActionCommand(ACTION_COMMAND& tAction_Command)
{
    if (m_pCurActionState != nullptr) {
        m_pCurActionState->HandleActionCommand(this, tAction_Command);
        return;
    }

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
            if(CanUseSkill(L"Q") == true)
            {
                Set_CurActionCommand(tAction_Command);
                Set_WaitActionState(L"CLiDailin_Q");
            }
            break;
        }

        case ACTION_COMMAND_TYPE::ATTACK_W:
        {
            if (CanUseSkill(L"W") == true)
            {
                Set_CurActionCommand(tAction_Command);
                Set_WaitActionState(L"CLiDailin_W");
            }
            break;
        }

        case ACTION_COMMAND_TYPE::ATTACK_E:
        {
            if (CanUseSkill(L"E") == true)
            {
                Set_CurActionCommand(tAction_Command);
                Set_WaitActionState(L"CLiDailin_E");
            }
            break;
        }

        case ACTION_COMMAND_TYPE::ATTACK_R:
        {
            if (CanUseSkill(L"R") == true)
            {
                Set_CurActionCommand(tAction_Command);
                Set_WaitActionState(L"CLiDailin_R");
            }
            break;
        }
    }
}

_bool CPlayer::Update_Move_To_Pos(_float fTimeDelta)
{
    return m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
}

void CPlayer::Move_To_Pos(_float3 vPos, _bool bOperateNavi)
{
    m_pMoveCom->Move_To_Pos(vPos, bOperateNavi);
}

void CPlayer::Stop_Move_To_Pos()
{
    m_pMoveCom->Stop_Move_To_Pos();
}

COOL_INFO* CPlayer::Get_CoolInfo(const _tchar* SkillName)
{
    if (SkillName == L"W")
    {
        return &tWCool;
    }
    else if (SkillName == L"E")
    {
        return &tECool;
    }
    else if (SkillName == L"R")
    {
        return &tRCool;
    }

    return nullptr;
}

_bool CPlayer::CanUseSkill(const _tchar* SkillName)
{
    if (SkillName == L"Q")
    {
        if (tQCool.fAccCoolDown == 0.f) {
            return true;
        }
    }
    if (SkillName == L"W")
    {
        if (tWCool.fAccCoolDown == 0.f) {
            return true;
        }
    }
    else if (SkillName == L"E")
    {
        if (tECool.fAccCoolDown == 0.f) {
            return true;
        }
    }
    else if (SkillName == L"R")
    {
        if (tRCool.fAccCoolDown == 0.f) {
            return true;
        }
    }

    return false;
}

HRESULT CPlayer::Ready_Components()
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

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{ };
    AABBDesc.vSize = _float3(0.7f, 0.2f, 0.7f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
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

    return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
    return S_OK;
}

HRESULT CPlayer::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    // Q
    CUI_StackSkillIcon::CUI_STACKSKILLICON_DESC StackSkillIconDesc{};

    StackSkillIconDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    StackSkillIconDesc.iUILayer = ETOUI(UILAYER::SLOT);

    StackSkillIconDesc.fScaleRatioX = 0.04f;
    StackSkillIconDesc.fScaleRatioY = 0.071f;
    StackSkillIconDesc.fPosRatioX = -0.07f;
    StackSkillIconDesc.fPosRatioY = -0.4f;
    StackSkillIconDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_Q";
    StackSkillIconDesc.eBlendState = CUI_Default::DEFAULT;

    StackSkillIconDesc.pStackCoolInfo = &tQCool;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StackSkillIcon"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &StackSkillIconDesc)))
        return E_FAIL;

    // W
    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC WIconDesc{};

    WIconDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    WIconDesc.iUILayer = ETOUI(UILAYER::SLOT);
    
    WIconDesc.fScaleRatioX = 0.04f;
    WIconDesc.fScaleRatioY = 0.071f;
    WIconDesc.fPosRatioX = 0.0f;
    WIconDesc.fPosRatioY = -0.4f;
    WIconDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_W";
    WIconDesc.eBlendState = CUI_Default::DEFAULT;
    
    WIconDesc.pCoolInfo = &tWCool;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_NormalSkillIcon"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &WIconDesc)))
        return E_FAIL;

    // E
    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC EIconDesc{};

   EIconDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
   EIconDesc.iUILayer = ETOUI(UILAYER::SLOT);
   
   EIconDesc.fScaleRatioX = 0.04f;
   EIconDesc.fScaleRatioY = 0.071f;
   EIconDesc.fPosRatioX = 0.07f;
   EIconDesc.fPosRatioY = -0.4f;
   EIconDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_E";
   EIconDesc.eBlendState = CUI_Default::DEFAULT;

   EIconDesc.pCoolInfo = &tECool;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_NormalSkillIcon"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &EIconDesc)))
        return E_FAIL;

    // R
    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC RIconDesc{};

    RIconDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    RIconDesc.iUILayer = ETOUI(UILAYER::SLOT);
    
    RIconDesc.fScaleRatioX = 0.04f;
    RIconDesc.fScaleRatioY = 0.071f;
    RIconDesc.fPosRatioX = 0.14f;
    RIconDesc.fPosRatioY = -0.4f;
    RIconDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_R";
    RIconDesc.eBlendState = CUI_Default::DEFAULT;
    
    RIconDesc.pCoolInfo = &tRCool;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_NormalSkillIcon"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &RIconDesc)))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Key_Input()
{
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
        // if(몬스터 클릭)
        if (m_pGameInstance->Key_Pressing(DIK_A) /* 몬스터 이면 */)
        {
            ACTION_COMMAND tAction_Command{};
            tAction_Command.eCommandType = ACTION_COMMAND_TYPE::ATTACK;
            // 몬스터 포인터 넣고
            tAction_Command.pGameObject = nullptr;

            // 테스트용 위치
            tAction_Command.vTargetPos = CInGame_Manager::GetInstance()->MapPIcking();

            Process_ActionCommand(tAction_Command);
        }
        else
        {
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
}

void CPlayer::CoolTimer(_float fTimeDelta)
{
    // Q
    if (tQCool.fAccCoolDown > 0.f)
    {
        tQCool.fAccCoolDown -= fTimeDelta;
        if (tQCool.fAccCoolDown < 0.f) {
            tQCool.fAccCoolDown = 0.f;
        }
    }

    if (tQCool.fAccSubCoolDown > 0.f)
    {
        tQCool.fAccSubCoolDown -= fTimeDelta;
        if (tQCool.fAccSubCoolDown < 0.f) {
            tQCool.fAccSubCoolDown = 0.f;

            tQCool.fAccCoolDown = tQCool.fCurCoolDown;
            tQCool.fStack = 0;
        }
    }

    // W
    if (tWCool.fAccCoolDown > 0.f && tWCool.bCoolWait == false)
    {
        tWCool.fAccCoolDown -= fTimeDelta;
        if (tWCool.fAccCoolDown < 0.f) {
            tWCool.fAccCoolDown = 0.f;
        }
    }

    // E
    if (tECool.fAccCoolDown > 0.f && tECool.bCoolWait == false)
    {
        tECool.fAccCoolDown -= fTimeDelta;
        if (tECool.fAccCoolDown < 0.f) {
            tECool.fAccCoolDown = 0.f;
        }
    }


    // R
    if (tRCool.fAccCoolDown > 0.f && tRCool.bCoolWait == false)
    {
        tRCool.fAccCoolDown -= fTimeDelta;
        if (tRCool.fAccCoolDown < 0.f) {
            tRCool.fAccCoolDown = 0.f;
        }
    }
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    for (auto& pair : m_States) {
        Safe_Release(pair.second);
    }
    m_States.clear();

    Safe_Release(m_pWeapon);
    Safe_Release(m_pBody);

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
