#pragma once

#include "CTransform.h"

NS_BEGIN(Engine)

class CGameInstance;
class CCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	struct GAMEOBJECT_DESC
	{
		CTransform::TRANSFORM_DESC tTransformDesc = {};
		_bool bIsInactive{};
	};

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

protected:
	// 원형(Prototype) 객체를 생성(Create)할 때 호출
	HRESULT Initialize_Prototype();
	// 원형을 복제한 사본 객체를 생성(Clone) 때 호출
	// 원형 단계에서 채울 수 없는 런타임 의존 정보(예: 플레이어 포인터 등)는 pArg로 전달받아 추가 초기화
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Parallel_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; };
	virtual HRESULT Render_OutLine() { return S_OK; };

	virtual void OnCollision_Enter(const COLLISION_INFO& tCollision);
	virtual void OnCollision_Stay(const COLLISION_INFO& tCollision);
	virtual void OnCollision_Exit(const COLLISION_INFO& tCollision);

public:
	class CComponent* Find_Component(const _wstring& wstrComponentTag);

	CCollider* Get_Collider(_uint iIndex);
	virtual void Set_IsInactive(_bool bIsInactive) { m_bIsInactive = bIsInactive; }

#ifdef _DEBUG
	const map<const _wstring, CComponent*>& Get_Componets() { return m_Components; }
#endif

	_uint Get_UILayer() { return m_iUILayer; }

protected:
	ID3D11Device*									m_pDevice{ nullptr };
	ID3D11DeviceContext*							m_pContext{ nullptr };

	CGameInstance*									m_pGameInstance{ nullptr };

	map<const _wstring, class CComponent*>			m_Components;
	CTransform*										m_pTransformCom{ nullptr };

	_bool											m_bIsInactive{};
	_bool											m_bIsCulling{};

	vector<CCollider*>								m_Colliders;

	_uint m_iUILayer{};

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& wstrPrototypeTag,
		const _wstring& wstrComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END