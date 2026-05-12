#pragma once

#include "CBase.h"

/* 1.화면에 그려져야할 객체들을 그리는 순서대로 모아놓는다. */
/* 2.보관된 순서대로 객체들의 드로우콜을 해준다. */

NS_BEGIN(Engine)

class CGameObject;
class CGameInstance;

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

private:
	HRESULT Initialize();

public:
	void Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
	HRESULT Draw();

#ifdef _DEBUG
	void Add_DebugComponent(class CComponent* pComponent);
#endif

private:
	// 후처리 셰이더에서 사용 예정
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	list<CGameObject*>	m_RenderObjects[ETOUI(RENDERID::END)];
	CGameInstance* m_pGameInstance = { nullptr };

	// 사각형 그리기 위한 셰이더
	class CShader* m_pShader = { nullptr };
	// 후처리, 디버깅 용
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	// 직교 투영으로 그릴 것이기 때문에 재사용을 위해 Render에서
	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	ID3D11DepthStencilView* m_pMaxDSV = { nullptr };

#ifdef _DEBUG
	list<class CComponent*>		m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Combined();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();

	HRESULT Render_OutLine();

	HRESULT Render_UI();

private:
	HRESULT Ready_DepthStencil_Buffer();
	HRESULT Change_ViewportDesc(_uint iWidth, _uint iHeight);

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END