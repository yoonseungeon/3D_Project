#pragma once

#include "CComponent.h"
#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"
#include "CBounding_Frustum.h"

NS_BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;
public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg);

public:
	void XM_CALLCONV Update(_fmatrix TransformMatrix);

public:
	_bool Intersect(CCollider* pTarget);
	_bool XM_CALLCONV Intersect_Ray(_fvector vRayPos, _fvector vRayDir, _float& fDist);

public:
	void Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
	void Set_Layer(_uint iLayer) { m_iLayer = iLayer; }
	void Set_Mask(_uint iMask) { m_iMask = iMask; }

	CGameObject* Get_Owner() { return m_pOwner; }
	_uint Get_Layer() { return m_iLayer; }
	_uint Get_Mask() { return m_iMask; }

	void Set_IsColl(_bool isColl);

	void Set_ID(_uint iId) { m_iId = iId; }
	_uint Get_ID() { return m_iId; }

	void Set_Active(_bool bIsActive) { m_bIsActive = bIsActive; }
	_bool Get_Active() { return m_bIsActive; }

	void Set_CanMousePicking(_bool bCanMousePicking) { m_bCanMousePicking = bCanMousePicking; }
	_bool Get_CanMousePicking() { return m_bCanMousePicking; }

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	COLLIDER			m_eType = { COLLIDER::END_DECO };
	class CBounding*	m_pBounding = { nullptr };
	_bool				m_isColl = { false };

	CGameObject*	m_pOwner{};
	_uint			m_iLayer{};
	_uint			m_iMask{};

	_uint			m_iId{};

	_bool			m_bIsActive{ true };
	_bool			m_bCanMousePicking{ true };

#ifdef _DEBUG
private:
	// PrimitiveBatch: 정점 버퍼, 인덱스 버퍼 만들고, 토폴로지 설정 알아서 해줌
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	// 셰이더
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END