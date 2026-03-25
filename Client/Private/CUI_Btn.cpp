#include "CUI_Btn.h"

#include "CGameInstance.h"

CUI_Btn::CUI_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Btn::CUI_Btn(const CUI_Btn& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Btn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Btn::Initialize(void* pArg)
{
    CUI_BTN_DESC* pDesc = static_cast<CUI_BTN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    _float fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    _float fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;
    
    m_rcBtnRange.left = static_cast<_long>(fCenterX - fHalfSizeX);
    m_rcBtnRange.right = static_cast<_long>(fCenterX + fHalfSizeX);
    m_rcBtnRange.top = static_cast<_long>(fCenterY - fHalfSizeY);
    m_rcBtnRange.bottom = static_cast<_long>(fCenterY + fHalfSizeY);

    return S_OK;
}

void CUI_Btn::Priority_Update(_float fTimeDelta)
{
}

void CUI_Btn::Update(_float fTimeDelta)
{
}

void CUI_Btn::Late_Update(_float fTimeDelta)
{

}

HRESULT CUI_Btn::Render()
{
    return S_OK;
}

CUI_Btn::BTN_STATE CUI_Btn::Check_BtnState()
{
    POINT ptMouse = m_pGameInstance->Get_MouseClientPos();
    if (PtInRect(&m_rcBtnRange, ptMouse)){
        //m_pGameInstance->Get_DIMouseState(DIMB::LBUTTON);
    }
    
    return CUI_Btn::NORMAL;
}

void CUI_Btn::Free()
{
    __super::Free();
}
