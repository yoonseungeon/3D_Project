#pragma once

#include "CComponent.h"
#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"

NS_BEGIN(Engine)

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

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	COLLIDER			m_eType = { COLLIDER::END };
	class CBounding* m_pBounding = { nullptr };
	_bool				m_isColl = { false };

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