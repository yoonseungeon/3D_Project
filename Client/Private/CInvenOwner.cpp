#include "CInvenOwner.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CPartObject.h"
#include "CInventory.h"
#include "CAbstractPlayer.h"

CInvenOwner::CInvenOwner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CInvenOwner::CInvenOwner(const CInvenOwner& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CInvenOwner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInvenOwner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pInvetory = CInventory::Create();

    return S_OK;
}

void CInvenOwner::Priority_Update(_float fTimeDelta)
{
    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CInvenOwner::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }
}

void CInvenOwner::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CInvenOwner::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }
}

HRESULT CInvenOwner::Render()
{
    return S_OK;
}

_bool XM_CALLCONV CInvenOwner::IsInOpenRange(_fvector vPos, _float fWorldDistance)
{
    return false;
}

_bool CInvenOwner::TakeItemToInventory(_uint iSlotIndex)
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

void CInvenOwner::PlayOpenSound()
{
}

HRESULT CInvenOwner::Bind_OutLineShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_RawValue("g_OutLineLength", &m_fOutLineLength, sizeof(m_fOutLineLength))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_OutLineColor", &m_vOutLineColor, sizeof(m_vOutLineColor))))
        return E_FAIL;

    return S_OK;
}

void CInvenOwner::Free()
{
    Safe_Release(m_pInvetory);

    __super::Free();
}