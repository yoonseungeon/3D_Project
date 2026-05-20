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

    m_funcCallBack = pDesc->funcCallBack;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    m_fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;
    
    m_rcBtnRange.left = static_cast<_long>(fCenterX - m_fHalfSizeX);
    m_rcBtnRange.right = static_cast<_long>(fCenterX + m_fHalfSizeX);
    m_rcBtnRange.top = static_cast<_long>(fCenterY - m_fHalfSizeY);
    m_rcBtnRange.bottom = static_cast<_long>(fCenterY + m_fHalfSizeY);

    return S_OK;
}

void CUI_Btn::Priority_Update(_float fTimeDelta)
{
}

void CUI_Btn::Parallel_Update(_float fTimeDelta)
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

void CUI_Btn::Update_BtnState()
{
    POINT ptMouse = m_pGameInstance->Get_MouseClientPos();

    if (PtInRect(&m_rcBtnRange, ptMouse)){

        if (m_bIsHover == false)
        {
            m_bIsHover = true;
            m_bEnterHover = true;
        }
        else
        {
            m_bEnterHover = false;
        }

        if (m_bPressedInBtn && m_pGameInstance->Mouse_Up(DIMB::LBUTTON))
        {
            m_eCurBtnState = CUI_Btn::CLICKED;
            m_bPressedInBtn = false;
        }
        else if (m_bPressedInBtn || m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
        {
            m_eCurBtnState = CUI_Btn::PRESSED;
            m_bPressedInBtn = true;
        }
        else
        {
            m_eCurBtnState = CUI_Btn::HOVER;
        }

        return;
    }
    
    if (m_bPressedInBtn)
        m_bPressedInBtn = false;

    m_eCurBtnState = CUI_Btn::NORMAL;
    m_bIsHover = false;
    m_bEnterHover = false;
}

void CUI_Btn::BtnClick()
{
    m_funcCallBack();
}

void CUI_Btn::Free()
{
    __super::Free();
}
