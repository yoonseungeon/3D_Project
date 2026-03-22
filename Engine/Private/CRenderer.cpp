#include "CRenderer.h"

#include "CGameObject.h"
#include "CUIObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
    m_RenderObjects[ETOUI(eGroupID)].push_back(pGameObject);

    Safe_AddRef(pGameObject);
}

HRESULT CRenderer::Draw()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;
    if (FAILED(Render_NonBlend()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;
    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::PRIORITY)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::PRIORITY)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONBLEND)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::NONBLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::BLEND)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::BLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    m_RenderObjects[ETOUI(RENDERID::UI)].sort(
        [](CGameObject* pSrc, CGameObject* pDst)->bool {
            return static_cast<CUIObject*>(pSrc)->Get_UILayer() < static_cast<CUIObject*>(pDst)->Get_UILayer();
        }
    );

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::UI)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::UI)].clear();

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    for (auto& RenderObjects : m_RenderObjects)
    {
        for (auto& pRenderObject : RenderObjects)
        {
            Safe_Release(pRenderObject);
        }
        RenderObjects.clear();
    }

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
