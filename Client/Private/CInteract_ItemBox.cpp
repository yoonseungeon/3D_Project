#include "CInteract_ItemBox.h"

#include "CGameInstance.h"
#include "CSharedUI_Manager.h"

#include "CLiDailin.h"

CInteract_ItemBox::CInteract_ItemBox()
{
}

HRESULT CInteract_ItemBox::Initialize()
{
    return S_OK;
}

void CInteract_ItemBox::Enter(CLiDailin* pPlayer)
{
    pPlayer->Set_CanMoveCancle(true);

    Move_OR_Interact(pPlayer);
}

void CInteract_ItemBox::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
    if (m_bIsOpenUI == true)
        return;

    Move_OR_Interact(pPlayer);
}

void CInteract_ItemBox::Exit(CLiDailin* pPlayer)
{
    m_bIsOpenUI = false;
    CSharedUI_Manager::GetInstance()->PopDown_ItemBoxUI();

    pPlayer->Set_CanMoveCancle(false);
}

void CInteract_ItemBox::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
    pPlayer->DefaultProcess_ActionCommand(eAction_Command);
}

void CInteract_ItemBox::MoveToPos(CLiDailin* pPlayer, _float3& vPos)
{
    MOVEMENT_COMMAND tMovement_Command{};
    tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
    tMovement_Command.vTargetPos = vPos;

    pPlayer->Process_MovementCommand(tMovement_Command);
}

void CInteract_ItemBox::Move_OR_Interact(CLiDailin* pPlayer)
{
    const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();

    CInvenOwner* pInvenOwner = dynamic_cast<CInvenOwner*>(tAction_Command.pGameObject);
    if (pInvenOwner == nullptr)
        MSG_BOX("Bug Point 1: CInteract_ItemBox");

    CTransform* pTransform = dynamic_cast<CTransform*>(pInvenOwner->Find_Component(g_strTransformTag));
    if (pTransform == nullptr)
        MSG_BOX("Bug Point 2: CInteract_ItemBox");

    _vector vMyPos = pPlayer->Get_TransformCom()->Get_State(STATE::POSITION);

    const _float fDistance = 0.3f;
    if (pInvenOwner->IsInOpenRange(vMyPos, fDistance) == false)
    {
        _vector vTargetPos = pTransform->Get_State(STATE::POSITION);
        _float3 vPos{};
        XMStoreFloat3(&vPos, vTargetPos);
        MoveToPos(pPlayer, vPos);
    }
    else
    {
        CSharedUI_Manager::GetInstance()->PopUp_ItemBoxUI(pInvenOwner);
        pPlayer->Set_WaitMovementState(L"Idle");
        m_bIsOpenUI = true;
    }
}

CInteract_ItemBox* CInteract_ItemBox::Create()
{
    CInteract_ItemBox* pInstance = new CInteract_ItemBox();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CInteract_ItemBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteract_ItemBox::Free()
{
    __super::Free();
}
