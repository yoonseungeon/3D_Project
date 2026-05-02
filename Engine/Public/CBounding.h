#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	struct BOUNDING_DESC
	{
		_float3 vCenter{};

		const MODEL_LOCAL_MIN_MAX* pLocalXYZ{ nullptr };
	};

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) = 0;

public:
	virtual _bool Intersect(COLLIDER eTargetType, CBounding* pBounding) = 0;
	void Set_IsColl(_bool isColl) { m_isColl = isColl; }

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
#endif

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_bool					m_isColl = { false };

protected:
	virtual void Free() override;
};

NS_END