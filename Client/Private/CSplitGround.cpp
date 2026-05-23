#include "CSplitGround.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

CSplitGround::CSplitGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CSplitGround::CSplitGround(const CSplitGround& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CSplitGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSplitGround::Initialize(void* pArg)
{
    SPLIT_GROUND_DESC* pDesc = static_cast<SPLIT_GROUND_DESC*>(pArg);

    m_wstrPrototype_Component_Tag = pDesc->wstrPrototype_Component_Tag;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pInGame_Manager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pInGame_Manager);

    return S_OK;
}

void CSplitGround::Priority_Update(_float fTimeDelta)
{
}

void CSplitGround::Update(_float fTimeDelta)
{
}

void CSplitGround::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CSplitGround::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (m_pModelCom->Get_MaterialCount(i, MyTextureType_DIFFUSE) == 0)
        {
            continue;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
            return E_FAIL;
    }

#ifdef _DEBUG
    //m_pNavigationCom->Render();
#endif

    return S_OK;
}

void CSplitGround::OnMouse_Over_All(const COLLISION_RAY_INFO& tRayInfo)
{
    m_pInGame_Manager->Add_SplitGround(this);
}

_bool CSplitGround::Picking(_float3& vOutPos)
{
    vOutPos = { 0.f, 0.f, 0.f };

    _float4 vWorldRayPos{};
    _float4 vWorldRayDir{};
    m_pGameInstance->Get_WorldRay(vWorldRayPos, vWorldRayDir);

    XMVECTOR vRayPos{};
    XMVECTOR vRayDir{};

    vRayPos = XMLoadFloat4(&vWorldRayPos);
    vRayDir = XMLoadFloat4(&vWorldRayDir);

    // 월드
    XMMATRIX matInvWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    vRayPos = XMVector3TransformCoord(vRayPos, matInvWorld);

    vRayDir = XMVector3TransformNormal(vRayDir, matInvWorld);


    vRayDir = XMVector3Normalize(vRayDir);

    _float fShortDist = FLT_MAX;
    _float fDist{};
    _bool bSuccess{};

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        const string strMeshName = m_pModelCom->Get_MeshName(i);

 /*       if (strMeshName.find("Base") == string::npos && strMeshName.find("RegionBorder") == string::npos && strMeshName.find("Ground") == string::npos) {
            continue;
        }*/

        const vector<_uint>& vecIdxData = m_pModelCom->Get_IdxData(i);
        if (vecIdxData.size() == 0) {
            continue;
        }

        const vector<_float3>& vecVtxData = m_pModelCom->Get_VtxData(i);

        _int iIdx = { -1 };

        for (_uint j = 0; j < vecIdxData.size() / 3; ++j)
        {
            _bool bCollision = TriangleTests::Intersects(vRayPos, vRayDir,
                XMVectorSetW(XMLoadFloat3(&vecVtxData[vecIdxData[++iIdx]]), 1.f),
                XMVectorSetW(XMLoadFloat3(&vecVtxData[vecIdxData[++iIdx]]), 1.f),
                XMVectorSetW(XMLoadFloat3(&vecVtxData[vecIdxData[++iIdx]]), 1.f),
                fDist
            );
            if (bCollision) {
                if (fShortDist > fDist) {
                    fShortDist = fDist;
                    bSuccess = true;
                }
            }
        }
    }

    if (bSuccess)
    {
        XMStoreFloat3(&vOutPos, XMVector3TransformCoord(vRayPos + vRayDir * fShortDist,
            XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()
            )
        ));

        return true;
    }

    return false;
}

HRESULT CSplitGround::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), m_wstrPrototype_Component_Tag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    CCollider* pColliderCom;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{ };
    AABBDesc.pLocalXYZ = m_pModelCom->Get_LocalXYZ();

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    m_Colliders.push_back(pColliderCom);

    pColliderCom->Set_Layer(ETOUI(Collision_Layer::GROUND));
    pColliderCom->Set_CanMousePicking(false);

    m_pGameInstance->Add_PickingCollider(pColliderCom);

    return S_OK;
}

HRESULT CSplitGround::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CSplitGround* CSplitGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSplitGround* pInstance = new CSplitGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CSplitGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSplitGround::Clone(void* pArg)
{
    CSplitGround* pInstance = new CSplitGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CSplitGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSplitGround::Free()
{
    Safe_Release(m_pInGame_Manager);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
