#pragma once

#include "CVIBuffer.h"

NS_BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

private:
	HRESULT XM_CALLCONV Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg);				// 나중에

private:
	// 이 메쉬가 어떤 머테리얼을 쓰는지
	_uint m_iMaterialIndex{};

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;		// 나중에
protected:
	virtual void Free() override;
};

NS_END