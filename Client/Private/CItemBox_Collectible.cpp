#include "CItemBox_Collectible.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"
#include "CInGame_Manager.h"

#include "CInventory.h"
#include "CAbstractPlayer.h"

CItemBox_Collectible::CItemBox_Collectible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CItemSpawner{ pDevice, pContext }
{
}

CItemBox_Collectible::CItemBox_Collectible(const CItemBox_Collectible& Prototype)
    : CItemSpawner{ Prototype }
{
}

HRESULT CItemBox_Collectible::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemBox_Collectible::Initialize(void* pArg)
{
    ITEMBOX_COLLECTIBLE_DESC* pDesc = static_cast<ITEMBOX_COLLECTIBLE_DESC*>(pArg);

    m_iItemId = pDesc->iItemId;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->wstrModelPrototypeTag)))
        return E_FAIL;

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    return S_OK;
}

void CItemBox_Collectible::Priority_Update(_float fTimeDelta)
{

}

void CItemBox_Collectible::Parallel_Update(_float fTimeDelta)
{
}

void CItemBox_Collectible::Update(_float fTimeDelta)
{

}

void CItemBox_Collectible::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(STATE::POSITION), 2.f) == false)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CItemBox_Collectible::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (m_pModelCom->Get_MaterialCount(i, MyTextureType_NORMALS) == 0)
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(1)))
                return E_FAIL;
        }

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CItemBox_Collectible::Ready_Components(wstring wstrModelPrototypeTag)
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
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ITEMBOX_COLLECTIBLE));
    pColliderCom->Set_Active(false);

    return S_OK;
}

HRESULT CItemBox_Collectible::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CItemBox_Collectible* CItemBox_Collectible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItemBox_Collectible* pInstance = new CItemBox_Collectible(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CItemBox_Collectible");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItemBox_Collectible::Clone(void* pArg)
{
    CItemBox_Collectible* pInstance = new CItemBox_Collectible(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CItemBox_Collectible");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemBox_Collectible::Free()
{
    __super::Free();
}