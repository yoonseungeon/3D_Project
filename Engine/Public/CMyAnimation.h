#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CMyModel;
class CMyBone;
class CMyChannel;

class CMyAnimation final : public CBase
{
private:
	CMyAnimation();
	CMyAnimation(const CMyAnimation& Prototype);
	virtual ~CMyAnimation() = default;

private:
	HRESULT Initialize(const myAnimation* pMyAnimation, CMyModel* pModel);

public:
	_bool Update_TransformationMatrices(const vector<CMyBone*>& Bones, _float fTimeDelta, _bool isLoop);

	void Update_TransformationMatZeorKeyFrame(const vector<CMyBone*>& Bones);

	void Reset_KeyFrameIndex();
	void Reset_CurrentTrackPosition() { m_fCurrentTrackPosition = 0.f; }

	void Set_AniSpeed(_float fAniSpeed) { m_fAniSpeed = fAniSpeed; }

	_float Get_AniPlayRatio() { return m_fCurrentTrackPosition / m_fDuration; }

private:
	// 현재 애니메이션 트랙의 총 길이(시간 아님)
	_float				m_fDuration{};
	// 현재 트랙의 초당 재생 속도(트랙 내에서 1초에 얼마나 재생해야 할까)
	_float				m_fTickPerSecond{};
	// 현재 재생 위치
	_float				m_fCurrentTrackPosition{};

	// 이 애니메이션의 재생을 위해 상태를 제어 해야하는 뼈(node)의 개수
	_uint				m_iNumChannels{};
	// 뼈는 트랙의 총 길이에서 각 구간마다 다른 상태를 가질 것임
	// 애니메이션 이산적으로 저장해서 보간해줘야 함.
	// 모든 프레임 x, 키 프레임(주요 프레임)
	vector<CMyChannel*>	m_Channels; 	// 뼈 하나의 상태를 저장하는 클래스
	// 보간할 때 키프레임 구간 왼쪽 인덱스.(왼쪽 오른쪽 차이만큼 보간)
	vector<_uint>		m_CurrentKeyFrameIndices;

	_float m_fAniSpeed{ 1.f };

public:
	static CMyAnimation* Create(const myAnimation* pMyAnimation, CMyModel* pModel);
	CMyAnimation* Clone();
protected:
	virtual void Free() override;
};

NS_END