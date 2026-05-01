#pragma once

#include "CBounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	struct BOUNDING_SPHERE_DESC : public CBounding::BOUNDING_DESC
	{
		_float fRadius{};
	};

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

private:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void XM_CALLCONV Update(_fmatrix TransformMatrix) override;

public:
	virtual _bool Intersect(COLLIDER eTargetType, CBounding* pBounding) override;
	const BoundingSphere* Get_Desc() const { return m_pDesc; }

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc);
protected:
	virtual void Free() override;
};

NS_END