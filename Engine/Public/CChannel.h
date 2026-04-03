#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CModel;
class CBone;

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAINodeAnim, CModel* pModel);

public:
	void Update_TransformationMatrix(const vector<CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyIndex);

private:
	// CModel의 CBone 벡터의 인덱스를 들고있어야 함.
	// 뼈의 접근해서 행렬 업데이트 시켜줘야 해서
	_uint				m_iBoneIndex{};
	// 트랙에서 상태(행렬) 개수
	_uint				m_iNumKeyFrames{};
	// 트랙 특정 위치에서 이 뼈의 상태(행렬)
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAINodeAnim, CModel* pModel);
protected:
	virtual void Free() override;
};

NS_END