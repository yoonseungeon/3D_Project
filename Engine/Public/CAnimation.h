#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CModel;
class CBone;
class CChannel;

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, CModel* pModel);
	void	Update_TransformationMatrices(const vector<CBone*>& Bones, _float fTimeDelta);

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
	vector<CChannel*>	m_Channels; 	// 뼈 하나의 상태를 저장하는 클래스


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, CModel* pModel);
protected:
	virtual void Free() override;
};

NS_END