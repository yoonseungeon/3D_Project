#include "CLumia_Ground.h"

#include "CGameInstance.h"

CLumia_Ground::CLumia_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CLumia_Ground::CLumia_Ground(const CLumia_Ground& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CLumia_Ground::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLumia_Ground::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CLumia_Ground::Priority_Update(_float fTimeDelta)
{
}

void CLumia_Ground::Update(_float fTimeDelta)
{
}

void CLumia_Ground::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CLumia_Ground::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
            continue;

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

_float3 CLumia_Ground::Picking()
{
    const POINT ptMouse = m_pGameInstance->Get_MouseClientPos();

    _uint iNumViewport = { 1 };
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

    _float4 vMouse = { (static_cast<_float>(ptMouse.x) / ViewportDesc.Width) * 2.f -1.f,
                        (static_cast<_float>(ptMouse.y) / ViewportDesc.Height) * -2.f + 1.f,
                         0.f,
                         1.f
                     };

    // 투영
    XMVECTOR vMouseViewPos = XMVector3TransformCoord(XMLoadFloat4(&vMouse),
            XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))
    );

    // 뷰 스페이스
    XMVECTOR vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    XMVECTOR vRayDir = vMouseViewPos - vRayPos;

    vRayPos = XMVector3TransformCoord(vRayPos,
        XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))
    );

    vRayDir = XMVector3TransformNormal(vRayDir,
        XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))
    );


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

        if (strMeshName.find("Base") == string::npos && strMeshName.find("RegionBorder") == string::npos && strMeshName.find("Ground") == string::npos) {
            continue;
        }

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

    _float3 vPos = {};

    if (bSuccess)
    {        
        XMStoreFloat3(&vPos, XMVector3TransformCoord(vRayPos + vRayDir * fShortDist,
                XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()
                )
            )       
        );
    }

    return vPos;
}

HRESULT CLumia_Ground::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Lumia_Ground"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC NaviDesc{};

    NaviDesc.bIsGround = true;
    NaviDesc.pParentMarix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLumia_Ground::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CLumia_Ground* CLumia_Ground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLumia_Ground* pInstance = new CLumia_Ground(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLumia_Ground");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLumia_Ground::Clone(void* pArg)
{
    CLumia_Ground* pInstance = new CLumia_Ground(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLumia_Ground");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLumia_Ground::Free()
{
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
