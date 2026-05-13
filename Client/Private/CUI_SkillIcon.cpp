#include "CUI_SkillIcon.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CSkillCoolDisplay.h"

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
    m_iMaxSkillLevel = pDesc->iMaxSkillLevel;
    m_fRatioYNoExtend = pDesc->fRatioYNoExtend;

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

CGameObject* CUI_SkillIcon::Clone(void* pArg)
{
    return nullptr;
}

void CUI_SkillIcon::Free()
{
    Safe_Release(m_pInGameManager);
    Safe_Release(m_pSkillcoolDisplay);

    __super::Free();
}
