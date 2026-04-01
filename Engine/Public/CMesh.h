#pragma once

#include "CVIBuffer.h"

NS_BEGIN(Engine)

class CModel;

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

private:
	HRESULT XM_CALLCONV Initialize_Prototype(MODEL eType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg);				// 나중에

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, vector<class CBone*>& Bones);

private:
	// 이 메쉬가 어떤 머테리얼을 쓰는지
	_uint m_iMaterialIndex{};

	_uint m_iNumBones{};
	vector<_uint>	m_BoneIndices;  // 이 mseh에 필요한 뼈(node)(부모 뼈에서의 인덱스 )
	_float4x4		m_BoneMatrices[g_iNumMeshBones] = {}; // 위 인덱스에  해당하는 부모 뼈 행렬

	// 이 mesh가 영향을 받는 bone 개수만큼 저장
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT XM_CALLCONV Ready_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	// 사전 행렬 x -> 뼈의 행렬이 먼저 곱해져야 함.
	// 따라서 뼈에다가 사전 변환 행렬을 곱해줄 거임.
	HRESULT Ready_AnimMesh(CModel* pModel, const aiMesh* pAIMesh);

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;		// 나중에
protected:
	virtual void Free() override;
};

NS_END