#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CRenderTarget;

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	// strMRTTag에 해당하는 RT List 바인딩
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	// 백버퍼로 복구
	HRESULT End_MRT();

#ifdef _DEBUG
public:
	// 윈도우 좌표로 위치 세팅
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	// 묶어 놓은 단위로 그림
	HRESULT Render_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };

	map<const _wstring, CRenderTarget*>			m_RenderTargets;
	// 지형, 플레이어(NonBledn) 그리기 전에 바인딩
	// 한 번에 같이 바인딩할 애들 List로 모아놓음.
	// 위에 그럼 필요없냐? 특정 조건에 따라 추가적으로 더 필요할 수도 있고,
	// 빼 버릴 수도 있어야 한다. 여기서 빼면 아예 없어져서 위에 거 필요함.
	map<const _wstring, list<CRenderTarget*>>	m_MRTs; // Multi Render Target

private:
	CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
	list<CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free();
};

NS_END