#include "CUI_SkillIcon.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

CUI_SkillIcon::CUI_SkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{
}

CUI_SkillIcon::CUI_SkillIcon(const CUI_SkillIcon& Prototype)
    : CUI_Image{ Prototype }
{
}

HRESULT CUI_SkillIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_SkillIcon::Initialize(void* pArg)
{
    m_pInGameManager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pInGameManager);

    CUI_SKILLICON_DESC* pDesc = static_cast<CUI_SKILLICON_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_eSkillSlot = pDesc->eSkillSlot;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    m_fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;

    m_rcTextRange.left = static_cast<_long>(fCenterX - m_fHalfSizeX);
    m_rcTextRange.right = static_cast<_long>(fCenterX + m_fHalfSizeX);
    m_rcTextRange.top = static_cast<_long>(fCenterY - m_fHalfSizeY);
    m_rcTextRange.bottom = static_cast<_long>(fCenterY + m_fHalfSizeY);

    return S_OK;
}

void CUI_SkillIcon::Priority_Update(_float fTimeDelta)
{
}

void CUI_SkillIcon::Parallel_Update(_float fTimeDelta)
{
}

void CUI_SkillIcon::Update(_float fTimeDelta)
{
}

void CUI_SkillIcon::Late_Update(_float fTimeDelta)
{    
}

HRESULT CUI_SkillIcon::Render()
{

    return S_OK;
}

HRESULT CUI_SkillIcon::RenderText()
{
    const _float fTextStartX = static_cast<_float>(m_rcTextRange.left);
    const _float fTextStartY = static_cast<_float>(m_rcTextRange.top);

    if (m_bTextRender) {
        m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
            m_wstCoolText.data(),
            _float2(fTextStartX, fTextStartY),
            XMVectorSet(1.f, 1.f, 1.f, 1.f),
            _float2(fDefaultFontSize * 0.8f, fDefaultFontSize * 0.8f)
        );
    }

    return S_OK;
}

CGameObject* CUI_SkillIcon::Clone(void* pArg)
{
    return nullptr;
}

void CUI_SkillIcon::Free()
{
    Safe_Release(m_pInGameManager);

    __super::Free();
}
