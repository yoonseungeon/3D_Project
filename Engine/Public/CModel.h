#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class CMesh;
class CMaterial;
class CShader;
class CBone;
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

private:
	HRESULT XM_CALLCONV Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg);

public:
	_uint Get_NumMeshes() const { return static_cast<_uint>(m_iNumMeshes); }
	// 이름이 같은 CBone(node)의 인덱스를 리턴하는 함수
	_int Get_BoneIndex(const _char* pBoneName);

public:
	void Set_AnimationIndex(_uint iIndex, _bool isLoop = false) { m_iCurrentAnimationIndex = iIndex; m_isAnimLoop = isLoop; }

public:
	_bool Play_Animation(_float fTimeDelta);

public:
	//특정 텍스처를 셰이더로 던진다.
	HRESULT Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iIndex);
	// 정점, 인덱스 버퍼 바인딩 및 draw 호출
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex);


private:	
	const aiScene*	m_pMyScene{ nullptr }; /* 파일로부터 읽어낸 모든 정보를 담고 있는다. */
	Importer		m_Importer{};
	MODEL			m_eType{ MODEL::END_DECO };

private:
	size_t				m_iNumMeshes{};
	vector< CMesh*>		m_Meshes;		// Model하나 당 여러 개의 Mesh를 갖는다.

	size_t				m_iNumMaterials{};
	vector<CMaterial*>	m_Materials;

	vector<CBone*>		m_Bones;

	// Anim인 경구 뼈에게 매 프레임 곱해줘야 한다.
	// m_TransformationMatrix가 계속 바뀌어서 계속 곱해줘야 함.
	_float4x4			m_PreTransformMatrix = {};

	// 애니메이션 -> 어떤 뼈 조작. 시간에 따라 상태(크기, 회전, 이동) 저장.
	// 애니메이션 재생 -> 현재 재생 위치에 맞는 뼈의 상태 행렬을 가지고 와서 갱신
	_uint				m_iNumAnimations{};
	vector<CAnimation*>	m_Animations;

	_uint				m_iCurrentAnimationIndex{};
	_bool				m_isAnimLoop{ false };


private:
	HRESULT XM_CALLCONV Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT				Ready_Materials(const _char* pModelFilePath);
	HRESULT				Ready_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT				Ready_Animations();

public:
	static CModel* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END