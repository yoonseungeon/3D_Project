#include "CRenderer.h"

#include "CGameObject.h"
#include "CGameInstance.h"

#include "CUIObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    auto tViewportDesc = m_pGameInstance->Get_ViewportDesc();

    /* 렌더 타겟들을 만든다. */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    // Shade Target는 Diffuse랑 곱할 것인데, 여기에 스페큘러를 누적해서 Diffuse랑 곱해버리면, 스페큘러에 1을 줘도 최대 원본 밝기이다.
    // 따라서 Render Target을 따로 만들고 Shade랑 Diffuse를 곱한 후 더해줘야 한다.
    // 더할 것이기 때문에 반드시 0(검은색)으로 초기화해야 한다.
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    // 스페큘러는 카메라 방향이 필요함.
    // 깊이를 통해 픽셀의 월드 위치를 알아낼 수 있다.
    // 깊이 정밀도가 낮으면 픽셀의 월드 위치 복원이 정확하지 않다. -> Specular가 끊겨 보인다
    // 그래서 DXGI_FORMAT_R32G32B32A32_FLOAT 사용 
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    // Shadow
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), tViewportDesc.x, tViewportDesc.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* 만든 렌더타겟들을 장치에 동시에 바인딩되는 기준으로 모은다. */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    // 오브젝트 그릴 때 깊이 기록
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    // 스페큘러 따로 기록
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;
    // 그림자 따로 기록
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
    if (m_pShader == nullptr)
        return E_FAIL;

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (m_pVIBuffer == nullptr)
        return E_FAIL;

    // 원점 // 화면 크기
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(static_cast<_float>(tViewportDesc.x), static_cast<_float>(tViewportDesc.y), 1.f));
    // 항등
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    // 직교 투영
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(tViewportDesc.x), static_cast<_float>(tViewportDesc.y), 0.f, 1.f));


#ifdef _DEBUG
    // 윈도우 좌표로 위치 세팅
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.f, 450.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), 200.f, 200.f, 400.f, 400.f)))
        return E_FAIL;
#endif

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

    if (FAILED(Render_Shadow()))
        return E_FAIL;

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_Lights()))
        return E_FAIL;
    if (FAILED(Render_Combined()))
        return E_FAIL;
    if (FAILED(Render_NonLight()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_Debug()))
        return E_FAIL;
#endif

    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
void CRenderer::Add_DebugComponent(CComponent* pComponent)
{
    m_DebugComponents.push_back(pComponent);
    Safe_AddRef(pComponent);
}
#endif

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

HRESULT CRenderer::Render_Shadow()
{
    // Shadow 깊이 버퍼 바인딩
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::SHADOW)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render_Shadow();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::SHADOW)].clear();

    // 다시 백버퍼
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    // 같은 Texture를 SRV로 읽기 RTV로 쓰기 동시에 안되어서 경고 계속 뜸
    // 그래서 SRV 셰이더에 바인딩된 거 해제
    ID3D11ShaderResourceView* pResetSRV[8] = {};
    m_pContext->PSSetShaderResources(0, 8, pResetSRV);

    // 빛 연산이 필요한 객체들을 그린다.
    // 백버퍼 빼고 Diffuse, Normal 버퍼 세팅
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONBLEND)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::NONBLEND)].clear();

    // 원상 복구
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    // 빛 연산한 결과를 저장하기 위한 과정

  /* Shade */
  // 빛이 여러 개면 누적해서 그릴거라 Acc
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    // Normal 던지기(Normal은 계속 유지하고, 빛은 새로 던지면서 Shade에 누적)
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    // Specular 타겟을 계산하기 위해 현재 픽셀의 월드 위치를 복원하려고 바인딩
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    // 원점 // 사이즈는 화면 크기
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    // 항등
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    // 직교 투영 행렬
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    // 월드 위치 복원용
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInverse", m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInverse", m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))))
        return E_FAIL;
    // 스페큘러 계산용
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    //사각형 버퍼 바인딩
    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    // 빛 개수만큼 셰이더 전역 변수 던지고, Render 호출
    if (FAILED(m_pGameInstance->Render_Light(m_pShader, m_pVIBuffer)))
        return E_FAIL;

    // 백버퍼로 복구
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
    // 렌더 타겟들 결과 조합해서 진짜 백버퍼에 그림.

     // Diffuse
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    // Shade
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;

    // Specular
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ShadowLightViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ShadowLightProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMBINED))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    // 블랜딩도 안하고 빛 연산도 안하고
     // NonBlend는 무조건 빛 연산
     // AlphaBlend는 후처리로 빛 연산 x
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERID::NONLIGHT)])
    {
        if (pRenderObject != nullptr)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ETOUI(RENDERID::NONLIGHT)].clear();

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

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
    // Componet Render 호출
    for (auto& pDebugCom : m_DebugComponents)
    {
        pDebugCom->Render();
        Safe_Release(pDebugCom);
    }
    m_DebugComponents.clear();


    // 항등
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    // 직교 투영 행렬
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 버퍼 바인딩
    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    //// 이 그룹 그려줘
    //// 월드 행렬과 텍스처는 각자
    //m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
    //m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_RT_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer);

    return S_OK;
}
#endif

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

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
