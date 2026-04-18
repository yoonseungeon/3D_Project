#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CMyModel;
class CMyBone;

class CMyChannel final : public CBase
{
private:
	CMyChannel();
	virtual ~CMyChannel() = default;

public:
	HRESULT Initialize(const myNodeAnim* pMyNodeAnim, CMyModel* pModel);

public:
	void Update_TransformationMatrix(const vector<CMyBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyIndex);

	const string& Get_ChannelName() { return m_strName; }
	_uint Get_BoneIndex() { return m_iBoneIndex; }


	void Get_KeyFrameZeror(vector<KEYFRAME>& KeyFrames);

private:
	// CModel의 CMyBone 벡터의 인덱스를 들고있어야 함.
	// 뼈의 접근해서 행렬 업데이트 시켜줘야 해서
	_uint				m_iBoneIndex{};
	// 트랙에서 상태(행렬) 개수
	_uint				m_iNumKeyFrames{};
	// 트랙 특정 위치에서 이 뼈의 상태(행렬)
	vector<KEYFRAME>	m_KeyFrames;

	// Overlay
	string				m_strName;

public:
	static CMyChannel* Create(const myNodeAnim* pMyNodeAnim, CMyModel* pModel);
protected:
	virtual void Free() override;
};

NS_END