#include "CTarget_Manager.h"

#include "CRenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (Find_RenderTarget(strTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	// Map에 있는지 검사
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	auto pMRTList = Find_MRT(strMRTTag);

	// List 없으면 만들고
	if (pMRTList == nullptr)
	{
		list<CRenderTarget*> MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else // 있으면 추가
	{
		pMRTList->push_back(pRenderTarget);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag)
{
	auto pMRTList = Find_MRT(strMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	// 기존 백버퍼랑 DSV 가지고 옴. Ref Cnt 증가됨.
	// 나중에 복구하기 위해서 저장
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

	// 배열로 넘겨줘야 함.
	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint iNumRenderTargets = {};

	// 배열에 그대로 저장
	for (auto& pRenderTarget : *pMRTList)
	{
		// 색 초기화
		pRenderTarget->Clear();
		// RTV를 꺼내와야 함.
		pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	// 기존 DSV는 다시 바인딩. 깊이 버퍼 여러 개면 깊이 테스트 힘듦?.
	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	// 백버퍼 배열에 저장
	ID3D11RenderTargetView* pRenderTargets[8] = {
		m_pBackBufferRTV,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	};

	// 백버퍼 복구
	m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pOriginalDSV);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pOriginalDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	auto		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	auto pMRTList = Find_MRT(strMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}
#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
	auto iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CTarget_Manager(pDevice, pContext);
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}

	for (auto& Pair : m_RenderTargets)
	{
		Safe_Release(Pair.second);
	}
	m_RenderTargets.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}