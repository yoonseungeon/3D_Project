#pragma once

#include "CBase.h"

/* assimp 뼈를 표현하는 데이터의 종류 */
/* aiNode, aiBone, aiNodeAnim:*/

/* 부모의 상태를 포함한 뼈의 상태정보를 표현한다. */

NS_BEGIN(Engine)

class CMyBone final : public CBase
{
private:
	CMyBone();
	virtual ~CMyBone() = default;

private:
	HRESULT Initialize(const myNode* pMyNode, _int iParentIndex);

public:
	_bool Compare_Name(const _char* pBoneName) { return !strcmp(pBoneName, m_szName); }
	const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
	const _float4x4* Get_TransformationMatrixPtr() const { return &m_TransformationMatrix; }

	void XM_CALLCONV Update_CombinedTransformMatrices(const vector<CMyBone*>& Bones, _fmatrix PreTransformMatrix);

	void XM_CALLCONV Set_TransformationMatrix(_fmatrix TransformationMatrix);

	const KEYFRAME& Get_LocalPos() { return m_tLocalPos; }

private:
	// 이름 저장해야 함.(무기 붙이기, 이팩트 붙이기에 사용.)
	// aiNode, aiBone, aiNodeAnim: 다 동기화 되어 있음.
	_char		m_szName[MAX_PATH]{};
	// 부모의 index 저장.(CModel의 Bone vector index임. 따라서 CModel에서 받아서 저장)
	_int		m_iParentIndex{ -1 };
	_float4x4	m_TransformationMatrix{};			// 로컬 행렬
	_float4x4	m_CombinedTransformationMatrix{};	// 부모 행렬이 곱해진 최종 행렬

	// 기본 뼈 나눠서 보관
	KEYFRAME m_tLocalPos{};

public:
	static CMyBone* Create(const myNode* pMyNode, _int iParentIndex);
	CMyBone* Clone();
protected:
	virtual void Free() override;
};

NS_END