#pragma once

#include "CVIBuffer.h"

NS_BEGIN(Engine)

class CMyModel;

class CMyMesh final : public CVIBuffer
{
private:
	CMyMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyMesh(const CMyMesh& Prototype);
	virtual ~CMyMesh() = default;

private:
	HRESULT XM_CALLCONV Initialize_Prototype(MODEL eType, CMyModel* pModel, const myMesh* pMyMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg);				// 나중에

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, vector<class CMyBone*>& Bones);

private:
	// 이 메쉬가 어떤 머테리얼을 쓰는지
	_uint				m_iMaterialIndex{};

	_uint m_iNumBones{};
	vector<_uint>		m_BoneIndices;  // 이 mseh에 필요한 뼈(node)(부모 뼈에서의 인덱스 )
	_float4x4			m_BoneMatrices[g_iNumMeshBones] = {}; // 위 인덱스에 해당하는 부모 뼈 행렬

	// 이 mesh가 영향을 받는 bone 개수만큼 저장
	vector<_float4x4>	m_OffsetMatrices;

	// mesh 이름 저장.
	// 부착물들 안 그려지는 경우가 있음.(뼈 개수 0개인 애들)
	// 뼈 개수 0개면 정점 BLENDINDEX, BLENDWEIGHT 다 채우지 못한다.(다 날라감)
	// mesh 안에는 없지만 mesh 이름과 뼈 이름 같은게 하나 있음.
	// 이거 이름으로 연결 시켜버림.
	_char				m_szName[MAX_PATH] = {};

private:
	HRESULT XM_CALLCONV Ready_NonAnimMesh(const myMesh* pMyMesh, _fmatrix PreTransformMatrix);
	// 사전 행렬 x -> 뼈의 행렬이 먼저 곱해져야 함.
	// 따라서 뼈에다가 사전 변환 행렬을 곱해줄 거임.
	HRESULT Ready_AnimMesh(CMyModel* pModel, const myMesh* pMyMesh);

public:
	static CMyMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, CMyModel* pModel, const myMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;		// 나중에
protected:
	virtual void Free() override;
};

NS_END