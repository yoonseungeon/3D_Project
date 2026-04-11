#include "CWeapon.h"

#include "CGameInstance.h"
#include "CPlayer.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<CWeapon::WEAPON_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;

    m_pCurState = pDesc->pCurMoveState;
    m_iCurState = *m_pCurState;

    m_pCurATKType = pDesc->pCurATKType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

void CWeapon::Parallel_Update(_float fTimeDelta)
{
    Enter_State(fTimeDelta);

    if (m_bIsInactive == true)
    {
        return;
    }

    Execute_State(fTimeDelta);

    m_pModelCom->Play_Animation(fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
}

void CWeapon::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) 
    {
        return;
    }

    // SocketBone 행렬에서 위치는 맞는데, 스케일이 깨져서 스케일 1, 1, 1로 만들어 줌.
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);

    //for (size_t i = 0; i < 3; ++i)
    //    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    // 자신 월드 * 소켓 * 컨테이너 부모 이 순서로 곱함
    // 자신 월드 * 소켓 * Body * 컨테이너 부모가 더 정확하지만 Body는 움직이지 않아서 항등 행렬임.
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix);

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CWeapon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
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

HRESULT CWeapon::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Nunchaku"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

void CWeapon::Enter_State(_float fTimeDelta)
{
    // 애니메이션 상태가 바뀌었을 때 한 번만 실행
    if (m_iCurState != *m_pCurState)
    {
        switch (*m_pCurState)
        {
            case CPlayer::ACTION_STATE::ATK_P:
            {
                m_bIsInactive = false;
                if (*m_pCurATKType == 0) {
                    m_pModelCom->Set_AnimationIndex(ATK_1_WP, false);
                }
                else
                {
                    m_pModelCom->Set_AnimationIndex(ATK_2_WP, false);
                }
                break;
            }

            case CPlayer::ACTION_STATE::ATK_P_P:
            {
                m_bIsInactive = false;
                if (*m_pCurATKType == 0) {
                    m_pModelCom->Set_AnimationIndex(ATK_2P_WP, false);
                }
                else
                {
                    m_pModelCom->Set_AnimationIndex(ATK_1P_WP, false);
                }
                break;
            }

            case CPlayer::ACTION_STATE::IDLE_P:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(IDLE_WP, true);
                break;
            }

            case CPlayer::ACTION_STATE::RUN_P:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(RUN_WP, true);
                break;
            }

            case CPlayer::ACTION_STATE::Q1:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(Q1_WP, false);
                break;
            }

            case CPlayer::ACTION_STATE::Q2:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(Q2_WP, false);
                break;
            }

            case CPlayer::ACTION_STATE::Q3:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(Q3_WP, false);
                break;
            }

            case CPlayer::ACTION_STATE::E:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(IDLE_WP, true);
                break;
            }

            case CPlayer::ACTION_STATE::R:
            {
                m_bIsInactive = false;
                m_pModelCom->Set_AnimationIndex(IDLE_WP, true);
                break;
            }

            case CPlayer::ACTION_STATE::REST_P:
            case CPlayer::ACTION_STATE::CRAFT_P:
            case CPlayer::ACTION_STATE::COOK_P:
            case CPlayer::ACTION_STATE::COLLECT_P:
            {
                m_bIsInactive = true;
                break;
            }
        }

        m_iCurState = *m_pCurState;
    }
}

void CWeapon::Execute_State(_float fTimeDelta)
{
    switch (m_iCurState)
    {
        case CPlayer::ACTION_STATE::ATK_P:
        {
            break;
        }

        case CPlayer::ACTION_STATE::ATK_P_P:
        {
            break;
        }

        case CPlayer::ACTION_STATE::IDLE_P:
        {
            break;
        }

        case CPlayer::ACTION_STATE::RUN_P:
        {

            break;
        }

        case CPlayer::ACTION_STATE::Q1:
        {
            break;
        }

        case CPlayer::ACTION_STATE::Q2:
        {
            break;
        }

        case CPlayer::ACTION_STATE::Q3:
        {
            break;
        }

        case CPlayer::ACTION_STATE::E:
        {
            break;
        }

        case CPlayer::ACTION_STATE::R:
        {
            break;
        }
        case CPlayer::ACTION_STATE::REST_P:
        {
            break;
        }
        case CPlayer::ACTION_STATE::CRAFT_P:
        {
            break;
        }
        case CPlayer::ACTION_STATE::COOK_P:
        {
            break;
        }
        case CPlayer::ACTION_STATE::COLLECT_P:
        {
            break;
        }
    }
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon* pInstance = new CWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
