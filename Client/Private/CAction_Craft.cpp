#include "CAction_Craft.h"

#include "CGameInstance.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CCraftTool.h"
#include "CCraftHammer.h"
#include "CBurner.h"
#include "CFryingPan.h"

#include "CItem_Manager.h"
#include "CInventory.h"

CAction_Craft::CAction_Craft()
{
}

HRESULT CAction_Craft::Initialize(_uint iCraftMetalAni, _uint iCraftFoodAni)
{
    m_iCraftMetalAni = iCraftMetalAni;
    m_iCraftFoodAni = iCraftFoodAni;
    return S_OK;
}

void CAction_Craft::Enter(CLiDailin* pPlayer)
{
    m_iItemId = pPlayer->Get_CurActionCommand().Data_UInt.iItemIdx;

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(m_iItemId);
    m_eItemType = pItemDesc->eType;

    m_fMaxTime = CItem_Manager::GetInstance()->Get_CurItemCraftTime(m_iItemId);

    // Ani
    if (m_eItemType == ITEM_TYPE::FOOD)
    {
        pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(m_iCraftFoodAni, false);

        CBurner* pBurner = pPlayer->Get_Burner();
        pBurner->Set_IsInactive(false);

        CFryingPan* pFryingPan = pPlayer->Get_FryingPan();
        pFryingPan->Set_IsInactive(false);
        pFryingPan->Get_ModelCom()->Set_AnimationIndex(CFryingPan::CRAFT, false);
    }
    else
    {
        pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(m_iCraftMetalAni, false);
        CCraftTool* pCraftTool = pPlayer->Get_CraftTool();
        pCraftTool->Set_IsInactive(false);
        pCraftTool->Get_ModelCom()->Set_AnimationIndex(CCraftTool::APPEAR, false);
    }
    pPlayer->Get_Weapon()->Set_IsInactive(true);
    pPlayer->Set_MovementAniBlock(true);
    
    // Ani Speed

    // 이동
    pPlayer->Set_MoveBlock(true);
    pPlayer->Set_WaitMovementState(L"Idle");

    pPlayer->Set_CanMoveCancle(true);
}

void CAction_Craft::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
    m_fAccTime += fTimeDelta;

    CMyModel* pPlayerBodyModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();

    if (pPlayerBodyModel->IsAnimationFinished() == true
        || m_fAccTime >= m_fMaxTime)
    {
        pPlayer->Craft_Item(m_iItemId);
        pPlayer->Set_ActionEnd();
    }

    if (m_eItemType == ITEM_TYPE::FOOD)
    {
 
    }
    else
    {
        const _float fGrabHammerTime = 0.1f;
        if (m_bOnHammer == false && pPlayerBodyModel->Get_CurAniPlayRatio() >= fGrabHammerTime)
        {
            pPlayer->Get_CraftHammer()->Set_IsInactive(false);
            m_bOnHammer = true;
        }

        CCraftTool* pCraftTool = pPlayer->Get_CraftTool();
        if (pCraftTool->Get_ModelCom()->IsAnimationFinished() == true) {
            pCraftTool->Get_ModelCom()->Set_AnimationIndex(CCraftTool::CRAFT, false);
        }
    }

}

void CAction_Craft::Exit(CLiDailin* pPlayer)
{
    m_iItemId = -1;
    m_fAccTime = 0.f;

    // Ani
    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
    pPlayer->Get_Weapon()->Set_IsInactive(false);
    pPlayer->Set_MovementAniBlock(false);
    if (m_eItemType == ITEM_TYPE::FOOD)
    {
        pPlayer->Get_Burner()->Set_IsInactive(true);
        pPlayer->Get_FryingPan()->Set_IsInactive(true);
    }
    else
    {
        pPlayer->Get_CraftHammer()->Set_IsInactive(true);
        m_bOnHammer = false;
        pPlayer->Get_CraftTool()->Set_IsInactive(true);
    }


    // 이동
    pPlayer->Set_MoveBlock(false);

    pPlayer->Set_CanMoveCancle(false);
}

void CAction_Craft::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
 
}

CAction_Craft* CAction_Craft::Create(_uint iCraftMetalAni, _uint iCraftFoodAni)
{
    CAction_Craft* pInstance = new CAction_Craft();

    if (FAILED(pInstance->Initialize(iCraftMetalAni, iCraftFoodAni)))
    {
        MSG_BOX("Failed to Created: CAction_Craft");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAction_Craft::Free()
{
    __super::Free();
}
