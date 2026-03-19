#pragma once

#include "CTransform.h"

NS_BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	struct GAMEOBJECT_DESC
	{
		CTransform::TRANSFORM_DESC tTransformDesc = {};
	};

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	// 원형(Prototype) 객체를 생성(Create)할 때 호출
	virtual HRESULT Initialize_Prototype();
	// 원형을 복제한 사본 객체를 생성(Clone) 때 호출
	// 원형 단계에서 채울 수 없는 런타임 의존 정보(예: 플레이어 포인터 등)는 pArg로 전달받아 추가 초기화
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice					= { nullptr };
	ID3D11DeviceContext*	m_pContext					= { nullptr };

	CGameInstance*			m_pGameInstance			= { nullptr };

	map<const _wstring, class CComponent*>			m_Components;
	CTransform*				m_pTransformCom		= { nullptr };

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& wstrPrototypeTag,
		const _wstring& wstrComponentTag, CComponent** ppOut, void* pArg = nullptr);

	class CComponent* Find_Component(const _wstring& wstrComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END