#include "CItemBox.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"
#include "CInGame_Manager.h"

#include "CInventory.h"
#include "CAbstractPlayer.h"

CItemBox::CItemBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInvenOwner{ pDevice, pContext }
{
}

CItemBox::CItemBox(const CItemBox& Prototype)
    : CInvenOwner{ Prototype }
{
}

HRESULT CItemBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemBox::Initialize(void* pArg)
{
    ITEMBOX_DESC* pDesc = static_cast<ITEMBOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->wstrModelPrototypeTag)))
        return E_FAIL;

    m_pTransformCom->Set_Rotation(pDesc->vQuaternion);
    m_pTransformCom->Set_Scale(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

    m_eSpawnArea = pDesc->eSpawnArea;
    if (FAILED(Generate_Item()))
        return E_FAIL;

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    return S_OK;
}

void CItemBox::Priority_Update(_float fTimeDelta)
{

}

void CItemBox::Parallel_Update(_float fTimeDelta)
{
}

void CItemBox::Update(_float fTimeDelta)
{

}

void CItemBox::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CItemBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

_bool CItemBox::TakeItemToInventory(_uint iSlotIndex)
{
    if (iSlotIndex >= m_pInvetory->Get_InventorySize())
        return false;

    _uint iItemCnt{};
    _int iItemId = m_pInvetory->FindItemIdBySlotIndex(iSlotIndex, iItemCnt);

    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();

    if (pPlayer == nullptr)
        return false;

    _bool bResult = pPlayer ->TryEquip_AddInven(iItemId, iItemCnt);

    if (bResult == true)
    {
        m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex, iItemCnt);
        m_pInvetory->PullSlots();
    }

    return bResult;
}

_bool XM_CALLCONV CItemBox::IsInOpenRange(_fvector vPos, _float fWorldDistance)
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

HRESULT CItemBox::Ready_Components(wstring wstrModelPrototypeTag)
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), wstrModelPrototypeTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    CCollider* pColliderCom;

    /* For.Com_Collider_AABB */
    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{ };
    OBBDesc.pLocalXYZ = m_pModelCom->Get_LocalXYZ();

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&pColliderCom), &OBBDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);

    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ITEMBOX));

    return S_OK;
}

HRESULT CItemBox::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CItemBox::Generate_Item()
{
    _uint iGenerateItemCnt{};

    const unordered_map<_uint, ITEM_DESC>& ItemInfos = CItem_Manager::GetInstance()->Get_ItemInfos();

    for (const auto& pair : ItemInfos)
    {
        if (iGenerateItemCnt == m_iMaxSize) {
            return S_OK;
        }

        if (ETOUI(pair.second.eSpawnMap) & ETOUI(m_eSpawnArea)) {
            if(rand() % 100 <= 60)
            {
                m_pInvetory->Add_Item(pair.second.iItemID);
                ++iGenerateItemCnt;
            }
        }
    }

    return S_OK;
}

CItemBox* CItemBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItemBox* pInstance = new CItemBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CItemBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItemBox::Clone(void* pArg)
{
    CItemBox* pInstance = new CItemBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CItemBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemBox::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}