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
	HRESULT XM_CALLCONV Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg);				// 나중에

private:
	// 이 메쉬가 어떤 머테리얼을 쓰는지
	_uint m_iMaterialIndex{};
	_uint m_iNumBones{};

private:
	HRESULT Ready_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	// 사전 행렬 x -> 뼈의 행렬이 먼저 곱해져야 함.
	// 따라서 뼈에다가 사전 변환 행렬을 곱해줄 거임.
	HRESULT Ready_AnimMesh(const aiMesh* pAIMesh);

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;		// 나중에
protected:
	virtual void Free() override;
};

NS_END