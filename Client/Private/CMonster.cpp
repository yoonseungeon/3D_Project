#include "CMonster.h"

#include "CGameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CMonster::CMonster(const CMonster& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimationIndex(0, true);

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(
            m_pGameInstance->Random(0.f, 30.f),
            0.1f,
            m_pGameInstance->Random(0.f, 30.f),
            1.f
        ));

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Parallel_Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CMonster::Update(_float fTimeDelta)
{
}

void CMonster::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CMonster::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMonster::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Craft_Tool"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    CCollider* pColliderCom;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{ };
    AABBDesc.vSize = _float3(0.7f, 0.2f, 0.7f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);


    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));


    /* For.Com_Collider_Sphere */
    CBounding_Sphere::BOUNDING_SPHERE_DESC  SphereDesc{ };
    SphereDesc.fRadius = 0.7f;
    SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&pColliderCom), &SphereDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);


    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));


    /* For.Com_Collider_OBB */
    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{ };
    OBBDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * 0.5f, 0.f);
    OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&pColliderCom), &OBBDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);


    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));


    return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster* pInstance = new CMonster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
