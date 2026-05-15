#include "CNunchaku_AfterImage.h"

#include "CGameInstance.h"

#include "CLiDailin.h"

CNunchaku_AfterImage::CNunchaku_AfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CNunchaku_AfterImage::CNunchaku_AfterImage(const CNunchaku_AfterImage& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CNunchaku_AfterImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNunchaku_AfterImage::Initialize(void* pArg)
{
    NUNCHAKU_AFTERIMAGE_DESC* pDesc = static_cast<NUNCHAKU_AFTERIMAGE_DESC*>(pArg);

    m_iTexIdx = pDesc->iTexIdx;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    // ATK1
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.28f, 0.302f, 1.058f, 1.f));
    //_float4 vQuat = _float4(0.684f, 0.062f, 0.000559f, 0.727f);
    //m_pTransformCom->Set_Rotation(vQuat);

    //ATK2
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.243823f, 0.626138f, 0.846655f, 1.f));
    //_float4 vQuat = _float4(0.662921f, -0.017824f, 0.744414f, 0.077878f);
    //m_pTransformCom->Set_Rotation(vQuat);

    //ATK1_1, ATK1_2
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.056480f, 0.096296f, 0.131294f, 1.f));
    //_float4 vQuat = _float4(0.530984f, -0.493545f, 0.559280f, 0.402088f);
    //m_pTransformCom->Set_Rotation(vQuat);

    //ATK2_1
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.243823f, 0.626138f, 0.846655f, 1.f));
    //_float4 vQuat = _float4(0.662921f, -0.017824f, 0.744414f, 0.077878f);
    //m_pTransformCom->Set_Rotation(vQuat);

    //ATK2_2
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.806056f, 0.088632f, 0.680687f, 1.f));
    _float4 vQuat = _float4(0.344840f, -0.588919f, 0.478363f, 0.552656f);
    m_pTransformCom->Set_Rotation(vQuat);

    return S_OK;
}

void CNunchaku_AfterImage::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
    // 위치 조정
    {
        bool bChanged = false;

        float fMoveSpeed = 1.f;

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            fMoveSpeed = 0.1f;

        if (GetAsyncKeyState(VK_MENU) & 0x8000)
            fMoveSpeed = 5.f;

        float fMove = fMoveSpeed * fTimeDelta;

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            fMove *= -1.f;

        if (GetAsyncKeyState('B') & 0x8000)
        {
            m_vDebugPos.x += fMove;
            bChanged = true;
        }

        if (GetAsyncKeyState('N') & 0x8000)
        {
            m_vDebugPos.y += fMove;
            bChanged = true;
        }

        if (GetAsyncKeyState('M') & 0x8000)
        {
            m_vDebugPos.z += fMove;
            bChanged = true;
        }

        if (bChanged == true)
        {
            m_pTransformCom->Set_State(
                STATE::POSITION,
                XMLoadFloat4(&m_vDebugPos)
            );

            wchar_t szDebug[256]{};
            swprintf_s(
                szDebug,
                L"Pos = { %.6ff, %.6ff, %.6ff, 1.f }\n",
                m_vDebugPos.x,
                m_vDebugPos.y,
                m_vDebugPos.z
            );

            OutputDebugStringW(szDebug);
        }
    }

    // 회전 조정
    {
        bool bChanged = false;

        float fSpeedDegree = 60.f;

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            fSpeedDegree = 10.f;

        float fAngle = XMConvertToRadians(fSpeedDegree) * fTimeDelta;

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            fAngle *= -1.f;

        XMVECTOR qCur = XMLoadFloat4(&m_vDebugQuat);
        qCur = XMQuaternionNormalize(qCur);

        auto Apply_WorldRotation = [&](FXMVECTOR vAxis)
            {
                XMVECTOR qDelta = XMQuaternionRotationAxis(vAxis, fAngle);

                // 월드축 기준
                qCur = XMQuaternionMultiply(qCur, qDelta);
                qCur = XMQuaternionNormalize(qCur);

                bChanged = true;
            };

        if (GetAsyncKeyState('J') & 0x8000)
        {
            Apply_WorldRotation(XMVectorSet(1.f, 0.f, 0.f, 0.f));
        }

        if (GetAsyncKeyState('K') & 0x8000)
        {
            Apply_WorldRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f));
        }

        if (GetAsyncKeyState('L') & 0x8000)
        {
            Apply_WorldRotation(XMVectorSet(0.f, 0.f, 1.f, 0.f));
        }

        if (bChanged == true)
        {
            XMStoreFloat4(&m_vDebugQuat, qCur);

            m_pTransformCom->Set_Rotation(m_vDebugQuat);

            wchar_t szDebug[256]{};
            swprintf_s(
                szDebug,
                L"Quat = { %.6ff, %.6ff, %.6ff, %.6ff }\n",
                m_vDebugQuat.x,
                m_vDebugQuat.y,
                m_vDebugQuat.z,
                m_vDebugQuat.w
            );

            OutputDebugStringW(szDebug);
        }
    }
#endif
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

void CNunchaku_AfterImage::Compute_CombinedMatrix(const _float4x4* vParentMatrix)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())
        * XMLoadFloat4x4(vParentMatrix));
}

void CNunchaku_AfterImage::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
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
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.28f, 0.302f, 1.058f, 1.f));
            _float4 vQuat = _float4(0.684f, 0.062f, 0.000559f, 0.727f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2_WP):
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.243823f, 0.626138f, 0.846655f, 1.f));
            _float4 vQuat = _float4(0.662921f, -0.017824f, 0.744414f, 0.077878f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_1P_WP):
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.056480f, 0.096296f, 0.131294f, 1.f));
            _float4 vQuat = _float4(0.530984f, -0.493545f, 0.559280f, 0.402088f);
            m_pTransformCom->Set_Rotation(vQuat);
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2P_WP):
        {
            if (iATKCount == 1)
            {
                m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.243823f, 0.626138f, 0.846655f, 1.f));
                _float4 vQuat = _float4(0.662921f, -0.017824f, 0.744414f, 0.077878f);
                m_pTransformCom->Set_Rotation(vQuat);
            }
            else
            {
                m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.806056f, 0.088632f, 0.680687f, 1.f));
                _float4 vQuat = _float4(0.344840f, -0.588919f, 0.478363f, 0.552656f);
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

