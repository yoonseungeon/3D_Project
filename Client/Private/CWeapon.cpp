#include "CWeapon.h"

#include "CGameInstance.h"
#include "CLiDailin.h"

#include "CTrailEffect.h"
#include "CNunchaku_AfterImage.h"
#include "CSlashEffect.h"

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

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fOutLineLength = 0.013f;
    m_vOutLineColor = { 0.f, 0.f, 0.f, 1.f };

    //if (FAILED(Ready_Layer_Trail(TEXT("Layer_Trail"))))
    //    return E_FAIL;
     
    if (FAILED(Ready_Layer_Nunchaku_AfterImage(TEXT("Nunchaku_AfterImage"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Slash_Effect(TEXT("Nunchaku_Slash_Effect"))))
        return E_FAIL;

    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

void CWeapon::Parallel_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

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

    Generate_Effect();

    m_pNunchaku_AfterImage->Compute_CombinedMatrix(m_pParentMatrix);
    m_pSlashEffect->Compute_CombinedMatrix(m_pParentMatrix);

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::OUTLINE, this);
}

HRESULT CWeapon::Render()
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

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::DEFAULT))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWeapon::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    // 광원 뷰스페이스, 투영 행렬 던져줌
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::SHADOW))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWeapon::Render_OutLine()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(Bind_OutLineShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::OUTLINE))))
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

    return S_OK;
}

HRESULT CWeapon::Bind_OutLineShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_OutLineLength", &m_fOutLineLength, sizeof(m_fOutLineLength))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_OutLineColor", &m_vOutLineColor, sizeof(m_vOutLineColor))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Ready_Layer_Nunchaku_AfterImage(const _wstring& strLayerTag)
{
    CNunchaku_AfterImage::NUNCHAKU_AFTERIMAGE_DESC Desc{};
    Desc.iTexIdx = 1;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nunchaku_AfterImage"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pNunchaku_AfterImage))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Ready_Layer_Slash_Effect(const _wstring& strLayerTag)
{   
    CSlashEffect::SLASH_EFFECT_DESC Desc{};
    Desc.iTexIdx = 0;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nunchaku_SlashEffect"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pSlashEffect))))
        return E_FAIL;

    return S_OK;
}

void CWeapon::Generate_Effect()
{
    //enum class Nunchaku_Ani {
    //    IDLE_WP = 12,
    //    RUN_WP = 4,
    //    ATK_1_WP = 0, ATK_2_WP = 2,
    //    ATK_1P_WP = 1, ATK_2P_WP = 3,
    //    Q1_WP = 7, Q2_WP = 9, Q3_WP = 10,
    //};

    _uint iAniIndex = m_pModelCom->Get_CurAniIndex();
    _float fAniRatio = m_pModelCom->Get_AniPlayRatio(iAniIndex);

    _float fEffectStartTime{};
    _float fEffectEndTime{};
    _uint iATKCount = {1};
    _float fAlpha{};

    switch (iAniIndex) {
        case ETOUI(Nunchaku_Ani::ATK_1_WP):
        {
            fEffectStartTime = { 0.096f };
            fEffectEndTime = { 0.307f };
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2_WP):
        {
            fEffectStartTime = { 0.115f };
            fEffectEndTime = { 0.384f };
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_1P_WP):
        {            
            if (fAniRatio <= 0.32f)
            {
                fEffectStartTime = { 0.115f };
                fEffectEndTime = { 0.307f };
            }
            else
            {
                fEffectStartTime = { 0.346f };
                fEffectEndTime = { 0.442f };
                iATKCount = 2;
            }
            break;
        }
        case ETOUI(Nunchaku_Ani::ATK_2P_WP):
        {
            if (fAniRatio <= 0.32f)
            {
                fEffectStartTime = { 0.115f };
                fEffectEndTime = { 0.153f };
            }
            else
            {
                fEffectStartTime = { 0.336f };
                fEffectEndTime = { 0.442f };
                iATKCount = 2;
            }
            break;
        }
        default:
        {
            m_pNunchaku_AfterImage->Set_IsInactive(true);
            m_pSlashEffect->Set_IsInactive(true);
            return;
        }
    }

    if(fAniRatio >= fEffectStartTime && fAniRatio <= fEffectEndTime)
    {
        fAlpha = (fAniRatio - fEffectStartTime) / (fEffectEndTime - fEffectStartTime);
        fAlpha = 1 - fAlpha;

        m_pNunchaku_AfterImage->Set_IsInactive(false);
        m_pNunchaku_AfterImage->Set_EffectTransform(iAniIndex, iATKCount);
        m_pNunchaku_AfterImage->Set_Alpha(fAlpha / 1.5f);

        m_pSlashEffect->Set_IsInactive(false);
        m_pSlashEffect->Set_EffectTransform(iAniIndex, iATKCount);
        m_pSlashEffect->Set_Alpha(fAlpha / 3.f);

    }
    else
    {
        m_pNunchaku_AfterImage->Set_IsInactive(true);
        m_pSlashEffect->Set_IsInactive(true);
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
    Safe_Release(m_pNunchaku_AfterImage);
    Safe_Release(m_pSlashEffect);
    Safe_Release(m_pTrailEffect);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
