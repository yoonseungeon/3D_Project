#pragma once

#include "CBounding.h"

NS_BEGIN(Engine)

class CBounding_Frustum final : public CBounding
{
public:
	struct BOUNDING_FRUSTUM_DESC : public CBounding::BOUNDING_DESC
	{
		_float3 vOrigin{};
		_float3 vRadians{};

		// tan으로 넣어줘야 함.
		_float fRightSlope{};
		_float fLeftSlope{};
		_float fTopSlope{};
		_float fBottomSlope{};

		_float fNear{};
		_float fFar{};
	};

private:
	CBounding_Frustum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Frustum() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) override;

public:
	virtual _bool Intersect(COLLIDER eTargetType, CBounding* pBounding) override;
	virtual _bool XM_CALLCONV Intersect_Ray(_fvector vRayPos, _fvector vRayDir, _float& fDist);

	const BoundingFrustum* Get_Desc() const { return m_pDesc; }

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingFrustum* m_pOriginalDesc = { nullptr };
	BoundingFrustum* m_pDesc = { nullptr };

public:
	static CBounding_Frustum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc);
protected:
	virtual void Free() override;
};

NS_END