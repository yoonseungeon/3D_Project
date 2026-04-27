#include "CRenderTarget.h"

#include "CGameInstance.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	D3D11_TEXTURE2D_DESC TextureDesc{};

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	// 직교투영용으로 윈도우 좌표를 DX 월드 좌표계로

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(fSizeX, fSizeY, 1.f));

	auto tViewportDesc = CGameInstance::GetInstance()->Get_ViewportDesc();

	m_WorldMatrix._41 = fX - tViewportDesc.x * 0.5f;
	m_WorldMatrix._42 = -fY + tViewportDesc.y * 0.5f;
	m_WorldMatrix._43 = 0.1f;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	// 뷰, 투영(직교)는 Render에서

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;


	// 0번 디버깅 용도
	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}
#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	//SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Diffuse.dds"));

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}