#pragma once

#include "CComponent.h"
#include "CMyModelImporter.h"

NS_BEGIN(Engine)

class CMyMesh;
class CMyMaterial;
class CShader;
class CMyBone;
class CMyAnimation;

class ENGINE_DLL CMyModel final : public CComponent
{
private:
	CMyModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyModel(const CMyModel& Prototype);
	virtual ~CMyModel() = default;

private:
	HRESULT XM_CALLCONV Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX);
	HRESULT Initialize(void* pArg);

public:
	_uint Get_NumMeshes() const { return static_cast<_uint>(m_iNumMeshes); }
	// 이름이 같은 CMyBone(node)의 인덱스를 리턴하는 함수
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

	// 피킹
	const vector<_float3>& Get_VtxData(_uint iMeshIdx);
	const vector<_uint>& Get_IdxData(_uint iMeshIdx);

private:	
	const myScene*	m_pMyScene{ nullptr }; /* 파일로부터 읽어낸 모든 정보를 담고 있는다. */
	CMyModelImporter		m_Importer{};
	MODEL			m_eType{ MODEL::END };

private:
	size_t				m_iNumMeshes{};
	vector<CMyMesh*>		m_Meshes;		// Model하나 당 여러 개의 Mesh를 갖는다.

	size_t				m_iNumMaterials{};
	vector<CMyMaterial*>	m_Materials;

	vector<CMyBone*>		m_Bones;

	// Anim인 경구 뼈에게 매 프레임 곱해줘야 한다.
	// m_TransformationMatrix가 계속 바뀌어서 계속 곱해줘야 함.
	_float4x4			m_PreTransformMatrix = {};

	// 애니메이션 -> 어떤 뼈 조작. 시간에 따라 상태(크기, 회전, 이동) 저장.
	// 애니메이션 재생 -> 현재 재생 위치에 맞는 뼈의 상태 행렬을 가지고 와서 갱신
	_uint				m_iNumAnimations{};
	vector<CMyAnimation*>	m_Animations;

	_uint				m_iCurrentAnimationIndex{};
	_bool				m_isAnimLoop{ false };

	_bool				m_bStoreVtxIdx{};

private:
	HRESULT XM_CALLCONV Ready_Meshes(_fmatrix PreTransformMatrix);
	HRESULT				Ready_Materials(const _char* pModelFilePath);
	HRESULT				Ready_Bones(const myNode* pMyNode, _int iParentIndex);
	HRESULT				Ready_Animations();

public:
	static CMyModel* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), _bool bStoreVTXIDX = false);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END