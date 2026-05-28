#include "CUI_MiniMap.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"
#include "CUI_Image.h"

CUI_MiniMap::CUI_MiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_MiniMap::CUI_MiniMap(const CUI_MiniMap& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_MiniMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MiniMap::Initialize(void* pArg)
{
    CUI_MINIMAP_DESC* pDesc = static_cast<CUI_MINIMAP_DESC*>(pArg);

    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_PlayerIcon(L"Layer_PlayerIcon")))
        return E_FAIL;

    m_pInGame_Manager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pInGame_Manager);


    Init_MiniMap();

    return S_OK;
}

void CUI_MiniMap::Priority_Update(_float fTimeDelta)
{
}

void CUI_MiniMap::Parallel_Update(_float fTimeDelta)
{
}

void CUI_MiniMap::Update(_float fTimeDelta)
{
}

void CUI_MiniMap::Late_Update(_float fTimeDelta)
{
    Update_MiniMapPos();

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_MiniMap::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(BLENDSTATE::MINIMAP))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_MiniMap::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_MiniMap"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_MiniMap::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCenterUV",&m_vCenter,sizeof(m_vCenter))))
        return E_FAIL;

    return S_OK;
}

_float2 CUI_MiniMap::Rotate_2D(const _float2& v, _float fAngle)
{
    _float s = sinf(fAngle);
    _float c = cosf(fAngle);

    return _float2(v.x * c - v.y * s, v.x * s + v.y * c);
}

void CUI_MiniMap::Init_MiniMap()
{
    m_fWorldMinX = { -177.880371f };
    m_fWorldMaxX = { 185.200012f };

    m_fWorldMinZ = { -88.982605f };
    m_fWorldMaxZ = { 225.459763f };

    m_vWorldCenter.x = (m_fWorldMinX + m_fWorldMaxX) * 0.5f;
    m_vWorldCenter.y = (m_fWorldMinZ + m_fWorldMaxZ) * 0.5f;

    m_vWorldHalf.x = (m_fWorldMaxX - m_fWorldMinX) * 0.5f;
    m_vWorldHalf.y = (m_fWorldMaxZ - m_fWorldMinZ) * 0.5f;

    m_fMiniMapRotRadian = XMConvertToRadians(45.f);

    // 억지로 맞춤 비율, 위치...
    m_vCenterUV = { 0.525f, 0.505f };
    m_vMinimapUVSize = { 0.5f, 0.5f };

    _float fTexLength = 988.f;
    _float fUHalfMargin = 99.f;
    _float fActiveHalfUV = (fTexLength - (fUHalfMargin * 2.f)) * 0.5f;
    _float fActiveRadiusUV = fActiveHalfUV / fTexLength;

    m_fWorldToMiniMapScale = fActiveRadiusUV / m_vWorldHalf.y;
} 

void CUI_MiniMap::Update_MiniMapPos()
{
    _float3 vPlayerPos = m_pInGame_Manager->Get_PlayerPos();

    _float2 vPlayerDelta{};
    vPlayerDelta.x = vPlayerPos.x - m_vWorldCenter.x;
    vPlayerDelta.y = -(vPlayerPos.z - m_vWorldCenter.y);

    _float2 vRotPlayerPos = Rotate_2D(vPlayerDelta, m_fMiniMapRotRadian);

    vRotPlayerPos.x *= m_fWorldToMiniMapScale;
    vRotPlayerPos.y *= m_fWorldToMiniMapScale;

    m_vCenter.x = m_vCenterUV.x + vRotPlayerPos.x;
    m_vCenter.y = m_vCenterUV.y + vRotPlayerPos.y;
}

HRESULT CUI_MiniMap::Ready_Layer_PlayerIcon(const _wstring& strLayerTag)
{
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.025f;
    Desc.fScaleRatioY = Desc.fScaleRatioX / static_cast<_float>(g_iWinSizeY) * static_cast<_float>(g_iWinSizeX);
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY;
    
    Desc.iUILayer = ETOUI(UILAYER::PANEL_DECO);
    
    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_CharMap_LiDailin_S000";
    
    Desc.eBlendState = CUI_Default::ALPHABLEND;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CUI_MiniMap* CUI_MiniMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_MiniMap* pInstance = new CUI_MiniMap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_MiniMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_MiniMap::Clone(void* pArg)
{
    CUI_MiniMap* pInstance = new CUI_MiniMap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_MiniMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_MiniMap::Free()
{
    Safe_Release(m_pInGame_Manager);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}