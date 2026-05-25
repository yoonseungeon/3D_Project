#include "CUI_GameResult.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CUI_Image.h"
#include "CGameResultTextBox.h"

CUI_GameResult::CUI_GameResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_GameResult::CUI_GameResult(const CUI_GameResult& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_GameResult::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_GameResult::Initialize(void* pArg)
{
    CUI_GAMERESULT_DESC* pDesc = static_cast<CUI_GAMERESULT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    //if (FAILED(Ready_Components()))
    //    return E_FAIL;

    if (FAILED(Ready_Layer_GameResult(TEXT("Layer_UI_GameResult"))))
        return E_FAIL;

    GameResultInit();

    CInGame_Manager::GetInstance()->Set_GameResultUI(this);

    return S_OK;
}

void CUI_GameResult::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_O) == true)
    {
        GameResultStart();
    }
    if (m_pGameInstance->Key_Down(DIK_P) == true)
    {
        GameResultReset();
    }
}

void CUI_GameResult::Parallel_Update(_float fTimeDelta)
{
}

void CUI_GameResult::Update(_float fTimeDelta)
{
    Update_GameResult(fTimeDelta);
}

void CUI_GameResult::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    //m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_GameResult::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CUI_GameResult::GameResultStart()
{
    if (m_bIsPlay == false)
    {
        m_bIsPlay = true;
        CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::RESULT_VICTORY));

        for (_uint i = 0; i < m_CUI_Circles.size(); ++i)
        {
            m_CUI_Circles[i]->Set_IsInactive(false);
            CTransform* pTransform = dynamic_cast<CTransform*>(m_CUI_Circles[i]->Find_Component(g_strTransformTag));
            if (pTransform == nullptr)
                continue;

            const _float fStartSize = m_CircleSizeInfos[i].fStartSize;
            pTransform->Set_Scale(fStartSize, fStartSize, 1.f);
        }

        m_pBg->Set_IsInactive(false);
        m_pTextBox->GameResultStart();

        m_bResultEnd = false;
    }
}

void CUI_GameResult::GameResultInit()
{
    GameResultReset();

    _float fWinSizeY = static_cast<_float>(g_iWinSizeY);

    // 1
    const _float fInner1MaxSize = fWinSizeY * 2.67f;
    const _float fInner1TargetSize = fWinSizeY * 0.4f;

    TargetsizeInfo tInner1 = { fInner1MaxSize, fInner1TargetSize };
    m_CircleSizeInfos.push_back(tInner1);

    // 2
    const _float fInner2MaxSize = fInner1MaxSize + 90.f;
    const _float fInner2TargetSize = fInner1TargetSize + 90.f;

    TargetsizeInfo tInner2 = { fInner2MaxSize, fInner2TargetSize };
    m_CircleSizeInfos.push_back(tInner2);

    // 3
    const _float fInner3MaxSize = fInner2MaxSize + 130.f;
    const _float fInner3TargetSize = fInner2TargetSize + 130.f;

    TargetsizeInfo tInner3 = { fInner3MaxSize, fInner3TargetSize };
    m_CircleSizeInfos.push_back(tInner3);

    // 4
    const _float fInner4MaxSize = fInner3MaxSize + 70.f;
    const _float fInner4TargetSize = fInner3TargetSize + 70.f;

    TargetsizeInfo tInner4 = { fInner4MaxSize, fInner4TargetSize };
    m_CircleSizeInfos.push_back(tInner4);
}

void CUI_GameResult::GameResultReset()
{
    m_fMaxTime = 5.f;
    m_fAccTime = 0.f;
    m_bIsPlay = false;

    for (_uint i = 0; i < m_CUI_Circles.size(); ++i)
    {
        m_CUI_Circles[i]->Set_IsInactive(true);
    }
    m_pBg->Set_IsInactive(true);
    m_pTextBox->GameResultReset();
}

HRESULT CUI_GameResult::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), m_wstrTexturePrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_GameResult::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

HRESULT CUI_GameResult::Ready_Layer_GameResult(const _wstring& strLayerTag)
{
    CUI_Image* pImage{ nullptr };

    // Circle
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioY = 0.625f;
    Desc.fScaleRatioX = Desc.fScaleRatioY * static_cast<_float>(g_iWinSizeY) / static_cast<_float>(g_iWinSizeX);
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iUILayer = ETOUI(UILAYER::END_DECO);

    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Img_GameResult_Deco";

    Desc.vColor = COLOR_TO_FLOAT(12, 190, 247);

    Desc.iTexIdx = 0;

    Desc.fImageAlpha = 0.5f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;
    m_CUI_Circles.push_back(pImage);

    Desc.fImageAlpha = 1.f;

    Desc.iTexIdx = 1;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_CUI_Circles.push_back(pImage);


    Desc.iTexIdx = 2;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_CUI_Circles.push_back(pImage);


    Desc.iTexIdx = 3;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_CUI_Circles.push_back(pImage);

    // BG
    CUI_Image::CUI_IMAGE_DESC BgDesc{};

    BgDesc.fScaleRatioY = 1.0f;
    BgDesc.fScaleRatioX = 1.0f;
    BgDesc.fPosRatioX = 0.f;
    BgDesc.fPosRatioY = 0.f;
    BgDesc.iUILayer = ETOUI(UILAYER::BG_END);
    
    BgDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    BgDesc.eBlendState = CUI_Default::ALPHABLEND;
    BgDesc.wstrTexturePrototypeTag = L"Prototype_Texture_Img_GameResult_Bg_Color0";

    BgDesc.fImageAlpha = 0.7f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &BgDesc, reinterpret_cast<CGameObject**>(&m_pBg))))
        return E_FAIL;

    // TextBox
    CGameResultTextBox::GAMERESULT_TEXTBOX_DESC TextBoxDesc{};

    TextBoxDesc.fScaleRatioX = 1.0f;
    TextBoxDesc.fScaleRatioY = 1.0f / 4.f;
    TextBoxDesc.fPosRatioX = 0.f;
    TextBoxDesc.fPosRatioY = 0.f;
    TextBoxDesc.iUILayer = ETOUI(UILAYER::END);
    
    TextBoxDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    TextBoxDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    TextBoxDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";
    
    TextBoxDesc.vColor = COLOR_TO_FLOAT(12, 190, 247);
    
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_GameResultTextBox"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &TextBoxDesc, reinterpret_cast<CGameObject**>(&m_pTextBox))))
        return E_FAIL;

    return S_OK;
}

void CUI_GameResult::Update_GameResult(_float fTimeDelta)
{
    if (m_bIsPlay == false)
        return;

    m_fAccTime += fTimeDelta;
    if (m_fAccTime >= m_fMaxTime)
    {
        m_bResultEnd = true;
        return;
    }

    const _float fCurRatio = m_fAccTime / m_fMaxTime;

    Update_CircleLayer(fCurRatio);
    Update_CircleRot(fTimeDelta, fCurRatio);
    Update_Bg(fCurRatio);
    m_pTextBox->Set_Ratio(fCurRatio);
}

void CUI_GameResult::Update_CircleLayer(_float fRatio)
{
    const _float fInnerCircleStart = 0.08f;
    const _float fInnerCircleEnd = 0.16f;

    _float fInnerCircleRatio = (fRatio - fInnerCircleStart) / (fInnerCircleEnd - fInnerCircleStart);
    MyHelper::FloatClamp(fInnerCircleRatio, 0.f, 1.f);

    fInnerCircleRatio = sinf(XMConvertToRadians(fInnerCircleRatio * 110.f));

    for (_uint i = 0; i < m_CUI_Circles.size(); ++i)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(m_CUI_Circles[i]->Find_Component(g_strTransformTag));
        if (pTransform == nullptr)
            continue;

        const _float fStartSize = m_CircleSizeInfos[i].fStartSize;
        const _float fTargetSize = m_CircleSizeInfos[i].fTargetSize;
        const _float fDeltaSize = (fStartSize - fTargetSize) * fInnerCircleRatio;

        const _float fFinalSize = fStartSize - fDeltaSize;
        pTransform->Set_Scale(fFinalSize, fFinalSize, 1.f);
    }
}

void CUI_GameResult::Update_CircleRot(_float fTimeDelta, _float fRatio)
{
    const _float fCircleRotStart = 0.16f;
    const _float fCircleRotEnd = 1.0;

    _float fCircleRotRatio = (fRatio - fCircleRotStart) / (fCircleRotEnd - fCircleRotStart);
    MyHelper::FloatClamp(fCircleRotRatio, 0.f, 1.f);

    CTransform* pTransform = dynamic_cast<CTransform*>(m_CUI_Circles[INNER4]->Find_Component(g_strTransformTag));
    if (pTransform == nullptr)
        return;
    pTransform->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta, 0.1f);
}

void CUI_GameResult::Update_Bg(_float fRatio)
{
    const _float fFadeInStart = 0.0f;
    const _float fIFadeInEnd = 0.16f;

    _float fFadeInRatio = (fRatio - fFadeInStart) / (fIFadeInEnd - fFadeInStart);
    MyHelper::FloatClamp(fFadeInRatio, 0.f, 1.f);

    m_pBg->Set_Alpha(fFadeInRatio * 0.7f);
}

CUI_GameResult* CUI_GameResult::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_GameResult* pInstance = new CUI_GameResult(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_GameResult");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_GameResult::Clone(void* pArg)
{
    CUI_GameResult* pInstance = new CUI_GameResult(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_GameResult");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_GameResult::Free()
{
    for(auto& pImage : m_CUI_Circles)
        Safe_Release(pImage);
    m_CUI_Circles.clear();

    Safe_Release(m_pTextBox);
    Safe_Release(m_pBg);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
