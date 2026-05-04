#include "CItemSpawner.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CInventory.h"
#include "CAbstractPlayer.h"

CItemSpawner::CItemSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInvenOwner{ pDevice, pContext }
{
}

CItemSpawner::CItemSpawner(const CItemSpawner& Prototype)
    : CInvenOwner{ Prototype }
{
}

HRESULT CItemSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemSpawner::Initialize(void* pArg)
{
    ITEMSPAWNER_DESC* pDesc = static_cast<ITEMSPAWNER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_Rotation(pDesc->vQuaternion);
    m_pTransformCom->Set_Scale(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

    m_eSpawnArea = pDesc->eSpawnArea;

    return S_OK;
}

void CItemSpawner::Priority_Update(_float fTimeDelta)
{

}

void CItemSpawner::Parallel_Update(_float fTimeDelta)
{
}

void CItemSpawner::Update(_float fTimeDelta)
{

}

void CItemSpawner::Late_Update(_float fTimeDelta)
{
}

HRESULT CItemSpawner::Render()
{
    return S_OK;
}

_bool CItemSpawner::TakeItemToInventory(_uint iSlotIndex)
{
    if (iSlotIndex >= m_pInvetory->Get_InventorySize())
        return false;

    _uint iItemCnt{};
    _int iItemId = m_pInvetory->FindItemIdBySlotIndex(iSlotIndex, iItemCnt);

    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();

    if (pPlayer == nullptr)
        return false;

    _bool bResult = pPlayer->TryEquip_AddInven(iItemId, iItemCnt);

    if (bResult == true)
    {
        m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex, iItemCnt);
        m_pInvetory->PullSlots();
    }

    return bResult;
}

_bool XM_CALLCONV CItemSpawner::IsInOpenRange(_fvector vPos, _float fWorldDistance)
{
    _matrix matWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    _matrix matWorldInverse = XMMatrixInverse(nullptr, matWorld);

    _vector vPlayerLocalPos = XMVector3TransformCoord(vPos, matWorldInverse);

    _float fX = XMVectorGetX(vPlayerLocalPos);
    _float fY = XMVectorGetY(vPlayerLocalPos);
    _float fZ = XMVectorGetZ(vPlayerLocalPos);

    const MODEL_LOCAL_MIN_MAX* pLocalXYZ = m_pModelCom->Get_LocalXYZ();

    MyHelper::FloatClamp(fX, pLocalXYZ->vMin.x, pLocalXYZ->vMax.x);
    MyHelper::FloatClamp(fY, pLocalXYZ->vMin.y, pLocalXYZ->vMax.y);
    MyHelper::FloatClamp(fZ, pLocalXYZ->vMin.z, pLocalXYZ->vMax.z);

    _vector vWorldClosePoint = XMVector3TransformCoord(XMVectorSet(fX, fY, fZ, 1.f), matWorld);

    // 2D로 볼 때 가장 가까운 거리는 아닌데... 쓸만함.
    _float fLength = XMVectorGetX(
        XMVector3Length(
            XMVectorSetY(vWorldClosePoint, 0.f) - XMVectorSetY(vPos, 0.f)
        )
    );

    if (fLength <= fWorldDistance)
        return true;

    return false;
}

void CItemSpawner::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}