#include "CNunchaku_AfterImage.h"

#include "CGameInstance.h"

#include "CLiDailin.h"

CNunchaku_AfterImage::CNunchaku_AfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect{ pDevice, pContext }
{

}

CNunchaku_AfterImage::CNunchaku_AfterImage(const CNunchaku_AfterImage& Prototype)
    : CEffect{ Prototype }
{

}

HRESULT CNunchaku_AfterImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNunchaku_AfterImage::Initialize(void* pArg)
{
    NUNCHAKU_AFTERIMAGE_DESC* pDesc = static_cast<NUNCHAKU_AFTERIMAGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNunchaku_AfterImage::Priority_Update(_float fTimeDelta)
{
    //__super::Priority_Update(fTimeDelta);
}

void CNunchaku_AfterImage::Parallel_Update(_float fTimeDelta)
{
}

void CNunchaku_AfterImage::Update(_float fTimeDelta)
{

}

void CNunchaku_AfterImage::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CNunchaku_AfterImage::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pShaderCom->Begin(ETOUI(MESHEFFECT_SHADER::ALPHABLEND))))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CNunchaku_AfterImage::Set_EffectTransform(_uint iCurAni, _uint iATKCount)
{
    if (m_iCurAni == iCurAni && m_iATKCount == iATKCount)
        return;

    m_iCurAni = iCurAni;
    m_iATKCount = iATKCount;

    switch (m_iCurAni) {
        case ETOUI(Nunchaku_Ani::ATK_1_WP):
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.102318f, -0.203970f, 0.582808f, 1.f));
            _float4 vQuat = _float4(0.408793f, -0.466730f, 0.650110f, 0.438643f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2_WP):
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.162569f, 0.759658f, 0.699038f, 1.f));
            _float4 vQuat = _float4(0.643475f, -0.019691f, 0.743762f, 0.179920f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_1P_WP):
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.102318f, -0.203970f, 0.582808f, 1.f));
            _float4 vQuat = _float4(0.408793f, -0.466730f, 0.650110f, 0.438643f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2P_WP):
        {
            if (iATKCount == 1)
            {
                m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.162569f, 0.759658f, 0.699038f, 1.f));
                _float4 vQuat = _float4(0.643475f, -0.019691f, 0.743762f, 0.179920f);
                m_pTransformCom->Set_Rotation(vQuat);
            }
            else
            {
                m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.102318f, -0.203970f, 0.582808f, 1.f));
                _float4 vQuat = _float4(0.408793f, -0.466730f, 0.650110f, 0.438643f);
                m_pTransformCom->Set_Rotation(vQuat);
            }
            break;
        }
        default:
        {
            return;
        }
    }

}

HRESULT CNunchaku_AfterImage::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_Nunchaku_AfterImage0",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_Fx_Nunchaku",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNunchaku_AfterImage::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTexIdx)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;

    return S_OK;
}

CNunchaku_AfterImage* CNunchaku_AfterImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNunchaku_AfterImage* pInstance = new CNunchaku_AfterImage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CNunchaku_AfterImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNunchaku_AfterImage::Clone(void* pArg)
{
    CNunchaku_AfterImage* pInstance = new CNunchaku_AfterImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CNunchaku_AfterImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNunchaku_AfterImage::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}

